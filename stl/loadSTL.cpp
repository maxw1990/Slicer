#include "loadSTL.h"
#include "geometry/Geometry.h"
#include <iostream>
#include <fstream>
#include <array>
#include <sstream>

std::vector<Geometry::Triangle> stl::loadBinarySTL(const std::string &filename)
{
    std::vector<Geometry::Triangle> triangles;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return triangles;
    }

    // Skip the 80-byte header
    char header[80];
    file.read(header, 80);

    // Read the number of triangles
    uint32_t numTriangles;
    file.read(reinterpret_cast<char *>(&numTriangles), sizeof(uint32_t));

    triangles.resize(numTriangles);
    for (uint32_t i = 0; i < numTriangles; ++i)
    {
        Geometry::Triangle &triangle = triangles[i];
        file.read(reinterpret_cast<char *>(&triangle), sizeof(Geometry::Triangle));
        file.ignore(2); // Ignore the attribute byte count
    }

    file.close();
    return triangles;
}

std::vector<Geometry::Triangle> stl::loadAsciiSTL(const std::string &filename)
{
    std::vector<Geometry::Triangle> triangles;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return triangles;
    }

    std::string line;
    Geometry::Triangle triangle;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        if (word == "facet")
        {
            iss >> word; // Skip "normal"
            iss >> triangle.normal.x >> triangle.normal.y >> triangle.normal.z;
        }
        else if (word == "vertex")
        {
            for (int i = 0; i < 3; ++i)
            {
                if (i > 0)
                    std::getline(file, line); // Read the next vertex line if i > 0
                std::istringstream vertexIss(line);
                vertexIss >> word; // Skip "vertex"
                vertexIss >> triangle.corner[i].x >> triangle.corner[i].y >> triangle.corner[i].z;
            }
            triangles.push_back(triangle);
        }
    }

    file.close();
    return triangles;
}

bool stl::isBinarySTL(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        return false;
    }

    // Read the first 80 bytes of the file (the header in binary STL)
    char header[80];
    file.read(header, 80);

    // Check if the header contains non-ASCII characters
    bool isBinary = false;
    for (int i = 0; i < 80; ++i)
    {
        if (header[i] < 0 || header[i] > 127)
        {
            isBinary = true;
            break;
        }
    }

    // If the header does not contain non-ASCII characters, check the first word
    if (!isBinary)
    {
        std::string headerStr(header, 80);
        if (headerStr.find("solid") != std::string::npos)
        {
            // Check for the presence of "facet" keyword to confirm ASCII STL
            std::string line;
            while (std::getline(file, line))
            {
                if (line.find("facet") != std::string::npos)
                {
                    return false; // It's ASCII STL
                }
            }
        }
    }

    // If the header contains non-ASCII characters or the word "solid" is not followed by "facet"
    return true; // It's binary STL
}
