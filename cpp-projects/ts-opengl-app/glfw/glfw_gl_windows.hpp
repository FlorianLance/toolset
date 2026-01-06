
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

// opengl
#include "opengl/shader/shader_program.hpp"

// glfw
#include <GLFW/glfw3.h>

namespace tool::gl  {

class GlfwGlWindow{

public :

    GlfwGlWindow();
    ~GlfwGlWindow();

    auto init(int width, int height) -> bool;
    auto start() -> void;

private :

    auto init_glfw() -> void;
    auto init_glew() -> bool;
    auto create_window(int width, int height) -> bool;
    auto process_input(GLFWwindow *window) -> void;
    static auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void;

private :

    GLFWwindow *window = nullptr;

    ShaderProgram meshShader;

    // transformations
    geo::Mat4<double> projection;
    geo::Mat4<double> model;
    geo::Mat4<double> view;
};

}

