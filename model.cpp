#include "model.h"
#include <fstream>
#include <string>
#include <sstream>


void Model::init(std::string filename)
{
    std::ifstream input(filename);
    if (input.fail())
        return;
    std::string line;
    

    char slash;
    std::string trash;
    int extra_val;
    Vec3f vertex;
    Vec3f norm;
    Vec3f texture;

    while (getline(input, line))
    {
        std::istringstream iline(line);
        iline >> trash;

        if (trash == "v")
        {
            iline >> vertex.x() >> vertex.y() >> vertex.z();
            verts.push_back(vertex);
        }

        if (trash == "vt")
        {
            iline >> texture.x() >> texture.y() >> texture.z();
            textures.push_back(texture);
        }

        if (trash == "vn")
        {
            iline >> norm.x() >> norm.y() >> norm.z();
            norms.push_back(norm);
        }

        else if (trash == "f")
        {
            faces.push_back(Face());
            for (int i = 0; i < 3; i++)
            {
                iline >> (faces.back()).v.at(i) >> slash >> (faces.back()).vt.at(i) >> slash >> (faces.back()).vn.at(i);
            }
        }
    }

    n_faces = faces.size();
    n_verts = verts.size();
}