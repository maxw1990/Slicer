#pragma once
#include <vector>

namespace Geometry
{
    class Triangle;
}
// use active object for png creation. add an image task to the object when image was successfully created.
// add a wait and stop in the runner.
class SlicerAlgo
{
public:
    bool run();
    SlicerAlgo(float height, float sliceHeight, std::vector<Geometry::Triangle> &stlTris, std::string path);

private:
    float objHeight;
    float sliceHeight;
    std::string m_outputImagePath;
    std::vector<Geometry::Triangle> &m_stl;
};
