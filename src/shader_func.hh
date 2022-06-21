#pragma once

#include <GL/glew.h>
#include "program.hh"
#include "matrix.hh"

#define NB_SHADERS 2

typedef bool (*shader_func)(program *, glm::mat4);

bool init_dunes_shader(program *program, glm::mat4 view);
bool init_skybox_shader(program *program, glm::mat4 view);

static std::string shader_paths[] = {
    "shaders/vertex.vert",
    "shaders/fragment.frag",
    "shaders/skybox_vertex.vert", 
    "shaders/skybox_fragment.frag"
};

static shader_func shader_array[] = {
    init_dunes_shader,
    init_skybox_shader
};
