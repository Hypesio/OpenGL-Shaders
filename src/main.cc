#include <cstddef>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <ostream>

#include "object_vbo.hh"
#include "camera.hh"
#include "input.hh"
#include "matrix.hh"
#include "mouse.hh"
#include "program.hh"
#include "shader_func.hh"
#include "textures.hh"

std::vector<program *> programs;

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
        dunes, -1,
        glGetAttribLocation(programs[0]->get_program_id(), "normalFlat"), -1,
        glGetAttribLocation(programs[0]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();


    obj_init(dunes);

    programs[0]->set_objects(dunes);

    // Load obj for skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    unsigned int cubemapTexture = loadSkybox();
    TEST_OPENGL_ERROR();

    obj *sky = obj_create(NULL);
    sky->vao = skyboxVAO; 
    sky->vbo = skyboxVBO;
    sky->mc = cubemapTexture;
    programs[1]->set_objects(sky);
    
    return true;
}

bool init_shaders()
{
    program *first_prog =
        program::make_program(shader_paths[0], shader_paths[1]);
    if (!first_prog)
        return false;

    programs.push_back(first_prog);

    program *skybox_prog =
        program::make_program(shader_paths[2], shader_paths[3]);
    if (!skybox_prog)
        return false;

    programs.push_back(skybox_prog);

    return true;
}

bool init_textures()
{
    // GLuint textureCubemap = loadSkybox();

    return true;
}

bool update_shaders(Camera *camera)
{
    // Dunes
    programs[0]->use();
    shader_array[0](programs[0], camera);

    // Skybox
    glDepthFunc(GL_LEQUAL); TEST_OPENGL_ERROR();
    programs[1]->use();
    shader_array[1](programs[1], camera);
    glDepthFunc(GL_LESS); TEST_OPENGL_ERROR();

    return true;
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

    if (programs.size() == 0)
    {
        TEST_OPENGL_ERROR();
        std::cerr << "Programs not initialized !" << std::endl;
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

    bool first = true;
    while (!glfwWindowShouldClose(window))
    {
        process_input(window, camera);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        update_shaders(camera);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (first) {
            std::cout << "End of display of first frame" << std::endl;
            first = false;
        }
    }

    return 0;
}