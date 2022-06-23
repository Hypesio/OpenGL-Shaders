#pragma once

#include <GL/glew.h>
#include "program.hh"
#include "matrix.hh"
#include "camera.hh"

#define NB_SHADERS 2

typedef bool (*shader_func)(program *, Camera*);

bool init_dunes_shader(program *program, Camera* camera);
bool init_skybox_shader(program *program, Camera* camera);
bool init_water_shader(program *program, Camera* camera);

static std::string shader_paths[] = {
    "shaders/vertex.vert",
    "shaders/fragment.frag",
    "shaders/skybox_vertex.vert", 
    "shaders/skybox_fragment.frag",
    "shaders/water_vertex.vert",
    "shaders/water_fragment.frag"
};

static shader_func shader_array[] = {
    init_dunes_shader,
    init_skybox_shader,
    init_water_shader
};
