

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

#include "imgui_dc_ui_drawer.hpp"

// std
#include <format>

// local
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "imgui/extra/ImGuiFileDialog.h"
#include "imgui/misc/cpp/imgui_stdlib.h"


using namespace tool::graphics;
using namespace tool::cam;

// [[maybe_unused]] static constexpr const char* clientDataActionItems[] = {
//     "None", "Process", "Process-send", "Process-display", "Process-send-display"
// };

// auto client_data_action_combo(std::string_view label, const char* id, ClientDataAction &cda, bool &updateP) -> void{
//     int guiSelection = static_cast<int>(cda);
//     tool::ImGuiUiDrawer::text(label);
//     ImGui::SameLine();
//     ImGui::SetNextItemWidth(170.f);
//     if(ImGui::Combo(id, &guiSelection, clientDataActionItems, IM_ARRAYSIZE(clientDataActionItems))){
//         auto nCDA = static_cast<ClientDataAction>(guiSelection);
//         if(nCDA != cda){
//             cda = nCDA;
//             updateP = true;
//         }
//     }
// }

// [[maybe_unused]] static constexpr const char* serverDataActionItems[] = {
//     "None", "Process", "Process-display"
// };

// auto server_data_action_combo(std::string_view label, const char* id, ServerDataAction &sda, bool &updateP) -> void{
//     int guiSelection = static_cast<int>(sda);
//     tool::ImGuiUiDrawer::text(label);
//     ImGui::SameLine();
//     ImGui::SetNextItemWidth(120.f);
//     if(ImGui::Combo(id, &guiSelection, serverDataActionItems, IM_ARRAYSIZE(serverDataActionItems))){
//         auto nSDA = static_cast<ServerDataAction>(guiSelection);
//         if(nSDA != sda){
//             sda = nSDA;
//             updateP = true;
//         }
//     }
// }

// auto recorder_data_action_combo(std::string_view label, const char* id, ServerDataAction &sda, bool &updateP) -> void{
//     int guiSelection = static_cast<int>(sda);
//     tool::ImGuiUiDrawer::text(label);
//     ImGui::SameLine();
//     ImGui::SetNextItemWidth(120.f);
//     if(ImGui::Combo(id, &guiSelection, serverDataActionItems, IM_ARRAYSIZE(serverDataActionItems)-1)){
//         auto nSDA = static_cast<ServerDataAction>(guiSelection);
//         if(nSDA != sda){
//             sda = nSDA;
//             updateP = true;
//         }
//     }
// }


[[maybe_unused]] static constexpr const char* cloudColorModeItems[] = {
    "Depth colormap", "Depth-sized color image"
};

auto cloud_color_mode_combo(std::string_view label, const char* id, CloudColorMode &ccm, bool &updateP) -> void{
    int guiSelection = static_cast<int>(ccm);
    tool::ImGuiUiDrawer::text(label);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);
    if(ImGui::Combo(id, &guiSelection, cloudColorModeItems, IM_ARRAYSIZE(cloudColorModeItems))){
        auto nCCM = static_cast<CloudColorMode>(guiSelection);
        if(nCCM != ccm){
            ccm = nCCM;
            updateP = true;
        }
    }
}

[[maybe_unused]] static constexpr const char* cloudGenerationModeItems[] = {
    "From depth", "From decoded cloud"
};

auto cloud_generation_mode_combo(std::string_view label, const char* id, CloudGenerationMode &cgm, bool &updateP) -> void{
    int guiSelection = static_cast<int>(cgm);
    tool::ImGuiUiDrawer::text(label);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(130.f);
    if(ImGui::Combo(id, &guiSelection, cloudGenerationModeItems, IM_ARRAYSIZE(cloudGenerationModeItems))){
        auto nCGM = static_cast<CloudGenerationMode>(guiSelection);
        if(nCGM != cgm){
            cgm = nCGM;
            updateP = true;
        }
    }
}



