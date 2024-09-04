
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

#include "imgui_dc_clouds_scene_drawer.hpp"

// std
#include <format>

// base
#include "thirdparty/BinPack2D/binpack2d.hpp"
#include "utility/logger.hpp"

// local
#include "imgui-tb/imgui_helper.hpp"
#include "imgui_ui_drawer.hpp"
#include "opengl/shader/shaders_manager.hpp"

using namespace tool;
using namespace tool::graphics;
using namespace tool::cam;
using namespace tool::geo;

auto DCCloudsSceneDrawer::initialize(size_t nbDrawers) -> void {

    fboD.init();
    cloudsD.resize(nbDrawers);

    for(auto &cloudD : cloudsD){
        cloudD = std::make_unique<DCCloudDrawer>();
        cloudD->initialize();                
    }

    gridD.initialize(1.f,1.f, 20, 20, true);
    filteringPlanesDotD.initialize(0.05f);
}

auto DCCloudsSceneDrawer::reset() -> void{
    for(auto &cloudD : cloudsD){
        cloudD.reset();
    }
}

auto DCCloudsSceneDrawer::update_from_frame(size_t idCloud, std::shared_ptr<cam::DCFrame> frame) -> bool {
    if(idCloud < cloudsD.size()){
        return cloudsD[idCloud]->init_from_frame(std::move(frame));
    }
    return false;
}

auto DCCloudsSceneDrawer::update_from_colored_cloud_data(size_t idCloud, const ColoredCloudData &cloud) -> bool{
    if(idCloud < cloudsD.size()){
        return cloudsD[idCloud]->init_from_colored_cloud_data(cloud);
    }
    return false;
}

auto DCCloudsSceneDrawer::draw_clouds_to_fbo() -> void{
    draw_clouds_to_fbo(fboD);
    fboD.reset_states();
    m_redrawClouds = false;
}

