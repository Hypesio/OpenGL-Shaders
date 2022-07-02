#include "shader_func.hh"

#include <GL/gl.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <ostream>

#include "camera.hh"
#include "program.hh"
#include "input.hh"

glm::vec3 light_pos(3., 1000., 0.7);
glm::vec4 clip_plane = vec4(0, 1, 0, 0);


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

void set_clip_plane(vec4 new_clip_plane) {
    clip_plane = new_clip_plane;
}

bool init_dunes_shader(program *program, Camera *camera)
{
    // std::cout << "Init dune shader" << std::endl;
    //  Shaders
    init_view_projection(program, camera->get_view());

    glm::vec3 color_vec(0.97, 0.89, 0.71);
    GLuint color = program->GetUniformLocation("color");
    glUniform3fv(color, 1, glm::value_ptr(color_vec));

    GLuint pos = program->GetUniformLocation("light_pos");
    glUniform3fv(pos, 1, glm::value_ptr(light_pos));

    GLuint id_plane = program->GetUniformLocation("clip_plane");
    glUniform4f(id_plane, clip_plane.x, clip_plane.y, clip_plane.z, clip_plane.w);

    // Objects
    obj *objects = program->get_objects();

    GLuint texID1 = program->GetUniformLocation("normal_map");
    glUniform1i(texID1, 0); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);  TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, objects->values[0]);  TEST_OPENGL_ERROR();

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

    /*GLuint cloud1 = program->GetUniformLocation("cloud_tiny");
    glUniform1i(cloud1, 1); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE1);  TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, objects->values[0]);  TEST_OPENGL_ERROR();

    GLuint pos = program->GetUniformLocation("time_passed");
    glUniform1f(pos, Time::get_time_passed());*/

    display_obj(objects);
    glBindVertexArray(0);

    // std::cout << "End init dune shader" << std::endl;
    return true;
}

bool init_water_shader(program *program, Camera *camera)
{
    glm::mat4 view = camera->get_view();
    init_view_projection(program, view);

    GLuint id_plane = program->GetUniformLocation("clip_plane");
    glUniform4f(id_plane, clip_plane.x, clip_plane.y, clip_plane.z, clip_plane.w);

    GLuint pos_cam = program->GetUniformLocation("cam_pos");
    glUniform3fv(pos_cam, 1, glm::value_ptr(camera->cameraPos));

    GLuint pos = program->GetUniformLocation("light_pos");
    glUniform3fv(pos, 1, glm::value_ptr(light_pos));

    pos = program->GetUniformLocation("time_passed");
    glUniform1f(pos, Time::get_time_passed());

    obj *objects = program->get_objects();
    
    GLuint texID = program->GetUniformLocation("reflection_texture");
    glUniform1i(texID, 0); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, objects->values[1]);

    GLuint texID2 = program->GetUniformLocation("refraction_texture");
    glUniform1i(texID2, 1); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE1);  TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, objects->values[3]);  TEST_OPENGL_ERROR();

    GLuint texID3 = program->GetUniformLocation("refraction_depth_texture");
    glUniform1i(texID3, 2); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE2);  TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, objects->values[4]);  TEST_OPENGL_ERROR();

    GLuint texID4 = program->GetUniformLocation("normal_map");
    glUniform1i(texID4, 3); TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE3);  TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, objects->values[6]);  TEST_OPENGL_ERROR();
    
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
