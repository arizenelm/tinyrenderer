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
    std::vector<Vec2f> vt = { Vec2f({0, 0}),  Vec2f({0, 0}),  Vec2f({0, 0}) };
    std::vector<Vec3f> vn = { Vec3f({0, 0, 0}),  Vec3f({0, 0, 0}),  Vec3f({0, 0, 0})};
};

struct Vertex
{
    Vec3f v = {0, 0, 0};
    Vec2f vt = {0, 0};

};

inline float scalar_mult(Vec3f const& a, Vec3f const& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

template<typename T>
inline T heaviside(T a) { return a > 0 ? a : 0; } 

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

void draw_empty_face(std::vector<Vec3f> const& v, TGAImage& image, TGAColor color)
{
    if (v.size() != 3)
        throw (std::out_of_range("incorrect size of face"));
    for (int i = 0; i < 3; i++)
    {
        line(v[i][0], v[i][1], v[(i+1) % 3][0], v[(i+1) % 3][1], image, color);
    }
}


std::unique_ptr<std::vector<float>> raw_line_all(Vec3f& v0, Vec3f& v1, Vec3f& i0, Vec3f& i1, int t0, int t1, Vec3f& light_dir)
{
    
    std::unique_ptr<std::vector<float>> result = std::make_unique<std::vector<float>>();

    int x0 = v0.x();
    int y0 = v0.y();
    int z0 = v0.z();
    int x1 = v1.x();
    int y1 = v1.y();
    int z1 = v1.z();

    i0.normalize();
    i1.normalize();

    float intens0 = std::abs(scalar_mult(light_dir, i0));  
    float intens1 = std::abs(scalar_mult(light_dir, i1));

    result->reserve((x1 - x0 + 1) * 4);
    float t = t0;
    float y = y0;
    float z = z0;
    float intens = intens0;
    int x = x0;
    float k1 = float(y1 - y0) / (x1 - x0);
    float k2 = float(z1 - z0) / (x1 - x0);
    float k3 = float(intens1 - intens0) / (x1 - x0);
    float k4 = float(t1 - t0) / (x1 - x0);
    do
    {
        result->push_back(int(y + 0.5));
        result->push_back(int(z + 0.5));
        result->push_back(intens);
        result->push_back(t);
        y += k1;
        z += k2;
        intens += k3;
        t += k4;
        x++;
    } while (x <= x1);
    
    return result;
}


void draw_colored_face1(Face face, TGAImage& image, TGAImage& texture, std::vector<int>& zbuffer, Vec3f& light_dir)
{
    int l, r, m;

    if(face.v[0].x() > face.v[1].x())
    {
        std::swap(face.v[0], face.v[1]);
        std::swap(face.vt[0], face.vt[1]);
        std::swap(face.vn[0], face.vn[1]);
    }
    if(face.v[0].x() > face.v[2].x())        
    {
        std::swap(face.v[0], face.v[2]);
        std::swap(face.vt[0], face.vt[2]);
        std::swap(face.vn[0], face.vn[2]);
    }
    if(face.v[1].x() > face.v[2].x())        
    {
        std::swap(face.v[1], face.v[2]);
        std::swap(face.vt[1], face.vt[2]);
        std::swap(face.vn[1], face.vn[2]);
    }

    l = face.v[0].x();
    m = face.v[1].x();
    r = face.v[2].x();

    auto left_to_right = raw_line_all (face.v[0], face.v[2], face.vn[0], face.vn[2], face.vt[0].t(), face.vt[2].t(), light_dir);
    auto left_to_middle = raw_line_all (face.v[0], face.v[1], face.vn[0], face.vn[1], face.vt[0].t(), face.vt[1].t(), light_dir);
    auto middle_to_right = raw_line_all (face.v[1], face.v[2], face.vn[1], face.vn[2], face.vt[1].t(), face.vt[2].t(), light_dir);

    int u = face.vt[0].u();
    
    for (int x = l; x <= r; x++)
    {
        bool second_segment = x > m;

        auto get_lower = [&](int ii) { return second_segment ? middle_to_right->at((x - m) * 4 + ii) : left_to_middle->at((x - l) * 4 + ii); };
        auto get_upper = [&](int ii) { return left_to_right->at((x - l) * 4 + ii); }; 

        float lower_segment[4] = { get_lower(0), get_lower(1), get_lower(2), get_lower(3) };
        float upper_segment[4] = { get_upper(0), get_upper(1), get_upper(2), get_upper(3) };

        if (lower_segment[0] > upper_segment[0])
            std::swap(lower_segment, upper_segment);

        int y0 = lower_segment[0];
        int y1 = upper_segment[0];

        float z0 = lower_segment[1];
        float z1 = upper_segment[1];

        float intens0 = lower_segment[2];
        float intens1 = upper_segment[2];

        float t0 = lower_segment[3];
        float t1 = upper_segment[3];
     
        float k1 = float(z1 - z0) / (y1 - y0);
        float k2 = float(intens1 - intens0) / (y1 - y0);
        float k3 = float(t1 - t0) / (y1 - y0);
        float z = z0;
        float t = t0;
        float intensity = intens0;
        for (int y = y0; y < y1; y++)
        {
            if (zbuffer[x + y * WIDTH] < z)
            {
                TGAColor temp(texture.get(u, t));
                image.set(x, y, TGAColor(temp[2] * intensity, temp[1]* intensity, temp[0] * intensity));
                zbuffer[x + y * WIDTH] = (int)z;
            }
            z += k1;
            intensity += k2;
            t += k3;
        }
        u++;
    }
    
}


void draw_face(Face face, TGAImage const& image, TGAImage const& texture, std::vector<int>& zbuffer, Vec3f& lightdir)
{
    int l, r, m;

    if(face.v[0].x() > face.v[1].x())
    {
        std::swap(face.v[0], face.v[1]);
        std::swap(face.vt[0], face.vt[1]);
        std::swap(face.vn[0], face.vn[1]);
    }
    if(face.v[0].x() > face.v[2].x())        
    {
        std::swap(face.v[0], face.v[2]);
        std::swap(face.vt[0], face.vt[2]);
        std::swap(face.vn[0], face.vn[2]);
    }
    if(face.v[1].x() > face.v[2].x())        
    {
        std::swap(face.v[1], face.v[2]);
        std::swap(face.vt[1], face.vt[2]);
        std::swap(face.vn[1], face.vn[2]);
    }

    l = face.v[0].x();
    m = face.v[1].x();
    r = face.v[2].x();

    if (l == r)
        return;

    float intens1, intens0;
    float y0, z0, t0, i0;
    float y1, z1, y1, i1;

    std::array<float, 3> dy;
    std::array<float, 3> dz;
    std::array<float, 3> dt;
    std::array<float, 3> di;
    std::array<int, 3> lrm = {l, r, m};

    //float left_to_middle;
    //float middle_to_right;

    //if (m > l)
    //    left_to_middle = m - l;
    //if (m < r)
    //    middle_to_right = r - m;
    for (int i = 0; i < 3; i++) {
        auto f = [](int ii){ return ii + (ii < 2); };   // 0 -> 1, 1 -> 2, 2 -> 2
        auto g = [](int ii){ return ii % 2; };          // 0 -> 0, 1 -> 1, 2 -> 0
        intens0 = std::abs(scalar_mult(lightdir, face.vn[g(i)].normalized()));  
        intens1 = std::abs(scalar_mult(lightdir, face.vn[f(i)].normalized()));
        dy[i] = float(face.v[f(i)].y() - face.v[g(i)].y()) / (lrm[f(i)] - lrm[g(i)]);
        dz[i] = float(face.v[f(i)].z() - face.v[g(i)].z()) / (lrm[f(i)] - lrm[g(i)]);
        dt[i] = float(face.vt[f(i)].y() - face.vt[g(i)].y()) / (face.vt[f(i)].x() - face.vt[g(i)].x());
        di[i] = float(intens1 - intens0) / (lrm[f(i)] - lrm[g(i)]); // divide by zero!
    }

    Vec3f ltm {face.v[1].x() - face.v[0].x(), face.v[1].y() - face.v[0].y(), face.v[1].z() - face.v[0].z()};
    Vec3f mtr {face.v[2].x() - face.v[1].x(), face.v[2].y() - face.v[1].y(), face.v[2].z() - face.v[1].z()};
    Vec3f ltr {face.v[2].x() - face.v[0].x(), face.v[2].y() - face.v[0].y(), face.v[2].z() - face.v[0].z()};

    Vec2f ltm_t {face.vt[1].x() - face.vt[0].x(), face.vt[1].y() - face.vt[0].y()};
    Vec2f mtr_t {face.vt[2].x() - face.vt[1].x(), face.vt[2].y() - face.vt[1].y()};
    Vec2f ltr_t {face.vt[2].x() - face.vt[0].x(), face.vt[2].y() - face.vt[0].y()};

    //float k0 = ltm.normalize();
    //float k1 = ltm.normalize();
    //float k2 = ltm.normalize();



    Vec3f v1, v2;
    Vec2f vt1, vt2, vd, vtd;
    float i1, i2;

    float intens_m = heaviside(scalar_mult(lightdir, face.vn[1]));
    float intens_l = heaviside(scalar_mult(face.vn[0], lightdir));
    float intens_r = heaviside(scalar_mult(face.vn[2], lightdir));

    for (int i = l; i <= r; i++) {
        if (i <= m) {
            float k1 = float(i - l) / (m - l);      // To be optimized...
            if (std::isnan(k1)) k1 = 1;
            float k2 = float(i - l) / (r - l);
            v1 = face.v[0] + k1 * ltm;
            v2 = face.v[0] + k2 * ltr;  
            vt1 = face.vt[0] + k1 * ltm_t;
            vt2 = face.vt[0] + k2 * ltr_t;
            i1 = intens_l + k1;
            i2 = intens_l + k2;
        }
        else {

        }
        vd = {v2.x() - v1.x(), v2.y() - v1.y()};
        vtd = vt2 - vt1;
        float bottom = std::min(v1.y(), v2.y());
        float top = std::max(v1.y(), v2.y());
        for (int j = bottom; j <= top; j++) {
            // bottom can be equal to top
        }

    }


}

int main()
{
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    TGAImage texture_diffuse;
    texture_diffuse.read_tga_file("obj/african_head_diffuse.tga");
    texture_diffuse.flip_vertically();
    
    Model model;
    model.init("obj/african_head.obj");
    int n = model.nfaces();
    Face face;
    std::vector<Vec3f> world_coords(3);
    std::vector<int> zbuffer(WIDTH * HEIGHT);
    Vec3f light_dir(0, 0, 1);
    light_dir.normalize();
 
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
       
            face.v[j] = world_coords[j] = model.verts[model.faces[i].v[j] - 1];
            face.vt[j] = model.textures[model.faces[i].vt[j] - 1];
            face.vn[j] = model.norms[model.faces[i].vn[j] - 1];
            face.v[j].x() = ((face.v[j].x() + 1) / 2) * WIDTH;
            face.v[j].y() = ((face.v[j].y() + 1) / 2) * HEIGHT;
            face.v[j].z() = ((face.v[j].z() + 1) / 2) * HEIGHT;
            face.vt[j].u() *= 1024;
            face.vt[j].t() *= 1024;
        }
        std::vector<Vec3f> basis(2);
        basis[0] = Vec3f(world_coords[1].x() - world_coords[0].x(), world_coords[1].y() - world_coords[0].y(), world_coords[1].z() - world_coords[0].z());
        basis[1] = Vec3f(world_coords[2].x() - world_coords[0].x(), world_coords[2].y() - world_coords[0].y(), world_coords[2].z() - world_coords[0].z());
        Vec3f norm;
        for (int j = 0; j < 3; j++)
           norm.at(j) = basis[0][(j + 1) % 3] * basis[1][(j + 2) % 3] - basis[0][(j + 2) % 3] * basis[1][(j + 1) % 3];
        norm.normalize();
        float intensity = light_dir[0] * norm[0] + light_dir[1] * norm[1] + light_dir[2] * norm[2];
        try {
        if (intensity > 0)
            draw_colored_face1(face, image, texture_diffuse, zbuffer, light_dir);
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return 0;
        }
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