#include <cstddef>
#include <fstream>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iterator>
#include <ostream>

#include "camera.hh"
#include "input.hh"
#include "lib/obj.hh"
#include "mouse.hh"
#include "object_vbo.hh"
#include "program.hh"
#include "shader_func.hh"
#include "textures.hh"
#include "light.hh"

#define HEIGHT 900
#define WIDTH 1400
#define HEIGHT_SHADOW 1000
#define WIDTH_SHADOW 1000

std::vector<program *> programs;

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window,
                               int width, int height)
{
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

    // glEnable(GL_CULL_FACE);
    TEST_OPENGL_ERROR();

    glClearColor(1.0, 0.0, 0.0, 1.0);
    TEST_OPENGL_ERROR();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    return true;
}

bool init_object()
{
    // Load obj from file
    obj *dunes = obj_create("data/dunes_with_big_lake.obj");
    if (!dunes)
    {
        std::cout << "Dunes cannot be created" << std::endl;
        return false;
    }

    obj_set_vert_loc(
        dunes, 
        glGetAttribLocation(programs[0]->get_program_id(), "tangent"),
        glGetAttribLocation(programs[0]->get_program_id(), "normal_flat"),
        glGetAttribLocation(programs[0]->get_program_id(), "uv"),
        glGetAttribLocation(programs[0]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();

    obj_init(dunes);
    std::cout << "Start display" << std::endl;
    programs[0]->add_object(dunes);
    programs[0]->values[0] = loadSandTexture();
    programs[0]->values[1] = loadTexture("sand/upwind.png");
    
    

    // Load obj for skybox
    obj *skybox = obj_create("data/sphere.obj");
    obj_set_vert_loc(
        skybox, -1,
        glGetAttribLocation(programs[1]->get_program_id(), "normalFlat"),
        glGetAttribLocation(programs[1]->get_program_id(), "uv"),
        glGetAttribLocation(programs[1]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();
    obj_init(skybox);
    programs[1]->add_object(skybox);

    // Load obj for water
    obj *planeWater = obj_create("data/plane.obj");
    obj_set_vert_loc(
        planeWater, -1,
        glGetAttribLocation(programs[2]->get_program_id(), "normal_flat"),
        glGetAttribLocation(programs[2]->get_program_id(), "uv"),
        glGetAttribLocation(programs[2]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();
    obj_init(planeWater);
    programs[2]->add_object(planeWater);

    // Frame buffer reflection
    GLuint frame_buffer_number, depth_buffer, rendered_texture;
    int error = generate_render_texture(frame_buffer_number, WIDTH, HEIGHT,
                                        rendered_texture, depth_buffer);
    if (error == -1)
        std::cout << "Failed to init render reflection texture for water"
                  << std::endl;
    else
    {
        programs[2]->values[0] = frame_buffer_number;
        programs[2]->values[1] = rendered_texture;
    }
    

    // Frame buffer refraction
    GLuint frame_buffer_number2;
    error = generate_render_texture(frame_buffer_number2, WIDTH, HEIGHT,
                                    rendered_texture, depth_buffer);
    if (error == -1)
        std::cout << "Failed to init render refraction texture for water"
                  << std::endl;
    else
    {
        programs[2]->values[2] = frame_buffer_number2;
        programs[2]->values[3] = rendered_texture;
        programs[2]->values[4] = depth_buffer;
    }
    programs[2]->values[6] = loadTexture("water_normal.png", false);

    // Load Palm trees
    obj *palm = obj_create("data/palm_trees.obj");

    obj_set_vert_loc(
        palm, -1,
        glGetAttribLocation(programs[3]->get_program_id(), "normal_flat"),
        -1,
        glGetAttribLocation(programs[3]->get_program_id(), "position"));
    TEST_OPENGL_ERROR();

    obj_init(palm);
    programs[3]->add_object(palm);

    // Shadow shader - Need to have all objects
    programs[4]->add_object(dunes); 
    programs[4]->add_object(planeWater);

    return true;
}

bool init_shaders()
{
    program *sand_prog =
        program::make_program(shader_paths[0], shader_paths[1]);
    if (!sand_prog)
        return false;

    programs.push_back(sand_prog);

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

    program *palm_prog =
        program::make_program(shader_paths[6], shader_paths[7]);
    if (!palm_prog)
        return false;

    programs.push_back(palm_prog);
    
    program * shadow_prog = program::make_program(shader_paths[8], shader_paths[9]);
    if (!shadow_prog)
        return false;

    programs.push_back(shadow_prog);

    return true;
}

bool update_shaders(Camera *camera, int exclude_shader = -1,
                    glm::vec4 clip_plane = vec4(0, 1, 0, 0))
{
    set_clip_plane(clip_plane);
    // Dunes
    programs[0]->use();
    shader_array[0](programs[0], camera);
    glDisable(GL_CULL_FACE);

    // Skybox
    // glCullFace(GL_FRONT);
    if (exclude_shader != 1 && exclude_shader != -2)
    {
        glDepthFunc(GL_LEQUAL);
        TEST_OPENGL_ERROR();

        programs[1]->use();
        shader_array[1](programs[1], camera);
        glDepthFunc(GL_LESS);
        TEST_OPENGL_ERROR();
    }
    // glCullFace(GL_BACK);

    // water
    if (exclude_shader != 2 && exclude_shader != -2)
    {
        programs[2]->use();
        shader_array[2](programs[2], camera);
    }

    programs[3]->use();
    shader_array[3](programs[3], camera);

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

    // Main light creation
    GLuint fbo_shadow, depth_buffer; 
    generate_depth_texture_shadow(fbo_shadow, WIDTH_SHADOW, HEIGHT_SHADOW, depth_buffer);
    vec3 light_pos = vec3(300., 1000., 0.7);
    DirectionalLight* light = new DirectionalLight(light_pos, fbo_shadow, depth_buffer);
    set_light(light);

    Mouse::init_mouse(camera);
    glfwSetCursorPosCallback(window, Mouse::mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    bool first = true;
    while (!glfwWindowShouldClose(window))
    {
        Time::update_time_passed();
        process_input(window, camera);

        // *** WORK IN PROGRESS *** Render from lights for shadows  ...
        glViewport(0, 0, WIDTH_SHADOW, HEIGHT_SHADOW);
        glBindFramebuffer(GL_FRAMEBUFFER, light->fbo_shadow);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        //glViewport(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2); 
        //glViewport(0, 0, WIDTH, HEIGHT);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW); // Backface display 
        programs[4]->use();
        shader_array[4](programs[4], light->camera);
        glDisable(GL_CULL_FACE);

        glEnable(GL_CLIP_PLANE0);
        glViewport(0, 0, WIDTH, HEIGHT);

        // Render for the water refraction
        glBindFramebuffer(GL_FRAMEBUFFER,
                          programs[2]->values[2]);
        TEST_OPENGL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update_shaders(camera, -2, vec4(0, -1, 0, -0.000001));


        // Render for the water reflection
        glEnable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER,
                          programs[2]->values[0]);
        TEST_OPENGL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update_water_cam(camera, water_cam);
        update_shaders(water_cam, 2, vec4(0, 1, 0, -0.00001f));

        // Be sure the frame buffer target the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        TEST_OPENGL_ERROR();
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