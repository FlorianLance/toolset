
/*******************************************************************************
** Toolset-ts-imgui-gl-engine                                                 **
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

// opengl
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
    m_sizeI = {static_cast<int>(sizeI.x),static_cast<int>(sizeI.y)};
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

        if(mouseButtonsReleased[0]){
            Log::fmessage("Release click {} {}\n", m_sizeI.x(), m_sizeI.y());
            // auto rc = m_camera.screen_raycast({hoveringPixel});

            {




            }



            float x_ndc = 1.0f- (2.0f * hoveringPixel.x()) / m_sizeI.x();
            float y_ndc =(2.0f * hoveringPixel.y()) / m_sizeI.y() - 1.0f; // Flip Y since pixel coords are inverted

            Log::fmessage("x_ndc {} y_ndc {}\n", x_ndc, y_ndc);
            // Create ray direction in NDC space
            geo::Vec4f ray_dir_ndc(x_ndc, y_ndc, 1, 0);

            // Transform through inverse projection matrix
            geo::Mat4d inv_proj = inverse(m_camera.projection());
            auto ray_dir_world = inv_proj.multiply_vector(ray_dir_ndc.conv<double>());

            // Transform through inverse view matrix
            geo::Mat4d inv_view = inverse(m_camera.view());
            geo::Vec4d world_pos = inv_view.multiply_vector(ray_dir_world);
            world_pos /= world_pos.w();


            {
                geo::Vec4f ray_dir_ndc2(x_ndc, y_ndc, -1, 0);
                auto ray_dir_world2 = inv_proj.multiply_vector(ray_dir_ndc2.conv<double>());
                auto world_pos2 = inv_view.multiply_vector(ray_dir_world2);
                world_pos2 = normalize(world_pos2);

                geo::Vec4d orig(0,0,0,1);
                orig = m_camera.projection().multiply_point(orig);
                // raycast_signal(world_pos.xyz(), world_pos.xyz() + world_pos2.xyz()*10.0);
            }



            // world_pos.x() *= -1.0;
            // Normalize and return the direction vector
            // return world_pos.head<3>() / world_pos.w();


            float x = 1.0f- (2.0f * hoveringPixel.x()) / m_sizeI.x();
            float y = (2.0f * hoveringPixel.y()) / m_sizeI.y() - 1.0f; // Flip Y since pixel coords are inverted


            // // float x = (2.0f * screenPos.x()) / m_screen->width()- 1.0f;
            // float y = 1.0f - (2.0f * screenPos.y()) / m_screen->height();
            float z = 1.0f;
            geo::Vec3f ray_nds = geo::Vec3f(x, y, z);
            // Log::fmessage(" -> ray_nds {} {} {}\n", ray_nds.x(), ray_nds.y(), ray_nds.z());

            geo::Vec4f ray_clip = geo::Vec4f(ray_nds.x(),ray_nds.y(), -1.0, 1.0);
            // ray_dir_ndc.z() *= -1.f;

            geo::Vec4d ray_eye = inv_proj.multiply_vector(ray_clip.conv<double>());
            // ray_eye /= ray_eye.w();

            geo::Vec3d ray_wor = inv_view.multiply_vector(ray_eye).xyz();
            // // don't forget to normalise the vector at some point
            ray_wor = normalize(ray_wor);
            // ray_wor.y() *= -1.f;
            // Log::fmessage(" -> ray_wor {} {} {}\n", ray_wor.x(), ray_wor.y(), ray_wor.z());


            // raycast_signal(m_camera.position(), m_camera.direction());
            // raycast_signal(world_pos.xyz(), geo::Vec3d(0,0,1));

            // raycast_signal(m_camera.position(), ray_wor);

            // float imageAspectRatio = m_sizeI.x() / m_sizeI.y(); // assuming width > height
            // float Px = (2 * ((x + 0.5) / m_sizeI.x()) - 1) * tan(m_camera.fov() / 2 * tool::PI<float> / 180) * imageAspectRatio;
            // float Py = (1 - 2 * ((y + 0.5) / m_sizeI.y()) * tan(m_camera.fov() / 2 * tool::PI<float> / 180));
            // geo::Vec3f rayOrigin = geo::Vec3f(0, 0, 0);
            // // Matrix44f cameraToWorld;
            // // cameraToWorld.set(...); // set matrix
            // geo::Vec3f rayOriginWorld, rayPWorld;
            // // m_camera.projection();
            // auto c = inverse(m_camera.projection());
            // c *= inverse(m_camera.view());

            // c.multiply_vector()

            // cameraToWorld.multVectMatrix(rayOrigin, rayOriginWorld);
            // cameraToWorld.multVectMatrix(Vec3f(Px, Py, -1), rayPWorld);
            // geo::Vec3f rayDirection = rayPWorld - rayOriginWorld;
            // rayDirection.normalize(); // it's a direction so don't forget to normalize


            // // First, transform the 3D point from world space to camera space.
            // // It is inefficient to compute the inverse of the cameraToWorld matrix
            // // in this function; it should be computed just once outside the function,
            // // and the worldToCamera matrix should be passed in instead.
            // Vec3f pCamera;
            // Matrix44f worldToCamera = cameraToWorld.inverse();
            // worldToCamera.multVecMatrix(pWorld, pCamera);

            // // Coordinates of the point on the canvas, using perspective projection.
            // Vec2f pScreen;
            // pScreen.x = pCamera.x / -pCamera.z;
            // pScreen.y = pCamera.y / -pCamera.z;

            // // If the absolute values of the x- or y-coordinates exceed the canvas width
            // // or height, respectively, the point is not visible.
            // if (std::abs(pScreen.x) > canvasWidth || std::abs(pScreen.y) > canvasHeight)
            //     return false;

            // // Normalize the coordinates to range [0,1].
            // Vec2f pNDC;
            // pNDC.x = (pScreen.x + canvasWidth / 2) / canvasWidth;
            // pNDC.y = (pScreen.y + canvasHeight / 2) / canvasHeight;

            // // Convert to pixel coordinates and invert the y-coordinate.
            // pRaster.x = std::floor(pNDC.x * imageWidth);
            // pRaster.y = std::floor((1 - pNDC.y) * imageHeight);
        }

    }
}

