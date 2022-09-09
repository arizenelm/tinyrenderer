#ifndef MODEL_H
#define MODEL_H

#include "geometry.h"
#include <string>
#include <vector>

class Model
{
private:
    unsigned int n_faces;
    unsigned int n_verts;
    
public:
    std::vector<Vec3f> verts;
    std::vector<Vec3f> norms;
    std::vector<Vec3f> textures;
    struct Face
    {
        Vec3i v = {0, 0, 0};
        Vec3i vt = {0, 0, 0};
        Vec3i vn = {0, 0, 0};
    };
    std::vector<Face> faces;

    Model() : n_faces(0), n_verts(0) {}

    void init(std::string);

    inline unsigned int nfaces() { return n_faces; }
    inline unsigned int nverts() { return n_verts; }
    
};

#endif