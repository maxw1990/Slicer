#include "contourGen.h"
#include "isPolygonInside.h"
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>

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

static Geometry::RectInt ComputeFloatBBox(const std::vector<Geometry::Point2D> &poly)
{
    float minX = poly[0].x, maxX = poly[0].x, minY = poly[0].y, maxY = poly[0].y;
    for (const auto &p : poly)
    {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }
    return {(int)std::floor(minX), (int)std::ceil(maxX), (int)std::floor(minY), (int)std::ceil(maxY)};
}

static bool PointInPolygon(const Geometry::Polygon &poly, float x, float y)
{
    bool inside = false;
    size_t n = poly.size();
    size_t j = n - 1;
    for (size_t i = 0; i < n; j = i++)
    {
        const Geometry::Point2D &pi = poly[i];
        const Geometry::Point2D &pj = poly[j];
        if (((pi.y > y) != (pj.y > y)) && (x < (pj.x - pi.x) * (y - pi.y) / (pj.y - pi.y + 1e-12f) + pi.x))
            inside = !inside;
    }
    return inside;
}

static float ComputeArea(const std::vector<Geometry::Point2D> &poly)
{
    if (poly.size() < 3)
        return 0.f;
    float area = 0.f;
    for (size_t i = 0; i < poly.size(); ++i)
    {
        const Geometry::Point2D &a = poly[i];
        const Geometry::Point2D &b = poly[(i + 1) % poly.size()];
        area += a.x * b.y - b.x * a.y;
    }
    return std::abs(area) * 0.5f;
}

enum class EdgeType
{
    Left,
    Right,
    Bottom,
    Top
};
static bool IsInside(const Geometry::Point2D &p, EdgeType e, float v)
{
    switch (e)
    {
    case EdgeType::Left:
        return p.x >= v;
    case EdgeType::Right:
        return p.x <= v;
    case EdgeType::Bottom:
        return p.y >= v;
    case EdgeType::Top:
        return p.y <= v;
    }
    return false;
}
static Geometry::Point2D Intersect(const Geometry::Point2D &a, const Geometry::Point2D &b, EdgeType e, float v)
{
    Geometry::Point2D r = a;
    float dx = b.x - a.x, dy = b.y - a.y;
    if (e == EdgeType::Left || e == EdgeType::Right)
    {
        if (std::abs(dx) < 1e-12f)
            return r;
        float t = (v - a.x) / dx;
        r.x = v;
        r.y = a.y + t * dy;
    }
    else
    {
        if (std::abs(dy) < 1e-12f)
            return r;
        float t = (v - a.y) / dy;
        r.y = v;
        r.x = a.x + t * dx;
    }
    return r;
}

static std::vector<Geometry::Point2D> ClipAgainstEdge(const std::vector<Geometry::Point2D> &input, EdgeType e, float v)
{
    std::vector<Geometry::Point2D> out;
    if (input.empty())
        return out;
    Geometry::Point2D prev = input.back();
    bool prevInside = IsInside(prev, e, v);
    for (const Geometry::Point2D &curr : input)
    {
        bool currInside = IsInside(curr, e, v);
        if (currInside)
        {
            if (!prevInside)
                out.push_back(Intersect(prev, curr, e, v));
            out.push_back(curr);
        }
        else if (prevInside)
        {
            out.push_back(Intersect(prev, curr, e, v));
        }
        prev = curr;
        prevInside = currInside;
    }
    return out;
}
static std::vector<Geometry::Point2D> ClipPolygonToPixel(const std::vector<Geometry::Point2D> &poly, int px, int py)
{
    std::vector<Geometry::Point2D> out = poly;
    out = ClipAgainstEdge(out, EdgeType::Left, px);
    out = ClipAgainstEdge(out, EdgeType::Right, px + 1);
    out = ClipAgainstEdge(out, EdgeType::Bottom, py);
    out = ClipAgainstEdge(out, EdgeType::Top, py + 1);
    return out;
}


// ---------- AA Rasterizer ----------
void RasterizePolygonAA(const Geometry::Polygon &poly, float intensity, Geometry::Matrix &matrix, bool subtract = false)
{
    if (poly.size() < 3)
        return;
    Geometry::RectInt bbox = ComputeFloatBBox(poly);
    bbox.left = std::max(0, bbox.left);
    bbox.top = std::max(0, bbox.top);
    bbox.right = std::min(matrix.Width() - 1, bbox.right);
    bbox.bottom = std::min(matrix.Height() - 1, bbox.bottom);

    for (int y = bbox.top; y <= bbox.bottom; ++y)
    {
        for (int x = bbox.left; x <= bbox.right; ++x)
        {
            int insideCount = 0;
            if (PointInPolygon(poly, (float)x, (float)y))
                insideCount++;
            if (PointInPolygon(poly, (float)x + 1.f, (float)y))
                insideCount++;
            if (PointInPolygon(poly, (float)x + 1.f, (float)y + 1.f))
                insideCount++;
            if (PointInPolygon(poly, (float)x, (float)y + 1.f))
                insideCount++;

            float coverage = 0.f;
            if (insideCount == 4)
                coverage = 1.f;
            else if (insideCount > 0)
            {
                auto clipped = ClipPolygonToPixel(poly, x, y);
                if (!clipped.empty())
                    coverage = ComputeArea(clipped);
            }
            else
                continue;

            coverage = std::min(1.f, std::max(0.f, coverage));
            float &dst = matrix.GetFastRef(x, y);

            if (subtract)
                dst = dst * (1.0f - coverage * intensity);
            else
                dst = dst + (1.0f - dst) * coverage * intensity;
        }
    }
}

    void createImageFromContours(Geometry::Matrix& array, std::vector<Geometry::Contour>& contour){
        auto nestedPolyList = fillNestedPolygonList(contour);

        for(auto& poly : nestedPolyList){
            scaleContour(poly.poly, 6.0, 6.0);
        }

        for(auto nestedPoly : nestedPolyList){
            RasterizePolygonAA(nestedPoly.poly, 1.0f, array, nestedPoly.nestingLevel % 2 == 1);
        }
    }
}