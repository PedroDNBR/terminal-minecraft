#pragma once
struct ChunkCoord
{
    int x = 0, z = 0;

    bool operator==(const ChunkCoord& other) const
    {
        return x == other.x && z == other.z;
    }

    ChunkCoord operator+(const ChunkCoord& other) const {
        return { x + other.x, z + other.z };
    }

    ChunkCoord operator-(const ChunkCoord& other) const {
        return { x - other.x, z - other.z };
    }

    ChunkCoord operator*(int scalar) const {
        return { x * scalar, z * scalar };
    }

    ChunkCoord& operator+=(const ChunkCoord& other) {
        x += other.x;
        z += other.z;
        return *this;
    }

    ChunkCoord& operator-=(const ChunkCoord& other) {
        x -= other.x;
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

struct ChunkCoordHash
{
    size_t operator()(const ChunkCoord& v) const
    {
        return std::hash<int>()(v.x * 73856093 ^ v.z * 19349663);
    }
};