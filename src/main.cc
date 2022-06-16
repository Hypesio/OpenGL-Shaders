#include "camera.hh"
#include "object_vbo.hh"

#include <cstddef>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "input.hh"
#include "matrix.hh"
#include "program.hh"
#include "mouse.hh"

program *program;

std::string vertex_shd;
std::string fragment_shd;

GLuint teapot_vao_id;

void display(GLFWwindow *window) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  TEST_OPENGL_ERROR();
  glBindVertexArray(teapot_vao_id);
  TEST_OPENGL_ERROR();

  glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size());
  TEST_OPENGL_ERROR();
  glBindVertexArray(0);
  TEST_OPENGL_ERROR();

  glfwSwapBuffers(window);
  glfwPollEvents();
  TEST_OPENGL_ERROR();
}

void framebuffer_size_callback(__attribute__((unused))GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

GLFWwindow *init_glfw() {
  glfwInit();
  TEST_OPENGL_ERROR();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  TEST_OPENGL_ERROR();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  TEST_OPENGL_ERROR();
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  TEST_OPENGL_ERROR();
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Desert Project !", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);
  TEST_OPENGL_ERROR();
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  TEST_OPENGL_ERROR();

  return window;
}

bool init_glew() { return (glewInit() == GLEW_OK); }

bool init_GL() {
  glEnable(GL_DEPTH_TEST);
  TEST_OPENGL_ERROR();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  TEST_OPENGL_ERROR();

  glEnable(GL_CULL_FACE);
  TEST_OPENGL_ERROR();

  glClearColor(0.4, 0.4, 0.4, 1.0);
  TEST_OPENGL_ERROR();

  std::cout << "Clear color init" << std::endl;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  return true;
}

bool init_object() {
  GLuint vbo_ids[2];

  GLint vertex_location =
      glGetAttribLocation(program->get_program_id(), "position");
  TEST_OPENGL_ERROR();

  GLint normal_flat_location =
      glGetAttribLocation(program->get_program_id(), "normalFlat");
  TEST_OPENGL_ERROR();

  glGenVertexArrays(1, &teapot_vao_id);
  TEST_OPENGL_ERROR();
  glBindVertexArray(teapot_vao_id);
  TEST_OPENGL_ERROR();

  glGenBuffers(2, vbo_ids);
  TEST_OPENGL_ERROR();

  if (vertex_location != -1) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);
    TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(float),
                 vertex_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(vertex_location);
    TEST_OPENGL_ERROR();
  }

  if (normal_flat_location != -1) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[1]);
    TEST_OPENGL_ERROR();

    glBufferData(GL_ARRAY_BUFFER,
                 normal_flat_buffer_data.size() * sizeof(float),
                 normal_flat_buffer_data.data(), GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();

    glVertexAttribPointer(normal_flat_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(normal_flat_location);
    TEST_OPENGL_ERROR();
  }

  glBindVertexArray(0);
  TEST_OPENGL_ERROR();

  return true;
}

std::string load(const std::string &filename) {
  std::ifstream input_src_file(filename, std::ios::in);
  std::string ligne;
  std::string file_content = "";
  if (input_src_file.fail()) {
    std::cerr << "FAIL\n";
    return "";
  }

  while (getline(input_src_file, ligne)) {
    file_content = file_content + ligne + "\n";
  }

  file_content += '\0';
  input_src_file.close();
  return file_content;
}

bool init_shaders() {
  std::string vertex_src = load("shaders/vertex.vert");
  std::string fragment_src = load("shaders/fragment.frag");

  char *vertex_shd_src =
      (char *)std::malloc(vertex_src.length() * sizeof(char));
  char *fragment_shd_src =
      (char *)std::malloc(fragment_src.length() * sizeof(char));

  vertex_src.copy(vertex_shd_src, vertex_src.length());
  fragment_src.copy(fragment_shd_src, fragment_src.length());

  vertex_shd = vertex_src.c_str();
  fragment_shd = fragment_src.c_str();

  return true;
}

bool init_textures() { return true; }

bool update_POV(glm::mat4 view) {

  GLuint model_view_matrix =
      glGetUniformLocation(program->get_program_id(), "model_view_matrix");
  TEST_OPENGL_ERROR();
  glUniformMatrix4fv(model_view_matrix, 1, GL_FALSE, glm::value_ptr(view));
  TEST_OPENGL_ERROR();

  glm::mat4 mat_2 =
      glm::mat4(5.00000, 0.00000, 0.00000, 0.00000, 0.00000, 5.00000, 0.00000,
                0.00000, 0.00000, 0.00000, -1.00020, -1.00000, 0.00000, 0.00000,
                -10.00100, 0.00000);

  GLuint projection_matrix =
      glGetUniformLocation(program->get_program_id(), "projection_matrix");
  TEST_OPENGL_ERROR();
  glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, glm::value_ptr(mat_2));
  TEST_OPENGL_ERROR();

  glm::vec3 color_vec(0.5, 0.4, 0.7);
  GLuint color = glGetUniformLocation(program->get_program_id(), "color");
  glUniform3fv(color, 1, glm::value_ptr(color_vec));

  glm::vec3 light_color_vec(1, 1, 0.6);
  GLuint light_color =
      glGetUniformLocation(program->get_program_id(), "light_color");
  glUniform3fv(light_color, 1, glm::value_ptr(light_color_vec));

  glm::vec3 light_pos(3., 3., 0.7);
  GLuint pos = glGetUniformLocation(program->get_program_id(), "light_pos");
  glUniform3fv(pos, 1, glm::value_ptr(light_pos));

  return true;
}

bool init_POV() {
  glm::mat4 view = glm::mat4(0.57735, -0.33333, 0.57735, 0.00000, 0.00000,
                             0.66667, 0.57735, 0.00000, -0.57735, -0.33333,
                             0.57735, 0.00000, 0.00000, 0.00000, -17, 1.00000);
  return update_POV(view);
}

int main() {
  GLFWwindow *window = init_glfw();
  if (window == nullptr) {
    TEST_OPENGL_ERROR();
    std::exit(-1);
  }

  if (!init_glew()) {
    TEST_OPENGL_ERROR();
    std::exit(-1);
  }

  if (!init_GL()) {
    TEST_OPENGL_ERROR();
    std::exit(-1);
  }

  if (!init_shaders()) {
    TEST_OPENGL_ERROR();
    std::exit(-1);
  }

  program = program::make_program(vertex_shd, fragment_shd);
  if (program) {
    program->use();
    std::cerr << "Program initialized !" << std::endl;
  } else {
    TEST_OPENGL_ERROR();
    std::cerr << "Program not initialized !" << std::endl;
    std::exit(-1);
  }

  if (!init_object()) {
    TEST_OPENGL_ERROR();
    std::exit(-1);
  }

  if (!init_textures()) {
    TEST_OPENGL_ERROR();
    std::exit(-1);
  }


  Camera *camera = new Camera();

  Mouse::init_mouse(camera);
  glfwSetCursorPosCallback(window, Mouse::mouse_callback);
  // tell GLFW to capture our mouse
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


  while (!glfwWindowShouldClose(window)) {
    process_input(window, camera);

    update_POV(camera->view);
    program->use();

    display(window);
    
  }

  return 0;
}