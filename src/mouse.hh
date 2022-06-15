#pragma once

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "camera.hh"

class Mouse {
public:
static inline bool firstMouse;
  static inline double lastX;
  static inline double lastY;
  static inline float yaw;
  static inline float pitch;
  static inline glm::vec3 direction;
  static inline Camera *camera;

  static void init_mouse(Camera *cam) {
    lastX = 400;
    lastY = 300;
    firstMouse = true;
    yaw = -90.0f;
    pitch = 0;
    camera = cam;
  }

  static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    // Code from learnopengl.com
    
    if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera->cameraFront = glm::normalize(direction);
  }
};