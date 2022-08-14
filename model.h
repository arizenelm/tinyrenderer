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
    std::vector<std::vector<int>> faces;

    Model() : n_faces(0), n_verts(0) {}

    void init(std::string);

    inline unsigned int nfaces() { return n_faces; }
    inline unsigned int nverts() { return n_verts; }
    
};

#endif