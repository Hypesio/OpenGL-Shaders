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

#define HEIGHT 900
#define WIDTH 1400

void init_view_projection(program *program, glm::mat4 view)
{
    GLuint model_view_matrix = program->GetUniformLocation("model_view_matrix");

    glUniformMatrix4fv(model_view_matrix, 1, GL_FALSE, &view[0][0]);
    TEST_OPENGL_ERROR();

    glm::mat4 projection = glm::frustum(-0.1, 0.1, -0.1, 0.1, 0.1, 1000.0);
    /*glm::mat4 projection =
        glm::mat4(5.00000, 0.00000, 0.00000, 0.00000, 0.00000, 5.00000, 0.00000,
                  0.00000, 0.00000, 0.00000, -1.00020, -1.00000, 0.00000,
                  0.00000, -10.00100, 0.00000);*/

    GLuint projection_matrix = program->GetUniformLocation("projection_matrix");
    glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));
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

void set_clip_plane(vec4 new_clip_plane)
{
    clip_plane = new_clip_plane;
}

bool init_dunes_shader(program *program, Camera *camera)
{
    // std::cout << "Init dune shader" << std::endl;
    //  Shaders
    init_view_projection(program, camera->get_view());

    glm::vec3 color_vec(0.97, 0.89, 0.71);
    program->set_uniform_vec3("color", color_vec);
    program->set_uniform_vec3("light_pos", light_pos);
    program->set_uniform_float("time_passed", Time::get_time_passed());
    program->set_uniform_vec3("camera_pos", camera->cameraPos);
    program->set_uniform_vec4("clip_plane", clip_plane.x, clip_plane.y, clip_plane.z,
                clip_plane.w);

    // Objects
    obj *objects = program->get_objects();

    program->set_texture_2D("normal_map", 0, objects->values[0]);
    program->set_texture_2D("dust_texture", 1, objects->values[1]);

    obj_proc(objects);
    TEST_OPENGL_ERROR();

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
    /*glActiveTexture(GL_TEXTURE0);
    TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_CUBE_MAP, objects->mc);*/

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

    program->set_uniform_vec4("clip_plane", clip_plane.x, clip_plane.y, clip_plane.z,
                clip_plane.w);
    program->set_uniform_vec3("light_pos", light_pos);
    program->set_uniform_vec3("cam_pos", camera->cameraPos);
    program->set_uniform_float("time_passed", Time::get_time_passed());

    obj *objects = program->get_objects();

    program->set_texture_2D("reflection_texture", 0, objects->values[1]);
    program->set_texture_2D("refraction_texture", 1, objects->values[3]);
    program->set_texture_2D("refraction_depth_texture", 2, objects->values[4]);
    program->set_texture_2D("normal_map", 3, objects->values[6]);

    display_obj(objects);

    glBindVertexArray(0);

    return true;
}

void update_water_cam(Camera *main_cam, Camera *water_cam)
{
    // Set a the good position
    water_cam->cameraPos = glm::vec3(
        main_cam->cameraPos.x, -main_cam->cameraPos.y, main_cam->cameraPos.z);

    // Adapt camera front
    vec3 front = main_cam->cameraFront;
    water_cam->cameraFront = glm::vec3(front.x, -front.y, front.z);

    water_cam->update_view();
}
