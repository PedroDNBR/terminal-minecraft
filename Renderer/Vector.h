#include <cmath>

#pragma once
struct Vector3
{
    float x = 0, y = 0, z = 0;

    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    Vector3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    float& operator[](int i)
    {
        return (&x)[i];
    }

    const float& operator[](int i) const
    {
        return (&x)[i];
    }

    static Vector3 normalize(const Vector3& v) {
        {
            float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            return { v.x / len, v.y / len, v.z / len };
        }
    }
};