auto DCUIDrawer::draw_dc_filters_settings_tab_item(const std::string &tabItemName, cam::DCMode mode, cam::DCFiltersSettings &filters) -> std::tuple<bool, bool>{

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return {false, false};
    }

    bool update = false;
    ImGuiUiDrawer::title("DEPTH FILTERING");
    {
        ImGui::Text("Basic filtering:");
        ImGui::Indent();
        {
            float minMaxD[2] = {filters.minDepthF, filters.maxDepthF};
            auto dRange = (cam::dc_depth_range(mode)*1000.f).conv<int>();
            auto diff   = dRange.y() - dRange.x();

            ImGuiUiDrawer::text(std::format("Depth (factor): min: {}mm, max: {}mm", static_cast<int>(dRange.x() + minMaxD[0]*diff),  static_cast<int>(dRange.x() + minMaxD[1]*diff)));
            ImGui::Indent();
            if(ImGui::SliderFloat2("###settings_depth_min_max_factors", minMaxD, 0.f, 1.f)){
                filters.minDepthF = minMaxD[0];
                filters.maxDepthF = minMaxD[1];
                update = true;
            }
            ImGui::Unindent();

            auto dRes = cam::dc_depth_resolution(mode);
            float minMaxW[2] = {filters.minWidthF, filters.maxWidthF};
            ImGuiUiDrawer::text(std::format("Width (factor): min: {}pix, max: {}pix", static_cast<int>(minMaxW[0]*cam::dc_depth_width(dRes)),  static_cast<int>(minMaxW[1]*cam::dc_depth_width(dRes))));
            ImGui::Indent();
            if(ImGui::SliderFloat2("###settings_width_min_max_factors", minMaxW, 0.f, 1.f)){
                filters.minWidthF = minMaxW[0];
                filters.maxWidthF = minMaxW[1];
                update = true;
            }
            ImGui::Unindent();

            float minMaxH[2] = {filters.minHeightF, filters.maxHeightF};
            ImGuiUiDrawer::text(std::format("Height (factor): min: {}pix, max: {}pix", static_cast<int>(minMaxH[0]*cam::dc_depth_height(dRes)),  static_cast<int>(minMaxH[1]*cam::dc_depth_height(dRes))));
            ImGui::Indent();
            if(ImGui::SliderFloat2("###settings_height_min_max_factors", minMaxH, 0.f, 1.f)){
                filters.minHeightF = minMaxH[0];
                filters.maxHeightF = minMaxH[1];
                update = true;
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();

        ImGui::Text("From depth-sized color:");
        ImGui::Indent();
        {
            if(ImGui::Checkbox("Enable color filtering###settings_filter_enable_color_filtering_checkbox", &filters.filterDepthWithColor)){
                update = true;
            }
            if(filters.filterDepthWithColor){

                ImGui::Indent();

                ImGui::Text("Filtered color");
                ImGui::Indent();
                if(ImGui::ColorEdit3("###settings_filtered_color_coloredit3", filters.filterColor.array.data())){
                    filters.filterColor.clamp(0.f,1.f);
                    update = true;
                }
                ImGui::Unindent();


                ImGui::Text("Max diff color");
                ImGui::Indent();
                auto mdcd = filters.maxDiffColor.array.data();

                //            static tool::gl::Texture2D tex;
                //            static std::vector<std::uint8_t> tData;
                //            if(tData.size() != 100*25*3){
                //               tData.resize(100*25*3);
                //            }

                ImGuiFloatS fs;
                fs.defaultValue = 0.f;
                fs.min = 0.f;
                fs.max = 360.f;
                fs.speedDrag = 0.1f;
                fs.speedInc = 1.f;
                fs.format = "%.1f";

                ImGuiDragS ds;
                ds.widthDrag = 60.f;
                ds.defaultButton = false;
                ds.decButton = true;
                ds.incButton = true;
                ds.displayText = true;
                ds.displayTextLeft = true;
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Hue","hue", mdcd, fs, ds)){
                    update = true;
                }

                fs.speedDrag = 0.001f;
                fs.speedInc = 0.01f;
                fs.max = 1.f;
                fs.format = "%.3f";
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Saturation","saturation", mdcd+1, fs, ds)){
                    update = true;
                }

                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Value","value", mdcd+2, fs, ds)){
                    update = true;
                }

                ImGui::Unindent();
            }
        }
        ImGui::Unindent();

        ImGui::Text("From infra:");
        ImGui::Indent();
        {
            ImGui::BeginDisabled(true);
            if(ImGui::Checkbox("Enable infra filtering###settings_filter_enable_infra_filtering_checkbox", &filters.filterDepthWithInfra)){
                update = true;
            }
            ImGui::EndDisabled();
        }
        ImGui::Unindent();

        ImGui::Text("From cloud:");
        ImGui::Indent();
        {
            if(ImGui::Checkbox("Enable cloud filtering###settings_filter_enable_cloud_filtering_checkbox", &filters.filterDepthWithCloud)){
                update = true;
            }
            if(filters.filterDepthWithCloud){
                ImGui::Text("Plane filtering:");
                ImGui::Indent();

                int mode = static_cast<int>(filters.p1FMode);
                if(ImGui::RadioButton("None###mode_none_plane1", &mode,0)){
                    update = true;
                    filters.p1FMode = cam::PlaneFilteringMode::None;
                }
                ImGui::SameLine();
                if(ImGui::RadioButton("Remove above###mode_above_plane1", &mode,1)){
                    update = true;
                    filters.p1FMode = cam::PlaneFilteringMode::Above;
                }
                ImGui::SameLine();
                if(ImGui::RadioButton("Remove below###mode_below_plane1", &mode,2)){
                    update = true;
                    filters.p1FMode = cam::PlaneFilteringMode::Below;
                }
                if(filters.p1FMode != cam::PlaneFilteringMode::None){

                    ImGui::Text("Point A (mm):");
                    auto p1APtr = filters.p1A.array.data();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","pA_x", p1APtr, modelTrFs, modelTrDs)){
                        update = true;
                    }
                    ImGui::SameLine();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","pA_y", p1APtr+1, modelTrFs, modelTrDs)){
                        update = true;
                    }
                    ImGui::SameLine();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","pA_z", p1APtr+2, modelTrFs, modelTrDs)){
                        update = true;
                    }

                    ImGui::Text("Point B (mm):");
                    auto p1BPtr = filters.p1B.array.data();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","pB_x", p1BPtr, modelTrFs, modelTrDs)){
                        update = true;
                    }
                    ImGui::SameLine();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","pB_y", p1BPtr+1, modelTrFs, modelTrDs)){
                        update = true;
                    }
                    ImGui::SameLine();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","pB_z", p1BPtr+2, modelTrFs, modelTrDs)){
                        update = true;
                    }

                    ImGui::Text("Point C (mm):");
                    auto p1CPtr = filters.p1C.array.data();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","pC_x", p1CPtr, modelTrFs, modelTrDs)){
                        update = true;
                    }
                    ImGui::SameLine();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","pC_y", p1CPtr+1, modelTrFs, modelTrDs)){
                        update = true;
                    }
                    ImGui::SameLine();
                    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","pC_z", p1CPtr+2, modelTrFs, modelTrDs)){
                        update = true;
                    }
                }
                ImGui::Unindent();


                ImGui::Text("Point distance filtering:");
                ImGui::Indent();

                if(ImGui::Checkbox("Enable###settings_point_distance_filtering_checkbox", &filters.removeFromPointDistance)){
                    update = true;
                }

                ImGui::Text("Point (mm):");
                auto pPtr = filters.pSphere.array.data();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","pS_x", pPtr, modelTrFs, modelTrDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","pS_y", pPtr+1, modelTrFs, modelTrDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","pS_z", pPtr+2, modelTrFs, modelTrDs)){
                    update = true;
                }

                ImGuiDragS ds;
                ds.widthDrag = 60.f;
                ds.decButton = true;
                ds.incButton = true;
                ds.displayText = true;
                ds.displayTextLeft = true;
                ds.defaultButton = false;

                ImGuiFloatS fs;
                fs.defaultValue = 1000.f;
                fs.min = 1.f;
                fs.max = 10000.f;
                fs.speedInc = 10.f;
                fs.speedDrag = 1.0f;
                fs.format = "%.1f";

                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Max distance:", "settings_max_distance_from_point_dragfloat", &filters.maxSphereDistance, fs, ds)){
                    update = true;
                }

                ImGui::Unindent();


                ImGui::Text("Oriented-bounding-box filtering:");
                ImGui::Indent();

                if(ImGui::Checkbox("Enable###settings_point_inside_oob_filtering_checkbox", &filters.keepOnlyPointsInsideOOB)){
                    update = true;
                }


                ImGui::Text("Position (mm):");
                auto trPtr = filters.oob.position.array.data();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","oob_x", trPtr, modelTrFs, modelTrDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","oob_y", trPtr+1, modelTrFs, modelTrDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","oob_z", trPtr+2, modelTrFs, modelTrDs)){
                    update = true;
                }

                ImGui::Text("Rotation (°):");
                auto rotPtr = filters.oob.rotation.array.data();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","oob_rot_x", rotPtr, modelRotFs, modelRotDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","oob_rot_y", rotPtr+1, modelRotFs, modelRotDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","oob_rot_z", rotPtr+2, modelRotFs, modelRotDs)){
                    update = true;
                }

                ImGui::Text("Size:");
                auto scPtr = filters.oob.size.array.data();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","oob_sc_x", scPtr, modelScFs, modelScDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","oob_sc_y", scPtr+1, modelScFs, modelScDs)){
                    update = true;
                }
                ImGui::SameLine();
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","oob_sc_z", scPtr+2, modelScFs, modelScDs)){
                    update = true;
                }

                ImGui::Unindent();
            }

        }
        ImGui::Unindent();

        ImGui::Text("From body-tracking:");
        ImGui::Indent();
        {
            ImGui::BeginDisabled(true);
            if(ImGui::Checkbox("Enable body tracking filtering###settings_filter_enable_body_tracking_filtering_checkbox", &filters.filterDepthWithBodyTracking)){
                update = true;
            }
            ImGui::EndDisabled();
        }
        ImGui::Unindent();

        ImGui::Separator();

        ImGui::Text("Complex:");
        ImGui::Indent();{

            ImGuiIntS is;
            is.defaultValue = 1;
            is.min = 1;
            is.max = 8;
            is.speedInc =1;
            is.speedDrag = 1;

            ImGuiFloatS fs;
            fs.defaultValue = 10.f;
            fs.min = 0.f;
            fs.max = 100.f;
            fs.speedInc = 1.f;
            fs.speedDrag = 0.1f;
            fs.format = "%.1f";

            ImGuiDragS ds;
            ds.widthDrag = 60.f;
            ds.decButton = true;
            ds.incButton = true;
            ds.displayText = true;
            ds.displayTextLeft = true;
            ds.defaultButton = false;

            if(ImGui::Checkbox("Do local depth difference filtering", &filters.doLocalDiffFiltering)){
                update = true;
            }

            ImGui::Indent();
            if(ImGuiUiDrawer::draw_drag_float_with_buttons("Max value:","settings_local_diff_dragfloat", &filters.maxLocalDiff, fs, ds)){
                update = true;
            }
            ImGui::Unindent();

            if(ImGui::Checkbox("Do minimum neighbours filtering", &filters.doMinNeighboursFiltering)){
                update = true;
            }
            ImGui::Indent();
            ds.widthDrag = 40.f;
            int nbMinN = filters.nbMinNeighbours;
            if(ImGuiUiDrawer::draw_drag_int_with_buttons("Min nb of neighbours","settings_nb_min_neighbours", &nbMinN, is, ds)){
                filters.nbMinNeighbours = nbMinN;
                update = true;
            }

            is.max = 10;
            int nbMinL = filters.minNeighboursLoops;
             if(ImGuiUiDrawer::draw_drag_int_with_buttons("Loops:","settings_min_neighbours_nb_loop", &nbMinL, is, ds)){
                 filters.minNeighboursLoops = nbMinL;
                update = true;
            }
            ImGui::Unindent();

            if(ImGui::Checkbox("Do erosion", &filters.doErosion)){
                update = true;
            }
            ImGui::Indent();
                int nbErosionL = filters.erosionLoops;
                if(ImGuiUiDrawer::draw_drag_int_with_buttons("Loops:","settings_erosion_nb_loop", &nbErosionL, is, ds)){
                    filters.erosionLoops = nbErosionL;
                    update = true;
                }
            ImGui::Unindent();

            if(ImGui::Checkbox("Keep only bigger cluster###settings_keep_only_biggest_cluster_checkbox", &filters.keepOnlyBiggestCluster)){
                update = true;
            }

            if(ImGui::Checkbox("Remove after closest point###settings_remove_after_closest_point_checkbox", &filters.removeAfterClosestPoint)){
                update = true;
            }
            ImGui::Indent();
            if(ImGuiUiDrawer::draw_drag_float_with_buttons("max distance after closest point:","settings_max_distance_after_closest_point_dragfloat",
                &filters.maxDistanceAfterClosestPoint, fs, ds)){
                update = true;
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
    }

    ImGuiUiDrawer::title("DEPTH-SIZED COLOR FILTERING");
    {
        ImGui::Indent();
        if(ImGui::Checkbox("Invalidate color from depth###settings_invalidate_color_from_depth_checkbox", &filters.invalidateColorFromDepth)){
            update = true;
        }
        ImGui::Unindent();
    }

    ImGuiUiDrawer::title("INFRA FILTERING");
    {
        ImGui::Indent();
        if(ImGui::Checkbox("Invalidate infra from depth###settings_invalidate_infra_from_depth_checkbox", &filters.invalidateInfraFromDepth)){
            update = true;
        }
        ImGui::Unindent();
    }

    ImGui::EndTabItem();

    return {true, update};
}

