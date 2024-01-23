

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

#include "opengl/gl_texture.hpp"

using namespace tool::graphics;
using namespace tool::cam;


auto DCUIDrawer::draw_dc_filters_settings_tab_item(const std::string &tabItemName, cam::DCMode mode, cam::DCFiltersSettings &filters, bool &autoUpdate) -> std::tuple<bool,bool>{

    static_cast<void>(autoUpdate);

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return {false, false};
    }

    bool update = false;

    ImGuiUiDrawer::title2("PIXELS");
    {
        float minMaxD[2] = {filters.minDepthF, filters.maxDepthF};


        auto dRange = (cam::depth_range(mode)*1000.f).conv<int>();
        auto diff   = dRange.y() - dRange.x();

        ImGuiUiDrawer::text(std::format("Depth (factor): min: {}mm, max: {}mm", static_cast<int>(dRange.x() + minMaxD[0]*diff),  static_cast<int>(dRange.x() + minMaxD[1]*diff)));
        ImGui::Indent();
        if(ImGui::SliderFloat2("###settings_depth_min_max_factors", minMaxD, 0.f, 1.f)){
            filters.minDepthF = minMaxD[0];
            filters.maxDepthF = minMaxD[1];
            update = true;
        }
        ImGui::Unindent();

        auto resolution = cam::mixed_resolution(mode);

        float minMaxW[2] = {filters.minWidthF, filters.maxWidthF};
        ImGuiUiDrawer::text(std::format("Width (factor): min: {}pix, max: {}pix", static_cast<int>(minMaxW[0]*resolution.x()),  static_cast<int>(minMaxW[1]*resolution.x())));
        ImGui::Indent();
        if(ImGui::SliderFloat2("###settings_width_min_max_factors", minMaxW, 0.f, 1.f)){
            filters.minWidthF = minMaxW[0];
            filters.maxWidthF = minMaxW[1];
            update = true;
        }
        ImGui::Unindent();

        float minMaxH[2] = {filters.minHeightF, filters.maxHeightF};
        ImGuiUiDrawer::text(std::format("Height (factor): min: {}pix, max: {}pix", static_cast<int>(minMaxH[0]*resolution.y()),  static_cast<int>(minMaxH[1]*resolution.y())));
        ImGui::Indent();
        if(ImGui::SliderFloat2("###settings_height_min_max_factors", minMaxH, 0.f, 1.f)){
            filters.minHeightF = minMaxH[0];
            filters.maxHeightF = minMaxH[1];
            update = true;
        }
        ImGui::Unindent();


        ImGui::Text("Plane filtering:");
        ImGui::Indent();

        int mode = static_cast<int>(filters.p1FMode);
        if(ImGui::RadioButton("None###mode_none_plane1", &mode,0)){
            update = true;
            filters.p1FMode = cam::DCFiltersSettings::PlaneFilteringMode::None;
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Remove above###mode_above_plane1", &mode,1)){
            update = true;
            filters.p1FMode = cam::DCFiltersSettings::PlaneFilteringMode::Above;
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Remove below###mode_below_plane1", &mode,2)){
            update = true;
            filters.p1FMode = cam::DCFiltersSettings::PlaneFilteringMode::Below;
        }
        if(filters.p1FMode != cam::DCFiltersSettings::PlaneFilteringMode::None){

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
    }

    ImGuiUiDrawer::title2("COLOR");
    {
        ImGui::Text("JPEG compression rate");        
        ImGui::SameLine();
        ImGui::SetNextItemWidth(75.f);
        int compressionRate = filters.jpegCompressionRate;
        if(ImGui::SliderInt("###compression_rate", &compressionRate, 5, 100)){
            filters.jpegCompressionRate = compressionRate;
            update = true;
        }

        if(ImGui::Checkbox("Filter depth with color###settings_filter_depth_with_color_checkbox", &filters.filterDepthWithColor)){
            update = true;
        }

        if(filters.filterDepthWithColor){

            ImGui::Indent();

            ImGui::Text("Filtered color");
            ImGui::Indent();
            if(ImGui::ColorEdit3("###settings_filtered_color_coloredit3", filters.filterColor.rgb().array.data())){
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
//            ImGui::SameLine();
//            tex.init_or_update_8ui(100,25,3, tData.data());
//            ImGui::Image(tex.id(), {100,25}, ImVec2(0,0), ImVec2(1,1));

            fs.speedDrag = 0.001f;
            fs.speedInc = 0.01f;
            fs.max = 1.f;
            fs.format = "%.3f";
            if(ImGuiUiDrawer::draw_drag_float_with_buttons("Saturation","saturation", mdcd+1, fs, ds)){
                update = true;
            }
//            ImGui::SameLine();
//            tex.init_or_update_8ui(100,25,3, tData.data());
//            ImGui::Image(tex.id(), {100,25}, ImVec2(0,0), ImVec2(1,1));

            if(ImGuiUiDrawer::draw_drag_float_with_buttons("Value","value", mdcd+2, fs, ds)){
                update = true;
            }
//            ImGui::SameLine();
//            tex.init_or_update_8ui(100,25,3, tData.data());
//            ImGui::Image(tex.id(), {100,25}, ImVec2(0,0), ImVec2(1,1));

            ImGui::Unindent();
        }
    }

    ImGuiUiDrawer::title2("GEOMETRY");
    {

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
    }

    ImGuiUiDrawer::title2("INVALIDATE");
    {
        if(ImGui::Checkbox("Color from depth###settings_invalidate_color_from_depth_checkbox", &filters.invalidateColorFromDepth)){
            update = true;
        }

        ImGui::SameLine();
        if(ImGui::Checkbox("Infra from depth###settings_invalidate_infra_from_depth_checkbox", &filters.invalidateInfraFromDepth)){
            update = true;
        }
    }

    ImGui::EndTabItem();

    return {true, update};
}

auto DCUIDrawer::draw_dc_scene_display_setings_tab_item(const std::string &tabItemName, DCSceneDisplaySettings &display, bool &autoUpdate)  -> bool {

    if (!ImGui::BeginTabItem(tabItemName.c_str())){
        return false;
    }

    bool update = false;

    if(ImGui::ColorEdit4("Background color###background_scene_color", display.backgroundColor.array.data())){
        update = true;
    }

    ImGui::Spacing();
    ImGui::Separator();

    bool manualUpdate = false;
    if(ImGui::Button("Update###scene_display_update_button")){
        manualUpdate = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto update###scene_display_auto_update_cb", &autoUpdate)){}

    ImGui::EndTabItem();

    return (update && autoUpdate) || manualUpdate;
}

auto DCUIDrawer::draw_dc_cloud_display_setings_tab_item(const std::string &tabItemName, DCCloudDisplaySettings &display, bool &autoUpdate)  -> bool {

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    bool update = false;

    if(ImGui::Checkbox("Cloud visible###display_cloud_visible", &display.cloudVisible)){
        update = true;
    }
    if(ImGui::Checkbox("Force cloud color###display_force_cloud_color", &display.forceCloudColor)){
        update = true;
    }
    if(ImGui::ColorEdit4("Cloud color###display_cloud_color", display.cloudColor.array.data())){
        update = true;
    }
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Color factor", &display.factorUnicolor, 0.01f, 0.f, 1.f)){
        update = true;
    }

    ImGui::Separator();    
    if(ImGui::Checkbox("Use voxels###display_use_voxels", &display.useVoxels)){
        update = true;
    }

    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Size points###display_size_points", &display.sizePoints, 0.1f, 0.1f, 30.f, "%.1f")){
        update = true;
    }

    ImGuiDragS  voxelsD = {100.f, true, true, true, true, false};
    ImGuiFloatS voxelFS = {0.005f, 0.001f, 0.05f, 0.0001f, 0.001f};

    float sizeV = display.sizeVoxels;
    if(ImGuiUiDrawer::draw_drag_float_with_buttons("Size voxel","display_size_voxels", &sizeV, voxelFS, voxelsD)){
        update = true;
        display.sizeVoxels = sizeV;
    }

    ImGui::Spacing();
    ImGui::Separator();

    bool manualUpdate = false;
    if(ImGui::Button("Update###display_cloud_update_button")){
        manualUpdate = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto update###display_cloud_auto_update_cb", &autoUpdate)){}

    ImGui::EndTabItem();

    return (update && autoUpdate) || manualUpdate;
}

auto DCUIDrawer::draw_dc_model_tab_item(const std::string &tabItemName, cam::DCModelSettings &model, bool &autoUpdate) -> bool{

    static_cast<void>(autoUpdate);

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    bool update = false;
    auto d = model.transformation.array.data();

    ImGuiUiDrawer::title2("Calibration matrix");
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

    ImGuiUiDrawer::title2("Additional transformation");

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

auto DCUIDrawer::draw_dc_recorder_tab_item(
    const std::string &tabItemName,
    cam::DCRecorderStates &rStates,
    cam::DCRecorderSettings &rSettings,
    bool &autoUpdate) -> bool{


    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.data())){
        return false;
    }

    ImGuiUiDrawer::title2("ACTIONS");
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
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Timeline", "recorder_timeline", &currentTime, iSettings, dSettings)){
        rStates.currentTime = currentTime;
        rStates.moveTime = true;        
    }

    if(ImGui::Button("Save")){        
        ImGuiFileDialog::Instance()->OpenDialog("Save recording", "Choose file to save", ".kvid", ".");
    }

    ImGuiUiDrawer::title2("SETTINGS");
    bool update = false;        
    ImGui::SetNextItemWidth(50.f);
    if(ImGui::DragInt("Max number of frames per camera", &rSettings.cameraMaxFramesToRecord, 1.0f, 1, 100000)){
        update = true;
    }

