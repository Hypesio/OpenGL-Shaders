#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>

#include "lib/obj.hh"

#define TEST_OPENGL_ERROR()                                                    \
    do                                                                         \
    {                                                                          \
        GLenum err = glGetError();                                             \
        if (err != GL_NO_ERROR)                                                \
            std::cerr << "OpenGL ERROR!" << __LINE__ << ", number " << err << std::endl;             \
    } while (0)

class program
{
public:
    program() = default;
    ~program() = default;
    char *getlog(GLint id, GLenum type);
    bool isready();
    void use();
    void set_shader_id(GLuint shd_id, GLenum type);

    GLuint load_shader(std::string &, GLenum type);
    void link_program();

    inline GLuint get_program_id()
    {
        return program_id_;
    }
        
    inline obj *get_objects()
    {
        return objects_;
    }

    inline void set_objects(obj *objects)
    {
        objects_ = objects;
    }

    inline void set_vertex_shd(std::string &vertex_shd)
    {
        vertex_shd_ = vertex_shd;
    }

    inline void set_fragment_shd(std::string &fragment_shd)
    {
        fragment_shd_ = fragment_shd;
    }

    static program *make_program(std::string &vertex_shader, std::string &fragment_shader)
    {
        program *prog = new program();

        auto vertex_id = prog->load_shader(vertex_shader, GL_VERTEX_SHADER);
        if (!vertex_id)
            return nullptr;

        prog->set_shader_id(vertex_id, GL_VERTEX_SHADER);
        prog->set_vertex_shd(vertex_shader);

        auto fragment_id = prog->load_shader(fragment_shader, GL_FRAGMENT_SHADER);
        if (!fragment_id)
        {
            TEST_OPENGL_ERROR();
            // free buffer
            return nullptr;
        }

        prog->set_shader_id(fragment_id, GL_FRAGMENT_SHADER);
        prog->set_fragment_shd(fragment_shader);

        prog->link_program();

        return prog;
    }

private:
    GLuint program_id_;

    GLuint vertex_shd_id_;
    std::string vertex_shd_;

    GLuint fragment_shd_id_;
    std::string fragment_shd_;

    obj *objects_;

    bool ready_ = false;
};
