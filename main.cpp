
#include "tgaimage/tgaimage.h"
#include "model.h"
#include <iostream>
#include <cmath>
#include <exception>

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
    
    
    int x;
    int y;
    x = x0;
    y = y0;
    int dx = x1 - x0, dx_2 = x1 - x0;
    int dy = abs(y1 - y0);
    int inc = (y1 - y0) / abs(y1 - y0);
    int dy_2 = 2 * abs(y1 - y0);
    image.set(y, x, color);
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
        throw (std::exception());
    for (int i = 0; i < 3; i++)
    {
        line(v[i][0], v[i][1], v[(i+1) % 3][0], v[(i+1) % 3][1], image, color);
    }
}


int main()
{
    unsigned int HEIGHT = 1000;
    unsigned int WIDTH = 1000;
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    
    Model model;
    model.init("obj/african_head.obj");

    int n = model.nfaces();
    std::vector<Vec3f> face(3);


    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            //std::cout << model.faces[i][j] - 1 << std::endl;
            face[j] = model.verts[model.faces[i][j] - 1];
            //std::cout << face[j].x() << " " << face[j].y() << std::endl;
            face[j].x() = ((face[j][0] + 1) / 2) * WIDTH;
            face[j].y() = ((face[j][1] + 1) / 2) * HEIGHT;
            //std::cout << face[j].x() << " " << face[j].y() << std::endl;
        }
        //std::cout << face[0].x() << " " << face[0].y() << face[1].x() << " " << face[1].y() << face[2].x() << " " << face[2].y() << std::endl;
        draw_face(face, image, white); 
    }

    image.flip_horizontally();
    image.write_tga_file("output.tga");
    return 0;
}