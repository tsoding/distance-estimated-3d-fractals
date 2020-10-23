#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

struct vec3
{
    float x, y, z;
};

vec3 operator+(vec3 a, vec3 b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 operator*(float s, vec3 a)
{
    return {a.x * s, a.y * s, a.z * s};
}

template <typename T>
T max(T x, T y)
{
    return x < y ? y : x;
}

float length(vec3 a)
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

const int MAXIMUM_RAY_STEPS = 69;
const float MINIMUM_DISTANCE = 5.0f;

float DistanceEstimator(vec3 p)
{
    const float R = 200.0f;
    return max(0.0f, length(p) - R);
}

float trace(vec3 from, vec3 direction)
{
    float totalDistance = 0.0f;

    int steps;
    for (steps = 0; steps < MAXIMUM_RAY_STEPS; ++steps) {
        vec3 p = from + totalDistance * direction;
        float distance = DistanceEstimator(p);
        totalDistance += distance;
        if (distance < MINIMUM_DISTANCE) break;
    }

    return 1.0f - (float)(steps) / (float)(MAXIMUM_RAY_STEPS);
}

const int PIXEL_COLUMNS = 800;
const int PIXEL_ROWS = 600;
const float PIXEL_WIDTH = 1.0f;
const float PIXEL_HEIGHT = 1.0f;
const float SCREEN_WIDTH = (float) PIXEL_COLUMNS * PIXEL_WIDTH;
const float SCREEN_HEIGHT = (float) PIXEL_ROWS * PIXEL_HEIGHT;

const float EYE_DISTANCE = 50.0f;

struct RGBA
{
    uint8_t r, g, b, a;
};

const RGBA RED = {255, 0, 0, 255};

RGBA canvas[PIXEL_COLUMNS * PIXEL_ROWS] = {};

int main(int argc, char *argv[])
{
    for (int row = 0; row < PIXEL_ROWS; ++row) {
        for (int column = 0; column < PIXEL_COLUMNS; ++column) {
            float x = column * PIXEL_WIDTH  - SCREEN_WIDTH  * 0.5f + PIXEL_WIDTH  * 0.5f;
            float y = row    * PIXEL_HEIGHT - SCREEN_HEIGHT * 0.5f + PIXEL_HEIGHT * 0.5f;
            float z = EYE_DISTANCE;
            float t = trace({x, y, z}, {x, y, z});
            canvas[row * PIXEL_COLUMNS + column] = {
                (uint8_t) floorf(255.0f * t),
                (uint8_t) floorf(255.0f * t),
                (uint8_t) floorf(255.0f * t),
                255
            };
        }
    }

    stbi_write_png("output.png", SCREEN_WIDTH, SCREEN_HEIGHT, 4, canvas, sizeof(RGBA) * SCREEN_WIDTH);

    return 0;
}
