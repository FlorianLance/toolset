
/*******************************************************************************
** Toolset-3d-engine                                                          **
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

#pragma once

// base
#include "graphics/camera/camera.hpp"

// opengl-utility
#include "opengl/buffer/framebuffer_object.hpp"
#include "opengl/texture/texture_2d_tbo.hpp"

namespace tool::graphics {


struct PixelInfo{
    std::uint32_t objectID = 0;
    std::uint32_t frawID = 0;
    std::uint32_t primID = 0;
};

class ImguiFboUiDrawer{

public:

    ImguiFboUiDrawer();

    auto initialize() -> void;
    auto resize(const geo::Pt2<int> &size) -> void;

    inline auto bind() -> void {m_fbo.bind();}
    inline auto unbind() -> void {m_fbo.unbind();}
    auto draw() -> void;
    auto set_gl_states(geo::Pt4f color = {0.0f, 0.0f, 0.0f, 1.0f}) -> void;
    auto reset_states() noexcept-> void {m_cameraUpdated=false;m_screenUpdated=false;}
    auto update_viewport() -> void;

    inline auto camera() -> graphics::Camera* {return &m_camera;}
    auto texture_size() const noexcept -> geo::Pt2<int>{return {m_texture.width(),m_texture.height()};}
    auto texture_id() const noexcept -> GLuint {return m_texture.id();}
    auto is_camera_updated() const noexcept -> bool{return m_cameraUpdated;}
    auto is_screen_updated() const noexcept -> bool{return m_screenUpdated;}


    double rotationSpeed = 0.05;
    float scrollSpeed = 0.1f;
    float movingSpeed = 0.05f;
    float translateSpeed = 0.01f;
    bool invertTexture = true;

    bool initialized = false;

    geo::Pt2<int> hoveringPixel = {-1,-1};
    std::array<bool, 5> mouseButtonsPressed;
    std::array<bool, 5> mouseButtonsReleased;

private:

    auto restore_viewport() -> void;
    auto check_inputs() -> void;
    GLint m_viewport[4];
    gl::FBO m_fbo;
    gl::Texture2D m_texture;
    gl::RBO m_depthTexture;
    graphics::Camera m_camera;       
    graphics::Screen m_screen;

    bool m_cameraUpdated = false;
    bool m_screenUpdated = false;

};
}
