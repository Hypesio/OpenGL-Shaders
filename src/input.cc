#include "input.hh"
#include "camera.hh"
#include "matrix.hh"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>

// Handle camera movement
void camera_moves(GLFWwindow *window, Camera *camera, float deltaTime) {
  const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    camera->cameraPos += cameraSpeed * camera->cameraFront;
    std::cout << "Press W" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera->cameraPos -= cameraSpeed * camera->cameraFront;
    std::cout << "Press S" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera->cameraPos -=
        glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) *
        cameraSpeed;
        std::cout << "Press A" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera->cameraPos +=
        glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) *
        cameraSpeed;
        std::cout << "Press D" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera->cameraPos += camera->cameraUp * cameraSpeed;
        std::cout << "Press Space" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    camera->cameraPos -= camera->cameraUp * cameraSpeed;
        std::cout << "Press LShift" << std::endl;
  }

  camera->update_view();
}

float last_frame = 0;

void process_input(GLFWwindow *window, Camera *camera) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float currentFrame = glfwGetTime();
  float deltaTime = currentFrame - last_frame;
  last_frame = currentFrame;

  camera_moves(window, camera, deltaTime);
}
