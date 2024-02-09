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
#include "thirdparty/sigslot/signal.hpp"

// local
#include "camera/settings/dc_model_settings.hpp"
#include "camera/settings/dc_filters_settings.hpp"
#include "imgui_dc_cloud_drawer.hpp"
#include "imgui_fbo_ui_drawer.hpp"

namespace tool::graphics {

struct DCCloudsSceneDrawer{

    auto initialize(size_t nbDrawers) -> void;
    auto reset() -> void;

    inline auto has_to_redraw_clouds() const noexcept -> bool{
        return fboD.is_camera_updated() || fboD.is_screen_updated() || m_redrawClouds;
    }

    auto update_from_frame(size_t idCloud, std::shared_ptr<cam::DCFrame> frame) -> bool;
    auto update_from_colored_cloud_data(size_t idCloud, const geo::ColoredCloudData &cloud) -> bool;

    auto draw_clouds_to_fbo() -> void;
    auto draw_clouds_to_fbo(ImguiFboUiDrawer &fboD) -> void;
    auto draw_fbo(tool::geo::Pt2<int> size) -> void;

    // imgui
    // # child
    auto draw_color_texture_imgui_child(size_t idCloud, const std::string &windowName, geo::Pt2f sizeWindow) -> void;
    auto draw_depth_texture_imgui_child(size_t idCloud, const std::string &windowName, geo::Pt2f sizeWindow) -> void;
    auto draw_infra_texture_imgui_child(size_t idCloud, const std::string &windowName, geo::Pt2f sizeWindow) -> void;
    // # position
    auto draw_color_texture_imgui_at_position(size_t idCloud, const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::string_view text) -> void;
    auto draw_depth_texture_imgui_at_position(size_t idCloud, const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::optional<std::string> text = {}) -> void;
    auto draw_infra_texture_imgui_at_position(size_t idCloud, const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::optional<std::string> text = {}) -> void;
    // # tab
    auto draw_all_clouds_drawers_in_one_tab(bool drawColor=true, bool drawDepth=true, bool drawInfra=true, bool drawCloud=true, std::string_view cloudTabName = "Cloud"sv) -> void;
    auto draw_cloud_drawer_tab(size_t idDrawer, bool focusWindow, std::string_view name, bool drawColor=true, bool drawDepth=true, bool drawInfra=true, bool drawCloud=true, std::optional<geo::Pt2<int>> sizeW = std::nullopt) -> void;

    // settings
    auto update_cloud_display_settings(size_t idCloud, const DCCloudDisplaySettings &cloudDisplay) -> void;
    auto update_scene_display_settings(const DCSceneDisplaySettings &sdS) -> void;
    auto update_model(size_t idCloud, const cam::DCModelSettings &model)  -> void;

    auto update_filters_settings(size_t idCloud, const cam::DCFiltersSettings &filters) -> void;

    // signals
    sigslot::signal<size_t, size_t, geo::Pt2<int>, ColorRGBA8> mouse_pressed_color_signal;
    sigslot::signal<size_t, size_t, geo::Pt2<int>, ColorRGB8> mouse_pressed_depth_signal;
    sigslot::signal<size_t, size_t, geo::Pt2<int>, ColorRGB8> mouse_pressed_infra_signal;

    // drawers
    std::vector<DCCloudDrawer> cloudsD;
    std::unique_ptr<gl::GridDrawer> gridD = nullptr;    
    ImguiFboUiDrawer fboD;
    // settings
    DCSceneDisplaySettings display;

    // test
    std::unique_ptr<gl::TriangleLineDrawer> plane1D = nullptr;
    std::unique_ptr<gl::SphereDrawer> plane1PointsD = nullptr;

    geo::Matrix<float,4,4> plane1Tr = geo::Mat4f::identity();

private:

    bool m_redrawClouds = false;
    unsigned int m_tabId = 0;

    auto compute_textures_rectangles(geo::Pt2f parentSize, const std::vector<const gl::Texture2D*> &textures) -> std::vector<std::tuple<geo::Pt2f,geo::Pt2f>>;
};
}
