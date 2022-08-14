#include "model.h"
#include <fstream>
#include <string>
#include <sstream>


void Model::init(std::string filename)
{
    std::ifstream input(filename);
    std::string line;
    

    char trash, slash;
    int extra_val;
    Vec3i vertex;

    while (getline(input, line))
    {
        std::istringstream iline(line);
        iline >> trash;

        if (trash == 'v')
        {
            iline >> vertex.x() >> vertex.y() >> vertex.z();
            verts.push_back(vertex);
        }

        else if (trash == 'f')
        {
            faces.push_back(std::vector<int>(3));
            for (int i = 0; i < 3; i++)
            {
                iline >> (faces.front())[i] >> slash >> extra_val >> slash >> extra_val;
            }
        }
    }

    n_faces = faces.size();
    n_verts = verts.size();
}