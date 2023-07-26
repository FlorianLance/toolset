
/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

#include "k4sg_gl_window.hpp"

// std
#include <format>

// base
#include "utility/logger.hpp"
#include "utility/paths.hpp"
#include "utility/cmd_args.hpp"
#include "utility/benchmark.hpp"

// opengl-utilityh
#include "opengl/buffer/framebuffer_object.hpp"

// 3d-engine
#include "engine/managers.hpp"


using namespace tool;
using namespace tool::gl;
using namespace tool::graphics;

K4SGGlWindow::K4SGGlWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context) :
    BaseSfmlGlWindow(title, screen, context) {
}

auto K4SGGlWindow::init_shaders() -> bool{

    const std::vector<Shader::Type> VS_FS         = {Shader::Type::vertex,Shader::Type::fragment};
    const std::vector<Shader::Type> VS_FS_GS      = {Shader::Type::vertex,Shader::Type::fragment, Shader::Type::geometry};

    std::vector<std::pair<std::string, const std::vector<Shader::Type>*>> shadersNames={
        {"solid",&VS_FS},
        {"cloud",&VS_FS},
        {"voxelCloud",&VS_FS_GS}
    };

    if(!Paths::shadersDir.has_value()){
        Logger::error("No shaders directory found.\n");
        return false;
    }

    // find shaders dir path
    for(const auto &shaderName : shadersNames){

        std::vector<std::string> paths;
        paths.reserve(shaderName.second->size());
        for(const auto &shaderType : *shaderName.second){
            paths.push_back(Paths::get_shader(shaderName.first, Shader::get_ext(shaderType)));
        }

        Logger::message(std::format("Load shader [{}].\n", shaderName.first));
        if(!Managers::shaders->load_shader(shaderName.first, paths)){
            Logger::error("Fail to load all shaders.\n");
            Managers::shaders->unbind();
            return false;
        }
        Logger::message(std::format("Shader [{}] loaded with path [{}].\n", shaderName.first, paths.front()));
    }
    Managers::shaders->unbind();

    return true;
}


auto K4SGGlWindow::initialize_gl() -> bool{

    Logger::message("K4ScanerGrabberWindow::initialize_gl\n");

    // flags
    glEnable(GL_MULTISAMPLE); // msaa
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Managers::initialize();

    // shaders
    Logger::message("Init shaders\n");
    if(!init_shaders()){
        return m_glInitialized = false;
    }

    // camera
    Logger::message("Init camera\n");
    m_camera.set_direction(0.,0.,0.);

    VAO::unbind();

    return m_glInitialized = true;
}

auto K4SGGlWindow::draw_gl() -> void{

    BenchGuard g("update_gl");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl::FBO::unbind();
}






