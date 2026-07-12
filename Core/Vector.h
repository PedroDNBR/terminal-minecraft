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
            if (len == 0.0f)
                return { 0, 0, 0 };
            return { v.x / len, v.y / len, v.z / len };
        }
    }
};

struct Vector3Int
{
    int x = 0, y = 0, z = 0;

    bool operator==(const Vector3Int& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    Vector3Int operator+(const Vector3Int& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    Vector3Int operator-(const Vector3Int& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    Vector3Int operator*(int scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    Vector3Int& operator+=(const Vector3Int& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3Int& operator-=(const Vector3Int& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
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
};

struct Vector2
{
    float x = 0, y = 0;

    bool operator==(const Vector2& other) const
    {
        return x == other.x && y == other.y;
    }

    Vector2 operator+(const Vector2& other) const {
        return { x + other.x, y + other.y };
    }

    Vector2 operator-(const Vector2& other) const {
        return { x - other.x, y - other.y };
    }

    Vector2 operator*(float scalar) const {
        return { x * scalar, y * scalar };
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
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

    static Vector2 normalize(const Vector2& v) {
        {

            float len = sqrt(v.x * v.x + v.y * v.y);
            if (len == 0.0f)
                return { 0, 0 };
            return { v.x / len, v.y / len };
        }
    }
};