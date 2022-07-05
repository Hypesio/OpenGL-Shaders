#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <GL/glew.h>
#include <glm/gtx/dual_quaternion.hpp>
#include "camera.hh"

class DirectionalLight
{
public:
    glm::vec3 light_pos;
    glm::vec3 direction;
    Camera *camera;
    GLuint fbo_shadow;
    GLuint depth_buffer;

    DirectionalLight(glm::vec3 _light_pos, GLuint _fbo_shadow, GLuint _depth_buffer)
    {
        light_pos = _light_pos;
        direction = normalize(light_pos);
        camera = new Camera();
        fbo_shadow = _fbo_shadow;
        depth_buffer = _depth_buffer;
        camera->cameraFront = direction;
        camera->cameraPos = vec3(0,0,0) + direction * 100.0f;
        camera->update_view();
    }

    /*glm::mat4 get_view_matrix(float distance = 100.0f) {
        return glm::lookAt(distance * light_pos, -light_pos, glm::vec3(0.0f, 0.0f, 1.0f));
    }*/

    glm::mat4 get_projection_matrix() {
        float near_plane = 0.1f, far_plane = 1000.0f;
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    }
};