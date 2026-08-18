#pragma once
#include "../Core/Vector.h"
#include "../Terminal/Colors.h"
#include "../World/Cube.h"
#include "Noise/Noise.h"

constexpr int FACE_SHAPE[6] = {
    -2,
    -2,
    -1,
    -1,
    0,
    -3
};

static constexpr float MAX_SUN_HEIGHT = 0.45f;
static constexpr int DAY_LIGHT = SHADE_LEVELS - 1;
static constexpr int NIGHT_LIGHT = 2;

static constexpr uint8_t SKYLIGHT_FALLOFF = 5;

struct Lighting
{
	int globalLight;
    int faceShading[6];
};

static Lighting computeLighting(float timeOfDay)
{
    Lighting lighting{};

    float sunAngle = timeOfDay * 6.2831853f - 1.5707963f;
    float sunX = cosf(sunAngle);
    float sunY = sinf(sunAngle);

    float dayAmount = smoothstep(sunY, -0.2f, 0.2f);

    if (sunY > MAX_SUN_HEIGHT) sunY = MAX_SUN_HEIGHT;

    if (sunY < 0.0f) sunY = 0.0f;

    Vector3 sunDirection = { sunX, sunY, 0.0f };

    lighting.globalLight = NIGHT_LIGHT + (int)roundf(dayAmount * (DAY_LIGHT - NIGHT_LIGHT));

    for (int f = 0; f < 6; f++)
    {
        Vector3Int d = cubeFacesDirections[f];
        float facing = d.x * sunDirection.x + d.y * sunDirection.y + d.z * sunDirection.z;
        lighting.faceShading[f] = (int)roundf(fmaxf(0.0f, facing) * dayAmount);
    }
    return lighting;
}