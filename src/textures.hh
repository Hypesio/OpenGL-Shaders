#pragma once 

#include <iostream>
#include <vector>
#include <GL/glew.h>


unsigned int loadSkybox();
unsigned int loadCubemap(std::vector<std::string> faces);
unsigned int generate_render_texture(GLuint &frame_buffer_number, int width, int height);