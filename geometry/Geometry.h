#ifndef GEOMETRY_H
#define GEOMETRY_H
#pragma once
#include <vector>
#include <math.h>
#include <iostream>

namespace Geometry
{

class Vector{
    public:
        Vector(float x,float y, float z);
        Vector();
        float scalar(Vector p);
        float length();
        bool isNull();
        Vector multiply(float alpha);
        Vector div(float divisor);
        Vector add(Vector v);
        Vector sub(Vector v);
        static Vector sub(Vector a, Vector b);

        bool operator==(const Vector& b) const;
        bool operator!=(const Vector& b) const;

    float x,y,z;
};

class Plane{
    private:
    float height;
    Vector normal;
    public:
    Plane(Vector normal);
    
    Vector getU();

    Vector getV();
    float getDistance();
    void setDistance(float distance);
    Vector getNormal();
    void setNormal(Vector normal);
    float distanceToPoint(Vector vertex);
};

class Triangle{
    public:
    Triangle();
    Triangle(Vector one, Vector two, Vector three, Vector norm);

    Triangle sub(Vector s);
    bool checkVector(Vector v);
    Vector combination(Plane p, int iCorner, int jCorner);
    std::vector<Vector> intersectPlane(Plane p);

    Vector normal;
    Vector corner[3];
};

void translateTriangulation(std::vector<Triangle>& triangulation, double dx, double dy);

// Struktur, um einen Punkt zu repräsentieren
struct Point2D {
    float x, y;
    // bool operator==(const Point2D& other) const {
    //     return x == other.x && y == other.y;
    // }
    bool operator!=(const Point2D& other) const;
};

bool operator==(const Point2D& lhs, const Point2D& rhs);

// LessThanComparable
bool operator<( Point2D a, Point2D b );

using Polygon = std::vector<Point2D>;
using Contour = std::vector<Point2D>;


class Line{
    Vector vector[2];
    Line(Vector start, Vector end);
};

class Line2D{
    public:
    Point2D start;
    Point2D end;
    Line2D(float xS, float yS,float xE, float yE);
    Line2D(Point2D xS, Point2D yS);
};

// Typdefinition für ein 2D-Array (Pixel-Array)
using PixelArray = std::vector<std::vector<uint8_t>>;

// Funktion, um eine Linie in das Pixel-Array zu zeichnen
void drawLine(PixelArray &array, Point2D p2, Point2D p1, int color);

// Funktion, um eine Linie in das Pixel-Array zu zeichnen
void drawLine(PixelArray &array, int x0, int y0, int x1, int y1, int color);

// Funktion, um das Pixel-Array auszugeben
void printArray(const PixelArray &array);

void translatePolygon(std::vector<Triangle>& triangulation, double dx, double dy);

//scanLine algo chatgpt
// Funktion, um das Polygon zu füllen
void fillPolygonScanLine(PixelArray &array, const std::vector<Point2D> &vertices, int color);

void* convertMatrixToImage(const std::vector<std::vector<uint8_t>>& matrix, int w, int h);

}

#endif // MYCLASS_H