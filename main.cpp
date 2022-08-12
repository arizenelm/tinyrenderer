
#include "tinyrenderer/tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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
    
    float s = ((float)(y1 - y0)) / (x1 - x0);

    int x;
    float y;
    x = x0;
    y = y0;
    image.set(y, x, color);
    do
    {
        x += 1;
        y += s;
        if (swapped)
            image.set((int)y, x, color);
        else
            image.set(x, (int)y, color);
    } while (x < x1);
    

}

int main()
{
    TGAImage image(100, 100, TGAImage::RGB);
    line(13, 20, 80, 40, image, white);
    line(20, 13, 40, 80, image, red);
    line(80, 40, 13, 20, image, red);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}