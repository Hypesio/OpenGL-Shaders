#include "shader_func.hh"

#include <GL/gl.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <ostream>

#include "camera.hh"
#include "program.hh"

glm::vec3 light_pos(3., 1000., 0.7);

void init_view_projection(program *program, glm::mat4 view)
{
    GLuint model_view_matrix = program->GetUniformLocation("model_view_matrix");

    glUniformMatrix4fv(model_view_matrix, 1, GL_FALSE, &view[0][0]);
    TEST_OPENGL_ERROR();

    glm::mat4 mat_2 =
        glm::mat4(5.00000, 0.00000, 0.00000, 0.00000, 0.00000, 5.00000, 0.00000,
                  0.00000, 0.00000, 0.00000, -1.00020, -1.00000, 0.00000,
                  0.00000, -10.00100, 0.00000);

    GLuint projection_matrix = program->GetUniformLocation("projection_matrix");
    glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, glm::value_ptr(mat_2));
    TEST_OPENGL_ERROR();
}

void display_obj(obj *objects)
{
    if (objects != nullptr)
    {
        glBindVertexArray(objects->vao);
        TEST_OPENGL_ERROR();

        const struct obj_surf *sp = objects->sv;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sp->pibo);
        TEST_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3 * sp->pc, GL_UNSIGNED_INT,
                       (const GLvoid *)0);
        TEST_OPENGL_ERROR();
    }
}

bool init_dunes_shader(program *program, Camera *camera)
{
    // std::cout << "Init dune shader" << std::endl;
    //  Shaders
    init_view_projection(program, camera->get_view());

    glm::vec3 color_vec(0.9, 0.44, 0);
    GLuint color = program->GetUniformLocation("color");
    glUniform3fv(color, 1, glm::value_ptr(color_vec));

    glm::vec3 light_color_vec(1, 1, 0.6);
    GLuint light_color = program->GetUniformLocation("light_color");
    glUniform3fv(light_color, 1, glm::value_ptr(light_color_vec));

    GLuint pos = program->GetUniformLocation("light_pos");
    glUniform3fv(pos, 1, glm::value_ptr(light_pos));

    GLuint pos_cam = program->GetUniformLocation("cam_pos");
    glUniform3fv(pos_cam, 1, glm::value_ptr(camera->cameraPos));

    // Objects
    obj *objects = program->get_objects();
    display_obj(objects);
    glBindVertexArray(0);

    // std::cout << "End Init dune shader" << std::endl;

    return true;
}

bool init_skybox_shader(program *program, Camera *camera)
{
    // std::cout << "Init Skybox shader" << std::endl;
    glm::mat4 view = glm::mat4(glm::mat3(camera->get_view()));
    init_view_projection(program, view);

    obj *objects = program->get_objects();
    glActiveTexture(GL_TEXTURE0);
    TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_CUBE_MAP, objects->mc);
    display_obj(objects);
    glBindVertexArray(0);

    // std::cout << "End init dune shader" << std::endl;
    return true;
}

bool init_water_shader(program *program, Camera *camera)
{
    
    glm::mat4 view = camera->get_view();
    init_view_projection(program, view);

    obj *objects = program->get_objects();
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, objects->mc);
    
    GLuint texID = program->GetUniformLocation("rendered_texture");
    glUniform1i(texID, 0); TEST_OPENGL_ERROR();
    
    display_obj(objects);

    glBindVertexArray(0);

    

    return true;
}

void update_water_cam(Camera *main_cam, Camera *water_cam) {
    // Set a the good position
    water_cam->cameraPos = glm::vec3(main_cam->cameraPos.x, -main_cam->cameraPos.y, main_cam->cameraPos.z);
    
    // Adapt camera front
    vec3 front = main_cam->cameraFront;
    water_cam->cameraFront = glm::vec3(front.x, -front.y, front.z);
    
    water_cam->update_view();
}