
/*******************************************************************************
** Toolset-dc-manager                                                         **
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

#include "dcm_gl_window.hpp"

// base
#include "utility/logger.hpp"
#include "utility/paths.hpp"

// opengl-utility
#include "opengl/buffer/framebuffer_object.hpp"
#include "opengl/gl_functions.hpp"

// 3d-engine
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "engine/managers.hpp"
#include "imgui-tb/imgui_dc_ui_drawer.hpp"

using namespace tool::gl;
using namespace tool::graphics;

DCMGlWindow::DCMGlWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context) :
    BaseSfmlGlWindow(title, screen, context) {

    DCUIDrawer::set_manager_mode(true);
}

auto DCMGlWindow::init_shaders() -> bool{

    const std::vector<ShaderType> VS_FS     = {ShaderType::vertex, ShaderType::fragment};
    const std::vector<ShaderType> VS_FS_GS  = {ShaderType::vertex, ShaderType::fragment, ShaderType::geometry};

    std::vector<std::pair<std::string, const std::vector<ShaderType>*>> shadersNames={
        {"solid",     &VS_FS},
        {"cloud",     &VS_FS},
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

        if(paths.size() == 0){
            continue;
        }

        Logger::message(std::format("Load shader [{}] from path [{}].\n", shaderName.first, paths.front()));
        if(!Managers::shaders->load_shader(shaderName.first, paths)){
            Logger::error("Fail to load all shaders.\n");
            Managers::shaders->unbind();
            return false;
        }
        Logger::message(std::format("Shader [{}] loaded.\n", shaderName.first));
    }
    Managers::shaders->unbind();

    return true;
}


auto DCMGlWindow::initialize_gl() -> bool{

    Logger::message("K4ScanerManagerWindow::initialize_gl\n");

    // flags
    GL::enable(GL_MULTISAMPLE); // msaa
    GL::enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    GL::enable(GL_PROGRAM_POINT_SIZE);

    // implot
    //    ImPlot::CreateContext();

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

auto DCMGlWindow::initialize_imgui() -> void{
    // 51 / 105 / 173
    // 0.2 / 0.41 / 0.68
//    ImGui::GetStyle().Colors[ImGuiCol_TabActive]          = ImVec4(0.3f, 0.51f, 0.78f, 1.0f);
}

auto DCMGlWindow::draw_gl() -> void{

    GL::enable(GL_DEPTH_TEST);
    GL::enable(GL_STENCIL_TEST);

    // clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    gl::FBO::unbind();
}






