#include "textures.hh"

#include <endian.h>
#include <iostream>

#include "lib/obj.hh"
#include "lib/stb_image.h"
#include "program.hh"

std::string textures_path = "textures/";

unsigned int loadSkybox()
{
    std::vector<std::string> faces = { "skybox/right.jpg",  "skybox/left.jpg",  "skybox/top.jpg",
                                       "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg" };
    return loadCubemap(faces);
}

unsigned int loadTexture(std::string filename) {

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    std::string fullPath = textures_path;
    fullPath = fullPath.append(filename);
    void *data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Texture failed to load : " << fullPath
                    << std::endl;
        return -1;
    }
    free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        std::string fullPath = textures_path;
        fullPath = fullPath.append(faces[i].c_str());
        void *data =
            stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load : " << fullPath
                      << std::endl;
            return -1;
        }
        free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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