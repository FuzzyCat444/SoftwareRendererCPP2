#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include <initializer_list>
#include <vector>

inline double radians(double degrees)
{
    return degrees * 3.141592654 / 180.0;
}

inline double getT(double a, double b, double c)
{
    double den = b - a;
    if (den == 0.0)
        return 0.0;
    return (c - a) / den;
}

struct Vector2
{
    Vector2()
    : Vector2{ 0.0, 0.0 }
    {
    }

    Vector2(double x, double y)
        : x{ x }, y{ y }
    {
    }

    void add(Vector2 other)
    {
        x += other.x;
        y += other.y;
    }

    void sub(Vector2 other)
    {
        x -= other.x;
        y -= other.y;
    }

    void mul(Vector2 other)
    {
        x *= other.x;
        y *= other.y;
    }

    void div(Vector2 other)
    {
        x /= other.x;
        y /= other.y;
    }

    void scl(double s)
    {
        x *= s;
        y *= s;
    }

    void norm()
    {
        double length = len();
        x /= length;
        y /= length;
    }

    double len() const
    {
        return sqrt(len2());
    }

    double len2() const
    {
        return dot(*this);
    }

    double dot(Vector2 other) const
    {
        return x * other.x + y * other.y;
    }

    double x, y;
};

struct Vector3
{
    Vector3()
    : Vector3{ 0.0, 0.0, 0.0 }
    {
    }

    Vector3(double x, double y, double z)
        : x{ x }, y{ y }, z{ z }
    {
    }

    void add(Vector3 other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    void sub(Vector3 other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    void mul(Vector3 other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }

    void div(Vector3 other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
    }

    void scl(double s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    void norm()
    {
        double length = len();
        x /= length;
        y /= length;
        z /= length;
    }

    double len() const
    {
        return sqrt(len2());
    }

    double len2() const
    {
        return dot(*this);
    }

    double dot(Vector3 other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross(Vector3 other) const
    {
        return Vector3
        (
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    double x, y, z;
};

class Transform
{
public:
    virtual Vector3 apply(Vector3 v) const = 0;
    virtual Vector3 applyNormal(Vector3 n) const
    {
        return n;
    }
};

class Combined : public Transform
{
public:
    Combined()
    {
    }

    Combined(std::initializer_list<const Transform*> chain)
        : chain{ chain }
    {
    }

    Vector3 apply(Vector3 v) const override
    {
        for (int i = 0; i < chain.size(); i++)
            v = chain[i]->apply(v);
        return v;
    }

    Vector3 applyNormal(Vector3 n) const override
    {
        for (int i = 0; i < chain.size(); i++)
            n = chain[i]->applyNormal(n);
        return n;
    }
private:
    std::vector<const Transform*> chain;
};

class Translate : public Transform
{
public:
    Translate()
    : Translate{ { 0.0, 0.0, 0.0 } }
    {
    }

    Translate(Vector3 translation)
        : translation{ translation }
    {
    }

    Vector3 apply(Vector3 v) const override
    {
        v.add(translation);
        return v;
    }

    Vector3 applyNormal(Vector3 n) const override
    {
        return n;
    }
private:
    Vector3 translation;
};

class Scale : public Transform
{
public:
    Scale()
    : Scale{ { 1.0, 1.0, 1.0 } }
    {
    }

    Scale(Vector3 scalars)
        : scalars{ scalars },
          normalScalars{ scalars.y * scalars.z, scalars.x * scalars.z, scalars.x * scalars.y }
    {
    }

    Vector3 apply(Vector3 v) const override
    {
        v.mul(scalars);
        return v;
    }

    Vector3 applyNormal(Vector3 n) const override
    {
        n.mul(normalScalars);
        return n;
    }
private:
    Vector3 scalars;
    Vector3 normalScalars;
};

class Rotate : public Transform
{
public:
    enum class Axis
    {
        X, Y, Z
    };
    Rotate()
    : Rotate{ Axis::X, 0.0 }
    {
    }

    Rotate(Axis axis, double rotation)
        : axis{ axis },
        s{ sin(rotation) }, c{ cos(rotation) }
    {
    }

    Vector3 apply(Vector3 v) const override
    {
        Vector3 rotated = v;
        switch (axis)
        {
        case Axis::X:
            // Rotate Y to Z
            rotated.y = v.y * c - v.z * s;
            rotated.z = v.y * s + v.z * c;
            break;
        case Axis::Y:
            // Rotate Z to X
            rotated.z = v.z * c - v.x * s;
            rotated.x = v.z * s + v.x * c;
            break;
        case Axis::Z:
            // Rotate X to Y
            rotated.x = v.x * c - v.y * s;
            rotated.y = v.x * s + v.y * c;
            break;
        }
        return rotated;
    }

    Vector3 applyNormal(Vector3 n) const override
    {
        return apply(n);
    }
private:
    Axis axis;
    double s;
    double c;
};

#endif
