#ifndef CONTOURGEN_H
#define CONTOURGEN_H
#pragma once
#include "Geometry.h"
#include <unordered_map>

namespace contourGen
{

    struct Point2DHash
    {
        std::size_t operator()(const Geometry::Point2D &p) const;
    };

    using HashTable = std::unordered_multimap<Geometry::Point2D, Geometry::Point2D, Point2DHash>;

    void insertHash(HashTable &H, const Geometry::Point2D &u, const Geometry::Point2D &v);

    std::pair<Geometry::Point2D, Geometry::Point2D> removeEntry(HashTable &H, const Geometry::Point2D &p);

    std::vector<Geometry::Contour> contourConstruction(const std::vector<Geometry::Line2D> &segments);

    void scaleContour(Geometry::Contour &points, float scaleX, float scaleY);

    void createImageFromContours(Geometry::PixelArray &array, std::vector<Geometry::Contour> &contour);

} // namespace contourGen

#endif