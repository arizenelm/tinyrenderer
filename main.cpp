
#include "tgaimage/tgaimage.h"
#include "model.h"
#include <iostream>
#include <cmath>
#include <exception>
#include <algorithm>
#include <memory>

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
    image.set(y, x, color);   // fix needed? //
    do
    {
        x += 1;
        if (dy_2 >= dx_2)
        {
            y += inc;
            dx_2 += 2 * dx;
        }
        dy_2 += 2 * dy;
        if (swapped)
            image.set((int)y, x, color);
        else
            image.set(x, (int)y, color);
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

<<<<<<< HEAD

std::shared_ptr<std::vector<int>> raw_line_dx (int x0, int y0, int x1, int y1)
{
    
    std::unique_ptr<std::vector<int>> result = std::make_unique<std::vector<int>>();
    std::cout << "vector size before resizing is " << result->size() << std::endl;
    result->reserve(x1 - x0 + 1);
    std::cout << "vector size after resizing is " << result->size() << std::endl;
    float y = y0;
    int x = x0;
    float k = float(y1 - y0) / (x1 - x0);
    do
    {
        result->push_back(int(y));
        std::cout << result->front() << " " << int(y) << std::endl;
        y += k;
        x++;
    } while (x < x1);
    
    return result;
}


void draw_colored_face (std::vector<Vec3f> const& v, TGAImage& image, TGAColor color)
{
    draw_face(v, image, color);
    Vec3f left, right, middle;
    int l, r, m;
    l = std::min( { v[0][0], v[1][0], v[2][0] } );
    r = std::max( { v[0][0], v[1][0], v[2][0] } );
    for (int i = 0; i < 3; i++)
    {
        if (v[i][0] == l)
            left = v[i];
        else if (v[i][0] == r)
            right = v[i];
        else
            middle = v[i];
    } 
    m = middle[0];
    auto left_to_right = raw_line_dx (left[0], left[1], right[0], right[1]);
    auto left_to_middle = raw_line_dx (left[0], left[1], middle[0], middle[1]);
    auto middle_to_right = raw_line_dx (middle[0], middle[1], right[0], right[1]);
    for (int x = l; x <= m; x++)
    {
        int mn = std::min(left_to_middle->at(x - l), left_to_right->at(x - l));
        int mx = std::max(left_to_middle->at(x - l), left_to_right->at(x - l));
        for (int y = mn; y <= mx; y++)
        {
            image.set(x, y, color);
        }
    }
    
}


=======
//some comment//
>>>>>>> master
int main()
{
    unsigned int HEIGHT = 1000;
    unsigned int WIDTH = 1000;
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    
    Model model;
    model.init("obj/african_head.obj");

    int n = model.nfaces();
    std::vector<Vec3f> face(3);

/*
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            face[j] = model.verts[model.faces[i][j] - 1];
            face[j].x() = ((face[j][0] + 1) / 2) * WIDTH;
            face[j].y() = ((face[j][1] + 1) / 2) * HEIGHT;
        }
        draw_face(face, image, white); 
    }
*/
    

    std::vector<Vec3f> colored_face;
    colored_face.push_back(Vec3f(10, 10, 0));
    colored_face.push_back(Vec3f(500, 100, 0));
    colored_face.push_back(Vec3f(240, 700, 0));
    std::cout << colored_face.size() << std::endl;    
    draw_colored_face(colored_face, image, red);


    image.flip_horizontally();
    image.write_tga_file("output.tga");
    return 0;
}