auto DCUIDrawer::draw_dc_scene_display_setings_tab_item(const std::string &tabItemName, DCSceneDisplaySettings &display)  -> bool {

    if (!ImGui::BeginTabItem(tabItemName.c_str())){
        return false;
    }

    bool update = false;

    if(ImGui::Checkbox("Draw grid###scene_display_draw_grid", &display.drawGrid)){
        update = true;
    }

    if(ImGui::ColorEdit4("Background color###scene_display_background_scene_color", display.backgroundColor.array.data())){
        update = true;
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_cloud_display_setings_tab_item(const std::string &tabItemName, DCDeviceDisplaySettings &display)  -> bool {

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    bool update = false;

    ImGui::Text("Colors:");
    ImGui::Indent();
    if(ImGui::Checkbox("Force cloud color###display_force_cloud_color", &display.forceColor)){
        update = true;
    }
    if(ImGui::ColorEdit4("Cloud color###display_cloud_color", display.unicolor.array.data())){
        update = true;
    }
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Color factor", &display.factorUnicolor, 0.01f, 0.f, 1.f)){
        update = true;
    }

    ImGui::Unindent();

    ImGui::Separator();

    ImGui::Text("Geometry:");
    ImGui::Indent();
    if(ImGui::Checkbox("Enable backface culling###enable_backface_culling", &display.backFaceCulling)){
        update = true;
    }

    if(ImGui::Checkbox("Use circles###display_use_circles", &display.circles)){
        update = true;
    }

    ImGui::SetNextItemWidth(100.f);

    {
        ImGui::BeginDisabled(display.circles);

        ImGuiDragS  pointSizeD = {100.f, true, true, true, true, false};
        ImGuiFloatS pointSizeFS = {5.f, 0.1f, 30.0f, 0.1f, 0.1f};
        float pointSizeV = display.pointSize;
         if(ImGuiUiDrawer::draw_drag_float_with_buttons("Point size","display_size_points", &pointSizeV, pointSizeFS, pointSizeD)){
            update = true;
            display.pointSize = pointSizeV;
        }

         ImGui::EndDisabled();
    }

    ImGui::BeginDisabled(!display.circles);

    {
        ImGuiDragS  squareSizeD = {100.f, true, true, true, true, false};
        ImGuiFloatS squareSizeFS = {0.005f, 0.001f, 0.05f, 0.0001f, 0.001f};
        float squareSizeV = display.squareSize;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Square size","display_square_size", &squareSizeV, squareSizeFS, squareSizeD)){
            update = true;
            display.squareSize = squareSizeV;
        }
    }

    {
        ImGuiDragS  circleRadiusD = {100.f, true, true, true, true, false};
        ImGuiFloatS circleRadiusFS = {0.25f, 0.00f, 0.5f, 0.001f, 0.001f};
        float circleRadiusV = display.circleRadius;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Circle radius","display_circle_radius", &circleRadiusV, circleRadiusFS, circleRadiusD)){
            update = true;
            display.circleRadius = circleRadiusV;
        }
    }

    ImGui::EndDisabled();
    ImGui::Unindent();

    ImGui::Text("Visibility:");
    ImGui::Indent();
    if(ImGui::Checkbox("Show cloud###display_show_cloud", &display.showCapture)){
        update = true;
    }
    if(ImGui::Checkbox("Show camera frustum###display_show_camera_frustum", &display.showCameraFrustum)){
        update = true;
    }
    if(ImGui::Checkbox("Show filtering gizmos###display_show_filtering_gizmos", &display.showFilteringGizmos)){
        update = true;
    }
    if(ImGui::Checkbox("Show bodytracking###display_show_body_tracking", &display.showBodyTracking)){
        update = true;
    }
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_model_tab_item(const std::string &tabItemName, cam::DCModelSettings &model) -> bool{


    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    bool update = false;
    auto d = model.transformation.array.data();

    ImGuiUiDrawer::title("CALIBRATION MATRIX");
    if(ImGui::DragFloat4("###r0", d, 0.001f)){
        update = true;
    }
    if(ImGui::DragFloat4("###r1", d + 4, 0.001f)){
        update = true;
    }
    if(ImGui::DragFloat4("###r2", d + 8, 0.001f)){
        update = true;
    }
    if(ImGui::DragFloat4("###r3", d + 12, 0.001f)){
        update = true;
    }

    if(ImGui::Button("Reset###model_matrix_reset_button")){
        model.transformation = geo::Mat4f::identity();
        update = true;
    }
    ImGui::Spacing();
    ImGui::Separator();

    ImGuiUiDrawer::title("EXTRA TRANSFORMATION");

    ImGui::Spacing();
    ImGui::Text("Transformation to be aplied on the model matrix");    
    ImGui::Text("Rotation (euler angles):");

    auto rotPtr = model.rotation.array.data();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","rot_x", rotPtr, modelRotFs, modelRotDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","rot_y", rotPtr+1, modelRotFs, modelRotDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","rot_z", rotPtr+2, modelRotFs, modelRotDs)){
        update = true;
    }

    ImGui::Text("Translation (mm):");
    auto trPtr = model.translation.array.data();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","tr_x", trPtr, modelTrFs, modelTrDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","tr_y", trPtr+1, modelTrFs, modelTrDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","tr_z", trPtr+2, modelTrFs, modelTrDs)){
        update = true;
    }

    ImGui::Text("Scaling:");
    auto scPtr = model.scaling.array.data();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","sc_x", scPtr, modelScFs, modelScDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","sc_y", scPtr+1, modelScFs, modelScDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","sc_z", scPtr+2, modelScFs, modelScDs)){
        update = true;
    }

    if(ImGui::Button("Apply###model_apply_transformation_button")){
        model.transformation = geo::transform(model.scaling,model.rotation,model.translation) * model.transformation;
        model.rotation       = {};
        model.translation    = {};
        model.scaling        = {1.f,1.f,1.f};
        update = true;
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset###model_reset_transformation_button")){
        model.rotation       = {};
        model.translation    = {};
        model.scaling        = {1.f,1.f,1.f};
        update = true;
    }

    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_delay_tab_item(const std::string &tabItemName, cam::DCDelaySettings &delayS) -> bool{
    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    bool update = false;

    ImGuiUiDrawer::title("DELAY");

    int delay = static_cast<int>(delayS.delayMs);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Current delay", "delay", &delay, ImGuiIntS{0,0, 5000,1.f,100},ImGuiDragS())){
        delayS.delayMs = delay;
        update = true;
    }

    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_recorder_tab_item(
    const std::string &tabItemName,
    cam::DCVideoRecorderStates &rStates,
    cam::DCVideoRecorderSettings &rSettings,
    bool isServer) -> bool{

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.data())){
        return false;
    }

    ImGuiUiDrawer::title("ACTIONS");
    if(!rStates.isRecording){
        if(ImGui::Button("Start")){
            rStates.startRecording = true;
        }
    }else{
        if(ImGui::Button("Stop")){
            rStates.stopRecording = true;
        }
    }
    ImGui::SameLine();
    if(!rStates.isRecording){
        if(ImGui::Button("Reset")){
            rStates.resetRecording = true;
        }
    }

    ImGuiUiDrawer::text(std::format("Recording duration: {:6.4}", rStates.duration/1000.0));

    ImGui::Text("Nb frames recorded:");
    ImGui::Indent();
    for(size_t ii = 0; ii < rStates.nbFramesRecorded.size(); ++ii){
        ImGuiUiDrawer::text(std::format("Camera {}: Count: {} Current frame id: {}", ii,  rStates.nbFramesRecorded[ii], rStates.currentFrames[ii]));
    }
    ImGui::Unindent();

    ImGuiIntS iSettings;
    iSettings.min = 0;
    iSettings.max = static_cast<int>(rStates.duration);
    iSettings.speedInc = 30;
    iSettings.speedDrag = 10;

    ImGuiDragS dSettings;
    dSettings.widthDrag = 150;
    dSettings.decButton = true;
    dSettings.incButton = true;
    dSettings.displayText = true;
    dSettings.defaultButton = false;
    dSettings.displayTextLeft = true;

    int currentTime = static_cast<int>(rStates.currentTime);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Timeline (ms): ", "recorder_timeline", &currentTime, iSettings, dSettings)){
        rStates.currentTime = currentTime;
        rStates.moveTime = true;        
    }

    if(ImGui::Button("Save")){        
        ImGuiFileDialog::Instance()->OpenDialog("Save recording", "Choose file to save", ".kvid", ".");
    }

    ImGuiUiDrawer::title("SETTINGS");
    bool update = false;
    ImGui::Text("Max number of frames per camera:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50.f);
    if(ImGui::DragInt("###recorder_max_frames", &rSettings.deviceMaxFramesToRecord, 1.0f, 1, 100000)){
        update = true;
    }

    if(isServer){
        ImGuiUiDrawer::text_wrapped("[INFO] In order to be recorded, device data must be available from [LOCAL GENERATION] in device settings tab."sv, geo::Pt4f{0.5f,1.f,0.5f,1.f});
    }else{
        ImGuiUiDrawer::text_wrapped("[INFO] In order to be recorded, device data must be available from [LOCAL GENERATION] in device settings tab, if device is remote, use [LOCAL GENERATION] from [CLIENT SIDE]. "sv,  geo::Pt4f{0.5f,1.f,0.5f,1.f});
    }

    ImGui::Separator();

    ImGui::Text("[STORAGE]");
    ImGui::Indent();
    {
        ImGui::Text("[Primary]");
        ImGui::Indent();

        if(ImGui::Checkbox("depth###send_depth", &rSettings.dataFrameGenS.addDepth)){
            update = true;
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(!rSettings.dataFrameGenS.addDepth);
        bool compress = rSettings.dataFrameGenS.depthCM == cam::DCCompressionMode::FastPFor;
        if(ImGui::Checkbox("compress###stor_depth_comp", &compress)){
            rSettings.dataFrameGenS.depthCM = compress ? cam::DCCompressionMode::FastPFor : cam::DCCompressionMode::None;
            update = true;
        }
        ImGui::EndDisabled();

        if(ImGui::Checkbox("depth sized color###stor_depth_sized_color", &rSettings.dataFrameGenS.addDepthSizedColor)){
            update = true;
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(!rSettings.dataFrameGenS.addDepthSizedColor);
        compress = rSettings.dataFrameGenS.depthSizedColorCM == cam::DCCompressionMode::JPEG;
        if(ImGui::Checkbox("compress###stor_depth_sized_color_comp", &compress)){
            rSettings.dataFrameGenS.depthSizedColorCM = compress ? cam::DCCompressionMode::JPEG : cam::DCCompressionMode::None;
            update = true;
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(50.f);
        int compressionRate = rSettings.dataFrameGenS.depthSizedColorJPEGCQ;
        if(ImGui::SliderInt("###stor_depth_sized_color_comp_quality", &compressionRate, 15, 100)){
            rSettings.dataFrameGenS.depthSizedColorJPEGCQ = static_cast<std::uint8_t>(compressionRate);
            update = true;
        }
        ImGui::SameLine();
        ImGui::Text("q");

        ImGui::EndDisabled();
        ImGui::Unindent();

        ImGui::Separator();
        ImGui::Text("[Secondary]");
        ImGui::Indent();

        if(ImGui::Checkbox("original size color###stor_original_color", &rSettings.dataFrameGenS.addOriginalSizeColor)){
            update = true;
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(!rSettings.dataFrameGenS.addOriginalSizeColor);
        compress = rSettings.dataFrameGenS.originalSizeColorCM == cam::DCCompressionMode::JPEG;
        if(ImGui::Checkbox("compress###stor_original_color_comp", &compress)){
            rSettings.dataFrameGenS.originalSizeColorCM = compress ? cam::DCCompressionMode::JPEG : cam::DCCompressionMode::None;
            update = true;
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(50.f);
        compressionRate = rSettings.dataFrameGenS.originalSizeColorJPEGCQ;
        if(ImGui::SliderInt("###stor_original_size_color_comp_quality", &compressionRate, 15, 100)){
            rSettings.dataFrameGenS.originalSizeColorJPEGCQ = static_cast<std::uint8_t>(compressionRate);
            update = true;
        }
        ImGui::SameLine();
        ImGui::Text("q");
        ImGui::EndDisabled();

        if(ImGui::Checkbox("infra###stor_infra", &rSettings.dataFrameGenS.addInfra)){
            update = true;
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(!rSettings.dataFrameGenS.addInfra);
        compress = rSettings.dataFrameGenS.infraCM == cam::DCCompressionMode::FastPFor;
        if(ImGui::Checkbox("compress###stor_infra_comp", &compress)){
            rSettings.dataFrameGenS.infraCM = compress ? cam::DCCompressionMode::FastPFor : cam::DCCompressionMode::None;
            update = true;
        }
        ImGui::EndDisabled();
        ImGui::Unindent();
    }

    ImGui::Unindent();

    ImGui::Text("[LOCAL GENERATION]");

    ImGui::Indent();
    {
        ImGui::Text("[Cloud]");
        ImGui::Indent();

        if(ImGui::Checkbox("generate###gen_recorder_gen_cloud", &rSettings.frameGenS.cloud)){
            update = true;
        }
        ImGui::SameLine();
        cloud_color_mode_combo(""sv, "###gen_recorder_cloud_color_mode", rSettings.frameGenS.cloudColorMode, update);

        ImGui::Unindent();

        ImGui::Text("[Images]");
        ImGui::Indent();
        if(ImGui::Checkbox("original size color###gen_recorder_color_image", &rSettings.frameGenS.originalSizeColorImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("depth sized color###gen_recorder_depth_sized_color_image", &rSettings.frameGenS.depthSizedColorImage)){
            update = true;
        }

        if(ImGui::Checkbox("depth###gen_recorder_depth_image", &rSettings.frameGenS.depthImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("infra###gen_recorder_infra_image", &rSettings.frameGenS.infraImage)){
            update = true;
        }

        ImGui::Unindent();
    }
    ImGui::Unindent();



    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_player_tab_item(
    const std::string &tabItemName,
    cam::DCVideoPlayerStates &pStates,
    cam::DCVideoPlayerSettings &pSettings) -> bool{

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.data())){
        return false;
    }

    ImGuiUiDrawer::title("ACTIONS");

    if(!pStates.isPlaying){
        if(ImGui::Button("Play")){
            pStates.play = true;
        }
    }else{
        if(ImGui::Button("Pause")){
            pStates.pause = true;
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart")){
        pStates.restart = true;
    }

    ImGuiUiDrawer::text(std::format("Loaded video file: [{}]", pStates.fileName));
    ImGuiUiDrawer::text(std::format("Current time: {:6.4}", pStates.currentTime/1000.0));
    ImGui::SameLine();
    ImGuiUiDrawer::text(std::format("Duration: [{:6.4}]", pStates.duration/1000.0));

    ImGui::Text("Frames:");
    ImGui::Indent();
    for(size_t ii = 0; ii < pStates.nbFrames.size(); ++ii){
        ImGuiUiDrawer::text(std::format("Camera {}: Count: {} Current frame id: {}", ii,  pStates.nbFrames[ii], pStates.currentFrames[ii]));
    }
    ImGui::Unindent();

    ImGuiIntS iSettings;
    iSettings.min = 0;
    iSettings.max = static_cast<int>(pStates.duration);
    iSettings.speedInc = 30;
    iSettings.speedDrag = 10;

    ImGuiDragS dSettings;
    dSettings.widthDrag = 150;
    dSettings.decButton = true;
    dSettings.incButton = true;
    dSettings.displayText = true;
    dSettings.defaultButton = false;
    dSettings.displayTextLeft = true;

    int currentTime = static_cast<int>(pStates.currentTime);    
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Timeline (ms):", "player_timeline", &currentTime, iSettings, dSettings)){
        pStates.moveTime = true;
        pStates.currentTime = currentTime;
    }

    ImGui::Text("Edit");
    ImGui::Indent();
    if(ImGui::Button("Remove until current frame")){
        pStates.removeUntil = true;
    }
    if(ImGui::Button("Remove after current frame")){
        pStates.removeAfter = true;
    }
    // if(ImGui::Button("Merge")){
    //     pStates.merge = true;
    // }
    // if(ImGui::Button("Info")){
    //     pStates.info = true;
    // }
    ImGui::Unindent();

    ImGui::Text("I/O");
    ImGui::Indent();
    if(ImGui::Button("Load video")){
        ImGuiFileDialog::Instance()->OpenDialog("Load video", "Choose video to load", ".kvid", ".");
    }
    ImGui::SameLine();
    if(ImGui::Button("Save video")){
        ImGuiFileDialog::Instance()->OpenDialog("Save video", "Choose video to save", ".kvid", ".");
    }
    ImGui::SameLine();
    if(ImGui::Button("Save cloud")){
        ImGuiFileDialog::Instance()->OpenDialog("Save cloud", "Choose cloud to save", ".obj", ".");
    }
    ImGui::Unindent();

    ImGuiUiDrawer::title("SETTINGS");
    bool update = false;

    if(ImGui::Checkbox("Do loop", &pSettings.doLoop)){
        update = true;
    }

    ImGui::Text("[LOCAL GENERATION]");

    ImGui::Indent();
    {
        ImGui::Text("[Cloud]");
        ImGui::Indent();

        if(ImGui::Checkbox("generate###gen_player_gen_cloud", &pSettings.generation.cloud)){
            update = true;
        }
        ImGui::SameLine();
        cloud_color_mode_combo(""sv, "###gen_player_cloud_color_mode", pSettings.generation.cloudColorMode, update);

        ImGui::Unindent();

        ImGui::Text("[Images]");
        ImGui::Indent();
        if(ImGui::Checkbox("original size color###gen_player_color_image", &pSettings.generation.originalSizeColorImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("depth sized color###gen_player_depth_sized_color_image", &pSettings.generation.depthSizedColorImage)){
            update = true;
        }

        if(ImGui::Checkbox("depth###gen_player_depth_image", &pSettings.generation.depthImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("infra###gen_player_infra_image", &pSettings.generation.infraImage)){
            update = true;
        }

        ImGui::Unindent();
    }
    ImGui::Unindent();


    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_calibrator_tab_item(
    const std::string &tabItemName,
    bool useNormalFilteringSettings,
    cam::DCCalibratorStates &cStates,
    DCCalibratorDisplaySettings &cdSettings,
    cam::DCCalibratorSettings &cSettings) -> bool {

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.data())){
        return false;
    }
    bool update = false;

    int filteringMode = useNormalFilteringSettings ? 0 : 1;
    ImGui::Text("Use filtering mode:");
    ImGui::SameLine();
    if(ImGui::RadioButton("Normal###filters_normal_mode", &filteringMode, 0)){
        cStates.updateFilteringMode = true;
        cStates.filteringMode = filteringMode;
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Calibration###filters_calibration_mode", &filteringMode, 1)){
        cStates.updateFilteringMode = true;
        cStates.filteringMode = filteringMode;
    }
    ImGui::Spacing();


    ImGuiDragS ds;
    ds.widthDrag     = 80.f;
    ds.defaultButton = true;
    ds.displayTextLeft = false;

    ImGuiUiDrawer::title("CLIENTS TO USE");
    {
        ImGui::Text("Model:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        if(ImGuiUiDrawer::combo("###model_calibrator", &cSettings.modelSelectionId, cSettings.models)){
            update = true;
            cStates.updateDisplaySettings = true;
        }

        ImGui::SameLine();
        ImGui::Text("Source:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        if(ImGuiUiDrawer::combo("###source_calibrator", &cSettings.sourceSelectionId, cSettings.sources)){
            update = true;
            cStates.updateDisplaySettings = true;
        }
    }

    ImGuiUiDrawer::title("CAPTURE");
    {
        int duration = static_cast<int>(cSettings.durationMs);
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Duration (ms)", "capture duration", &duration, {5000, 100, 120000, 100, 1000}, ds)){
            cSettings.durationMs = duration;
            update = true;
        }

        int timeToWait = static_cast<int>(cSettings.timeToWaitBeforeRegisteringMs);
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Time to wait before registering (ms)", "capture wait", &timeToWait, {0, 100, 120000, 100, 1000}, ds)){
            cSettings.timeToWaitBeforeRegisteringMs = timeToWait;
            update = true;
        }

        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Max number of frames", "capture max frames nb", &cSettings.maxFramesToRecord, {1000, 1, 10000, 1, 100}, ds)){
            update = true;
        }

        ImGuiUiDrawer::text(std::format("Time elasped [{}] ms", cStates.elapsedTime.count()));
        ImGuiUiDrawer::text("Frames registered count:"sv);
        for(size_t ii = 0; ii < cStates.nbFramesRegistered.size(); ++ii){
            ImGuiUiDrawer::text(std::format("Cloud [{}], nb frames [{}]", ii, cStates.nbFramesRegistered[ii]));
        }

        if(ImGui::Button("Reset")){
            cStates.resetCalibration = true;
        }
        ImGui::SameLine();
        if(!cStates.isRegistering){
            if(ImGui::Button("Start")){
                cStates.startCalibration = true;
            }
        }else{
            if(ImGui::Button("Stop")){
                cStates.stopCalibration = true;
            }
        }
    }

    ImGuiUiDrawer::title("PROCESSING");
    {
        if(ImGui::Checkbox("Use processed clouds", &cSettings.useProcessed)){
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }

        if(ImGui::Checkbox("Remove outliers", &cSettings.removeOutliers)){
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }
        ImGui::SameLine();
        ImGui::Text("with max distance");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if(ImGui::DragFloat("###Max distance of outliers", &cSettings.maxDistanceOutlier, 0.01f, 0.01f, 2.f)){
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }

        ImGui::Spacing();

        if(ImGui::Checkbox("Downsample", &cSettings.downSample)){
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }
        ImGui::SameLine();
        ImGui::Text("with voxel size");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        float dVS = static_cast<float>(cSettings.downSampleVoxelSize);
        if(ImGui::DragFloat("###Downsample voxel size", &dVS, 0.01f, 0.01f, 1.f)){
            cSettings.downSampleVoxelSize = dVS;
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }

        if(ImGui::Checkbox("Compute sphere center", &cSettings.computeSphereCenter)){
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }
        ImGui::SameLine();
        ImGui::Text("with ray");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if(ImGui::DragFloat("###Sphere ray", &cSettings.ballRay, 0.01f, 0.01f, 2.f)){
            update = true;
            cStates.recomputeRegisteringProcessing = true;
        }
    }

    ImGuiUiDrawer::title("DISPLAY");
    {
        if(ImGui::Checkbox("Calibration clouds", &cdSettings.displayCalibrationCloud)){
            cStates.updateDisplaySettings = true;
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("Processed clouds", &cdSettings.displayProcessedCloud)){
            cStates.updateDisplaySettings = true;
            update = true;
        }
    }

    ImGuiUiDrawer::title("CALIBRATION");
    {
        ImGui::Text("Registation:");
        ImGui::SameLine();
        if (ImGui::RadioButton("RANSAC", cSettings.doRansac)) {
            cSettings.doRansac = true;
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("ICP", !cSettings.doRansac)){
            cSettings.doRansac = false;
            update = true;
        }

        ImGui::Checkbox("Show advanced parameters", &cSettings.advancedParameters);
        if(cSettings.advancedParameters){
            if(cSettings.doRansac){

                if(ImGuiUiDrawer::draw_drag_int_with_buttons("Nb Tries", "Ransac nb tries", &cSettings.ransac.nbTries, {10,1,1000, 1, 1}, ds)){
                    update = true;
                }
                if(ImGuiUiDrawer::draw_drag_int_with_buttons("N", "Ransac N", &cSettings.ransac.N, {3,1,100, 1, 1}, ds)){
                    update = true;
                }
                if(ImGuiUiDrawer::draw_drag_int_with_buttons("Max iteration", "Ransac max ite", &cSettings.ransac.maxIteration, {100000,1,100000000, 1, 1000}, ds)){
                    update = true;
                }

                ImGuiFloatS fs;
                fs.defaultValue = 0.999f;
                fs.min          = 0.009f;
                fs.max          = 0.99999f;
                fs.speedDrag    = 0.00001f;
                fs.speedInc     = 0.001f;
                fs.format       = "%.5f";

                float confidence = static_cast<float>(cSettings.ransac.confidence);
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Confidence","Ransac confidence", &confidence, fs, ds)){
                    cSettings.ransac.confidence = confidence;
                    update = true;
                }

                float distanceThreshold = static_cast<float>(cSettings.ransac.distanceThreshold);
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Distance threshold","Ransac dist thresh", &distanceThreshold, fs, ds)){
                    cSettings.ransac.distanceThreshold = distanceThreshold;
                    update = true;
                }

                fs.defaultValue = 0.9f;
                fs.min          = 0.8f;
                fs.max          = 0.99f;
                fs.speedDrag    = 0.001f;
                fs.speedInc     = 0.01f;
                fs.format       = "%.2f";
                float similaritiesThreshold = static_cast<float>(cSettings.ransac.similaritiesThreshold);
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Similarities threshold","Ransac sim thresh", &similaritiesThreshold, fs, ds)){
                    cSettings.ransac.similaritiesThreshold = similaritiesThreshold;
                    update = true;
                }

                if(ImGui::Checkbox("Mutal filter", &cSettings.ransac.mutualFilter)){
                    update = true;
                }
                ImGui::Unindent();
            }else{

                if(ImGuiUiDrawer::draw_drag_int_with_buttons("Max iteration", "ICP max it", &cSettings.icpMaxIteration, {30,1,1000, 1, 1}, ds)){
                    update = true;
                }

                ImGuiFloatS fs;
                fs.defaultValue = 0.999f;
                fs.min          = 0.009f;
                fs.max          = 0.99999f;
                fs.speedDrag    = 0.00001f;
                fs.speedInc     = 0.001f;
                fs.format       = "%.5f";

                float maxDistCorr = static_cast<float>(cSettings.icpMaxDistanceCorr);
                if(ImGuiUiDrawer::draw_drag_float_with_buttons("Max distance correspondance","ICP max dist corr", &maxDistCorr, fs, ds)){
                    cSettings.icpMaxDistanceCorr = maxDistCorr;
                    update = true;
                }
            }
        }

        if(ImGui::Button("Calibrate")){
            cStates.calibrate = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Validate calibration")){
            cStates.validateCalibration = true;
        }
    }

    ImGui::EndTabItem();

    return update;
}

auto DCUIDrawer::draw_dc_device_settings_tab_item( const std::string &tabItemName, cam::DCDeviceSettings &device, std::optional<cam::DCClientType> clientType) -> std::tuple<bool,bool>{

    bool update = false;

    if (ImGui::BeginTabItem(tabItemName.c_str())){
        update |= draw_dc_config(device.configS);
        update |= draw_dc_data_settings(device.configS.typeDevice, device.dataS, clientType);
        ImGui::EndTabItem();
        return {true, update};
    }
    return {false, update};
}

auto DCUIDrawer::draw_dc_config(cam::DCConfigSettings &config) -> bool{

    ImGuiUiDrawer::title("CONFIG");
    ImGui::Spacing();

    ImGui::Text("Device:");
    ImGui::Indent();

    bool update = false;

    if(ImGui::Checkbox("Open###settings_open_device", &config.openDevice)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Start reading###settings_open_camera", &config.startReading)){
        update = true;
    }

    ImGui::Spacing();

    // init
    if(modesNames.empty()){
        for(const auto &m : k4Modes){
            modesNames[m] = std::string(cam::dc_mode_name(m));
        }
        for(const auto &m : fbModes){
            modesNames[m] = std::string(cam::dc_mode_name(m));
        }
        for(const auto &m : fmeModes){
            modesNames[m] = std::string(cam::dc_mode_name(m));
        }
        for(const auto &m : fmuModes){
            modesNames[m] = std::string(cam::dc_mode_name(m));
        }
    }

    ImGui::Text("Type:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.f);

    int guiCurrentTypeSelection = static_cast<int>(config.typeDevice);
    if(ImGui::Combo("###settings_device_type", &guiCurrentTypeSelection, devicesTypes, IM_ARRAYSIZE(devicesTypes))){        
        auto nDeviceType = static_cast<cam::DCType>(guiCurrentTypeSelection);
        if(nDeviceType != config.typeDevice){
            config.typeDevice = nDeviceType;
            config.mode       = cam::dc_default_mode(config.typeDevice);
            update = true;
        }
    }

    if(config.typeDevice != cam::DCType::FemtoMegaEthernet){

        if(ImGui::Checkbox("Use serial number", &config.useSerialNumber)){
            update = true;
        }

        if(!config.useSerialNumber){

            ImGui::Text("Device id:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(230.f);

            if(devicesNames.size() != 0){
                if(ImGui::BeginCombo("###settings_device_id", devicesNames[config.idDevice].c_str())){
                    for(size_t ii = 0; ii < devicesNames.size(); ++ii){
                        bool selected = ii == config.idDevice;
                        if (ImGui::Selectable(devicesNames[ii].c_str(),selected)){
                            config.idDevice = static_cast<std::uint32_t>(ii);
                            update = true;
                        }
                        if(selected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }else{
                ImGui::Text("No device found.");
            }
        }else{
            ImGui::Text("Device serial number:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150.f);
            if(ImGui::InputText("###settings_serial_number", &config.serialNumber)){
                update = true;
            }
        }

    }else{
        ImGui::Text("Device IPV4 address:");
        int ipv4p = config.ipv4Address.x();
        ImGui::SetNextItemWidth(75.f);
        if(ImGui::InputInt("###ipv4_1", &ipv4p, 1, 1)){
            config.ipv4Address.x() = static_cast<std::uint8_t>(ipv4p);
            update = true;
        }
        ImGui::SameLine();
        ipv4p = config.ipv4Address.y();
        ImGui::SetNextItemWidth(75.f);
        if(ImGui::InputInt("###ipv4_2", &ipv4p, 1, 1)){
            config.ipv4Address.y() = static_cast<std::uint8_t>(ipv4p);
            update = true;
        }
        ImGui::SameLine();
        ipv4p = config.ipv4Address.z();
        ImGui::SetNextItemWidth(75.f);
        if(ImGui::InputInt("###ipv4_3", &ipv4p, 1, 1)){
            config.ipv4Address.z() = static_cast<std::uint8_t>(ipv4p);
            update = true;
        }
        ImGui::SameLine();
        ipv4p = config.ipv4Address.w();
        ImGui::SetNextItemWidth(75.f);
        if(ImGui::InputInt("###ipv4_4", &ipv4p, 1, 1)){
            config.ipv4Address.w() = static_cast<std::uint8_t>(ipv4p);
            update = true;
        }
    }


    ImGui::Spacing();
    ImGui::Text("Mode:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(230.f);

    auto currentModeName = modesNames[config.mode];
    if(config.typeDevice == cam::DCType::FemtoBolt){
        if(ImGui::BeginCombo("###settings_mode", currentModeName.c_str())){
            for(const auto &m : fbModes){
                bool selected = m == config.mode;
                if (ImGui::Selectable(modesNames[m].c_str(), selected)){
                    config.mode = m;
                    update = true;
                }
                if(selected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }else if(config.typeDevice == cam::DCType::FemtoMegaEthernet){        
        if(ImGui::BeginCombo("###settings_mode", currentModeName.c_str())){
            for(const auto &m : fmeModes){
                bool selected = m == config.mode;                
                if (ImGui::Selectable(modesNames[m].c_str(), selected)){
                    config.mode = m;
                    update = true;
                }
                if(selected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }else if(config.typeDevice == cam::DCType::FemtoMegaUSB){
        if(ImGui::BeginCombo("###settings_mode", currentModeName.c_str())){
            for(const auto &m : fmuModes){
                bool selected = m == config.mode;
                if (ImGui::Selectable(modesNames[m].c_str(), selected)){
                    config.mode = m;
                    update = true;
                }
                if(selected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }else if(config.typeDevice == cam::DCType::AzureKinect){
        if(ImGui::BeginCombo("###settings_mode", currentModeName.c_str())){
            for(const auto &m : k4Modes){
                bool selected = m == config.mode;
                if (ImGui::Selectable(modesNames[m].c_str(), selected)){
                    config.mode = m;
                    update = true;
                }
                if(selected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Synch:");
    ImGui::Indent();

    {
        if(config.typeDevice != DCType::AzureKinect){
            ImGui::Text("### Use Orbbec viewer software instead ###");
            ImGui::BeginDisabled(true);
        }

        ImGui::Text("Mode:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.f);
        int guiCurrentSynchModeSelection = static_cast<int>(config.synchMode);
        if(ImGui::Combo("###settings_synch_mode_combo", &guiCurrentSynchModeSelection, synchItems, IM_ARRAYSIZE(synchItems))){
            update          = true;
            config.synchMode = static_cast<cam::DCSynchronisationMode>(guiCurrentSynchModeSelection);
        }

        ImGui::Text("Subordinate delay (usec):");
        ImGui::SetNextItemWidth(80.f);
        ImGui::SameLine();
        if(ImGui::DragInt("###subordinate_delay_usec", &config.subordinateDelayUsec, 1.0f, 0, 100000)){
            update          = true;
        }

        ImGui::Spacing();

        if(ImGui::Checkbox("Synch color and depth images", &config.synchronizeColorAndDepth)){
            update          = true;
        }

        ImGui::Text("Delay between color and depth (usec)");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80.f);
        if(ImGui::DragInt("###delay_between_color_and_depth_usec", &config.delayBetweenColorAndDepthUsec, 1.0f, 100000, 100000)){
            update          = true;
        }
        ImGui::Unindent();

        if(config.typeDevice != DCType::AzureKinect){
            ImGui::EndDisabled();
        }
    }

    // ImGui::Spacing();
    // ImGui::Separator();
    // ImGui::Text("Body tracking: (SOON)");
    // ImGui::Indent();
    // ImGui::BeginDisabled(true);{

    //     if(ImGui::Checkbox("Enable (perfomance costly)", &config.btEnabled)){
    //         update = true;
    //     }

    //     ImGui::Text("Processing mode:");
    //     ImGui::SameLine();
    //     ImGui::SetNextItemWidth(150.f);
    //     int guiProcessingModeBodyTrackingSelection = static_cast<int>(config.btProcessingMode);
    //     if(ImGui::Combo("###settings_processing_mode_body_tracking", &guiProcessingModeBodyTrackingSelection, processingModeBodyTrackingItems, IM_ARRAYSIZE(processingModeBodyTrackingItems))){
    //         config.btProcessingMode = static_cast<DCBTProcessingMode>(guiProcessingModeBodyTrackingSelection);
    //         update = true;
    //     }

    //     // add gpu id
    //     // ...

    //     ImGui::Text("Orientation:");
    //     ImGui::SameLine();
    //     ImGui::SetNextItemWidth(150.f);
    //     int guiOrientationBodyTrackingSelection = static_cast<int>(config.btOrientation);
    //     if(ImGui::Combo("###settings_orientation_body_tracking", &guiOrientationBodyTrackingSelection, orientationBodyTrackingItems, IM_ARRAYSIZE(orientationBodyTrackingItems))){
    //         config.btOrientation = static_cast<DCBTSensorOrientation>(guiOrientationBodyTrackingSelection);
    //         update = true;
    //     }

    // }ImGui::EndDisabled();

    // ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Color-depth alignment:");
    ImGui::Indent();

    ImGui::Text("Translation (mm):");
    auto trPtr = config.colorAlignmentTr.array.data();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","col_depth_alignment_tr_x", trPtr, alignColDepthTrFs, modelTrDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","col_depth_alignment_tr_y", trPtr+1, alignColDepthTrFs, modelTrDs)){
        update = true;
    }
    ImGui::SameLine();
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","col_depth_alignment_tr_z", trPtr+2, alignColDepthTrFs, modelTrDs)){
        update = true;
    }


    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Misc:");
    ImGui::Indent();

    if(ImGui::Checkbox("Disable LED", &config.disableLED)){
        update = true;
    }

    ImGui::Unindent();
    ImGui::Spacing();

    return update;
}



auto DCUIDrawer::draw_dc_data_settings(cam::DCType type, cam::DCDataSettings &data, std::optional<cam::DCClientType> clientType) -> bool{

    ImGuiUiDrawer::title("DATA");

    bool update = false;
    bool isClient = clientType.has_value();
    bool localClient = isClient ? (clientType.value() == DCClientType::Local) : false;

    if(isClient && !localClient){
        ImGuiUiDrawer::title("[SERVER SIDE]", {0.5f,1.f,0.5f,1.f});
    }

    ImGui::Text("[CAPTURE]");
    ImGui::Indent();
    {
        ImGui::Text("[Primary]");
        ImGui::Indent();

        if(ImGui::Checkbox("color###cap_color", &data.server.capture.color)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("depth###cap_depth", &data.server.capture.depth)){
            update = true;
        }

        ImGui::Unindent();

        ImGui::Separator();
        ImGui::Text("[Secondary]");
        ImGui::Indent();

        if(ImGui::Checkbox("infra###cap_infra", &data.server.capture.infra)){
            update = true;
        }

        ImGui::Unindent();
    }
    ImGui::Unindent();


    ImGui::Spacing();

    if(!localClient){

        ImGui::Text("[SENDING]");
        ImGui::Indent();
        {
            ImGui::Text("[Primary]");
            ImGui::Indent();

            if(ImGui::Checkbox("depth###send_depth", &data.server.sending.addDepth)){
                update = true;
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(!data.server.sending.addDepth);
            bool compress = data.server.sending.depthCM == cam::DCCompressionMode::FastPFor;
            if(ImGui::Checkbox("compress###send_depth_comp", &compress)){
                data.server.sending.depthCM = compress ? cam::DCCompressionMode::FastPFor : cam::DCCompressionMode::None;
                update = true;
            }
            ImGui::EndDisabled();

            if(ImGui::Checkbox("depth sized color###send_depth_sized_color", &data.server.sending.addDepthSizedColor)){
                update = true;
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(!data.server.sending.addDepthSizedColor);
            compress = data.server.sending.depthSizedColorCM == cam::DCCompressionMode::JPEG;
            if(ImGui::Checkbox("compress###send_depth_sized_color_comp", &compress)){
                data.server.sending.depthSizedColorCM = compress ? cam::DCCompressionMode::JPEG : cam::DCCompressionMode::None;
                update = true;
            }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(50.f);
            int compressionRate = data.server.sending.depthSizedColorJPEGCQ;
            if(ImGui::SliderInt("###send_depth_sized_color_comp_quality", &compressionRate, 15, 100)){
                data.server.sending.depthSizedColorJPEGCQ = static_cast<std::uint8_t>(compressionRate);
                update = true;
            }
            ImGui::SameLine();
            ImGui::Text("q");

            ImGui::EndDisabled();
            ImGui::Unindent();

            ImGui::Separator();
            ImGui::Text("[Secondary]");
            ImGui::Indent();

            if(ImGui::Checkbox("original size color###send_original_color", &data.server.sending.addOriginalSizeColor)){
                update = true;
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(true);
            compress = data.server.sending.originalSizeColorCM == cam::DCCompressionMode::JPEG;
            if(ImGui::Checkbox("compress###send_original_color_comp", &compress)){
                data.server.sending.originalSizeColorCM = compress ? cam::DCCompressionMode::JPEG : cam::DCCompressionMode::None;
                update = true;
            }

            ImGui::SameLine();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(!data.server.sending.addOriginalSizeColor);
            ImGui::SetNextItemWidth(50.f);
            compressionRate = data.server.sending.originalSizeColorJPEGCQ;
            if(ImGui::SliderInt("###send_original_size_color_comp_quality", &compressionRate, 15, 100)){
                data.server.sending.originalSizeColorJPEGCQ = static_cast<std::uint8_t>(compressionRate);
                update = true;
            }
            ImGui::SameLine();
            ImGui::Text("q");
            ImGui::EndDisabled();

            if(ImGui::Checkbox("infra###send_com_infra", &data.server.sending.addInfra)){
                update = true;
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(!data.server.sending.addInfra);
            compress = data.server.sending.infraCM == cam::DCCompressionMode::FastPFor;
            if(ImGui::Checkbox("compress###send_comp_infra", &compress)){
                data.server.sending.infraCM = compress ? cam::DCCompressionMode::FastPFor : cam::DCCompressionMode::None;
                update = true;
            }
            ImGui::EndDisabled();
            ImGui::Unindent();
        }
        ImGui::Unindent();
        ImGui::Spacing();

    }

    ImGui::Text("[LOCAL GENERATION]");

    ImGui::Indent();
    {
        ImGui::Text("[Cloud]");
        ImGui::Indent();

        if(ImGui::Checkbox("generate###gen_server_gen_cloud", &data.server.generation.cloud)){
            update = true;
        }
        ImGui::SameLine();
        cloud_color_mode_combo(""sv, "###gen_server_cloud_color_mode", data.server.generation.cloudColorMode, update);

        ImGui::Unindent();

        ImGui::Text("[Images]");
        ImGui::Indent();
        if(ImGui::Checkbox("original size color###gen_server_color_image", &data.server.generation.originalSizeColorImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("depth sized color###gen_server_depth_sized_color_image", &data.server.generation.depthSizedColorImage)){
            update = true;
        }

        if(ImGui::Checkbox("depth###gen_server_depth_image", &data.server.generation.depthImage)){
            update = true;
        }
         ImGui::SameLine();
        if(ImGui::Checkbox("infra###gen_server_infra_image", &data.server.generation.infraImage)){
            update = true;
        }

        ImGui::Unindent();
    }
    ImGui::Unindent();


    if(!isClient || localClient){
        return update;
    }

    ImGuiUiDrawer::title("[CLIENT SIDE]", {0.5f,1.f,0.5f,1.f});

    ImGui::Text("[LOCAL GENERATION]");
    ImGui::Indent();
    {
        ImGui::Text("[Cloud]");
        ImGui::Indent();

        if(ImGui::Checkbox("generate###gen_client_gen_cloud", &data.clientGeneration.cloud)){
            update = true;
        }
        ImGui::SameLine();
        cloud_color_mode_combo(""sv, "###gen_client_cloud_color_mode", data.clientGeneration.cloudColorMode, update);

        ImGui::Unindent();

        ImGui::Text("[Images]");
        ImGui::Indent();
        if(ImGui::Checkbox("original size color###gen_client_color_image", &data.clientGeneration.originalSizeColorImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("depth sized color###gen_client_depth_sized_color_image", &data.clientGeneration.depthSizedColorImage)){
            update = true;
        }

        if(ImGui::Checkbox("depth###gen_client_depth_image", &data.clientGeneration.depthImage)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("infra###gen_client_infra_image", &data. clientGeneration.infraImage)){
            update = true;
        }

        ImGui::Unindent();
    }
    ImGui::Unindent();

    return update;
}

auto get_imgui_int_scale(ColorSettingsType sType, DCType dType) -> tool::ImGuiIntS{
    tool::ImGuiIntS intS;
    intS.min            = min_value(sType,dType);
    intS.max            = max_value(sType,dType);
    intS.defaultValue   = default_value(sType,dType);
    intS.speedInc       = step_value(sType,dType);
    intS.speedDrag      = intS.speedInc; // TODO: ...
    return intS;
}

auto DCUIDrawer::draw_dc_colors_settings_tab_item(const std::string &tabItemName, cam::DCType type, cam::DCColorSettings &colors) -> std::tuple<bool,bool>{

    using CST = ColorSettingsType;

    bool update = false;

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return {false, update};
    }

    int value;
    ImGuiDragS ds;
    ds.widthDrag = 100.f;

    ImGuiUiDrawer::title("CAMERA HARDWARE SETTINGS");

    if(is_available(CST::Auto_exposure, type)){
        if(ImGui::Checkbox("Auto exposure###auto_exposure_time", &colors.autoExposureTime)){
            update = true;
        }
    }

    ImGui::BeginDisabled(colors.autoExposureTime);
    if(is_available(CST::Exposure, type)){
        value = colors.exposureTime;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Exposure", "color_exposure", &value, get_imgui_int_scale(CST::Exposure, type), ds)){
            colors.exposureTime = static_cast<std::uint16_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }
    if(is_available(CST::Gain, type)){
        value = colors.gain;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Gain", "color_gain", &value, get_imgui_int_scale(CST::Gain, type), ds)){
            colors.gain = static_cast<std::uint8_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }
    ImGui::EndDisabled();

    if(is_available(CST::Brightness, type)){
        value = colors.brightness;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Brightness", "color_brightness", &value, get_imgui_int_scale(CST::Brightness, type), ds)){
            colors.brightness = static_cast<std::uint8_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Contrast, type)){
        value = colors.contrast;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Contrast", "color_contrast", &value, get_imgui_int_scale(CST::Contrast, type), ds)){
            colors.contrast = static_cast<std::uint8_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Saturation, type)){
        value = colors.saturation;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Saturation", "color_saturation", &value, get_imgui_int_scale(CST::Saturation, type), ds)){
            colors.saturation = static_cast<std::uint8_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Sharpness, type)){
        value = colors.sharpness;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Sharpness", "color_sharpness", &value, get_imgui_int_scale(CST::Sharpness, type), ds)){
            colors.sharpness = static_cast<std::uint8_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Auto_white_balance, type)){
        if(ImGui::Checkbox("Auto white balance###auto_white_balance", &colors.autoWhiteBalance)){
            update = true;
        }
    }

    ImGui::BeginDisabled(colors.autoWhiteBalance);
    if(is_available(CST::White_balance, type)){
        value = colors.whiteBalance;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Color temperature (K)", "color_white_balance", &value, get_imgui_int_scale(CST::White_balance, type), ds)){
            colors.whiteBalance = static_cast<std::uint16_t>(value);
            update = true;
        }
        ImGui::Spacing();
    }
    ImGui::EndDisabled();

    if(is_available(CST::Backlight_compensation, type)){
        if(ImGui::Button("D###default_Backlight compensation")){
            colors.backlightCompensation = default_value(CST::Backlight_compensation, type);
            update = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("Backlight compensation", &colors.backlightCompensation)){
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Power_line_frequency, type)){

        if(ImGui::Button("D###default_Powerline frequency")){
            colors.powerlineFrequency = static_cast<DCPowerlineFrequency>(default_value(CST::Power_line_frequency, type));
            update = true;
        }
        ImGui::SameLine();

        int guiSel;
        if(type == DCType::AzureKinect){

            if(colors.powerlineFrequency == DCPowerlineFrequency::F50){
                guiSel = 0;
            }else{
                guiSel = 1;
            }

            ImGui::SetNextItemWidth(100.f);
            if(ImGui::Combo("###settings_mode_combo", &guiSel, k4PowerlineFrequencyItems, IM_ARRAYSIZE(k4PowerlineFrequencyItems))){
                update       = true;

                if(guiSel == 0){
                    colors.powerlineFrequency = DCPowerlineFrequency::F50;
                }else{
                    colors.powerlineFrequency = DCPowerlineFrequency::F60;
                }
            }
            
        }else if(type == DCType::FemtoBolt || type == DCType::FemtoMegaEthernet || type == DCType::FemtoMegaUSB){

            if(colors.powerlineFrequency == DCPowerlineFrequency::Undefined){
                guiSel = 0;
            }else if(colors.powerlineFrequency == DCPowerlineFrequency::F50){
                guiSel = 1;
            }else{
                guiSel = 2;
            }

            ImGui::SetNextItemWidth(100.f);
            if(ImGui::Combo("###settings_mode_combo", &guiSel, obPowerlineFrequencyItems, IM_ARRAYSIZE(obPowerlineFrequencyItems))){
                update       = true;

                if(guiSel == 0){
                    colors.powerlineFrequency = DCPowerlineFrequency::Undefined;
                }else if(guiSel == 1){
                    colors.powerlineFrequency = DCPowerlineFrequency::F50;
                }else{
                    colors.powerlineFrequency = DCPowerlineFrequency::F60;
                }
            }
        }
        ImGui::SameLine();
        ImGui::Text("Powerline frequency");
        ImGui::Spacing();
    }

    if(is_available(CST::HDR, type)){

        if(ImGui::Button("D###default_hdr")){
            colors.hdr = default_value(CST::HDR, type);
            update     = true;
        }
        ImGui::SameLine();
        if(ImGui::Checkbox("HDR", &colors.hdr)){
            update = true;
        }
        ImGui::Spacing();
    }

    ImGui::EndTabItem();

    return {true, update};
}



//        ImGui::Text("Mask:");
//        ImGui::Indent();
//        {
//            if(ImGui::Button("Fill")){
//                filters.depthMask.fill(false);
//                update = true;
//            }
//            ImGui::SameLine();
//            if(ImGui::Button("Empty")){
//                filters.depthMask.fill(true);
//                update = true;
//            }
//            ImGui::SameLine();
//            ImGui::Text("Pencil");
//            ImGui::SameLine();
//            ImGui::SetNextItemWidth(60.f);

//            if(ImGui::Combo("###settings_pencil_combo", &filters.idPencil, sizesPencilItems, IM_ARRAYSIZE(sizesPencilItems))){
//                update = true;
//            }
//        }
//        ImGui::Unindent();
