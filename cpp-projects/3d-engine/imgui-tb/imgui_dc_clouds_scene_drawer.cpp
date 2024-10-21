
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

    if(!fboD.initialized){
        fboD.initialize();
        gridD.initialize(1.f,1.f, 20, 20, true);
        filteringPlanesDotD.initialize(0.05f);
    }

    if(cloudsD.size() < nbDrawers){
        while(cloudsD.size() != nbDrawers){
            auto cloudD = std::make_unique<DCCloudDrawer>();
            cloudD->initialize();
            cloudsD.push_back(std::move(cloudD));
        }
    }else{
        cloudsD.resize(nbDrawers);
    }
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

auto DCCloudsSceneDrawer::update_from_colored_cloud_data(size_t idCloud, const ColorCloud &cloud) -> bool{
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

    auto solidShader        = ShadersManager::get_instance()->get_ptr("solid"sv);
    auto pointsCloudShader  = ShadersManager::get_instance()->get_ptr("cloud"sv);
    auto circlesCloudShader = ShadersManager::get_instance()->get_ptr("voxelCloud"sv);

    if(display.drawGrid){
        if(auto shader = solidShader){

            shader->use();
            // transforms
            shader->set_uniform_matrix("view"sv, fboD.camera()->view().conv<float>());
            shader->set_uniform_matrix("projection"sv, fboD.camera()->projection().conv<float>());
            shader->set_uniform_matrix("model"sv, Mat4f::identity());
            // colors
            shader->set_uniform("enable_unicolor"sv, true);
            shader->set_uniform("unicolor"sv, Pt4f{1.f,1.f,1.f,1.f});
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
            shader->set_uniform("camera_position"sv, fboD.camera()->position().conv<float>());
            shader->set_uniform("camera_direction"sv, fboD.camera()->direction().conv<float>());
            //shader->set_uniform_matrix("transposed_inverse_model"sv, transpose(inverse(cloudD->model)));
            // color
            shader->set_uniform("enable_unicolor"sv, cloudD->display.forceColor);
            shader->set_uniform("unicolor"sv, cloudD->display.unicolor);
            shader->set_uniform("factor_unicolor"sv, cloudD->display.factorUnicolor);
            // geometry
            shader->set_uniform("backFaceCulling"sv, cloudD->display.backFaceCulling);

            if(cloudD->display.circles){
                shader->set_uniform("squareSize"sv, cloudD->display.squareSize);
                shader->set_uniform("circleRadius"sv, cloudD->display.circleRadius);
            }else{
                shader->set_uniform("pointSize"sv, cloudD->display.pointSize);
            }
            
            if(cloudD->display.showCapture){
                cloudD->cpD.draw();                
            }

        }else{
            Log::error("[DCCloudsSceneDrawer] Shaders with aliases \"cloud\" and \"voxelCloud\" must be available in the shader manager.\n"sv);
            break;
        }

        if(auto shader = solidShader){

            shader->use();
            // transforms
            shader->set_uniform_matrix("view"sv, fboD.camera()->view().conv<float>());
            shader->set_uniform_matrix("projection"sv, fboD.camera()->projection().conv<float>());
            shader->set_uniform_matrix("model"sv, cloudD->model);

            // color
            shader->set_uniform("enable_unicolor"sv, true);
            shader->set_uniform("unicolor"sv, cloudD->display.unicolor);
            if(cloudD->display.showCameraFrustum && cloudD->display.showCapture && m_displayFrustum){
                cloudD->frustumD.draw();
            }

            // body tracking
            if(cloudD->display.showBodyTracking && cloudD->display.showCapture){
                for(size_t ii = 0; ii < cloudD->nbBodies; ++ii){
                    shader->set_uniform("unicolor"sv, Pt4f{1.f,0.f,0.f, 1.f});
                    for(size_t jj = 0; jj < cloudD->jointsModels[ii].size(); ++jj){
                        const auto &jm = cloudD->jointsModels[ii][jj];
                        if(std::get<0>(jm)){
                            shader->set_uniform_matrix("model"sv, std::get<1>(jm) * cloudD->model);
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

                    shader->set_uniform_matrix("model"sv,  transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, p1) * cloudD->model);
                    shader->set_uniform("unicolor"sv, Pt4f{0.f,1.f,0.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform_matrix("model"sv, transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, p2) * cloudD->model);
                    shader->set_uniform("unicolor"sv, Pt4f{1.f,0.f,0.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform_matrix("model"sv, transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, p3) * cloudD->model);
                    shader->set_uniform("unicolor"sv, Pt4f{0.f,0.f,1.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform_matrix("model"sv, transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, meanPt) * cloudD->model);
                    shader->set_uniform("unicolor"sv, Pt4f{0.f,1.f,1.f, 1.f});
                    filteringPlanesDotD.draw();

                    shader->set_uniform("unicolor"sv, Pt4f{1.f,0.f,0.f, 1.f});
                    shader->set_uniform_matrix("model"sv, cloudD->model);

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
                    shader->set_uniform_matrix("model"sv, transform<float>(Pt3f(1.f,1.f,1.f), Pt3f{0.f,0.f,0.f}, sp) * cloudD->model);
                    shader->set_uniform("unicolor"sv, Pt4f{0.5f,0.5f,1.f, 1.f});
                    filteringPlanesDotD.draw();
                }

                if(cloudD->filtersS.keepOnlyPointsInsideOOB){
                    shader->set_uniform_matrix("model"sv,  cloudD->model);
                    shader->set_uniform("unicolor"sv, Pt4f{0.5f,0.5f,0.5f, 1.f});
                    cloudD->oobLinesD.update(cloudD->filtersS.oob);
                    cloudD->oobLinesD.draw();
                }
            }

        }else{
            Log::error("[DCCloudsSceneDrawer] Shaders with aliases \"solid\" must be available in the shader manager.\n"sv);
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
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8)){
            const auto &hp = cD->colorD.hoveringPixel;
            for(size_t idB = 0; idB < cD->colorD.mouseButtonsPressed.size(); ++idB){
                if(cD->colorD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_color_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_depth_sized_color_texture_imgui_child(size_t idCloud, const std::string &windowName, geo::Pt2f sizeWindow) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->depthSizedColorD.draw_child(windowName, sizeWindow);

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8)){
            const auto &hp = cD->depthSizedColorD.hoveringPixel;
            for(size_t idB = 0; idB < cD->depthSizedColorD.mouseButtonsPressed.size(); ++idB){
                if(cD->depthSizedColorD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_depth_sized_color_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_depth_texture_imgui_child(size_t idCloud, const std::string &windowName, Pt2f sizeWindow) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->depthD.draw_child(windowName, sizeWindow);

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGB8>(DCImageBufferType::DepthRGB8)){
            const auto &hp = cD->depthD.hoveringPixel;
            for(size_t idB = 0; idB < cD->depthD.mouseButtonsPressed.size(); ++idB){
                if(cD->depthD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_depth_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_infra_texture_imgui_child(size_t idCloud, const std::string &windowName, Pt2f sizeWindow) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->infraD.draw_child(windowName, sizeWindow);

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGB8>(DCImageBufferType::InfraredRGB8)){
            const auto &hp = cD->infraD.hoveringPixel;
            for(size_t idB = 0; idB < cD->infraD.mouseButtonsPressed.size(); ++idB){
                if(cD->infraD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_infra_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_bodies_id_map_texture_imgui_child(size_t idCloud, const std::string &windowName, geo::Pt2f sizeWindow) -> void{
    // ...
}


auto DCCloudsSceneDrawer::draw_color_texture_imgui_at_position(size_t idCloud, const Pt2f &screenPos, const Pt2f &sizeTexture, std::optional<std::string> text) -> void {

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->colorD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8)){
            const auto &hp = cD->colorD.hoveringPixel;
            for(size_t idB = 0; idB < cD->colorD.mouseButtonsPressed.size(); ++idB){
                if(cD->colorD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_color_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_depth_sized_color_texture_imgui_at_position(size_t idCloud, const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::optional<std::string> text) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->depthSizedColorD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8)){
            const auto &hp = cD->depthSizedColorD.hoveringPixel;
            for(size_t idB = 0; idB < cD->depthSizedColorD.mouseButtonsPressed.size(); ++idB){
                if(cD->depthSizedColorD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_depth_sized_color_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_depth_texture_imgui_at_position(size_t idCloud, const Pt2f &screenPos, const Pt2f &sizeTexture, std::optional<std::string> text) -> void {

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->depthD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGB8>(DCImageBufferType::DepthRGB8)){
            const auto &hp = cD->depthD.hoveringPixel;
            for(size_t idB = 0; idB < cD->depthD.mouseButtonsPressed.size(); ++idB){
                if(cD->depthD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_depth_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_infra_texture_imgui_at_position(size_t idCloud, const Pt2f &screenPos, const Pt2f &sizeTexture, std::optional<std::string> text) -> void {

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->infraD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorRGB8>(DCImageBufferType::InfraredRGB8)){
            const auto &hp = cD->infraD.hoveringPixel;
            for(size_t idB = 0; idB < cD->infraD.mouseButtonsPressed.size(); ++idB){
                if(cD->infraD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_infra_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_bodies_id_map_texture_imgui_at_position(size_t idCloud, const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::optional<std::string> text) -> void{

    auto cD = cloudsD[idCloud].get();

    // draw
    cD->bodiesIdMapD.draw_at_position(screenPos, sizeTexture, std::move(text));

    // check mouse inputs
    if(cD->lastFrame != nullptr){
        if(auto image = cD->lastFrame->image_buffer<ColorGray8>(DCImageBufferType::BodiesIdMap8)){
            const auto &hp = cD->bodiesIdMapD.hoveringPixel;
            for(size_t idB = 0; idB < cD->bodiesIdMapD.mouseButtonsPressed.size(); ++idB){
                if(cD->bodiesIdMapD.mouseButtonsPressed[idB]){
                    if(hp.x() >= 0 && hp.x() < static_cast<int>(image->width)  &&
                        hp.y() >= 0 && hp.y() < static_cast<int>(image->height) &&
                        !image->empty()){
                        mouse_pressed_bodies_id_signal(idCloud, idB, hp, (*image)[hp.y() * image->width + hp.x()]);
                    }
                }
            }
        }
    }
}

auto DCCloudsSceneDrawer::draw_all_clouds_drawers_in_one_tab(bool drawImages, bool drawCloud, std::string_view cloudTabName) -> void{


    if(ImGuiUiDrawer::begin_tab_bar(&m_tabId, "Frames_all###direct_all_frames_tab_bar")){

        std::vector<const gl::Texture2D*> textures;

        if(drawImages){
            if(ImGuiUiDrawer::begin_tab_item("Depth sized color###direct_all_depth_sized_color_tabitem")){

                for(const auto &cloudD : cloudsD){
                    textures.push_back(&cloudD->depthSizedColorT);
                }

                auto textPos = compute_textures_rectangles(content_region_size_available(), textures);
                auto cp = cursor_screen_position();
                for(size_t ii = 0; ii < textPos.size(); ++ii){
                    draw_depth_sized_color_texture_imgui_at_position(
                        ii,
                        std::get<0>(textPos[ii]) + cp,
                        std::get<1>(textPos[ii]),
                        std::format("Cam{}"sv,ii)
                    );
                }

                ImGui::EndTabItem();
            }

            if(ImGuiUiDrawer::begin_tab_item("Original color###direct_all_original_color_tabitem")){

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
                        std::format("Cam{}"sv,ii)
                        );
                }

                ImGui::EndTabItem();
            }

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
                        std::format("Cam{}"sv,ii)
                    );
                }
                ImGui::EndTabItem();
            }

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
                        std::format("Cam{}"sv,ii)
                    );
                }

                ImGui::EndTabItem();
            }

            if(ImGuiUiDrawer::begin_tab_item("Bodies ID map###direct_all_bodies_id_map_tabitem")){

                for(const auto &cloudD : cloudsD){
                    textures.push_back(&cloudD->bodiesIdMapT);
                }

                auto textPos = compute_textures_rectangles(content_region_size_available(), textures);
                auto cp = cursor_screen_position();
                for(size_t ii = 0; ii < textPos.size(); ++ii){
                    draw_bodies_id_map_texture_imgui_at_position(
                        ii,
                        std::get<0>(textPos[ii]) + cp,
                        std::get<1>(textPos[ii]),
                        std::format("Cam{}"sv,ii)
                    );
                }

                ImGui::EndTabItem();
            }
        }

        if(drawCloud){
            if(ImGuiUiDrawer::begin_tab_item(std::format("{}###direct_all_cloud_tabitem"sv,cloudTabName).c_str())){
                draw_fbo(content_region_size_available().conv<int>());
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }
}

auto DCCloudsSceneDrawer::draw_cloud_drawer_tab(size_t idDrawer, bool focusWindow, std::string_view name) -> void{

    if(focusWindow){        
        if(ImGuiUiDrawer::begin_tab_bar(&m_tabId, std::format("Frames###{}_frames_tab_bar"sv, name).data())){

            auto cD = cloudsD[idDrawer].get();

            // draw
            cD->depthSizedColorD.draw_tab_item(std::format("Depth sized color###{}_focus_depth_sized__color_tabitem"sv, name).data());
            cD->colorD.draw_tab_item(std::format("Original color###{}_focus_original_color_tabitem"sv, name).data());
            cD->depthD.draw_tab_item(std::format("Depth###{}_focus_depth_tabitem"sv, name).data());
            cD->infraD.draw_tab_item(std::format("Infra###{}_focus_infra_tabitem"sv, name).data());
            cD->bodiesIdMapD.draw_tab_item(std::format("Bodies ID map###{}_focus_bodies_id_map_tabitem"sv, name).data());
            if(ImGuiUiDrawer::begin_tab_item(std::format("Cloud###{}_focus_cloud_tabitem"sv, name).data())){
                draw_fbo(content_region_size_available().conv<int>());
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }else{
        auto sizeW = content_region_size_available() * 0.46f;

        if(ImGui::BeginChild("###colors_child", to_iv2(sizeW),false,ImGuiWindowFlags_NoScrollWithMouse)){
            if(ImGui::BeginTabBar("###colors_tabbar", ImGuiTabBarFlags_None)){
                auto cD = cloudsD[idDrawer].get();
                cD->depthSizedColorD.draw_tab_item(std::format("Depth sized color###{}_nofocus_depth_sized__color_tabitem"sv, name).data());
                cD->colorD.draw_tab_item(std::format("Original color###{}_nofocus_original_color_tabitem"sv, name).data());
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();
        draw_depth_texture_imgui_child(idDrawer, std::format("Depth###{}_nofocus_depth_tabchild"sv, name), sizeW);

        if(ImGui::BeginChild("###infra_bodies_id_map_child", to_iv2(sizeW),false,ImGuiWindowFlags_NoScrollWithMouse)){
            if(ImGui::BeginTabBar("###infra_bodies_id_map_tabbar", ImGuiTabBarFlags_None)){
                auto cD = cloudsD[idDrawer].get();
                cD->infraD.draw_tab_item(std::format("Infra###{}_nofocus_infra_tabitem"sv, name).data());
                cD->bodiesIdMapD.draw_tab_item(std::format("Bodies ID map###{}_nofocus_bodies_id_map_tabitem"sv, name).data());
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if(ImGui::BeginChild(std::format("CloudWindow###{}_nofocus_cloud_window_child"sv, name).data(), to_iv2(sizeW))){
            if(ImGuiUiDrawer::begin_tab_bar(&m_tabId, std::format("CloudTab###{}_cloud_tabbar"sv, name).data())){
                if(ImGuiUiDrawer::begin_tab_item(std::format("Cloud###{}_cloud_tabitem"sv, name).data())){
                    draw_fbo(content_region_size_available().conv<int>());
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }
}

auto DCCloudsSceneDrawer::update_device_display_settings(size_t idCloud, const DCDeviceDisplaySettings &cloudDisplay) -> void{
    if(idCloud < cloudsD.size()){
        cloudsD[idCloud]->display = cloudDisplay;
        m_redrawClouds = true;
    }
    // else{
    //     Log::error("invalid id cloud\n");
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
    fboD.draw();
    fboD.unbind();
}

