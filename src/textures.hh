#pragma once 

#include <iostream>
#include <vector>
#include <GL/glew.h>

int generate_render_texture(GLuint &frame_buffer_number, int width, int height, GLuint &rendered_texture, GLuint &depth_buffer);
unsigned int loadTexture(std::string filename, bool alpha = false);
unsigned int loadSandTexture();
int generate_depth_texture_shadow(GLuint &frame_buffer_number, int width, int height, GLuint &depth_buffer_texture);
