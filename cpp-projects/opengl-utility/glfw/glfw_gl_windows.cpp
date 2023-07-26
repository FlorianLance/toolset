/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

using namespace tool::gl;

GlfwGlWindow::GlfwGlWindow(){
}

GlfwGlWindow::~GlfwGlWindow(){
    glfwTerminate();
}

bool GlfwGlWindow::init(int width, int height){

    init_glfw();

    if(!create_window(width, height)){
        return false;
    }

    if(!init_glew()){
        return false;
    }

    // test
//    triVao.init();
    std::vector<std::uint32_t> ids = {0,1,2};
//    triVao.update_data(&triangle_vertices[0], 3, ids.data(), 1, GL_STATIC_DRAW);

    // init shaders
//    if(!meshShader.load_from_files("G:/DEV/Projets/RealityEx/expyvr/ressources/shaders/depth_camera_mesh.vert",
//                                   "G:/DEV/Projets/RealityEx/expyvr/ressources/shaders/depth_camera_mesh.frag")){
//        std::cerr << "Failed to load shaders. \n";
//        return false;
//    }


    return true;
}

void GlfwGlWindow::start(){

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

        meshShader.use();
        meshShader.set_uniform("view",         view.conv<float>());
        meshShader.set_uniform("model",        model.conv<float>());
        meshShader.set_uniform("projection",   projection.conv<float>());

        // draw
//        triVao.draw();


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void GlfwGlWindow::init_glfw(){

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
}

bool GlfwGlWindow::init_glew(){

    // init glew
    glewExperimental=true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return false;
    }
    return true;
}

bool GlfwGlWindow::create_window(int width, int height){

    window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == nullptr){
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, &GlfwGlWindow::framebuffer_size_callback);
    return true;
}

void GlfwGlWindow::process_input(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

void GlfwGlWindow::framebuffer_size_callback(GLFWwindow *window, int width, int height){
    static_cast<void>(window);
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
