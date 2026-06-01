#include <cmath>
#include <functional>

#pragma once
struct Vector3
{
    float x = 0, y = 0, z = 0;

    bool operator==(const Vector3& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

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

struct Vector2Int
{
    int x = 0, y = 0;

    bool operator==(const Vector2Int& other) const
    {
        return x == other.x && y == other.y;
    }

    Vector2Int operator+(const Vector2Int& other) const {
        return { x + other.x, y + other.y };
    }

    Vector2Int operator-(const Vector2Int& other) const {
        return { x - other.x, y - other.y };
    }

    Vector2Int operator*(float scalar) const {
        return { (int)(x * scalar), (int)(y * scalar) };
    }

    Vector2Int& operator+=(const Vector2Int& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2Int& operator-=(const Vector2Int& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    int& operator[](int i)
    {
        return (&x)[i];
    }

    const int& operator[](int i) const
    {
        return (&x)[i];
    }

    static Vector2Int normalize(const Vector2Int& v) {
        {
            int len = sqrt(v.x * v.x + v.y * v.y );
            return { v.x / len, v.y / len };
        }
    }
};

struct Vector2IntHash
{
    size_t operator()(const Vector2Int& v) const
    {
        return std::hash<int>()(v.x * 73856093 ^ v.y * 19349663);
    }
};