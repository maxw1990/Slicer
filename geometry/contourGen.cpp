#include "contourGen.h"
#include "isPolygonInside.h"
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

namespace contourGen
{

    std::size_t Point2DHash::operator()(const Geometry::Point2D& p) const {
        auto h1 = std::hash<float>{}(p.x);
        auto h2 = std::hash<float>{}(p.y);
        return h1 ^ (h2 << 1);
    }

    void insertHash(HashTable& H, const Geometry::Point2D& u, const Geometry::Point2D& v) {
        H.insert({u, v});
        H.insert({v, u});
    }

    std::pair<Geometry::Point2D, Geometry::Point2D> removeEntry(HashTable& H, const Geometry::Point2D& p) {
        auto range = H.equal_range(p);
        auto it = range.first;
        Geometry::Point2D u = it->second;
        H.erase(it); // Remove the first occurrence
        it = H.find(p);
        Geometry::Point2D v = it->second;
        H.erase(it);
        return {u, v};
    };

    std::vector<Geometry::Contour> contourConstruction(const std::vector<Geometry::Line2D>& segments){
        HashTable H;
        for (const auto& segment : segments) {
            insertHash(H, segment.start, segment.end);
        }

        std::vector<Geometry::Contour> contours;
        while (!H.empty()) {
            Geometry::Point2D p1 = H.begin()->first;
            auto [p2,last] = removeEntry(H, p1);
            Geometry::Contour contour = {p1, p2};
            Geometry::Point2D pj = p2;
            Geometry::Point2D pj_minus_1 = p1;

            while (pj != last) {
                auto [u, v] = removeEntry(H, pj);
                Geometry::Point2D pj_plus_1 = (u == pj_minus_1) ? v : u;
                contour.push_back(pj_plus_1);
                pj_minus_1 = pj;
                pj = pj_plus_1;
            }
            removeEntry(H,last);
            contours.push_back(contour);
        }

        return contours;
    };

    void scaleContour(Geometry::Contour& points, float scaleX, float scaleY) {
        if (points.empty()) return;
        // Scale the points around the center
        std::vector<std::pair<float, float>> scaledPoints;
        for (auto& point : points) {
            point.x = point.x  *scaleX;
            point.y = point.y  *scaleY;
        }
    }

    void createImageFromContours(Geometry::PixelArray& array, std::vector<Geometry::Contour>& contour){
        auto nestedPolyList = fillNestedPolygonList(contour);

        for(auto& poly : nestedPolyList){
            scaleContour(poly.poly,6.0,6.0);
        }

        for(auto poly:nestedPolyList){
            uint8_t color = 255;
            if(poly.nestingLevel % 2 != 0){
                color = 0;
            }
            fillPolygonScanLine(array,poly.poly,color);
        }
    }
}