#include "camera.hh"
#include "object_vbo.hh"

#include <cstddef>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "input.hh"
#include "matrix.hh"
#include "mouse.hh"
#include "program.hh"

std::vector<program *> programs;

void display(GLFWwindow *window)
{
    obj *objects = programs[0]->get_objects();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();
    glBindVertexArray(objects->vao);
    TEST_OPENGL_ERROR();

    const struct obj_surf *sp = objects->sv;
    // glDrawArrays(GL_TRIANGLES, 0, objects->vc * sizeof(struct obj_vert));
    if (sp->pibo)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sp->pibo);
        TEST_OPENGL_ERROR();
        glDrawElements(GL_TRIANGLES, 3 * sp->pc, GL_UNSIGNED_INT,
                       (const GLvoid *)0);
        TEST_OPENGL_ERROR();
    }

    glBindVertexArray(0);
    TEST_OPENGL_ERROR();

    glfwSwapBuffers(window);
    glfwPollEvents();
    TEST_OPENGL_ERROR();
}

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window,
                               int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

GLFWwindow *init_glfw()
{
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
    if (window == NULL)
    {
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

bool init_glew()
{
    return (glewInit() == GLEW_OK);
}

bool init_GL()
{
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

bool init_object()
{
    // Load obj from file
    obj *dunes = obj_create("data/simple_dunes.obj");
    if (!dunes)
        return false;

    obj_set_vert_loc(
        dunes, -1, glGetAttribLocation(programs[0]->get_program_id(), "normalFlat"),
        -1, glGetAttribLocation(programs[0]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();

    obj_init(dunes);

    programs[0]->set_objects(dunes);

    return true;
}

bool init_shaders()
{
    std::string vertex_src_path = "shaders/vertex.vert";
    std::string fragment_src_path = "shaders/fragment.frag";

    program *first_prog = program::make_program(vertex_src_path, fragment_src_path);
    if (!first_prog)
      return false;

    programs.push_back(first_prog);

    return true;
}

bool init_textures()
{
    return true;
}

bool update_POV(glm::mat4 view, GLuint program_id)
{
    GLuint model_view_matrix =
        glGetUniformLocation(program_id, "model_view_matrix");
    TEST_OPENGL_ERROR();
    glUniformMatrix4fv(model_view_matrix, 1, GL_FALSE, glm::value_ptr(view));
    TEST_OPENGL_ERROR();

    glm::mat4 mat_2 =
        glm::mat4(5.00000, 0.00000, 0.00000, 0.00000, 0.00000, 5.00000, 0.00000,
                  0.00000, 0.00000, 0.00000, -1.00020, -1.00000, 0.00000,
                  0.00000, -10.00100, 0.00000);

    GLuint projection_matrix =
        glGetUniformLocation(program_id, "projection_matrix");
    TEST_OPENGL_ERROR();
    glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, glm::value_ptr(mat_2));
    TEST_OPENGL_ERROR();

    glm::vec3 color_vec(0.5, 0.4, 0.7);
    GLuint color = glGetUniformLocation(program_id, "color");
    glUniform3fv(color, 1, glm::value_ptr(color_vec));

    glm::vec3 light_color_vec(1, 1, 0.6);
    GLuint light_color =
        glGetUniformLocation(program_id, "light_color");
    glUniform3fv(light_color, 1, glm::value_ptr(light_color_vec));

    glm::vec3 light_pos(3., 3., 0.7);
    GLuint pos = glGetUniformLocation(program_id, "light_pos");
    glUniform3fv(pos, 1, glm::value_ptr(light_pos));

    return true;
}

bool init_POV()
{
    glm::mat4 view = glm::mat4(
        0.57735, -0.33333, 0.57735, 0.00000, 0.00000, 0.66667, 0.57735, 0.00000,
        -0.57735, -0.33333, 0.57735, 0.00000, 0.00000, 0.00000, -17, 1.00000);
    return update_POV(view, programs[0]->get_program_id());
}

int main()
{
    GLFWwindow *window = init_glfw();
    if (window == nullptr)
    {
        TEST_OPENGL_ERROR();
        std::exit(-1);
    }

    if (!init_glew())
    {
        TEST_OPENGL_ERROR();
        std::exit(-1);
    }

    if (!init_GL())
    {
        TEST_OPENGL_ERROR();
        std::exit(-1);
    }

    if (!init_shaders())
    {
        TEST_OPENGL_ERROR();
        std::exit(-1);
    }


    if (programs.size() != 0)
    {
          for (const auto &pg : programs)
          {
            pg->use();
            std::cerr << "Program nb " << pg->get_program_id() << " initialized !" << std::endl;
          }
    }
    else
    {
        TEST_OPENGL_ERROR();
        std::cerr << "Program not initialized !" << std::endl;
        std::exit(-1);
    }

    if (!init_object())
    {
        TEST_OPENGL_ERROR();
        std::exit(-1);
    }

    if (!init_textures())
    {
        TEST_OPENGL_ERROR();
        std::exit(-1);
    }

    Camera *camera = new Camera();

    Mouse::init_mouse(camera);
    glfwSetCursorPosCallback(window, Mouse::mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window, camera);


        for (const auto &pg : programs)
        {
            update_POV(camera->view, pg->get_program_id());
            pg->use();
        }

        display(window);
    }

    return 0;
}