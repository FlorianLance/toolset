
/*******************************************************************************
** Toolset-imgui-opengl-engine                                                **
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


#include "imgui_fbo_ui_drawer.hpp"

// base
#include "utility/logger.hpp"

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

// opengl-utility
#include "opengl/gl_functions.hpp"

// local
#include "imgui-tb/imgui_helper.hpp"

using namespace tool::graphics;
using namespace tool::gl;

ImguiFboUiDrawer::ImguiFboUiDrawer() : m_camera(&m_screen, {0,0,0}, {0,0,1}){
    m_camera.set_fov(60.);
}

auto ImguiFboUiDrawer::initialize() -> void{
    m_fbo.clean();
    m_fbo.initialize();
    resize({1600,1200});
    initialized = true;
}

auto ImguiFboUiDrawer::resize(const geo::Pt2<int> &size) -> void{

    if(m_texture.width() == size.x() && m_texture.height() == size.y()){
        return;
    }
    m_screenUpdated = true;

    m_screen.resize(size.x(), size.y());
    m_camera.update_projection();

    // color texture
    m_texture.clean();
    m_texture.init_image_8u(size.x(),size.y(), 3);

    TextureOptions options;
    options.minFilter = TextureMinFilter::linear;
    options.magFilter = TextureMagFilter::linear;
    m_texture.set_texture_options(options);

    // picking texture
    // m_pickingTexture.clean();
    // m_pickingTexture.init_image_32u(size.x(),size.y(), 3);

    // options.minFilter = TextureMinFilter::nearest;
    // options.magFilter = TextureMagFilter::nearest;
    // m_pickingTexture.set_texture_options(options);
    m_depthTexture.clean();
    m_depthTexture.initialize();
    m_depthTexture.bind();
    m_depthTexture.set_data_storage(size.x(), size.y());

    m_fbo.attach_color_textures({
        &m_texture//, &m_pickingTexture
    });

    m_fbo.attach_depth_buffer(m_depthTexture);

    m_fbo.set_draw_buffers({
        tool::gl::FrameBuffer::Color0,
    });
    m_fbo.check_validity();
}

auto ImguiFboUiDrawer::update_viewport() -> void{

    if(m_texture.id() == 0){
        return;
    }

    glGetIntegerv(GL_VIEWPORT, m_viewport);
    glViewport(0,0, m_texture.width(), m_texture.height());
}


auto ImguiFboUiDrawer::set_gl_states(geo::Pt4f color) -> void{
    glClearColor(color.x(),color.y(),color.z(),color.w());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL::enable(GL_DEPTH_TEST);
}


auto ImguiFboUiDrawer::draw() -> void{

    auto size    = content_region_size_available();
    float scale  = std::min(1.f*size.y() / m_texture.height(),  1.f*size.x() / m_texture.width());
    ImVec2 sizeI = ImVec2(m_texture.width() * scale,m_texture.height() * scale);

    if(m_texture.id() == 0){
        ImGui::Text("Texture not initialized.");
    }else{
        if(invertTexture){
            ImGui::Image(reinterpret_cast<ImTextureID*>(m_texture.id()), sizeI,  ImVec2(0,1), ImVec2(1,0));
        }else{
            ImGui::Image(reinterpret_cast<ImTextureID*>(m_texture.id()), sizeI,  ImVec2(0,0), ImVec2(1,1));
        }

        auto io       = ImGui::GetIO();
        auto min      = ImGui::GetItemRectMin();
        auto size     = ImGui::GetItemRectSize();
        auto mousePos = io.MousePos;
        auto diff     = geo::Pt2f{mousePos.x-min.x, mousePos.y-min.y};

        if(diff.x() > 0 && diff.x() < size.x && diff.y() > 0 && diff.y() < size.y){

            hoveringPixel = (diff).conv<int>();
            for(int ii = 0; ii < 5; ++ii){
                mouseButtonsPressed[ii]  = io.MouseDown[ii];
                mouseButtonsReleased[ii] = io.MouseReleased[ii];
            }

        }else{
            hoveringPixel = {-1,-1};
            for(int ii = 0; ii < 5; ++ii){
                mouseButtonsPressed[ii]     = false;
                mouseButtonsReleased[ii]    = false;
            }
        }

    }

    check_inputs();
}

auto ImguiFboUiDrawer::restore_viewport() -> void{
    // restore
    gl::FBO::unbind();
    glViewport(
        static_cast<GLsizei>(m_viewport[0]),
        static_cast<GLsizei>(m_viewport[1]),
        static_cast<GLsizei>(m_viewport[2]),
        static_cast<GLsizei>(m_viewport[3])
    );
}

auto ImguiFboUiDrawer::check_inputs() -> void{

    ImGuiIO& io = ImGui::GetIO();
    if(ImGui::IsItemHovered()){

        const double xoffset = io.MouseDelta.x;
        const double yoffset = -io.MouseDelta.y;
        const double wheel   = io.MouseWheel;

        if(ImGui::IsMouseDown(0)){
            m_camera.set_direction(rotationSpeed*xoffset,rotationSpeed*yoffset,0.);
            m_cameraUpdated = true;
        }
        if(ImGui::IsMouseDown(1)){
            m_camera.set_direction(0.,0.,rotationSpeed*xoffset);
            m_cameraUpdated = true;
        }
        if(ImGui::IsMouseDown(2)){
            m_camera.move_up(translateSpeed*yoffset);
            m_camera.move_right(translateSpeed*xoffset);
            m_cameraUpdated = true;
        }
        if(io.MouseWheel != 0.f){
            m_camera.move_front(scrollSpeed*wheel);
            m_cameraUpdated = true;
        }

        // up key        
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_UpArrow)){
            m_camera.move_front(movingSpeed);
            m_cameraUpdated = true;
        }
        // down key
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_DownArrow)){
            m_camera.move_back(movingSpeed);
            m_cameraUpdated = true;
        }
        // left key
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftArrow)){
            m_camera.move_left(movingSpeed);
            m_cameraUpdated = true;
        }
        // right key
        if(ImGui::IsKeyDown(ImGuiKey::ImGuiKey_RightArrow)){
            m_camera.move_right(movingSpeed);
            m_cameraUpdated = true;
        }
        // R key
        if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R, false)){
            m_camera.reset_init_values();
            m_cameraUpdated = true;
        }
    }
}