//    double maxDurationS = 500.;
//    // output
//    std::vector<bool> camerasToRecord;
//    bool recordAllData = true;
//    bool recordColor   = true;
//    bool recordDepth   = true;
//    bool recordInfra   = true;
//    bool recordCloud   = true;
//    bool recordIMU     = true;
//    bool recordAudio   = true;
//    bool recordBodies  = true;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    bool manualUpdate = false;
    if(ImGui::Button("Update###recording_update_button")){
        manualUpdate = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto update###recording_auto_update_cb", &autoUpdate)){}


    ImGui::EndTabItem();

    return (update && autoUpdate) || manualUpdate;
}

auto DCUIDrawer::draw_dc_player_tab_item(
    const std::string &tabItemName,
    cam::DCPlayerStates &pStates,
    cam::DCPlayerSettings &pSettings,
    bool &autoUpdate) -> bool{

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.data())){
        return false;
    }

    ImGuiUiDrawer::title2("ACTIONS");

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
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Timeline", "player_timeline", &currentTime, iSettings, dSettings)){
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
    if(ImGui::Button("Merge")){
        pStates.merge = true;
    }
    if(ImGui::Button("Info")){
        pStates.info = true;
    }
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

    ImGuiUiDrawer::title2("SETTINGS");
    bool update = false;

    if(ImGui::Checkbox("Do loop", &pSettings.doLoop)){
        update = true;
    }

    bool manualUpdate = false;
    if(ImGui::Button("Update###player_update_button")){
        manualUpdate = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto update###player_auto_update_cb", &autoUpdate)){}


    ImGui::EndTabItem();

    return (update && autoUpdate) || manualUpdate;
}

