
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

// std
#include <iostream>

// opengl
#include "opengl/gl_texture.hpp"
#include "opengl/vao.hpp"
#include "opengl/shapes.hpp"
#include "opengl/shader/shader.hpp"

// glfw
#include <GLFW/glfw3.h>

namespace tool::gl  {

class GlfwGlWindow{


public :

    GlfwGlWindow();
    ~GlfwGlWindow();

    bool init(int width, int height);
    void start();

private :

    void init_glfw();
    bool init_glew();
    bool create_window(int width, int height);
    void process_input(GLFWwindow *window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

private :

    GLFWwindow *window = nullptr;


//    MeshVAO triVao;
    ShaderProgram meshShader;

//    Camera camera;

    // transformations
    geo::Mat4<double> projection;
    geo::Mat4<double> model;
    geo::Mat4<double> view;


};

}

