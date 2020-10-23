#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

struct vec3
{
    float x, y, z;
    vec3(float x, float y, float z):
        x(x), y(y), z(z)
    {}

    vec3():
        x(0.0f), y(0.0f), z(0.0f)
    {}
};

vec3 operator+(vec3 a, vec3 b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 operator-(vec3 a, vec3 b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 operator*(float s, vec3 a)
{
    return {a.x * s, a.y * s, a.z * s};
}

vec3 operator*(vec3 a, float s)
{
    return {a.x * s, a.y * s, a.z * s};
}

template <typename T>
T max(T x, T y)
{
    return x < y ? y : x;
}

template <typename T>
T min(T x, T y)
{
    return x > y ? y : x;
}

float length(vec3 a)
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3 normalize(vec3 a)
{
    const float l = length(a);
    return vec3(a.x / l, a.y / l, a.z / l);
}

const int MAXIMUM_RAY_STEPS = 69 * 2;
const float MINIMUM_DISTANCE = 5.0f;

const float EYE_DISTANCE = 50.0f;

const int PIXEL_COLUMNS = 800;
const int PIXEL_ROWS = 600;
const float PIXEL_WIDTH = 1.0f;
const float PIXEL_HEIGHT = 1.0f;

const float SCREEN_WIDTH = (float) PIXEL_COLUMNS * PIXEL_WIDTH;
const float SCREEN_HEIGHT = (float) PIXEL_ROWS * PIXEL_HEIGHT;

const float R = (float) PIXEL_COLUMNS;
const vec3 C = {0.0f, 0.0f, EYE_DISTANCE + R};

float DE(vec3 z)
{
    vec3 a1 = vec3(1,1,1);
    vec3 a2 = vec3(-1,-1,1);
    vec3 a3 = vec3(1,-1,-1);
    vec3 a4 = vec3(-1,1,-1);
    vec3 c;
    int n = 0;
    float dist, d;
    const int Iterations = 3;
    const float Scale = 100.0f;
    while (n < Iterations) {
        c = a1; dist = length(z-a1);
        d = length(z-a2); if (d < dist) { c = a2; dist=d; }
        d = length(z-a3); if (d < dist) { c = a3; dist=d; }
        d = length(z-a4); if (d < dist) { c = a4; dist=d; }
        z = Scale*z-c*(Scale-1.0f);
        n++;
    }

    return length(z) * pow(Scale, (float)(-n));
}

float solid_sphere(vec3 p)
{
    return length(p - C) - R;
}

float hollow_sphere(vec3 p)
{
    return fabsf(length(p) - R);
}

float combined_spheres(vec3 p)
{
    return min(length(p) - 1.0f, length(p - vec3(2.0f, 0.0f, 0.0f)) - 1.0f);
}

template <typename DistanceEstimator>
float trace(vec3 from, vec3 direction, DistanceEstimator distance_estimator)
{
    float totalDistance = 0.0f;

    int steps;
    for (steps = 0; steps < MAXIMUM_RAY_STEPS; ++steps) {
        vec3 p = from + totalDistance * direction;
        float distance = distance_estimator(p);
        totalDistance += distance;
        if (distance < MINIMUM_DISTANCE) break;
    }

    // return 1.0f - (float)(steps) / (float)(MAXIMUM_RAY_STEPS);
    return 1.0f - fminf((float)(steps) / 20.0f, 1.0f);
}

struct RGBA
{
    uint8_t r, g, b, a;
};

const RGBA RED = {255, 0, 0, 255};

RGBA canvas[PIXEL_COLUMNS * PIXEL_ROWS] = {};

template <typename DistanceEstimator>
void render_to_file(const char *filepath, DistanceEstimator distance_estimator)
{
    for (int row = 0; row < PIXEL_ROWS; ++row) {
        for (int column = 0; column < PIXEL_COLUMNS; ++column) {
            float x = column * PIXEL_WIDTH  - SCREEN_WIDTH  * 0.5f + PIXEL_WIDTH  * 0.5f;
            float y = row    * PIXEL_HEIGHT - SCREEN_HEIGHT * 0.5f + PIXEL_HEIGHT * 0.5f;
            float z = EYE_DISTANCE;
            float t = trace(vec3(x, y, z), normalize(vec3(x, y, z)), distance_estimator);
            canvas[row * PIXEL_COLUMNS + column] = {
                (uint8_t) floorf(255.0f * t),
                (uint8_t) floorf(255.0f * t),
                (uint8_t) floorf(255.0f * t),
                255
            };
        }
    }

    int ret = stbi_write_png(filepath, PIXEL_COLUMNS, PIXEL_ROWS, 4, canvas, sizeof(RGBA) * PIXEL_COLUMNS);
    if (ret == 0) {
        fprintf(stderr, "Could not save to file `%s`\n", filepath);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    render_to_file("output.png", [] (vec3 p) {
        return fmin(
            length(p - vec3(-10.0f * 0, 0.0f, EYE_DISTANCE + R)) - R,
            length(p - vec3( 10.0f * 0, 0.0f, EYE_DISTANCE + R)) - R);
    });

#if 0
    for (int i = 0; i < 20; ++i) {
        char filepath[256];
        snprintf(filepath, sizeof(filepath), "out/output-%02d.png", i);
        printf("Rendering %s\n", filepath);
        render_to_file(filepath, [&i] (vec3 p) {
            return fmin(
                length(p - vec3(-10.0f * i, 0.0f, EYE_DISTANCE + R)) - R,
                length(p - vec3( 10.0f * i, 0.0f, EYE_DISTANCE + R)) - R);
        });
    }
#endif

    return 0;
}
