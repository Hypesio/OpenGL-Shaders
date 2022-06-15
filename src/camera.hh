#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

using namespace glm;

class Camera {
public:
  vec3 cameraPos;
  vec3 cameraFront;
  vec3 cameraUp;
  mat4 view;

  Camera() {
    cameraPos = vec3(0.0f, 0.0f, 3.0f);
    cameraFront = vec3(0.0f, 0.0f, -1.0f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    view = mat4();

    reset_cam();
  }

  // Reset camera to neutral position
  void reset_cam() {

    vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
    vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraDirection = normalize(cameraPos - cameraTarget);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 cameraRight = normalize(cross(up, cameraDirection));
    vec3 cameraUp = cross(cameraDirection, cameraRight);

    mat4 view = mat4();
    view = lookAt(cameraPos, cameraTarget, cameraUp);
  }

  void update_view() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }
};