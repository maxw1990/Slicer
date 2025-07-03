#pragma once
#include <string>
#include <vector>

namespace Geometry
{
    class Triangle;
}

namespace stl
{
    extern std::vector<Geometry::Triangle> loadBinarySTL(const std::string &filename);

    extern std::vector<Geometry::Triangle> loadAsciiSTL(const std::string &filename);

    extern bool isBinarySTL(const std::string &filename);
}