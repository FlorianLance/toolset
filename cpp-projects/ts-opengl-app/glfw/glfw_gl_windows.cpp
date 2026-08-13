/*******************************************************************************
** Toolset-ts-opengl-app                                                      **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "glfw_gl_windows.hpp"

// std
#include <iostream>

// opengl
#include "opengl/texture/texture_2d_tbo.hpp"
#include "opengl/vao.hpp"
#include "opengl/shapes.hpp"

#include "utility/paths.hpp"

using namespace tool::gl;

GlfwGlWindow::GlfwGlWindow(){
}

GlfwGlWindow::~GlfwGlWindow(){
    glfwTerminate();
}

auto GlfwGlWindow::init(int width, int height) -> bool{

    init_glfw();

    if(!create_window(width, height)){
        return false;
    }

    m_screen.resize(width, height);
    m_screen.set_fov(60.0);
    m_screen.set_range(0.1, 100.);


    if(!init_glew()){
        return false;
    }

    axesD.initialize(0.15f);
    gridD.initialize(0.2f, 0.2f, 1000.f, 1000.f);

    std::vector<std::string> paths;
    paths.push_back("D:/DEV/Perso/toolset/cpp-projects/_resources/shaders/lines.vert.glsl");
    paths.push_back("D:/DEV/Perso/toolset/cpp-projects/_resources/shaders/lines.frag.glsl");

    // init shaders
    if(!linesShader.load_from_files(paths)){
        std::cerr << "Failed to load shaders. \n";
        return false;
    }

    return true;
}

auto GlfwGlWindow::start() -> void{

    // render loop
    while (!glfwWindowShouldClose(window)){
        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // set polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        linesShader.use();
        linesShader.set_uniform_matrix("view"sv,         m_camera.view_matrix().conv<float>(), true);
        linesShader.set_uniform_matrix("model"sv,        model.conv<float>(), true);
        linesShader.set_uniform_matrix("projection"sv,   m_screen.projection().conv<float>(), true);

        // draw
        axesD.draw();
        gridD.draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

auto GlfwGlWindow::init_glfw() -> void{

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
}

auto GlfwGlWindow::init_glew() -> bool{

    // init glew
    glewExperimental=true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return false;
    }
    return true;
}

auto GlfwGlWindow::create_window(int width, int height) -> bool{

    window = glfwCreateWindow(width, height, "GLFW OpenGL window", NULL, NULL);
    if (window == nullptr){
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, &GlfwGlWindow::framebuffer_size_callback);
    return true;
}

auto GlfwGlWindow::process_input(GLFWwindow *window) -> void{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        m_camera.move_left(0.05);
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        m_camera.move_right(0.05);
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        m_camera.move_up(0.05);
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        m_camera.move_down(0.05);
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        m_camera.rotate({1.0,0,0});
    }

    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS){
        m_camera.rotate({0.,1.0,0});
    }

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
        m_camera.rotate({0.,0.,1.});
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
        std::cout << "click 1";
    }
}

auto GlfwGlWindow::framebuffer_size_callback(GLFWwindow *window, int width, int height) -> void{
    static_cast<void>(window);
    std::cout << "framebuffer_size_callback" << width << height << "\n";
    // cam.
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