auto DCCloudsSceneDrawer::draw_clouds_to_fbo(ImguiFboUiDrawer &fboD) -> void {

    if(fboD.texture_id() == 0){
        return;
    }

    fboD.bind();
    fboD.update_viewport();
    fboD.set_gl_states(display.backgroundColor);

    auto solidShader        = ShadersManager::get_instance()->get_ptr("solid");
    auto pointsCloudShader  = ShadersManager::get_instance()->get_ptr("cloud");
    auto circlesCloudShader = ShadersManager::get_instance()->get_ptr("voxelCloud");

    if(display.drawGrid){
        if(auto shader = solidShader){

            shader->use();
            // transforms
            shader->set_uniform_matrix("view", fboD.camera()->view().conv<float>());
            shader->set_uniform_matrix("projection", fboD.camera()->projection().conv<float>());
            shader->set_uniform_matrix("model"sv, Mat4f::identity());
            // colors
            shader->set_uniform("enable_unicolor", true);
            shader->set_uniform("unicolor", Pt4f{1.f,1.f,1.f,1.f});
            gridD.draw();
        }
    }

    size_t idC = 0;
    for(auto &cloudD : cloudsD){

        if(display.drawOnlyCloudId != -1){
            if(idC++ != display.drawOnlyCloudId){
                continue;
            }
        }

        if(auto shader = cloudD->display.circles ? circlesCloudShader : pointsCloudShader){
            shader->use();
            // transforms
            shader->set_uniform_matrix("view"sv, fboD.camera()->view().conv<float>());
            shader->set_uniform_matrix("projection"sv, fboD.camera()->projection().conv<float>());
            shader->set_uniform_matrix("model"sv, cloudD->model);
            // camera
            shader->set_uniform("camera_position", fboD.camera()->position().conv<float>());
            shader->set_uniform("camera_direction", fboD.camera()->direction().conv<float>());
            //shader->set_uniform_matrix("transposed_inverse_model"sv, transpose(inverse(cloudD->model)));
            // color
            shader->set_uniform("enable_unicolor", cloudD->display.forceColor);
            shader->set_uniform("unicolor", cloudD->display.unicolor);
            shader->set_uniform("factor_unicolor", cloudD->display.factorUnicolor);            
            // geometry
            shader->set_uniform("backFaceCulling", cloudD->display.backFaceCulling);

            if(cloudD->display.circles){
                shader->set_uniform("squareSize", cloudD->display.squareSize);
                shader->set_uniform("circleRadius", cloudD->display.circleRadius);
            }else{
                shader->set_uniform("pointSize", cloudD->display.pointSize);
            }
            
            if(cloudD->display.showCapture){
                cloudD->cpD.draw();                
            }

        }else{
            Logger::error("[DCCloudsSceneDrawer] Shaders with aliases \"cloud\" and \"voxelCloud\" must be available in the shader manager.\n");
            break;
        }

        if(auto shader = solidShader){

            shader->use();
            // transforms
            shader->set_uniform_matrix("view", fboD.camera()->view().conv<float>());
            shader->set_uniform_matrix("projection", fboD.camera()->projection().conv<float>());
            shader->set_uniform_matrix("model"sv, cloudD->model);            

            // color
            shader->set_uniform("enable_unicolor", true);
            shader->set_uniform("unicolor", cloudD->display.unicolor);
            if(cloudD->display.showCameraFrustum && cloudD->display.showCapture && m_displayFrustum){
                cloudD->frustumD.draw();
                // shader->set_uniform("enable_unicolor", false);
                // cloudD->normalsD.draw();
                // shader->set_uniform("enable_unicolor", true);
            }


            // body tracking
            if(cloudD->display.showBodyTracking && cloudD->display.showCapture){
                for(size_t ii = 0; ii < cloudD->nbBodies; ++ii){
                    shader->set_uniform("unicolor", Pt4f{1.f,0.f,0.f, 1.f});
                    for(size_t jj = 0; jj < cloudD->jointsModels[ii].size(); ++jj){
                        const auto &jm = cloudD->jointsModels[ii][jj];
                        if(std::get<0>(jm)){
                            shader->set_uniform_matrix("model", std::get<1>(jm) * cloudD->model);
                            cloudD->btJointD.draw();
                        }
                    }
                }
            }


            // filtering planes
            if(cloudD->filtersS.filterDepthWithCloud && cloudD->display.showFilteringGizmos && cloudD->display.showCapture && m_displayGizmos){

                if(cloudD->filtersS.p1FMode != PlaneFilteringMode::None){
                    auto p1 = cloudD->filtersS.p1A;
                    auto p2 = cloudD->filtersS.p1B;
                    auto p3 = cloudD->filtersS.p1C;
                    Pt3f meanPt    = (p1+p2+p3)/3.f;

                    shader->set_uniform_matrix("model",  transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, p1) * cloudD->model);
                    shader->set_uniform("unicolor", Pt4f{0.f,1.f,0.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform_matrix("model", transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, p2) * cloudD->model);
                    shader->set_uniform("unicolor", Pt4f{1.f,0.f,0.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform_matrix("model", transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, p3) * cloudD->model);
                    shader->set_uniform("unicolor", Pt4f{0.f,0.f,1.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform_matrix("model", transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, meanPt) * cloudD->model);
                    shader->set_uniform("unicolor", Pt4f{0.f,1.f,1.f, 1.f});
                    filteringPlanesDotD.draw();


                    shader->set_uniform("unicolor", Pt4f{1.f,0.f,0.f, 1.f});
                    shader->set_uniform_matrix("model", cloudD->model);

                    auto v1 = vec(meanPt,p1);
                    auto v2 = vec(meanPt,p2);
                    auto v3 = vec(meanPt,p3);

                    for(int ii = 0; ii < 40; ++ii){
                        auto f = (-20+ii)*0.05f;
                        std::array<Pt3f, 3> dataP = {f*v1 + p1, f*v2 + p2, f*v3 + p3};
                        cloudD->planeFilteringLinesD.update(dataP);
                        cloudD->planeFilteringLinesD.draw();
                    }
                }

                if(cloudD->filtersS.removeFromPointDistance){

                    auto sp = cloudD->filtersS.pSphere;
                    shader->set_uniform_matrix("model", transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, sp) * cloudD->model);
                    shader->set_uniform("unicolor", Pt4f{0.5f,0.5f,1.f, 1.f});
                    filteringPlanesDotD.draw();
                }

                if(cloudD->filtersS.keepOnlyPointsInsideOOB){

                    shader->set_uniform_matrix("model",  cloudD->model);
                    shader->set_uniform("unicolor", Pt4f{0.5f,0.5f,0.5f, 1.f});
                    cloudD->oobLinesD.update(cloudD->filtersS.oob);
                    cloudD->oobLinesD.draw();
                }
            }

        }else{
            Logger::error("[DCCloudsSceneDrawer] Shaders with aliases \"solid\" must be available in the shader manager.\n");
            break;
        }
    }

    fboD.unbind();
}

auto DCCloudsSceneDrawer::draw_color_texture_imgui_child(size_t idCloud, const std::string &windowName, Pt2f sizeWindow) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->colorD.draw_child(windowName, sizeWindow);

    // check mouse inputs
    const auto &hp = cD->colorD.hoveringPixel;
    for(size_t idB = 0; idB < cD->colorD.mouseButtonsPressed.size(); ++idB){
        if(cD->lastFrame != nullptr && cD->colorD.mouseButtonsPressed[idB]){
            if(hp.x() >= 0 && hp.x() < static_cast<int>(cD->lastFrame->rgbaColor.width)  &&
                hp.y() >= 0 && hp.y() < static_cast<int>(cD->lastFrame->rgbaColor.height) &&
                !cD->lastFrame->rgbaColor.empty()){
                mouse_pressed_color_signal(idCloud, idB, hp, cD->lastFrame->rgbaColor[hp.y() * cD->lastFrame->rgbaColor.width + hp.x()]);
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_depth_texture_imgui_child(size_t idCloud, const std::string &windowName, Pt2f sizeWindow) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->depthD.draw_child(windowName, sizeWindow);

    // check mouse inputs
    const auto &hp = cD->depthD.hoveringPixel;
    for(size_t idB = 0; idB < cD->depthD.mouseButtonsPressed.size(); ++idB){
        if(cD->lastFrame != nullptr && cD->depthD.mouseButtonsPressed[idB]){
            if(hp.x() >= 0 && hp.x() < static_cast<int>(cD->lastFrame->rgbDepth.width)  &&
                hp.y() >= 0 && hp.y() < static_cast<int>(cD->lastFrame->rgbDepth.height) &&
                !cD->lastFrame->rgbDepth.empty()){
                mouse_pressed_depth_signal(idCloud, idB, hp, cD->lastFrame->rgbDepth[hp.y() * cD->lastFrame->rgbDepth.width + hp.x()]);
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_infra_texture_imgui_child(size_t idCloud, const std::string &windowName, Pt2f sizeWindow) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->infraD.draw_child(windowName, sizeWindow);

    // check mouse inputs
    const auto &hp = cD->infraD.hoveringPixel;
    for(size_t idB = 0; idB < cD->infraD.mouseButtonsPressed.size(); ++idB){
        if(cD->lastFrame != nullptr && cD->infraD.mouseButtonsPressed[idB]){
            if(hp.x() >= 0 && hp.x() < static_cast<int>(cD->lastFrame->rgbInfra.width)  &&
                hp.y() >= 0 && hp.y() < static_cast<int>(cD->lastFrame->rgbInfra.height) &&
                !cD->lastFrame->rgbInfra.empty()){
                mouse_pressed_infra_signal(idCloud, idB, hp, cD->lastFrame->rgbInfra[hp.y() * cD->lastFrame->rgbInfra.width + hp.x()]);
            }
        }
    }
}


auto DCCloudsSceneDrawer::draw_color_texture_imgui_at_position(size_t idCloud, const Pt2f &screenPos, const Pt2f &sizeTexture, std::string_view text) -> void {

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->colorD.draw_at_position(screenPos, sizeTexture, std::string(text));//std::move(text));

    // // TEST
    // auto cursorScreenPos = ImGui::GetCursorScreenPos();
    // ImGui::SetCursorScreenPos(to_iv2(screenPos));
    // cD->colorD.draw_child(std::string(text), sizeTexture);
    // ImGui::SetCursorScreenPos(cursorScreenPos);

    // check mouse inputs
    const auto &hp = cD->colorD.hoveringPixel;
    for(size_t idB = 0; idB < cD->colorD.mouseButtonsPressed.size(); ++idB){
        if(cD->lastFrame != nullptr && cD->colorD.mouseButtonsPressed[idB]){
            if(hp.x() >= 0 && hp.x() < static_cast<int>(cD->lastFrame->rgbaColor.width)  &&
                hp.y() >= 0 && hp.y() < static_cast<int>(cD->lastFrame->rgbaColor.height) &&
                !cD->lastFrame->rgbaColor.empty()){
                mouse_pressed_color_signal(idCloud, idB, hp, cD->lastFrame->rgbaColor[hp.y() * cD->lastFrame->rgbaColor.width + hp.x()]);
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_depth_texture_imgui_at_position(size_t idCloud, const Pt2f &screenPos, const Pt2f &sizeTexture, std::optional<std::string> text) -> void {

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->depthD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    const auto &hp = cD->depthD.hoveringPixel;
    for(size_t idB = 0; idB < cD->depthD.mouseButtonsPressed.size(); ++idB){
        if(cD->lastFrame != nullptr && cD->depthD.mouseButtonsPressed[idB]){
            if(hp.x() >= 0 && hp.x() < static_cast<int>(cD->lastFrame->rgbDepth.width)  &&
                hp.y() >= 0 && hp.y() < static_cast<int>(cD->lastFrame->rgbDepth.height) &&
                !cD->lastFrame->rgbDepth.empty()){
                mouse_pressed_depth_signal(idCloud, idB, hp, cD->lastFrame->rgbDepth[hp.y() * cD->lastFrame->rgbDepth.width + hp.x()]);
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_infra_texture_imgui_at_position(size_t idCloud, const Pt2f &screenPos, const Pt2f &sizeTexture, std::optional<std::string> text) -> void {

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->infraD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    const auto &hp = cD->infraD.hoveringPixel;
    for(size_t idB = 0; idB < cD->infraD.mouseButtonsPressed.size(); ++idB){
        if(cD->lastFrame != nullptr && cD->infraD.mouseButtonsPressed[idB]){
            if(hp.x() >= 0 && hp.x() < static_cast<int>(cD->lastFrame->rgbInfra.width)  &&
                hp.y() >= 0 && hp.y() < static_cast<int>(cD->lastFrame->rgbInfra.height) &&
                !cD->lastFrame->rgbInfra.empty()){
                mouse_pressed_infra_signal(idCloud, idB, hp, cD->lastFrame->rgbInfra[hp.y() * cD->lastFrame->rgbInfra.width + hp.x()]);
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_all_clouds_drawers_in_one_tab(bool drawColor, bool drawDepth, bool drawInfra, bool drawCloud, std::string_view cloudTabName) -> void{


    if(ImGuiUiDrawer::begin_tab_bar(&m_tabId, "Frames_all###direct_all_frames_tab_bar")){

        std::vector<const gl::Texture2D*> textures;

        if(drawColor){
            if(ImGuiUiDrawer::begin_tab_item("Color###direct_all_color_tabitem")){

                for(const auto &cloudD : cloudsD){
                    textures.push_back(&cloudD->colorT);
                }

                auto textPos = compute_textures_rectangles(content_region_size_available(), textures);
                auto cp = cursor_screen_position();
                for(size_t ii = 0; ii < textPos.size(); ++ii){
                    draw_color_texture_imgui_at_position(
                        ii,
                        std::get<0>(textPos[ii]) + cp,
                        std::get<1>(textPos[ii]),
                        std::format("Cam{}",ii)
                    );
                }

                ImGui::EndTabItem();
            }
        }
        if(drawDepth){
            if(ImGuiUiDrawer::begin_tab_item("Depth###direct_all_depth_tabitem")){

                for(const auto &cloudD : cloudsD){
                    textures.push_back(&cloudD->depthT);
                }

                auto textPos = compute_textures_rectangles(content_region_size_available(), textures);
                auto cp = cursor_screen_position();
                for(size_t ii = 0; ii < textPos.size(); ++ii){
                    draw_depth_texture_imgui_at_position(
                        ii,
                        std::get<0>(textPos[ii]) + cp,
                        std::get<1>(textPos[ii]),
                        std::format("Cam{}",ii)
                    );
                }
                ImGui::EndTabItem();
            }
        }
        if(drawInfra){
            if(ImGuiUiDrawer::begin_tab_item("Infra###direct_all_infra_tabitem")){

                for(const auto &cloudD : cloudsD){
                    textures.push_back(&cloudD->infraT);
                }

                auto textPos = compute_textures_rectangles(content_region_size_available(), textures);
                auto cp = cursor_screen_position();
                for(size_t ii = 0; ii < textPos.size(); ++ii){
                    draw_infra_texture_imgui_at_position(
                        ii,
                        std::get<0>(textPos[ii]) + cp,
                        std::get<1>(textPos[ii]),
                        std::format("Cam{}",ii)
                    );
                }

                ImGui::EndTabItem();
            }
        }

        if(drawCloud){
            if(ImGuiUiDrawer::begin_tab_item(std::format("{}###direct_all_cloud_tabitem",cloudTabName).c_str())){
                draw_fbo(content_region_size_available().conv<int>());
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }
}

auto DCCloudsSceneDrawer::draw_cloud_drawer_tab(size_t idDrawer, bool focusWindow, std::string_view name, bool drawColor, bool drawDepth, bool drawInfra, bool drawCloud, std::optional<Pt2<int>> sizeW) -> void{

    static_cast<void>(sizeW);
    if(focusWindow){        
        if(ImGuiUiDrawer::begin_tab_bar(&m_tabId, std::format("Frames###{}_frames_tab_bar", name).data())){

            if(drawColor){
                if(ImGuiUiDrawer::begin_tab_item(std::format("Color###{}_focus_color_tabitem", name).data())){
                    draw_color_texture_imgui_child(idDrawer, std::format("-###direct_focus_color_tabchild", name), content_region_size_available());
                    ImGui::EndTabItem();
                }
            }
            if(drawDepth){
                if(ImGuiUiDrawer::begin_tab_item(std::format("Depth###{}_focus_depth_tabitem", name).data())){
                    draw_depth_texture_imgui_child(idDrawer, std::format("-###direct_focus_depth_tabchild", name), content_region_size_available());
                    ImGui::EndTabItem();
                }
            }
            if(drawInfra){
                if(ImGuiUiDrawer::begin_tab_item(std::format("Infra###{}_focus_infra_tabitem", name).data())){
                    draw_infra_texture_imgui_child(idDrawer, std::format("-###direct_focus_infra_tabchild", name), content_region_size_available());
                    ImGui::EndTabItem();
                }
            }
            if(drawCloud){
                if(ImGuiUiDrawer::begin_tab_item(std::format("Cloud###{}_focus_cloud_tabitem", name).data())){
                    draw_fbo(content_region_size_available().conv<int>());
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    }else{
        auto sizeW = content_region_size_available() * 0.46f;
        if(drawColor){
            draw_color_texture_imgui_child(idDrawer, std::format("Color###{}_nofocus_color_tabchild", name), sizeW);
            if(drawDepth){
                ImGui::SameLine();
            }
        }
        if(drawDepth){
            draw_depth_texture_imgui_child(idDrawer, std::format("Depth###{}_nofocus_depth_tabchild", name), sizeW);
        }
        if(drawInfra){
            draw_infra_texture_imgui_child(idDrawer, std::format("Infra###{}_nofocus_infra_tabchild", name), sizeW);
            if(drawCloud){
                ImGui::SameLine();
            }
        }

        if(drawCloud){
            if(ImGui::BeginChild(std::format("CloudWindow###{}_nofocus_cloud_window_child", name).data(), to_iv2(sizeW))){
                if(ImGuiUiDrawer::begin_tab_bar(&m_tabId, std::format("CloudTab###{}_cloud_tabbar", name).data())){
                    if(ImGuiUiDrawer::begin_tab_item(std::format("Cloud###{}_cloud_tabitem", name).data())){
                        draw_fbo(content_region_size_available().conv<int>());
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::EndChild();
        }
    }
}

auto DCCloudsSceneDrawer::update_device_display_settings(size_t idCloud, const DCDeviceDisplaySettings &cloudDisplay) -> void{
    if(idCloud < cloudsD.size()){
        cloudsD[idCloud]->display = cloudDisplay;
        m_redrawClouds = true;
    }
    // else{
    //     Logger::error("invalid id cloud\n");
    // }
}

auto DCCloudsSceneDrawer::update_scene_display_settings(const DCSceneDisplaySettings &sdS) -> void{
    display        = sdS;
    m_redrawClouds = true;
}

auto DCCloudsSceneDrawer::update_model_settings(size_t idCloud, const cam::DCModelSettings &model) -> void{
    cloudsD[idCloud]->model = model.compute_full_transformation();
    m_redrawClouds = true;
}

// auto DCCloudsSceneDrawer::update_device_settings(size_t idCloud, const cam::DCDeviceSettings &deviceS) -> void{
//     cloudsD[idCloud]->deviceS = deviceS;

//     // auto mode = cloudsD[idCloud]->deviceS.configS.mode;
//     // auto dr = cam::dc_depth_resolution(mode);
//     // auto dRange = cam::dc_depth_range(mode);
//     // auto diff   = dRange.y() - dRange.x();
//     // auto hFov = dc_depth_h_fov(dr);
//     // auto vFov = dc_depth_v_fov(dr);
//     // cloudsD[idCloud]->frustumD.update(vFov, hFov/vFov, dRange.x() + cloudsD[idCloud]->filtersS.minDepthF * diff, dRange.x() + cloudsD[idCloud]->filtersS.maxDepthF * diff);

//     m_redrawClouds = true;
// }

auto DCCloudsSceneDrawer::update_filters_settings(size_t idCloud, const cam::DCFiltersSettings &filtersS) -> void {
    cloudsD[idCloud]->filtersS = filtersS;

    // auto mode = cloudsD[idCloud]->deviceS.configS.mode;
    // auto dr = cam::dc_depth_resolution(mode);
    // auto dRange = cam::dc_depth_range(mode);
    // auto diff   = dRange.y() - dRange.x();
    // auto hFov = dc_depth_h_fov(dr);
    // auto vFov = dc_depth_v_fov(dr);
    // cloudsD[idCloud]->frustumD.update(vFov, hFov/vFov, dRange.x() + cloudsD[idCloud]->filtersS.minDepthF * diff, dRange.x() + cloudsD[idCloud]->filtersS.maxDepthF * diff);

    m_redrawClouds = true;
}

auto DCCloudsSceneDrawer::compute_textures_rectangles(Pt2f parentSize, const std::vector<const gl::Texture2D *> &textures) -> std::vector<std::tuple<Pt2f, Pt2f>>{

    //    constexpr bool allow_flip = false;
    //    const auto runtime_flipping_mode = rectpack2D::flipping_option::DISABLED;
//    using spaces_type = rectpack2D::empty_spaces<allow_flip, rectpack2D::default_empty_spaces>;
//    using rect_type = rectpack2D::output_rect_t<spaces_type>;

//    std::vector<rect_type> rectangles;
//    for(const auto &texture : textures){
//        auto sizeF = Pt2f{1.f*texture->width(), 1.f*texture->height()};
//        rectangles.emplace_back(rectpack2D::rect_xywh(0,0,static_cast<int>(sizeF.x()),static_cast<int>(sizeF.y())));
//    }

//    auto report_successful = [](rect_type&) {
//        return rectpack2D::callback_result::CONTINUE_PACKING;
//    };

//    auto report_unsuccessful = [](rect_type&) {
//        return rectpack2D::callback_result::ABORT_PACKING;
//    };

//    const auto max_side = parentSize.x();
//    const auto discard_step = -4;
//    const auto result_size = rectpack2D::find_best_packing_dont_sort<spaces_type>(
//        rectangles,
//        rectpack2D::make_finder_input(
//            max_side,
//            discard_step,
//            report_successful,
//            report_unsuccessful,
//            runtime_flipping_mode
//        )
//    );

//    std::vector<std::tuple<Pt2f,Pt2f>> infos;
//    for(const auto &r : rectangles){
//        infos.push_back({{1.f*r.x,1.f*r.y},{1.f*r.w, 1.f*r.h}});
//    }
//    return infos;


    int idTry = 0;
    float factor = 1.f;
    bool fit = false;
    do {
        if(idTry > 20){
            return {};
        }


        BinPack2D::ContentAccumulator<const gl::Texture2D*> inputContent;

        for(const auto &texture : textures){
            auto sizeF = Pt2f{texture->width() * factor, texture->height() * factor};

            inputContent += BinPack2D::Content<const gl::Texture2D*>(
                texture, BinPack2D::Coord(), BinPack2D::Size(static_cast<int>(sizeF.x()), static_cast<int>(sizeF.y())), false
            );
        }


        BinPack2D::CanvasArray<const gl::Texture2D*> canvasArray = BinPack2D::UniformCanvasArrayBuilder<const gl::Texture2D*>(
            static_cast<int>(parentSize.x()),static_cast<int>(parentSize.y()),1
        ).Build();
        BinPack2D::ContentAccumulator<const gl::Texture2D*> remainder;

        if(canvasArray.Place( inputContent, remainder )){
            fit = true;

            // A place to store packed content.
            BinPack2D::ContentAccumulator<const gl::Texture2D*> outputContent;

            // Read all placed content.
            canvasArray.CollectContent( outputContent );

            std::vector<std::tuple<Pt2f,Pt2f>> infos;

            typedef BinPack2D::Content<const gl::Texture2D*>::Vector::iterator binpack2d_iterator;
            for( binpack2d_iterator itor = outputContent.Get().begin(); itor != outputContent.Get().end(); itor++ ) {
                const BinPack2D::Content<const gl::Texture2D*> &content = *itor;
                infos.push_back({{static_cast<float>(content.coord.x),static_cast<float>(content.coord.y)},
                                 {static_cast<float>(content.size.w),static_cast<float>(content.size.h)}});
            }

            return infos;

        }else{
            ++idTry;
            factor *= 0.9f;
        }
    }while(!fit);


    return {};
}

auto DCCloudsSceneDrawer::draw_fbo(Pt2<int> size) -> void{
    fboD.bind();
    fboD.resize(size);
    fboD.draw();
    fboD.unbind();
}

