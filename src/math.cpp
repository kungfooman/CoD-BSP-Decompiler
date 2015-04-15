#include "math.hpp"

bool within(double min, double value, double max)
{
    if (equal(value, min) || equal(value, max) || (value > min && value < max))
    {
        return true;
    }
    return false;
}

Normal triangle_normal(const Position &position1, const Position &position2, const Position &position3)
{
    Vector vector1(position2.x - position1.x, position2.y - position1.y, position2.z - position1.z);
    Vector vector2(position3.x - position1.x, position3.y - position1.y, position3.z - position1.z);
    Vector vector3;

    vector3.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
    vector3.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
    vector3.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

    Normal normal;
    double distance = sqrt((vector3.x * vector3.x) + (vector3.y * vector3.y) + (vector3.z * vector3.z));
    if (equal(distance, 0.0))
    {
        return Normal(0, 0, 0);
    }

    normal.x = -vector3.x / distance;
    normal.y = -vector3.y / distance;
    normal.z = -vector3.z / distance;
    return normal;
}
