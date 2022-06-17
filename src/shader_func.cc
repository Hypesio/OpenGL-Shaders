#include "shader_func.hh"

bool init_dunes_shader(GLuint program_id)
{
    glm::vec3 color_vec(0.5, 0.4, 0.7);
    GLuint color = glGetUniformLocation(program_id, "color");
    glUniform3fv(color, 1, glm::value_ptr(color_vec));

    glm::vec3 light_color_vec(1, 1, 0.6);
    GLuint light_color =
        glGetUniformLocation(program_id, "light_color");
    glUniform3fv(light_color, 1, glm::value_ptr(light_color_vec));

    glm::vec3 light_pos(3., 3., 0.7);
    GLuint pos = glGetUniformLocation(program_id, "light_pos");
    glUniform3fv(pos, 1, glm::value_ptr(light_pos));

    return true;
}