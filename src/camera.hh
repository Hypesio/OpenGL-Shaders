#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Camera {
  private : 
  mat4 view;
public:
  vec3 cameraPos;
  vec3 cameraFront;
  vec3 cameraUp;

  Camera() {
    cameraPos = vec3(0.0f, 0.0f, 3.0f);
    cameraFront = vec3(0.0f, 0.0f, -1.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    view = glm::mat4(
        0.57735, -0.33333, 0.57735, 0.00000, 0.00000, 0.66667, 0.57735, 0.00000,
        -0.57735, -0.33333, 0.57735, 0.00000, 0.00000, 0.00000, -17, 1.00000);

    reset_cam();
  }

  // Reset camera to neutral position
  void reset_cam() {
    cameraPos = vec3(0.0f, 0.0f, 3.0f);
    cameraFront = vec3(0.0f, 0.0f, -1.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);

    mat4 view = glm::mat4(
        0.57735, -0.33333, 0.57735, 0.00000, 0.00000, 0.66667, 0.57735, 0.00000,
        -0.57735, -0.33333, 0.57735, 0.00000, 0.00000, 0.00000, -17, 1.00000);
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }

  void update_view() {
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }

  glm::mat4 get_view() {
    return glm::make_mat4(glm::value_ptr(view));
  }
};