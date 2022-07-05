#include "shader_func.hh"

#include <GL/gl.h>
#include <cstddef>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

#include "camera.hh"
#include "input.hh"
#include "light.hh"
#include "program.hh"

glm::vec3 light_pos(3., 1000., 0.7);
glm::vec4 clip_plane = vec4(0, 1, 0, 0);
DirectionalLight *light = NULL;

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
    glUniformMatrix4fv(projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(projection));
    TEST_OPENGL_ERROR();
}

void display_obj(std::vector<obj *> objects)
{
    if (!objects.empty())
    {
        for (size_t i = 0; i < objects.size(); i++)
        {
            obj *o = objects[i];
            glBindVertexArray(o->vao);
            TEST_OPENGL_ERROR();

            const struct obj_surf *sp = o->sv;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sp->pibo);
            TEST_OPENGL_ERROR();
            glDrawElements(GL_TRIANGLES, 3 * sp->pc, GL_UNSIGNED_INT,
                           (const GLvoid *)0);
            TEST_OPENGL_ERROR();
        }
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
    program->set_uniform_vec3("light_dir", light->direction);
    program->set_uniform_float("time_passed", Time::get_time_passed());
    //program->set_uniform_vec3("camera_pos", camera->cameraPos);
    program->set_uniform_vec4("clip_plane", clip_plane.x, clip_plane.y, clip_plane.z,
                clip_plane.w);

    // Objects
    std::vector<obj *> objects = program->get_objects();

    program->set_texture_2D("normal_map", 0, program->values[0]);
    program->set_texture_2D("dust_texture", 1, program->values[1]);
    program->set_texture_2D("light_depth_texure", 2, light->depth_buffer);
    GLuint matrix = program->GetUniformLocation("light_projection_matrix");
    glUniformMatrix4fv(matrix, 1, GL_FALSE,
                       glm::value_ptr(light->get_projection_matrix()));

    obj_proc(objects[0]);
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

    std::vector<obj *> objects = program->get_objects();
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

    program->set_uniform_vec4("clip_plane", clip_plane.x, clip_plane.y,
                              clip_plane.z, clip_plane.w);
    program->set_uniform_vec3("light_dir", light->direction);
    program->set_uniform_vec3("cam_pos", camera->cameraPos);
    program->set_uniform_float("time_passed", Time::get_time_passed());
    GLuint matrix = program->GetUniformLocation("light_projection_matrix");
    glUniformMatrix4fv(matrix, 1, GL_FALSE,
                       glm::value_ptr(light->get_projection_matrix()));

    std::vector<obj *> objects = program->get_objects();

    program->set_texture_2D("reflection_texture", 0, program->values[1]);
    program->set_texture_2D("refraction_texture", 1, program->values[3]);
    program->set_texture_2D("refraction_depth_texture", 2, program->values[4]);
    program->set_texture_2D("normal_map", 3, program->values[6]);
    program->set_texture_2D("light_depth_texure", 4, light->depth_buffer);

    display_obj(objects);

    glBindVertexArray(0);

    return true;
}

bool init_shadow_shader(program *program, Camera *camera)
{
    GLuint model_view_matrix = program->GetUniformLocation("model_view_matrix");
    glUniformMatrix4fv(model_view_matrix, 1, GL_FALSE, &light->camera->get_view()[0][0]);
    TEST_OPENGL_ERROR();

    GLuint projection_matrix = program->GetUniformLocation("projection_matrix");
    glUniformMatrix4fv(projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(light->get_projection_matrix()));
    TEST_OPENGL_ERROR();

    std::vector<obj *> objects = program->get_objects();
    display_obj(objects);

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

bool init_palm_shader(program *program, Camera* camera)
{
    glm::mat4 view = camera->get_view();
    init_view_projection(program, view);

    program->set_uniform_vec3("light_pos", light_pos);

    std::vector<obj *> objects = program->get_objects();
    
    obj_set_prop_loc(objects[0], OBJ_KD, program->GetUniformLocation("kd_color"), -1, -1);
    /*
    float c[6];

    for (int i = 0; i < 3; i++)
    {
        obj_get_mtrl_c(objects, OBJ_KD, i, c);
        std::cout << "KD  = " << c[0] << " " << c[1] <<  " " << c[2] << " " << c[3] <<std::endl;
    }*/

    if (!objects.empty())
    {
        glBindVertexArray(objects[0]->vao);
        TEST_OPENGL_ERROR();

        const struct obj_surf *sp = objects[0]->sv;

        while(sp != nullptr && sp->pibo != 0 && sp->pc != 0)
        {
            if (0 <= sp->mi && sp->mi < objects[0]->mc)
                obj_render_mtrl(objects[0], sp->mi); TEST_OPENGL_ERROR();

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sp->pibo); TEST_OPENGL_ERROR();
            glDrawElements(GL_TRIANGLES, 3 * sp->pc, GL_UNSIGNED_INT,
                        (const GLvoid *)0); TEST_OPENGL_ERROR();
            sp++;
        }
    }

    glBindVertexArray(0);

    return true;
}
void set_light(DirectionalLight *l)
{
    light = l;
}
