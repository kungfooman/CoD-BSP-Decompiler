#ifndef MATH_H
#define MATH_H

#define EPSILON             0.000025
#define LEAST_VALUE_EPSILON 0.000100

#include <cmath>

inline
bool equal(double value1, double value2)
{
    return ((value1 == value2) || EPSILON > fabs(value1 - value2));
}

inline
double difference(double value1, double value2)
{
    return fabs(value1 - value2);
}

bool within(double min, double value, double max);

struct Vector
{
    double x, y, z;

    Vector()
    {
    }

    Vector(double x, double y, double z) :
        x(x),
        y(y),
        z(z)
    {
    }

    void operator() (double ix = 0.0, double iy = 0.0, double iz = 0.0)
    {
        x = ix;
        y = iy;
        z = iz;
    }

    double length() const
    {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    operator bool () const
    {
        if (equal(x, 0.0) && equal(y, 0.0) && equal(z, 0.0))
        {
            return false;
        }
        return true;
    }

    bool operator! () const
    {
        if (equal(x, 0.0) && equal(y, 0.0) && equal(z, 0.0))
        {
            return true;
        }
        return false;
    }

    bool operator== (const Vector &vector) const
    {
        if (equal(x, vector.x) && equal(y, vector.y) && equal(z, vector.z))
        {
            return true;
        }
        return false;
    }

    bool operator!= (const Vector &vector) const
    {
        if (equal(x, vector.x) && equal(y, vector.y) && equal(z, vector.z))
        {
            return false;
        }
        return true;
    }

    Vector operator+ () const
    {
        return Vector(x, y, z);
    }

    Vector operator+ (const Vector &vector) const
    {
        return Vector(x + vector.x, y + vector.y, z + vector.z);
    }

    void operator+= (const Vector &vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
    }

    Vector operator- () const
    {
        return Vector(-x, -y, -z);
    }

    Vector operator- (const Vector &vector) const
    {
        return Vector(x - vector.x, y - vector.y, z - vector.z);
    }

    void operator-= (const Vector &vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
    }

    Vector operator* (double scalar) const
    {
        return Vector(x * scalar, y * scalar, z * scalar);
    }

    Vector operator* (const Vector &vector) const
    {
        return Vector(x * vector.x, y * vector.y, z * vector.z);
    }

    void operator*= (double scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    void operator*= (const Vector &vector)
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
    }

    Vector operator/ (double scalar) const
    {
        return Vector(x / scalar, y / scalar, z / scalar);
    }

    Vector operator/ (const Vector &vector) const
    {
        return Vector(x / vector.x, y / vector.y, z / vector.z);
    }

    void operator/= (double scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    void operator/= (const Vector &vector)
    {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;
    }
};

typedef Vector Normal;
typedef Vector Position;
typedef Vector Vertex;

Normal triangle_normal(const Position &position1, const Position &position2, const Position &position3);

struct Triangle
{
    unsigned int id[3];

    Triangle(unsigned int id1, unsigned int id2, unsigned int id3)
    {
        id[0] = id1;
        id[1] = id2;
        id[2] = id3;
    }

    bool share_diagonal(Triangle &triangle, unsigned int *vertices)
    {
        unsigned int shared_vertices = 0;
        bool had_first_shared = false;

        unsigned int i = 0, j = 0;
        for (i = 0; i < 3; i++)
        {
            bool shared = false;
            for (j = 0; j < 3; j++)
            {
                if (id[i] == triangle.id[j])
                {
                    shared_vertices++;

                    if (!had_first_shared)
                    {
                        vertices[1] = id[i];
                        had_first_shared = true;
                    }
                    else
                    {
                        vertices[2] = id[i];
                    }
                    shared = true;
                    break;
                }
            }

            if (!shared)
            {
                vertices[0] = id[i];
            }
        }

        for (i = 0; i < 3; i++)
        {
            bool shared = false;
            for (j = 0; j < 3; j++)
            {
                if (triangle.id[i] == id[j])
                {
                    shared = true;
                    break;
                }
            }

            if (!shared)
            {
                vertices[3] = triangle.id[i];
                break;
            }
        }

        if (shared_vertices == 2)
        {
            return true;
        }
        return false;
    }
};

#endif // MATH_H
