

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


auto DCUIDrawer::draw_filters_tab_item(const std::string &tabItemName, camera::DCMode mode, camera::DCFiltersSettings &filters, bool &autoUpdate) -> std::tuple<bool,bool>{

    static_cast<void>(autoUpdate);

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return {false, false};
    }

    bool update = false;

    ImGuiUiDrawer::title2("PIXELS");
    {
        int minMaxD[2] = {filters.minDepthValue, filters.maxDepthValue};
        auto range = (camera::range(mode)*1000.f).conv<int>();
        if(minMaxD[0] < range.x()){
            minMaxD[0] = range.x();
        }
        if(minMaxD[1] > range.y()){
            minMaxD[1] = range.y();
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

        ImGui::Text("Depth (mm):");
        ImGui::Indent();
        if(ImGui::SliderInt2("###settings_depth_min_max_sliderint2", minMaxD, range.x(), range.y())){
            filters.minDepthValue = static_cast<std::int16_t>(minMaxD[0]);
            filters.maxDepthValue = static_cast<std::int16_t>(minMaxD[1]);
            update = true;
        }
        ImGui::Unindent();

        int minMaxWidth[2] = {static_cast<int>(filters.minWidth), static_cast<int>(filters.maxWidth)};
        auto depthRes = camera::depth_resolution(mode);
        if(minMaxWidth[1] > depthRes.x()){
            minMaxWidth[1] = depthRes.x();
        }
        ImGui::Text("Width (pixels):");
        ImGui::Indent();
        if(ImGui::SliderInt2("###settings_width_min_max_sliderint2", minMaxWidth, 0, depthRes.x())){
            filters.minWidth = static_cast<unsigned int>(minMaxWidth[0]);
            filters.maxWidth = static_cast<unsigned int>(minMaxWidth[1]);
            update = true;
        }
        ImGui::Unindent();

        int minMaxHeight[2] = {static_cast<int>(filters.minHeight), static_cast<int>(filters.maxHeight)};
        if(minMaxHeight[1] > depthRes.y()){
            minMaxHeight[1] = depthRes.y();
        }

        ImGui::Text("Height (pixels):");
        ImGui::Indent();
        if(ImGui::SliderInt2("###settings_height_min_max_sliderint2", minMaxHeight, 0, depthRes.y())){
            filters.minHeight = static_cast<unsigned int>(minMaxHeight[0]);
            filters.maxHeight = static_cast<unsigned int>(minMaxHeight[1]);
            update = true;
        }
        ImGui::Unindent();


        ImGui::Text("Plane1:");
        ImGui::Indent();

        int mode = static_cast<int>(filters.p1FMode);
        if(ImGui::RadioButton("No filtering###mode_none_plane1", &mode,0)){
            update = true;
            filters.p1FMode = camera::DCFiltersSettings::PlaneFilteringMode::None;
        }
        if(ImGui::RadioButton("Remove above###mode_above_plane1", &mode,1)){
            update = true;
            filters.p1FMode = camera::DCFiltersSettings::PlaneFilteringMode::Above;
        }
        if(ImGui::RadioButton("Remove below###mode_below_plane1", &mode,2)){
            update = true;
            filters.p1FMode = camera::DCFiltersSettings::PlaneFilteringMode::Below;
        }

        ImGui::Text("Rotation (euler angles):");
        auto rotPtr = filters.p1Rot.array.data();
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","p1_rot_x", rotPtr, modelRotFs, modelRotDs)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","p1_rot_y", rotPtr+1, modelRotFs, modelRotDs)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","p1_rot_z", rotPtr+2, modelRotFs, modelRotDs)){
            update = true;
        }
        ImGui::Text("Translation (mm):");
        auto trPtr = filters.p1Pos.array.data();
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("x","p1_tr_x", trPtr, modelTrFs, modelTrDs)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("y","p1_tr_y", trPtr+1, modelTrFs, modelTrDs)){
            update = true;
        }
        ImGui::SameLine();
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("z","p1_tr_z", trPtr+2, modelTrFs, modelTrDs)){
            update = true;
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

auto DCUIDrawer::draw_scene_display_setings_tab_item(const std::string &tabItemName, camera::DCSceneDisplaySettings &display, bool &autoUpdate)  -> bool {

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

auto DCUIDrawer::draw_cloud_display_setings_tab_item(const std::string &tabItemName, camera::DCCloudDisplaySettings &display, bool &autoUpdate)  -> bool {

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

auto DCUIDrawer::draw_calibration_tab_item(const std::string &tabItemName, camera::DCModelSettings &model, bool &autoUpdate) -> bool{

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

auto DCUIDrawer::draw_recording_tab_item(
    const std::string &tabItemName,
    camera::DCRecorderStates &rStates,
    camera::DCRecorderSettings &rSettings,
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

auto DCUIDrawer::draw_player_tab_item(
    const std::string &tabItemName,
    camera::DCPlayerStates &pStates,
    camera::DCPlayerSettings &pSettings,
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

auto DCUIDrawer::draw_calibrator_tab_item(
    const std::string &tabItemName,
    bool useNormalFilteringSettings,
    camera::DCCalibratorStates &cStates,
    DCCalibratorDrawerSettings &cdSettings,
    camera::DCCalibratorSettings &cSettings,
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

auto DCUIDrawer::draw_device_settings_tab_item(
        const std::string &tabItemName,
        const std::vector<std::string> &devicesName,
        camera::DCDeviceSettings &device,
        bool &updateDeviceList,
        bool &autoUpdate) -> bool{

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }
    bool update = false;

    draw_config(devicesName, device.configS, updateDeviceList, update);
    draw_data_settings(device.dataS, update);
    draw_actions_settings(device.actionsS, update);

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

auto DCUIDrawer::draw_config(const std::vector<std::string> &devicesName, camera::DCConfigSettings &config, bool &updateDeviceList, bool &updateP) -> void{

    ImGui::Spacing();
    ImGuiUiDrawer::text_centered("CONFIG");
    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Text("Device id:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(150.f);

    if(devicesName.size() != 0){
        if(ImGui::BeginCombo("###settings_device_id", devicesName[config.idDevice].c_str())){
            for(size_t ii = 0; ii < devicesName.size(); ++ii){
                bool selected = ii == config.idDevice;
                if (ImGui::Selectable(devicesName[ii].c_str(),selected)){
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

    updateDeviceList = ImGui::Button("Refresh devices list");
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Text("Mode:");
    ImGui::Indent();
    int guiCurrentModeSelection = static_cast<int>(config.mode);

    if(ImGui::Button("<###settings_mode_left")){
        if(guiCurrentModeSelection > 0){
            --guiCurrentModeSelection;
            updateP       = true;
            config.mode  = static_cast<camera::DCMode>(guiCurrentModeSelection);
        }
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150.f);
    if(ImGui::Combo("###settings_mode_combo", &guiCurrentModeSelection, modeItems, IM_ARRAYSIZE(modeItems))){
        updateP       = true;
        config.mode  = static_cast<camera::DCMode>(guiCurrentModeSelection);
    }
    ImGui::SameLine();
    if(ImGui::Button(">###settings_mode_right")){
        if(guiCurrentModeSelection < IM_ARRAYSIZE(modeItems)-1){
            ++guiCurrentModeSelection;
            updateP       = true;
            config.mode  = static_cast<camera::DCMode>(guiCurrentModeSelection);
        }
    }

    ImGui::Unindent();
    ImGui::Spacing();

    ImGui::Text("Synch mode:");
    ImGui::Indent();
    ImGui::SetNextItemWidth(100.f);
    int guiCurrentSynchModeSelection = static_cast<int>(config.synchMode);
    if(ImGui::Combo("###settings_synch_mode_combo", &guiCurrentSynchModeSelection, synchItems, IM_ARRAYSIZE(synchItems))){
        updateP          = true;
        config.synchMode = static_cast<camera::DCSynchronisationMode>(guiCurrentSynchModeSelection);
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
    if(ImGui::Checkbox("Disable LED", &config.disableLED)){
        updateP          = true;
    }

    ImGui::Spacing();
}

auto DCUIDrawer::draw_data_settings(camera::DCDataSettings &data, bool &updateP) -> void{

    ImGui::Spacing();
    ImGuiUiDrawer::text_centered("DATA");
    ImGui::Separator();

    ImGui::Spacing();
    ImGui::Text("Capture:");
    if(ImGui::Checkbox("audio###settings_capture_audio", &data.captureAudio)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("IMU###settings_capture_imu", &data.captureIMU)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("bodies (GPU-heavy)###settings_capture_bodies", &data.captureBodies)){
        updateP = true;
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
    ImGui::SameLine();
    if(ImGui::Checkbox("Audio###settings_send_audio", &data.sendAudio)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Bodies###settings_send_bodies", &data.sendBodies)){
        updateP = true;
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

auto DCUIDrawer::draw_actions_settings(camera::DCActionsSettings &actions, bool &updateP) -> void{

    ImGui::Spacing();
    ImGuiUiDrawer::text_centered("ACTIONS TO DO");
    ImGui::Separator();
    ImGui::Spacing();

    if(ImGui::Checkbox("Start device###settings_start_device", &actions.startDevice)){
        updateP = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Open camera###settings_open_camera", &actions.openCamera)){
        updateP = true;
    }

    ImGui::Spacing();
}


auto DCUIDrawer::draw_colors_settings_tab_item(const std::string &tabItemName, camera::DCColorSettings &colors, bool &autoUpdate) -> bool{

    static_cast<void>(autoUpdate);

    if (!ImGuiUiDrawer::begin_tab_item(tabItemName.c_str())){
        return false;
    }

    int guiSel = static_cast<int>(colors.exposureTimeAbsolute);
    bool update = false;

    ImGuiUiDrawer::title2("CAMERA HARDWARE SETTINGS");
    if(ImGui::Button("D###default_exposure_time")){
        colors.exposureTimeAbsolute = 5;
        update = true;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if(ImGui::Combo("###exposure_time", &guiSel, exposureTimesMicroSTimes, IM_ARRAYSIZE(exposureTimesMicroSTimes))){
        colors.exposureTimeAbsolute = guiSel;
        update = true;
    }
    ImGui::SameLine();
    ImGui::Text("Exposure time");
    ImGui::SameLine();
    if(ImGui::Checkbox("Auto###auto_exposure_time", &colors.autoExposureTime)){
        update = true;
    }
    ImGui::Spacing();


    int value = colors.brightness;
    ImGuiDragS ds;
    ds.widthDrag = 100.f;

    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Brightness", "color_brightness", &value, ImGuiIntS{128,0,255,1.f,1}, ds)){
        colors.brightness = value;
        update = true;
    }
    ImGui::Spacing();

    value = colors.contrast;
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Contrast", "color_contrast", &value, ImGuiIntS{5,0,10,1.f,1}, ds)){
        colors.contrast = value;
        update = true;
    }

    ImGui::Spacing();

    value = colors.saturation;
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Saturation", "color_saturation", &value, ImGuiIntS{32, 0, 63, 1.f, 1}, ds)){
        colors.saturation = value;
        update = true;
    }

    ImGui::Spacing();

    value = colors.sharpness;
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Sharpness", "color_sharpness", &value, ImGuiIntS{4,0,4,1.f,1}, ds)){
        colors.sharpness = value;
        update = true;
    }

    ImGui::Spacing();

    value = colors.gain;
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Gain", "color_gain", &value, ImGuiIntS{128, 0, 255, 1.f, 1}, ds)){
        colors.gain = value;
        update = true;
    }

    ImGui::Spacing();

    value = colors.whiteBalance;
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("White balance", "color_white_balance", &value, ImGuiIntS{4500, 2500, 12500, 1, 10}, ds)){
        colors.whiteBalance = value;
        update = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("A###auto_white_balance", &colors.autoWhiteBalance)){
        update = true;
    }

    ImGui::Spacing();

    if(ImGui::Button("D###default_Backlight compensation")){
        colors.backlightCompensation = false;
        update = true;
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Backlight compensation", &colors.backlightCompensation)){
        update = true;
    }
    ImGui::Spacing();

    if(ImGui::Button("D###default_Powerline frequency")){
        colors.powerlineFrequency = camera::PowerlineFrequency::F60;
        update = true;
    }
    ImGui::SameLine();

    guiSel = colors.powerlineFrequency == camera::PowerlineFrequency::F50 ? 0 : 1;        
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::Combo("###settings_mode_combo", &guiSel, powerlineFrequencyItems, IM_ARRAYSIZE(powerlineFrequencyItems))){
        update       = true;
        colors.powerlineFrequency = guiSel == 0 ? camera::PowerlineFrequency::F50 : camera::PowerlineFrequency::F60;
    }
    ImGui::SameLine();
    ImGui::Text("Powerline frequency:");

    ImGui::EndTabItem();

    return update;
}


