#include <cstddef>
#include <fstream>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <ostream>

#include "object_vbo.hh"
#include "camera.hh"
#include "input.hh"
#include "lib/obj.hh"
#include "matrix.hh"
#include "mouse.hh"
#include "program.hh"
#include "shader_func.hh"
#include "textures.hh"

#define HEIGHT 900
#define WIDTH 1400

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
        glfwCreateWindow(WIDTH, HEIGHT, "Desert Project !", NULL, NULL);
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

    //glEnable(GL_CULL_FACE);
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
    unsigned int cubemapTexture = loadSkybox();

    obj *skybox = obj_create("data/cube.obj");
    obj_set_vert_loc(
        skybox, -1,
        glGetAttribLocation(programs[1]->get_program_id(), "normalFlat"), -1,
        glGetAttribLocation(programs[1]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();
    obj_init(skybox);
    programs[1]->set_objects(skybox);
    skybox->mc = cubemapTexture;

    // Load obj for water
    obj *planeWater = obj_create("data/plane.obj");
    obj_set_vert_loc(
        planeWater, -1,
        glGetAttribLocation(programs[2]->get_program_id(), "normalFlat"),  glGetAttribLocation(programs[2]->get_program_id(), "uv"),
        glGetAttribLocation(programs[2]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();
    obj_init(planeWater);
    programs[2]->set_objects(planeWater);
    GLuint frame_buffer_number;
    planeWater->mc = generate_render_texture(frame_buffer_number, WIDTH, HEIGHT);
    if (planeWater->mc == -1)
        std::cout << "Failed to init render texture for water";
    planeWater->mm = frame_buffer_number;
    GLuint frame_buffer_number2;
    planeWater->sc = generate_render_texture(frame_buffer_number2, WIDTH, HEIGHT);
    if (planeWater->sc == -1)
        std::cout << "Failed to init render refraction texture for water";
    planeWater->sm = frame_buffer_number2;
    

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

    program *water_prog =
        program::make_program(shader_paths[4], shader_paths[5]);
    if (!water_prog)
        return false;

    programs.push_back(water_prog);

    return true;
}

bool update_shaders(Camera *camera, int exclude_shader = -1, glm::vec4 clip_plane = vec4(0, 1, 0, 0))
{
    set_clip_plane(clip_plane);
    // Dunes
    programs[0]->use();
    shader_array[0](programs[0], camera);
    glDisable(GL_CULL_FACE);

    // Skybox
    //glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    TEST_OPENGL_ERROR();

    programs[1]->use();
    shader_array[1](programs[1], camera);
    glDepthFunc(GL_LESS);
    TEST_OPENGL_ERROR();
    //glCullFace(GL_BACK);

    // water
    if (exclude_shader != 2)
    {
        programs[2]->use();
        shader_array[2](programs[2], camera);
    }

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

    Camera *camera = new Camera();
    Camera *water_cam = new Camera();

    Mouse::init_mouse(camera);
    glfwSetCursorPosCallback(window, Mouse::mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    bool first = true;
    while (!glfwWindowShouldClose(window))
    {
        process_input(window, camera);

        glEnable(GL_CLIP_PLANE0);

        // Render for the water reflection
        glEnable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, programs[2]->get_objects()->mm); TEST_OPENGL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glViewport(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
        update_water_cam(camera, water_cam);
        update_shaders(water_cam, -1, vec4(0, 1, 0, -0.00001f));

        // Render for the water reflection
        //glClear(GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, programs[2]->get_objects()->sm); TEST_OPENGL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glViewport(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
        update_shaders(camera, -1, vec4(0, -1, 0, -0.000001));

        // Be sure the frame buffer target the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); TEST_OPENGL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        framebuffer_size_callback(window, WIDTH, HEIGHT);
        update_shaders(camera);

        


        glfwSwapBuffers(window);
        glfwPollEvents();

        if (first)
        {
            std::cout << "End of display of first frame" << std::endl;
            first = false;
        }
    }

    return 0;
}