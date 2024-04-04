
/*******************************************************************************
** Toolset-dc-grabber                                                         **
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

#include "dcg_gl_window.hpp"

// std
#include <format>

// base
#include "utility/logger.hpp"
#include "utility/paths.hpp"
#include "utility/cmd_args.hpp"

// opengl-utilityh
#include "opengl/gl_functions.hpp"
#include "opengl/buffer/framebuffer_object.hpp"

// 3d-engine
#include "engine/managers.hpp"


using namespace tool;
using namespace tool::gl;
using namespace tool::graphics;

DCGGlWindow::DCGGlWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context) :
    BaseSfmlGlWindow(title, screen, context) {
}

auto DCGGlWindow::init_shaders() -> bool{

    Logger::log("DCGGlWindow::init_shaders\n");

    const std::vector<ShaderType> VS_FS         = {ShaderType::vertex,ShaderType::fragment};
    const std::vector<ShaderType> VS_FS_GS      = {ShaderType::vertex,ShaderType::fragment, ShaderType::geometry};

    std::vector<std::pair<std::string, const std::vector<ShaderType>*>> shadersNames={
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
            paths.push_back(Paths::get_shader(shaderName.first, get_ext(shaderType)));
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


auto DCGGlWindow::initialize_gl() -> bool{

    Logger::log("DCGGlWindow::initialize_gl\n");

    // flags
    GL::enable(GL_MULTISAMPLE); // msaa
    GL::enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    GL::enable(GL_PROGRAM_POINT_SIZE);

    Managers::initialize();

    // shaders
    if(!init_shaders()){
        return m_glInitialized = false;
    }

    // camera
    m_camera.set_direction(0.,0.,0.);

    VAO::unbind();

    return m_glInitialized = true;
}

auto DCGGlWindow::draw_gl() -> void{

    GL::enable(GL_DEPTH_TEST);
    GL::enable(GL_STENCIL_TEST);

    // clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    gl::FBO::unbind();
}






