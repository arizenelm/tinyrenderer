
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
unsigned int HEIGHT = 1000;
unsigned int WIDTH = 1000;

struct Face
{
    std::vector<Vec3f> v = { Vec3f({0, 0, 0}),  Vec3f({0, 0, 0}),  Vec3f({0, 0, 0})};
    std::vector<Vec3f> vt = { Vec3f({0, 0, 0}),  Vec3f({0, 0, 0}),  Vec3f({0, 0, 0})};
    std::vector<Vec3f> vn = { Vec3f({0, 0, 0}),  Vec3f({0, 0, 0}),  Vec3f({0, 0, 0})};
};

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


std::shared_ptr<std::vector<int>> raw_line_dydz (int x0, int y0, int z0, int x1, int y1, int z1)
{
    
    std::unique_ptr<std::vector<int>> result = std::make_unique<std::vector<int>>();
    result->reserve((x1 - x0 + 1) * 2);
    float y = y0;
    float z = z0;
    int x = x0;
    float k1 = float(y1 - y0) / (x1 - x0);
    float k2 = float(z1 - z0) / (x1 - x0);
    do
    {
        result->push_back(int(y));
        result->push_back(int(z));
        y += k1;
        z += k2;
        x++;
    } while (x <= x1);
    
    return result;
}


void draw_colored_face (Face face, TGAImage& image, TGAColor color, std::vector<int>& zbuffer)
{
    int l, r, m;

    if(face.v[0].x() > face.v[1].x())
        std::swap(face.v[0], face.v[1]);
        std::swap(face.vt[0], face.vt[1]);
        std::swap(face.vn[0], face.vn[1]);
    if(face.v[0].x() > face.v[2].x())        
        std::swap(face.v[0], face.v[2]);
        std::swap(face.vt[0], face.vt[2]);
        std::swap(face.vn[0], face.vn[2]);
    if(face.v[1].x() > face.v[2].x())        
        std::swap(face.v[1], face.v[2]);
        std::swap(face.vt[1], face.vt[2]);
        std::swap(face.vn[1], face.vn[2]);

    l = face.v[0].x();
    m = face.v[1].x();
    r = face.v[2].x();

    auto left_to_right = raw_line_dydz (face.v[0].x(), face.v[0].y(), face.v[0].z(), face.v[2].x(), face.v[2].y(), face.v[2].z());
    auto left_to_middle = raw_line_dydz (face.v[0].x(), face.v[0].y(), face.v[0].z(), face.v[1].x(), face.v[1].y(), face.v[1].z());
    auto middle_to_right = raw_line_dydz (face.v[1].x(), face.v[1].y(), face.v[1].z(), face.v[2].x(), face.v[2].y(), face.v[2].z());
    
    for (int x = l; x <= r; x++)
    {
        bool second_segment = x > m;
        int y0 = std::min((second_segment ? middle_to_right->at((x - m) * 2) : left_to_middle->at((x - l) * 2)), left_to_right->at((x - l) * 2));
        int y1 = std::max((second_segment ? middle_to_right->at((x - m) * 2) : left_to_middle->at((x - l) * 2)), left_to_right->at((x - l) * 2));

        int z0 = std::min((second_segment ? middle_to_right->at((x - m) * 2 + 1) : left_to_middle->at((x - l) * 2 + 1)), left_to_right->at((x - l) * 2 + 1));
        int z1 = std::max((second_segment ? middle_to_right->at((x - m) * 2 + 1) : left_to_middle->at((x - l) * 2 + 1)), left_to_right->at((x - l) * 2 + 1));
        float k = float(z1 - z0) / (y1 - y0);
        float z = z0;
        for (int y = y0; y <= y1; y++)
        {
            if (zbuffer[x + y * WIDTH] < z)
            {
                image.set(x, y, color);
                zbuffer[x + y * WIDTH] = (int)z;
            }
            z += k;
        }
    }
    
}


int main()
{
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    
    Model model;
    model.init("obj/african_head.obj");

    std::random_device rd;
    int seed = rd();
    std::default_random_engine generator(seed);
    typedef std::uniform_int_distribution<int> random_int;
    random_int random_color(0, 255);
    int n = model.nfaces();
    Face face;
    std::vector<Vec3f> world_coords(3);
    std::vector<int> zbuffer(WIDTH * HEIGHT);

    Vec3f light_dir(1, 0, 1);
    light_dir.normalize();
 
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
       
            face.v[j] = world_coords[j] = model.verts[model.faces[i].v[j] - 1];
            face.vt[j] = model.textures[model.faces[i].vt[j] - 1];
            face.vn[j] = model.norms[model.faces[i].vt[j] - 1];
            face.v[j].x() = ((face.v[j].x() + 1) / 2) * WIDTH;
            face.v[j].y() = ((face.v[j].y() + 1) / 2) * HEIGHT;
            face.v[j].z() = ((face.v[j].z() + 1) / 2) * HEIGHT;
        }
        std::vector<Vec3f> basis(2);
        basis[0] = Vec3f(world_coords[1].x() - world_coords[0].x(), world_coords[1].y() - world_coords[0].y(), world_coords[1].z() - world_coords[0].z());
        basis[1] = Vec3f(world_coords[2].x() - world_coords[0].x(), world_coords[2].y() - world_coords[0].y(), world_coords[2].z() - world_coords[0].z());
        Vec3f norm;
        for (int j = 0; j < 3; j++)
           norm.at(j) = basis[0][(j + 1) % 3] * basis[1][(j + 2) % 3] - basis[0][(j + 2) % 3] * basis[1][(j + 1) % 3];
        norm.normalize();
        float intensity = light_dir[0] * norm[0] + light_dir[1] * norm[1] + light_dir[2] * norm[2]; 
        if (intensity > 0)
            draw_colored_face(face, image, TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255), zbuffer); 
    }
  

    TGAImage image1(WIDTH, HEIGHT, TGAImage::RGB);
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            if (zbuffer[x + y * HEIGHT] > 0)
            {
                int z_color = (float(zbuffer[x + y * WIDTH]) / 1000) * 255;
                image1.set(x, y, TGAColor(z_color, z_color, z_color));
            }
        }
    }

    image.flip_horizontally();
    image.write_tga_file("output.tga");
    image1.flip_horizontally();
    image1.write_tga_file("zbuffer.tga");
    return 0;
}