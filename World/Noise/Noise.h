#pragma once
#include <cmath>

inline float valueNoise(int x, int z, unsigned seed)
{
    unsigned n = (unsigned)(x * 1619 + z * 31337) ^ (seed * 1013904223u);
    n = (n >> 8) ^ n;
    n *= 2246822519u;
    n ^= (n >> 11);
    n += (n << 6);
    n ^= (n >> 22);
    return (float)(n & 0xFFFF) / 65535.0f;
}

inline float smoothstep(float t) { return t * t * (3.0f - 2.0f * t); }

inline float smoothstep(float x, float edge0, float edge1)
{
    float t = (x - edge0) / (edge1 - edge0);
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return t * t * (3.0f - 2.0f * t);
}

inline float smoothNoise(float x, float z, unsigned seed)
{
    int   xi = (int)floorf(x);
    int   zi = (int)floorf(z);
    float fx = x - xi;
    float fz = z - zi;
    float ux = smoothstep(fx);
    float uz = smoothstep(fz);

    float v00 = valueNoise(xi, zi, seed);
    float v10 = valueNoise(xi + 1, zi, seed);
    float v01 = valueNoise(xi, zi + 1, seed);
    float v11 = valueNoise(xi + 1, zi + 1, seed);

    return v00 * (1 - ux) * (1 - uz) + v10 * ux * (1 - uz)
        + v01 * (1 - ux) * uz + v11 * ux * uz;
}

inline float fractalNoise(float x, float z, unsigned seed, int octaves = 2)
{
    float value = 0.0f;
    float amp = 1.0f;
    float freq = 1.0f;
    float maxVal = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        value += smoothNoise(x * freq, z * freq, seed + i * 997u) * amp;
        maxVal += amp;
        amp *= 0.5f;
        freq *= 2.0f;
    }

    return value / maxVal;
}

inline float valueNoise3D(int x, int y, int z, unsigned seed)
{
    unsigned n = (unsigned)(x * 1619 + y * 31337 + z * 6791)
        ^ (seed * 1013904223u);
    n = (n >> 8) ^ n;
    n *= 2246822519u;
    n ^= (n >> 11);
    n += (n << 6);
    n ^= (n >> 22);
    return (float)(n & 0xFFFF) / 65535.0f;
}
