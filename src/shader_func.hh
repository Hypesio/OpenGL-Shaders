#pragma once

#include <GL/glew.h>

#include "matrix.hh"

#define NB_SHADERS 2

typedef bool (*shader_func)(GLuint);

bool init_dunes_shader(GLuint program_id);

static shader_func shader_array[] = {
    init_dunes_shader
};
