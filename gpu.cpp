#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <math.h>
#include "aids.hpp"

using namespace aids;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const char *vertex_shader_source =
    "#version 130\n"
    "void main(void)\n"
    "{\n"
    "    int gray = gl_VertexID ^ (gl_VertexID >> 1);\n"
    "    gl_Position = vec4(\n"
    "        2 * (gray / 2) - 1,\n"
    "        2 * (gray % 2) - 1,\n"
    "        0.0,\n"
    "        1.0);\n"
    "}\n";
const char *fragment_shader_source =
    "#version 130\n"
    "out vec4 color;\n"
    "uniform float spread;\n"
    "\n"
    "#define MAXIMUM_RAY_STEPS (69 * 2)\n"
    "#define MINIMUM_DISTANCE 5.0\n"
    "#define EYE_DISTANCE 50.0\n"
    "#define PIXEL_COLUMNS 800\n"
    "#define PIXEL_ROWS 600\n"
    "#define PIXEL_WIDTH 1.0\n"
    "#define PIXEL_HEIGHT 1.0\n"
    "#define SCREEN_WIDTH (float(PIXEL_COLUMNS) * PIXEL_WIDTH)\n"
    "#define SCREEN_HEIGHT (float(PIXEL_ROWS) * PIXEL_HEIGHT)\n"
    "#define R float(PIXEL_COLUMNS)\n"
    "\n"
    "float DE(vec3 p) {\n"
    "    return min(\n"
    "        length(p - vec3(-spread, 0.0, EYE_DISTANCE + R)) - R,\n"
    "        length(p - vec3( spread, 0.0, EYE_DISTANCE + R)) - R);\n"
    "}\n"
    "\n"
    "float trace(vec3 from, vec3 direction)\n"
    "{\n"
    "    float totalDistance = 0.0;\n"
    "\n"
    "    int steps;\n"
    "    for (steps = 0; steps < MAXIMUM_RAY_STEPS; ++steps) {\n"
    "        vec3 p = from + totalDistance * direction;\n"
    "        float distance = DE(p);\n"
    "        totalDistance += distance;\n"
    "        if (distance < MINIMUM_DISTANCE) break;\n"
    "    }\n"
    "\n"
    "    return 1.0 - min(float(steps) / 20.0, 1.0);\n"
    "}\n"
    "\n"
    "void main(void) {\n"
    "    float x = gl_FragCoord.x * PIXEL_WIDTH  - SCREEN_WIDTH  * 0.5f + PIXEL_WIDTH  * 0.5f;\n"
    "    float y = gl_FragCoord.y * PIXEL_HEIGHT - SCREEN_HEIGHT * 0.5f + PIXEL_HEIGHT * 0.5f;\n"
    "    float z = EYE_DISTANCE;\n"
    "    float t = trace(vec3(x, y, z), normalize(vec3(x, y, z)));\n"
    "    color = vec4(t, t, t, 1.0);\n"
    "}\n";

struct Shader
{
    GLuint unwrap;
};

Shader compile_shader(const char *source_code, GLenum shader_type)
{
    GLuint shader = {};
    shader = glCreateShader(shader_type);
    if (shader == 0) {
        println(stderr, "Could not create a shader");
        exit(1);
    }

    glShaderSource(shader, 1, &source_code, 0);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar buffer[1024];
        int length = 0;
        glGetShaderInfoLog(shader, sizeof(buffer), &length, buffer);
        println(stderr, "Could not compile shader: ", buffer);
        exit(1);
    }

    return {shader};
}

struct Program
{
    GLuint unwrap;
};

Program link_program(Shader vertex_shader, Shader fragment_shader)
{
    GLuint program = glCreateProgram();

    if (program == 0) {
        println(stderr, "Could not create shader program");
        exit(1);
    }

    glAttachShader(program, vertex_shader.unwrap);
    glAttachShader(program, fragment_shader.unwrap);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar buffer[1024];
        int length = 0;
        glGetProgramInfoLog(program, sizeof(buffer), &length, buffer);
        println(stdout, "Could not link the program: ", buffer);
        exit(1);
    }

    return {program};
}

int main(int argc, char *argv[])
{
    if (!glfwInit()) {
        println(stderr, "Could not initialize GLFW");
        exit(1);
    }
    defer(glfwTerminate());

    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vodus", NULL, NULL);
    if (!window) {
        println(stderr, "Could not create window");
        exit(1);
    }

    glfwMakeContextCurrent(window);

    println(stdout, "Compiling vertex shader...");
    Shader vertex_shader = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    println(stdout, "Compiling fragment shader...");
    Shader fragment_shader = compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
    println(stdout, "Linking the program...");
    Program program = link_program(vertex_shader, fragment_shader);

    glUseProgram(program.unwrap);

    GLint spreadLocation = glGetUniformLocation(program.unwrap, "spread");
    float a = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        a = fmodf(a + 0.05f, 2 * M_PI);
        glUniform1f(spreadLocation, ((sin(a) + 1.0f) / 2.0f) * 200.0f);
        glDrawArrays(GL_QUADS, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
