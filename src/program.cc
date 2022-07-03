#include "program.hh"
#include <string>

void program::set_shader_id(GLuint shd_id, GLenum type)
{
    if (type == GL_VERTEX_SHADER)
        vertex_shd_id_ = shd_id;

    if (type == GL_FRAGMENT_SHADER)
        fragment_shd_id_ = shd_id;
}

GLuint program::load_shader(const std::string &src, GLenum type)
{
    GLint compile_status = GL_TRUE;

    GLuint shader_id = glCreateShader(type);
    TEST_OPENGL_ERROR();

    glShaderSource(shader_id, 1, (const GLchar **)&(src), 0);
    TEST_OPENGL_ERROR();

    glCompileShader(shader_id);
    TEST_OPENGL_ERROR();
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    TEST_OPENGL_ERROR();

    if (compile_status != GL_TRUE)
    {
        char *log = getlog(shader_id, GL_SHADER);
        if (log)
        {
            auto str = type == GL_VERTEX_SHADER ? " vertex " : "fragment ";
            std::cerr << "Shader " << str << shader_id << " : " << log << std::endl;
            std::free(log);
        }

        glDeleteShader(shader_id);
        return -1;
    }

    return shader_id;
}

char *program::getlog(GLint id, GLenum type)
{
    GLint log_size;
    char *log;
    if (type == GL_SHADER)
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_size);
    else if (type == GL_PROGRAM)
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_size);
    else
        return nullptr;

    log = (char *)std::calloc(log_size + 1, 1);
    if (log != 0)
    {
        if (type == GL_SHADER)
            glGetShaderInfoLog(id, log_size, &log_size, log);
        else if (type == GL_PROGRAM)
            glGetProgramInfoLog(program_id_, log_size, &log_size, log);
        return log;
    }

    return nullptr;
}

void program::link_program()
{
    GLint link_status = GL_TRUE;
    program_id_ = glCreateProgram();
    TEST_OPENGL_ERROR();
    if (program_id_ == 0)
        return;

    glAttachShader(program_id_, vertex_shd_id_);
    TEST_OPENGL_ERROR();

    glAttachShader(program_id_, fragment_shd_id_);
    TEST_OPENGL_ERROR();

    glLinkProgram(program_id_);
    TEST_OPENGL_ERROR();

    glGetProgramiv(program_id_, GL_LINK_STATUS, &link_status);
    TEST_OPENGL_ERROR();
    if (link_status != GL_TRUE)
    {
        char *program_log = getlog(program_id_, GL_PROGRAM);
        if (program_log)
        {
            std::cerr << "Program " << program_log << std::endl;
            std::free(program_log);
        }

        glDeleteProgram(program_id_);
        TEST_OPENGL_ERROR();

        glDeleteShader(vertex_shd_id_);
        TEST_OPENGL_ERROR();

        glDeleteShader(fragment_shd_id_);
        TEST_OPENGL_ERROR();

        program_id_ = 0;

        return;
    }

    ready_ = true;
}

void program::use()
{
    if (isready())
    {
        glUseProgram(program_id_);
        TEST_OPENGL_ERROR();
    }
    else
    {
        std::cerr << "Problem of init of program" << std::endl;
        TEST_OPENGL_ERROR();
    }
}

bool program::isready()
{
    return ready_;
}

void program::set_texture_2D(std::string name, int index, int value_index)
{
    glUniform1i(this->GetUniformLocation(name), index); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0 + index); TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, value_index); TEST_OPENGL_ERROR();
}

GLuint program::GetUniformLocation(const std::string &name) {
    GLuint id = glGetUniformLocation(program_id_, name.c_str());
    TEST_OPENGL_ERROR();
    if (id == -1)
    {
        std::cout << "Uniform asked doesn't exist: " << name << " in program " << program_id_
                  << std::endl;
    }
    return id;
}