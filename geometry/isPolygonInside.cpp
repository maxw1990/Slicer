#include "isPolygonInside.h"
#include <algorithm>
#include <queue>

bool isPointInPolygon(const Geometry::Point2D &pt, const std::vector<Geometry::Point2D> &poly)
{
    int n = poly.size();
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        if ((poly[i].y > pt.y) != (poly[j].y > pt.y) &&
            (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x))
        {
            inside = !inside;
        }
    }
    return inside;
}

bool doLineSegmentsIntersect(const Geometry::Point2D &p1, const Geometry::Point2D &q1, const Geometry::Point2D &p2, const Geometry::Point2D &q2)
{
    auto orientation = [](const Geometry::Point2D &p, const Geometry::Point2D &q, const Geometry::Point2D &r)
    {
        double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (val == 0)
            return 0;
        return (val > 0) ? 1 : 2;
    };

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4)
        return true;

    return false;
}

bool arePolygonsIntersecting(const std::vector<Geometry::Point2D> &poly1, const std::vector<Geometry::Point2D> &poly2)
{
    for (size_t i = 0; i < poly1.size(); i++)
    {
        for (size_t j = 0; j < poly2.size(); j++)
        {
            if (doLineSegmentsIntersect(
                    poly1[i], poly1[(i + 1) % poly1.size()],
                    poly2[j], poly2[(j + 1) % poly2.size()]))
            {
                return true;
            }
        }
    }
    return false;
}

PolygonRelationship determinePolygonRelationship(const std::vector<Geometry::Point2D> &poly1, const std::vector<Geometry::Point2D> &poly2)
{
    bool poly1InsidePoly2 = true;
    for (const auto &pt : poly1)
    {
        if (!isPointInPolygon(pt, poly2))
        {
            poly1InsidePoly2 = false;
            break;
        }
    }

    bool poly2InsidePoly1 = true;
    for (const auto &pt : poly2)
    {
        if (!isPointInPolygon(pt, poly1))
        {
            poly2InsidePoly1 = false;
            break;
        }
    }

    bool polygonsIntersect = arePolygonsIntersecting(poly1, poly2);

    if (poly1InsidePoly2 && !polygonsIntersect)
    {
        return POLY1_INSIDE_POLY2;
    }

    if (poly2InsidePoly1 && !polygonsIntersect)
    {
        return POLY2_INSIDE_POLY1;
    }

    if (polygonsIntersect)
    {
        return POLYGONS_INTERSECT;
    }

    return POLYGONS_OUTSIDE;
}

bool isPolygonInsidePolygon(const Geometry::Polygon &poly1, const Geometry::Polygon &poly2)
{
    for (const auto &pt : poly1)
    {
        if (!isPointInPolygon(pt, poly2))
        {
            return false;
        }
    }
    return true;
}

bool sortNestedPoly(nestPoly &i, nestPoly &j) { return (i.nestingLevel < j.nestingLevel); }

std::vector<nestPoly> fillNestedPolygonList(std::vector<Geometry::Contour> &polygons)
{
    std::vector<nestPoly> nestedPoly;
    for (auto poly : polygons)
    {
        nestedPoly.push_back(nestPoly{std::move(poly), 0});
    }

    for (int i = 0; i < (int)nestedPoly.size(); i++)
    {
        for (int j = 0; j < (int)nestedPoly.size(); j++)
        {
            if (i != j)
            {
                if (isPolygonInsidePolygon(nestedPoly[i].poly, nestedPoly[j].poly))
                {
                    nestedPoly[i].nestingLevel++;
                }
            }
        }
    }
    std::sort(nestedPoly.begin(), nestedPoly.end(), sortNestedPoly);
    return nestedPoly;
}