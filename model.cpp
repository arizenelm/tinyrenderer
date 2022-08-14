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
    

    char trash, slash;
    int extra_val;
    Vec3f vertex;

    while (getline(input, line))
    {
        std::istringstream iline(line);
        iline >> trash;

        if (trash == 'v')
        {
            iline >> vertex.x() >> vertex.y() >> vertex.z();
            //std::cout << vertex.x() << " " << vertex.y() << " " << vertex.z() << std::endl;
            verts.push_back(vertex);
        }

        else if (trash == 'f')
        {
            faces.push_back(std::vector<int>(3));
            for (int i = 0; i < 3; i++)
            {
                iline >> (faces.back())[i] >> slash >> extra_val >> slash >> extra_val;
            }
            //std::cout << (faces.front())[0] << " " << (faces.front())[1] << " " << (faces.front())[2] << std::endl;
        }
    }

    n_faces = faces.size();
    n_verts = verts.size();
}