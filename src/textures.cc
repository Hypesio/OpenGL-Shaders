#include "textures.hh"

#include <endian.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/vector_float4.hpp>
#include <iostream>

#include "program.hh"
#include "lib/obj.hh"
#include "lib/stb_image.h"
#include "program.hh"

std::string textures_path = "textures/";

unsigned int loadTexture(std::string filename, bool alpha) {

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    std::string fullPath = textures_path;
    fullPath = fullPath.append(filename);
    void *data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cout << "Texture failed to load : " << fullPath
                    << std::endl;
        return -1;
    }

    GLuint type_col = alpha ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, type_col, width,
                        height, 0, type_col, GL_UNSIGNED_BYTE, data);

    free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return textureID;
}

int generate_render_texture(GLuint &frame_buffer_number, int width, int height, GLuint &rendered_texture, GLuint &depth_buffer_texture)
{
    // Create the frame buffer target
    glGenFramebuffers(1, &frame_buffer_number);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_number);

    // Create the render texture target
    glGenTextures(1, &rendered_texture);
    glBindTexture(GL_TEXTURE_2D, rendered_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); TEST_OPENGL_ERROR();

    // Configure the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendered_texture,
                         0);

    // Add a depth buffer
    GLuint depth_buffer;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depth_buffer); TEST_OPENGL_ERROR();

    // Create a depth texture 
    glGenTextures(1, &depth_buffer_texture);
    glBindTexture(GL_TEXTURE_2D, depth_buffer_texture); TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, 0); TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_buffer_texture,
                         0); TEST_OPENGL_ERROR();

    GLenum drawBuffs[] = {GL_COLOR_ATTACHMENT0}; TEST_OPENGL_ERROR();
    glDrawBuffers(1, drawBuffs); TEST_OPENGL_ERROR();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;

    glBindFramebuffer(GL_FRAMEBUFFER, 0); TEST_OPENGL_ERROR();

    return 0;
}

int generate_depth_texture_shadow(GLuint &frame_buffer_number, int width, int height, GLuint &depth_buffer_texture) {
    // Create the frame buffer target
    glGenFramebuffers(1, &frame_buffer_number);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_number);

    // Add a depth buffer
    GLuint depth_buffer;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depth_buffer); TEST_OPENGL_ERROR();

    // Create a depth texture 
    glGenTextures(1, &depth_buffer_texture);
    glBindTexture(GL_TEXTURE_2D, depth_buffer_texture); TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, 0); TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float clamp_color[] = { 1.0f,1.0f,1.0f,1.0f } ;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer_texture,
                         0); TEST_OPENGL_ERROR();
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);



    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;

    glBindFramebuffer(GL_FRAMEBUFFER, 0); TEST_OPENGL_ERROR();

    return 0;
}

unsigned int loadSandTexture()
{
    GLuint normalmap_id;
    glGenTextures(1, &normalmap_id);
    TEST_OPENGL_ERROR();

    glActiveTexture(GL_TEXTURE2);
    TEST_OPENGL_ERROR();

    glBindTexture(GL_TEXTURE_2D, normalmap_id);
    TEST_OPENGL_ERROR();

    int width, height, nrChannels;
    std::string fullPath = textures_path;
    fullPath = fullPath.append("sand/sand_normal_map.jpg");
    void *data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
        return -1;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    TEST_OPENGL_ERROR();

    return normalmap_id;
}