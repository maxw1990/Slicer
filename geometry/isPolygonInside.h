#pragma once
#include "Geometry.h"

bool isPointInPolygon(const Geometry::Point2D &pt, const std::vector<Geometry::Point2D> &poly);

bool doLineSegmentsIntersect(const Geometry::Point2D &p1, const Geometry::Point2D &q1, const Geometry::Point2D &p2, const Geometry::Point2D &q2);
bool arePolygonsIntersecting(const std::vector<Geometry::Point2D> &poly1, const std::vector<Geometry::Point2D> &poly2);

enum PolygonRelationship
{
    POLY1_INSIDE_POLY2,
    POLY2_INSIDE_POLY1,
    POLYGONS_INTERSECT,
    POLYGONS_OUTSIDE
};

PolygonRelationship determinePolygonRelationship(const std::vector<Geometry::Point2D> &poly1, const std::vector<Geometry::Point2D> &poly2);

bool isPolygonInsidePolygon(const Geometry::Polygon &poly1, const Geometry::Polygon &poly2);

typedef struct
{
    Geometry::Contour poly;
    int nestingLevel;
} nestPoly;

bool sortNestedPoly(nestPoly &i, nestPoly &j);

std::vector<nestPoly> fillNestedPolygonList(std::vector<Geometry::Contour> &polygons);