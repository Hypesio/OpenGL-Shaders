#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>

#include "lib/obj.hh"
#include "utils.hh"

#define TEST_OPENGL_ERROR()                                                    \
    do                                                                         \
    {                                                                          \
        GLenum err = glGetError();                                             \
        if (err != GL_NO_ERROR)                                                \
            std::cerr << "OpenGL ERROR! " << __FILE__ << " " << __LINE__ << ", number " << err << std::endl;             \
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

    GLuint load_shader(const std::string &, GLenum type);
    void link_program();
    GLuint GetUniformLocation(const std::string &name);

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

    static program *make_program(const std::string &vertex_shader_path, const std::string &fragment_shader_path)
    {
        program *prog = new program();

        auto vertex_id = prog->load_shader(load(vertex_shader_path), GL_VERTEX_SHADER);
        if (!vertex_id)
            return nullptr;

        prog->set_shader_id(vertex_id, GL_VERTEX_SHADER);

        auto fragment_id = prog->load_shader(load(fragment_shader_path), GL_FRAGMENT_SHADER);
        if (!fragment_id)
        {
            TEST_OPENGL_ERROR();
            // free buffer
            return nullptr;
        }

        prog->set_shader_id(fragment_id, GL_FRAGMENT_SHADER);

        std::cout << "Shader path " << vertex_shader_path<< " and " << fragment_shader_path <<std::endl;
        prog->link_program();
        std::cout << "Program id was " << prog->get_program_id() << std::endl;

        return prog;
    }

private:
    GLuint program_id_;

    GLuint vertex_shd_id_;
    GLuint fragment_shd_id_;

    obj *objects_;

    bool ready_ = false;
};
