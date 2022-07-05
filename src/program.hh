#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <string>

#include "lib/obj.hh"
#include "utils.hh"
#include <vector>

#define TEST_OPENGL_ERROR()                                                    \
    do                                                                         \
    {                                                                          \
        GLenum err = glGetError();                                             \
        if (err != GL_NO_ERROR)                                                \
            std::cerr << "OpenGL ERROR! " << __FILE__ << " " << __LINE__ << ", number " << err << std::endl;             \
    } while (0)

class program
{
private:
    GLuint program_id_;

    GLuint vertex_shd_id_;
    GLuint fragment_shd_id_;

    std::vector<obj *> objects_;

    bool ready_ = false;

public:
    GLuint values[12]; 
    program() = default;
    ~program() = default;
    char *getlog(GLint id, GLenum type);
    bool isready();
    void use();
    void set_shader_id(GLuint shd_id, GLenum type);

    GLuint load_shader(const std::string &, GLenum type);
    void link_program();
    GLint GetUniformLocation(const std::string &name) const;
    void set_texture_2D(const std::string &name, int index, int value_index);

    inline GLuint get_program_id()
    {
        return program_id_;
    }
        
    inline std::vector<obj *> get_objects()
    {
        return objects_;
    }

    inline void add_object(obj *object)
    {
        objects_.push_back(object);
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

        prog->link_program();

        return prog;
    }

    inline void set_uniform_int(const std::string &name, int value) const
    {
        glUniform1i(GetUniformLocation(name), value); 
    }
    
    inline void set_uniform_float(const std::string &name, float value) const
    { 
        glUniform1f(GetUniformLocation(name), value); 
    }
    
    inline void set_uniform_vec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(GetUniformLocation(name), 1, &value[0]); 
    }

    inline void set_uniform_vec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(GetUniformLocation(name), x, y); 
    }
    
    inline void set_uniform_vec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(GetUniformLocation(name), 1, &value[0]); 
    }

    inline void set_uniform_vec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(GetUniformLocation(name), x, y, z); 
    }

    inline void set_uniform_vec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(GetUniformLocation(name), 1, &value[0]); 
    }

    inline void set_uniform_vec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(GetUniformLocation(name), x, y, z, w); 
    }


};
