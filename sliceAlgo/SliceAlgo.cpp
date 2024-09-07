#include "SliceAlgo.h"
#include <map>
#include <algorithm>
#include "geometry/Geometry.h"
#include "geometry/contourGen.h"
#include <chrono>
#include "png_lib/fpng.h"
#include "active-object/ActiveObject.h"
#include "active-object/IMessage.h"

class SavePng : public Message
{
public:
    SavePng(const void *pImage, float height,std::string path)
    :
    m_pImage(pImage), 
    m_height(height),
    m_path(path)
    {
    }
    bool execute() override
    {
        std::string file = m_path + "/output" + std::to_string(m_height) + ".png";
        std::cout << "printing slice: " << m_height << std::endl;
        bool success = fpng::fpng_encode_image_to_file(file.data(), m_pImage, 1920, 1080, 3, 0);
        return true;
    }

private:
    const void *m_pImage;
    const float m_height;
    const std::string m_path;
};

class PrintMessage : public Message
{
public:
    PrintMessage(const std::string &str) : message(str) {}
    bool execute() override
    {
        std::cout << message << std::endl;
        return true;
    }

private:
    std::string message;
};

SlicerAlgo::SlicerAlgo(float height, float sliceHeight, std::vector<Geometry::Triangle> &stlTris,  std::string path):
 objHeight(height),
 sliceHeight(sliceHeight),
 m_stl(stlTris),
 m_outputImagePath(path)
 {

 };

bool SlicerAlgo::run()
{
    fpng::fpng_init();
    Geometry::PixelArray array(1080, std::vector<uint8_t>(1920, 0));
    ActiveObject threadObject;
    threadObject.send(std::make_shared<PrintMessage>(PrintMessage("start Printing images")));
    Geometry::Plane p = Geometry::Plane(Geometry::Vector(0, 0, 1)); // z axis
    float objektHeigth = this->objHeight;                           // in mm
    int counter = 0;
    for (float cutterHeight = -1.f; cutterHeight < objektHeigth; cutterHeight += 1)
    {

        p.setDistance(cutterHeight + sliceHeight / 2.0f);
        std::vector<Geometry::Vector> intersectionPoints;
        Geometry::Vector planePos = p.getNormal().multiply(p.getDistance());

        std::vector<Geometry::Line2D> polyLines;
        for (int i = 0; i < (int)m_stl.size(); i++)
        {
            std::vector<Geometry::Vector> ints = m_stl[i].intersectPlane(p);

            std::vector<float> x(ints.size());
            std::vector<float> y(ints.size());

            for (int j = 0; j < (int)ints.size(); j++)
            {
                Geometry::Vector posInPlane = planePos.sub(ints.at(j));
                x[j] = p.getV().scalar(posInPlane);
                y[j] = p.getU().scalar(posInPlane);
            }

            if (x.size() > 1)
            {
                if (x[0] != x[1] || y[0] != y[1])
                {
                    polyLines.push_back(Geometry::Line2D(x[0], y[0], x[1], y[1]));
                }
            }
        }

        auto polygonArrays = contourGen::contourConstruction(polyLines);
        // std::vector<Geometry::Contour> polygonArrays;
        contourGen::createImageFromContours(array, polygonArrays);
        const void *pImage = Geometry::convertMatrixToImage(array, 1920, 1080);
        auto msg = std::make_shared<SavePng>(SavePng(pImage, cutterHeight,m_outputImagePath));
        threadObject.send(msg);
        counter++;
    };
    threadObject.WaitAndStop();
    return true;
}