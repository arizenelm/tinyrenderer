
#include "tgaimage/tgaimage.h"
#include "model.h"
#include <iostream>
#include <cmath>
#include <exception>
#include <algorithm>
#include <memory>
#include <random>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor some_random_color(154, 234, 99, 255);
const TGAColor some_blue(15, 0, 220, 255);


void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) 
{
    bool swapped = false;
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        swapped = true;
    }

    if (x1 < x0)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }    
    
    
    int x = x0;
    int y = y0;
    int dx = x1 - x0, dx_2 = x1 - x0;
    int dy = abs(y1 - y0);
    int inc = (y1 - y0) / abs(y1 - y0);
    int dy_2 = 2 * abs(y1 - y0);
    do
    {
        if (swapped)
            image.set((int)y, x, color);
        else
            image.set(x, (int)y, color);
        x += 1;
        if (dy_2 >= dx_2)
        {
            y += inc;
            dx_2 += 2 * dx;
        }
        dy_2 += 2 * dy;
    } while (x < x1);
    

}

void draw_face (std::vector<Vec3f> const& v, TGAImage& image, TGAColor color)
{
    if (v.size() != 3)
        throw (std::out_of_range("incorrect size of face"));
    for (int i = 0; i < 3; i++)
    {
        line(v[i][0], v[i][1], v[(i+1) % 3][0], v[(i+1) % 3][1], image, color);
    }
}


std::shared_ptr<std::vector<int>> raw_line_dx (int x0, int y0, int x1, int y1)
{
    
    std::unique_ptr<std::vector<int>> result = std::make_unique<std::vector<int>>();
    result->reserve(x1 - x0 + 1);
    float y = y0;
    int x = x0;
    float k = float(y1 - y0) / (x1 - x0);
    do
    {
        result->push_back(int(y));
        y += k;
        x++;
    } while (x <= x1);
    
    return result;
}


void draw_colored_face (std::vector<Vec3f>& v, TGAImage& image, TGAColor color)
{
    draw_face(v, image, color);
    int l, r, m;

    if(v[0].x() > v[1].x())
        std::swap(v[0], v[1]);
    if(v[0].x() > v[2].x())
        std::swap(v[0], v[2]);
    if(v[1].x() > v[2].x())
        std::swap(v[1], v[2]);

    l = v[0].x();
    m = v[1].x();
    r = v[2].x();

    auto left_to_right = raw_line_dx (v[0].x(), v[0].y(), v[2].x(), v[2].y());
    auto left_to_middle = raw_line_dx (v[0].x(), v[0].y(), v[1].x(), v[1].y());
    auto middle_to_right = raw_line_dx (v[1].x(), v[1].y(), v[2].x(), v[2].y());
    
    for (int x = l; x <= r; x++)
    {
        bool second_segment = x > m;
        int mn = std::min((second_segment ? middle_to_right->at(x - m) : left_to_middle->at(x - l)), left_to_right->at(x - l));
        int mx = std::max((second_segment ? middle_to_right->at(x - m) : left_to_middle->at(x - l)), left_to_right->at(x - l));
        for (int y = mn; y <= mx; y++)
        {
            image.set(x, y, color);
        }
    }
    
}


int main()
{
    unsigned int HEIGHT = 1000;
    unsigned int WIDTH = 1000;
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    
    Model model;
    model.init("obj/african_head.obj");

    std::random_device rd;
    int seed = rd();
    std::default_random_engine generator(seed);
    typedef std::uniform_int_distribution<int> random_int;
    random_int random_color(0, 255);
    int n = model.nfaces();
    std::vector<Vec3f> face(3);
    std::vector<Vec3f> world_coords(3);

    Vec3f light_dir(0.2, 0, 1);
 
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            face[j] = world_coords[j] = model.verts[model.faces[i][j] - 1];
            face[j].x() = ((face[j][0] + 1) / 2) * WIDTH;
            face[j].y() = ((face[j][1] + 1) / 2) * HEIGHT;
        }
        std::vector<Vec3f> v;
        v.reserve(2);
        v.push_back(Vec3f(world_coords[1].x() - world_coords[0].x(), world_coords[1].y() - world_coords[0].y(), world_coords[1].z() - world_coords[0].z()));
        v.push_back(Vec3f(world_coords[2].x() - world_coords[0].x(), world_coords[2].y() - world_coords[0].y(), world_coords[2].z() - world_coords[0].z()));
        Vec3f n;
        for (int j = 0; j < 3; j++)
           n.at(j) = v[0][(j + 1) % 3] * v[1][(j + 2) % 3] - v[0][(j + 2) % 3] * v[1][(j + 1) % 3];
        n.normalize();
        light_dir.normalize();
        float intensity = light_dir[0] * n[0] + light_dir[1] * n[1] + light_dir[2] * n[2]; 
        if (intensity > 0)
            draw_colored_face(face, image, TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255)); 
    }
  

    //std::vector<Vec3f> colored_face = {Vec3f(10, 10, 0), Vec3f(500, 100, 0), Vec3f(240, 700, 0)};
    //draw_colored_face(colored_face, image, red);


    image.flip_horizontally();
    image.write_tga_file("output.tga");
    return 0;
}