auto DCUIDrawer::draw_dc_calibrator_tab_item(
    const std::string &tabItemName,
    bool useNormalFilteringSettings,
    cam::DCCalibratorStates &cStates,
    DCCalibratorDrawerSettings &cdSettings,
    cam::DCCalibratorSettings &cSettings,
    bool &autoUpdate) -> bool {

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

    ImGuiUiDrawer::title2("GRABBERS TO USE");
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

    ImGuiUiDrawer::title2("CAPTURE");
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

    ImGuiUiDrawer::title2("PROCESSING");
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

    ImGuiUiDrawer::title2("DISPLAY");
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

    ImGuiUiDrawer::title2("CALIBRATION");
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

    bool manualUpdate = false;
    if(ImGui::Button("Update###calibration_update_button")){
        manualUpdate = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto update###calibration_auto_update_cb", &autoUpdate)){}

    ImGui::EndTabItem();

    return (update && autoUpdate) || manualUpdate;
}

auto DCUIDrawer::draw_dc_device_settings_tab_item(
        const std::string &tabItemName,
        cam::DCDeviceSettings &device,
        bool &updateDeviceList,
        bool &autoUpdate) -> bool{

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }
    bool update = false;
    
    draw_dc_config(device.configS, updateDeviceList, update);
    draw_dc_data_settings(device.configS.typeDevice, device.dataS, update);
    draw_dc_actions_settings(device.actionsS, update);

    ImGui::Spacing();
    ImGui::Separator();

    bool manualUpdate = false;
    if(ImGui::Button("Update###settings_update_button")){
        manualUpdate = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto update###settings_auto_update_cb", &autoUpdate)){}

    ImGui::EndTabItem();

    return  (update && autoUpdate) || manualUpdate;
}

auto DCUIDrawer::draw_dc_config(cam::DCConfigSettings &config, bool &updateDeviceList, bool &updateP) -> void{

    // init
    if(modesNames.empty()){
        for(const auto &m : k4Modes){
            modesNames[m] = std::string(cam::mode_name(m));
        }
        for(const auto &m : foModes){
            modesNames[m] = std::string(cam::mode_name(m));
        }
    }

    ImGuiUiDrawer::title2("CONFIG");
    ImGui::Spacing();

    ImGui::Text("Device type:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(150.f);

    int guiCurrentTypeSelection = static_cast<int>(config.typeDevice);
    if(ImGui::Combo("###settings_device_type", &guiCurrentTypeSelection, devicesTypes, IM_ARRAYSIZE(devicesTypes))){        
        auto nDeviceType = static_cast<cam::DCType>(guiCurrentTypeSelection);
        if(nDeviceType != config.typeDevice){
            config.typeDevice = nDeviceType;
            config.mode       = cam::default_camera_mode(config.typeDevice);
            updateP = true;
        }
    }
    updateDeviceList = ImGui::Button("Refresh devices list");
    ImGui::Unindent();

    ImGui::Text("Device id:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(150.f);

    if(devicesNames.size() != 0){
        if(ImGui::BeginCombo("###settings_device_id", devicesNames[config.idDevice].c_str())){
            for(size_t ii = 0; ii < devicesNames.size(); ++ii){
                bool selected = ii == config.idDevice;
                if (ImGui::Selectable(devicesNames[ii].c_str(),selected)){
                    config.idDevice = static_cast<std::uint32_t>(ii);
                    updateP = true;
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

    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Text("Mode:");
    ImGui::Indent();

    auto currentModeName = modesNames[config.mode];
    if(config.typeDevice == cam::DCType::FemtoOrbbec){
        if(ImGui::BeginCombo("###settings_mode", currentModeName.c_str())){
            for(const auto &m : foModes){
                bool selected = m == config.mode;
                if (ImGui::Selectable(modesNames[m].c_str(), selected)){
                    config.mode = m;
                    updateP = true;
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
                    updateP = true;
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

    ImGui::Text("Synch mode:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(100.f);
    int guiCurrentSynchModeSelection = static_cast<int>(config.synchMode);
    if(ImGui::Combo("###settings_synch_mode_combo", &guiCurrentSynchModeSelection, synchItems, IM_ARRAYSIZE(synchItems))){
        updateP          = true;
        config.synchMode = static_cast<cam::DCSynchronisationMode>(guiCurrentSynchModeSelection);
    }
    ImGui::Unindent();

    ImGui::Text("Subordinate delay (usec):");
    ImGui::SetNextItemWidth(80.f);
    ImGui::Indent();
    if(ImGui::DragInt("###subordinate_delay_usec", &config.subordinateDelayUsec, 1.0f, 0, 100000)){
        updateP          = true;
    }
    ImGui::Unindent();
    ImGui::Spacing();

    if(ImGui::Checkbox("Synch color and depth images", &config.synchronizeColorAndDepth)){
        updateP          = true;
    }

    ImGui::Text("Delay between color and depth (usec)");
    ImGui::SetNextItemWidth(80.f);
    ImGui::Indent();
    if(ImGui::DragInt("###delayt_between_color_and_depth_usec", &config.delayBetweenColorAndDepthUsec, 1.0f, 100000, 100000)){
        updateP          = true;
    }
    ImGui::Unindent();
    ImGui::Spacing();

    ImGui::Separator();
    if(ImGui::Checkbox("Enable body tracking (perfomance costly)", &config.enableBodyTracking)){
        updateP          = true;
    }
    ImGui::BeginDisabled(!config.enableBodyTracking);

    ImGui::Text("Processing mode:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(150.f);
    int guiProcessingModeBodyTrackingSelection = static_cast<int>(config.btProcessingMode);
    if(ImGui::Combo("###settings_processing_mode_body_tracking", &guiProcessingModeBodyTrackingSelection, processingModeBodyTrackingItems, IM_ARRAYSIZE(processingModeBodyTrackingItems))){
        config.btProcessingMode = static_cast<DCBTProcessingMode>(guiProcessingModeBodyTrackingSelection);
        updateP = true;
    }
    ImGui::Unindent();

    // add gpu id
    // ...

    ImGui::Text("Orientation:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(150.f);
    int guiOrientationBodyTrackingSelection = static_cast<int>(config.btOrientation);
    if(ImGui::Combo("###settings_orientation_body_tracking", &guiOrientationBodyTrackingSelection, orientationBodyTrackingItems, IM_ARRAYSIZE(orientationBodyTrackingItems))){
        config.btOrientation = static_cast<DCBTSensorOrientation>(guiOrientationBodyTrackingSelection);
        updateP = true;
    }
    ImGui::Unindent();

    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Separator();
    if(ImGui::Checkbox("Disable LED", &config.disableLED)){
        updateP          = true;
    }

    ImGui::Spacing();
}

auto DCUIDrawer::draw_dc_data_settings(cam::DCType type, cam::DCDataSettings &data, bool &updateP) -> void{

    ImGuiUiDrawer::title2("DATA");
    ImGui::Spacing();

    ImGui::Text("Capture:");

    if(type == DCType::AzureKinect){
        if(ImGui::Checkbox("audio###settings_capture_audio", &data.captureAudio)){
            updateP = true;
        }
        ImGui::SameLine();
    }
    if(ImGui::Checkbox("IMU###settings_capture_imu", &data.captureIMU)){
        updateP = true;
    }

    if(type == DCType::AzureKinect || type == DCType::FemtoOrbbec){
        ImGui::SameLine();
        if(ImGui::Checkbox("bodies (GPU-heavy)###settings_capture_bodies", &data.captureBodies)){
            updateP = true;
        }
    }

    ImGui::Spacing();
    ImGui::Text("Send:");
    if(ImGui::Checkbox("RGB###settings_send_rgb", &data.sendColor)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Depth###settings_send_depth", &data.sendDepth)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Infra###settings_send_infra", &data.sendInfra)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Cloud###settings_send_cloud", &data.sendCloud)){
        updateP = true;
    }

    if(ImGui::Checkbox("IMU###settings_send_imu", &data.sendIMU)){
        updateP = true;
    }

    if(type == DCType::AzureKinect){
        ImGui::SameLine();
        if(ImGui::Checkbox("Audio###settings_send_audio", &data.sendAudio)){
            updateP = true;
        }
    }

    if(type == DCType::AzureKinect || type == DCType::FemtoOrbbec){
        ImGui::SameLine();
        if(ImGui::Checkbox("Bodies###settings_send_bodies", &data.sendBodies)){
            updateP = true;
        }
    }

    ImGui::Spacing();
    ImGui::Text("Display locally on grabber (disable for efficiency):");
    if(ImGui::Checkbox("RGB###settings_display_rgb", &data.generateRGBLocalFrame)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Depth###settings_display_depth", &data.generateDepthLocalFrame)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Infra###settings_display_infra", &data.generateInfraLocalFrame)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Cloud###settings_display_cloud", &data.generateCloudLocal)){
        updateP = true;
    }
}

auto DCUIDrawer::draw_dc_actions_settings(cam::DCActionsSettings &actions, bool &updateP) -> void{

    ImGuiUiDrawer::title2("ACTIONS TO DO");
    ImGui::Spacing();
    
    if(ImGui::Checkbox("Start device###settings_start_device", &actions.openDevice)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Open camera###settings_open_camera", &actions.startReading)){
        updateP = true;
    }

    ImGui::Spacing();
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

auto DCUIDrawer::draw_dc_colors_settings_tab_item(const std::string &tabItemName, cam::DCType type, cam::DCColorSettings &colors, bool &autoUpdate) -> bool{

    using CST = ColorSettingsType;
    static_cast<void>(autoUpdate);

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    bool update = false;
    int value;
    ImGuiDragS ds;
    ds.widthDrag = 100.f;

    ImGuiUiDrawer::title2("CAMERA HARDWARE SETTINGS");

    if(is_available(CST::Auto_exposure, type)){
        if(ImGui::Checkbox("Auto exposure###auto_exposure_time", &colors.autoExposureTime)){
            update = true;
        }
    }

    ImGui::BeginDisabled(!colors.autoExposureTime);
    if(is_available(CST::Brightness, type)){
        value = colors.brightness;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Brightness", "color_brightness", &value, get_imgui_int_scale(CST::Brightness, type), ds)){
            colors.brightness = value;
            update = true;
        }
        ImGui::Spacing();
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(colors.autoExposureTime);

    if(is_available(CST::Exposure, type)){
        value = colors.exposureTime;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Exposure", "color_exposure", &value, get_imgui_int_scale(CST::Exposure, type), ds)){
            colors.exposureTime = value;
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Gain, type)){
        value = colors.gain;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Gain", "color_gain", &value, get_imgui_int_scale(CST::Gain, type), ds)){
            colors.gain = value;
            update = true;
        }
        ImGui::Spacing();
    }

    ImGui::EndDisabled();

    if(is_available(CST::Contrast, type)){
        value = colors.contrast;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Contrast", "color_contrast", &value, get_imgui_int_scale(CST::Contrast, type), ds)){
            colors.contrast = value;
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Saturation, type)){
        value = colors.saturation;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Saturation", "color_saturation", &value, get_imgui_int_scale(CST::Saturation, type), ds)){
            colors.saturation = value;
            update = true;
        }
        ImGui::Spacing();
    }

    if(is_available(CST::Sharpness, type)){
        value = colors.sharpness;
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("Sharpness", "color_sharpness", &value, get_imgui_int_scale(CST::Sharpness, type), ds)){
            colors.sharpness = value;
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
        if(ImGuiUiDrawer::draw_drag_int_with_buttons("White balance", "color_white_balance", &value, get_imgui_int_scale(CST::White_balance, type), ds)){
            colors.whiteBalance = value;
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
            colors.powerlineFrequency = default_value(CST::Power_line_frequency, type);
            update = true;
        }
        ImGui::SameLine();

        int guiSel;
        if(type == DCType::AzureKinect){
            guiSel = colors.powerlineFrequency-1;
            ImGui::SetNextItemWidth(100.f);
            if(ImGui::Combo("###settings_mode_combo", &guiSel, k4PowerlineFrequencyItems, IM_ARRAYSIZE(k4PowerlineFrequencyItems))){
                update       = true;
                colors.powerlineFrequency = guiSel + 1;
            }
        }else if(type == DCType::FemtoOrbbec){
            guiSel = colors.powerlineFrequency;
            ImGui::SetNextItemWidth(100.f);
            if(ImGui::Combo("###settings_mode_combo", &guiSel, obPowerlineFrequencyItems, IM_ARRAYSIZE(obPowerlineFrequencyItems))){
                update       = true;
                colors.powerlineFrequency = guiSel;
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

    return update;
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
