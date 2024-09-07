#include "Geometry.h"

namespace Geometry
{

    Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) 
    {
    };

    Vector::Vector() : x(0), y(0), z(0) 
    {
    };

    float Vector::scalar(Vector p)
    {
        return this->x * p.x + this->y * p.y + this->z * p.z;
    }

    float Vector::length()
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    bool Vector::isNull()
    {
        if (x == 0 && y == 0 && z == 0)
        {
            return true;
        }
        return false;
    }
    Vector Vector::multiply(float alpha)
    {
        return Vector(this->x * alpha, this->y * alpha, this->z * alpha);
    }
    Vector Vector::div(float divisor)
    {
        if (divisor == 0)
        {
            throw std::out_of_range("No 0 division");
        }
        return Vector(this->x / divisor, this->y / divisor, this->z / divisor);
    }
    Vector Vector::add(Vector v)
    {
        return Vector(this->x + v.x, this->y + v.y, this->z + v.z);
    }
    Vector Vector::sub(Vector v)
    {
        return Vector(this->x - v.x, this->y - v.y, this->z * v.z);
    }
    Vector Vector::sub(Vector a, Vector b)
    {
        return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    bool Vector::operator==(const Vector &b) const
    {
        if (b.x == this->x &&
            b.y == this->y &&
            b.z == this->z)
        {
            return true;
        }
        return false;
    };

    bool Vector::operator!=(const Vector &b) const
    {
        if (b.x != this->x ||
            b.y != this->y ||
            b.z != this->z)
        {
            return true;
        }
        return false;
    };

    Plane::Plane(Vector normal) : height(0), normal(normal) {};

    Vector Plane::getU()
    {
        Vector u(-normal.y, normal.x, 0);
        if (u.isNull())
        {
            u = Vector(-normal.z, 0, normal.x);
        }
        return u;
    };

    Vector Plane::getV()
    {
        Vector u(0, -normal.z, normal.y);
        if (u.isNull())
        {
            u = Vector(-normal.z, 0, normal.x);
        }
        return u;
    }
    float Plane::getDistance()
    {
        return height;
    }
    void Plane::setDistance(float distance)
    {
        this->height = distance;
    }
    Vector Plane::getNormal()
    {
        return normal;
    }
    void Plane::setNormal(Vector normal)
    {
        this->normal = normal;
    }
    float Plane::distanceToPoint(Vector vertex)
    {
        return vertex.scalar(normal) - height;
    }

    Triangle::Triangle() {};
    Triangle::Triangle(Vector one, Vector two, Vector three, Vector norm) : normal(norm)
    {
        this->corner[0] = one;
        this->corner[1] = two;
        this->corner[2] = three;
    };

    Triangle Triangle::sub(Vector s)
    {
        return Triangle(corner[0].sub(s), corner[1].sub(s), corner[2].sub(s), this->normal);
    }
    bool Triangle::checkVector(Vector v)
    {
        if (v.z >= 0 && v.z <= 1)
        {
            return true;
        }
        return false;
    }
    Vector Triangle::combination(Plane p, int iCorner, int jCorner)
    {
        if (iCorner > 2)
        {
            throw std::invalid_argument("i,j > 2");
        }
        if (jCorner > 2)
        {
            throw std::invalid_argument("i,j > 2");
        }
        float distanceI = p.distanceToPoint(corner[iCorner]);
        float distanceJ = p.distanceToPoint(corner[jCorner]);
        if (distanceI * distanceJ < 0)
        {
            if (distanceI > 0)
            {
                float factor = distanceI / (distanceI - distanceJ);
                Vector edge = Vector::sub(corner[jCorner], corner[iCorner]);
                return corner[iCorner].add(edge.multiply(factor));
            }
            else
            {
                float factor = distanceJ / (distanceJ - distanceI);
                Vector edge = Vector::sub(corner[iCorner], corner[jCorner]);
                return corner[jCorner].add(edge.multiply(factor));
            }
        }
        else if (distanceI == 0)
        {
            return corner[iCorner];
        }
        else if (distanceJ == 0)
        {
            return corner[jCorner];
        }
        return Vector();
    }
    std::vector<Vector> Triangle::intersectPlane(Plane p)
    {
        std::vector<Vector> result;
        Vector tmp[3];
        // if(combination(p,0,1) != combination(p,1,0)){
        //     tmp[0] = combination(p,0,1);
        //     tmp[1] = combination(p,1,0);
        //     tmp[2] = combination(p,2,1);
        // } else{
        tmp[0] = combination(p, 0, 1);
        tmp[1] = combination(p, 0, 2);
        tmp[2] = combination(p, 1, 2);
        //}

        for (int i = 0; i < 3; i++)
        {
            if (!tmp[i].isNull())
            {
                result.push_back(tmp[i]);
            };
        }
        return result;
    }

    void translateTriangulation(std::vector<Triangle> &triangulation, double dx, double dy)
    {
        for (auto &triangle : triangulation)
        {
            triangle.corner[0].x += dx;
            triangle.corner[0].y += dy;
            triangle.corner[1].x += dx;
            triangle.corner[1].y += dy;
            triangle.corner[2].x += dx;
            triangle.corner[2].y += dy;
        }
    }

    bool Point2D::operator!=(const Point2D &other) const
    {
        return x != other.x || y != other.y;
    }

    bool operator==(const Point2D &lhs, const Point2D &rhs)
    {
        bool test = std::fabs(lhs.x - rhs.x) < 1e-4 && std::fabs(lhs.y - rhs.y) < 1e-4;
        return test;
    }

    bool operator<(Point2D a, Point2D b) { return std::make_pair(a.x, a.y) < std::make_pair(b.x, b.y); }

    Line::Line(Vector start, Vector end)
    {
        this->vector[0] = start;
        this->vector[1] = end;
    }

    Line2D::Line2D(float xS, float yS, float xE, float yE)
    {
        this->start.x = xS;
        this->start.y = yS;
        this->end.x = xE;
        this->end.y = yE;
    }
    Line2D::Line2D(Point2D xS, Point2D yS)
    {
        this->start = xS;
        this->end = yS;
    }

    // Funktion, um eine Linie in das Pixel-Array zu zeichnen
    void drawLine(PixelArray &array, Point2D p2, Point2D p1, int color)
    {
        int dx = abs(p2.x - p1.x);
        int dy = abs(p2.y - p1.y);
        int sx = (p1.x < p2.x) ? 1 : -1;
        int sy = (p1.y < p2.y) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            if ((int)p1.x >= 0 && (int)p1.x < (int)array.size() && (int)p1.y >= 0 && (int)p1.y < (int)array[0].size())
            {
                array[static_cast<int>(p1.y)][static_cast<int>(p1.x)] = color;
            }

            if (p1.x == p2.x && p1.y == p2.y)
                break;

            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                p1.x += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                p1.y += sy;
            }
        }
    }

    void drawLine(PixelArray &array, int x0, int y0, int x1, int y1, int color)
    {

        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            if (x0 >= 0 && x0 < (int)array.size() && y0 >= 0 && y0 < (int)array[0].size())
            {
                array[x0][y0] = color;
            }

            if (x0 == x1 && y0 == y1)
                break;

            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    // Funktion, um das Pixel-Array auszugeben
    void printArray(const PixelArray &array)
    {
        for (const auto &row : array)
        {
            for (const auto &pixel : row)
            {
                std::cout << (pixel == 255 ? "#" : ".") << " ";
            }
            std::cout << std::endl;
        }
    }

    void translatePolygon(std::vector<Triangle> &triangulation, double dx, double dy)
    {
        for (auto &triangle : triangulation)
        {
            triangle.corner[0].x += dx;
            triangle.corner[0].y += dy;
            triangle.corner[1].x += dx;
            triangle.corner[1].y += dy;
            triangle.corner[2].x += dx;
            triangle.corner[2].y += dy;
        }
    }

    // scanLine algo chatgpt
    //  Funktion, um das Polygon zu füllen
    void fillPolygonScanLine(PixelArray &array, const std::vector<Point2D> &vertices, int color)
    {
        int minY = array.size(), maxY = 0;
        for (const auto &vertex : vertices)
        {
            if ((int)vertex.y < minY)
                minY = (int)vertex.y;
            if ((int)vertex.y > maxY)
                maxY = (int)vertex.y;
        }

        for (int y = minY; y <= maxY; ++y)
        {
            std::vector<int> nodes;
            size_t j = vertices.size() - 1;
            for (size_t i = 0; i < vertices.size(); ++i)
            {
                if (((int)vertices[i].y < y && (int)vertices[j].y >= y) || ((int)vertices[j].y < y && (int)vertices[i].y >= y))
                {
                    nodes.push_back((int)vertices[i].x + (y - (int)vertices[i].y) * ((int)vertices[j].x - (int)vertices[i].x) / ((int)vertices[j].y - (int)vertices[i].y));
                }
                j = i;
            }
            std::sort(nodes.begin(), nodes.end());

            for (size_t i = 0; i < nodes.size(); i += 2)
            {
                if (nodes[i] >= array[0].size())
                    break;
                if (nodes[i + 1] > 0)
                {
                    if (nodes[i] < 0)
                        nodes[i] = 0;
                    if (nodes[i + 1] >= array[0].size())
                        nodes[i + 1] = array[0].size() - 1;
                    for (int x = nodes[i]; x <= nodes[i + 1]; ++x)
                    {
                        array[y][x] = color;
                    }
                }
            }
        }
    }

    void *convertMatrixToImage(const std::vector<std::vector<uint8_t>> &matrix, int w, int h)
    {
        if (matrix.size() != h)
        {
            throw std::runtime_error("Invalid matrix dimensions");
        }

        size_t imageSize = w * 3 * h;
        unsigned char *image = new unsigned char[imageSize];

        for (int i = 0; i < h; ++i)
        {
            for (int j = 0; j < w; j++)
            {
                image[i * w * 3 + j * 3] = matrix[i][j];
                image[i * w * 3 + j * 3 + 1] = matrix[i][j];
                image[i * w * 3 + j * 3 + 2] = matrix[i][j];
            }
        }

        return static_cast<void *>(image);
    }
}