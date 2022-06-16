#pragma once

#include "camera.hh"
#include <GLFW/glfw3.h>

class Time {
public:
  static float inline last_frame = 0;

  static float deltaTime() {
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - last_frame;
    last_frame = currentFrame;
    return deltaTime;
  }
};

void process_input(GLFWwindow *window, Camera *camera);