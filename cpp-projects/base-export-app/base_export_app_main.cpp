
/***********************************************************************************
** base-export-app                                                                **
** MIT License                                                                    **
** Copyright (c) [2024] [Florian Lance][EPFL-LNCO]                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
************************************************************************************/

#include "dc_video_player_export.hpp"

auto dc_video_player_export_test() -> void{

    std::puts("Create DCPlayer\n");
    auto dcPlayer = create__dc_video_player();

    std::puts("Load from file\n");
    if(load_from_file__dc_video_player(dcPlayer, "E:/576p-07-02-24.kvid") == 1){
        std::puts("Loading successful\n");
    }else{
        std::puts("Loading failure\n");
    }

    std::puts("Set settings\n");
    set_looping_state__dc_video_player(dcPlayer, 0);

    std::puts("Infos:\n");
    std::puts(std::format("\tDuration: {}ms\n", duration_ms__dc_video_player(dcPlayer)).c_str());

    int nbCameras = cameras_count__dc_video_player(dcPlayer);
    std::puts(std::format("\tCameras count: {}\n", nbCameras).c_str());
    for(int idC = 0; idC < nbCameras; ++idC){
        std::puts(std::format("\tFrames count for camera n°{}: {}\n", idC, frames_count__dc_video_player(dcPlayer, idC)).c_str());
            tool::geo::Mat4f m;
        get_transform__dc_video_player(dcPlayer, idC, m.array.data());
        std::puts(std::format("\tModel for camera n °{}:\n\t\t{} {} {} {}\n\t\t{} {} {} {}\n\t\t{} {} {} {}\n\t\t{} {} {} {}\n",
                              idC, m.at(0),m.at(1),m.at(2),m.at(3),m.at(4),m.at(5),m.at(6),m.at(7),m.at(8),m.at(9),m.at(10),m.at(11),m.at(12),m.at(13),m.at(14),m.at(15)).c_str());
    }

    std::vector<std::vector<tool::cam::DCVertexMeshData>> cloudsData;
    cloudsData.resize(nbCameras);

    std::vector<size_t> cloudsResSize;
    cloudsResSize.resize(nbCameras);

    std::puts("Start playing:\n");
    start_playing__dc_video_player(dcPlayer);

    while(is_playing__dc_video_player(dcPlayer)){

        update__dc_video_player(dcPlayer);

        for(int idC = 0; idC < nbCameras; ++idC){
            int sizeCloud = get_current_frame_valid_vertices_count__dc_video_player(dcPlayer, idC);
            cloudsData[idC].resize(sizeCloud);
            std::puts(std::format("size cloud {}\n", sizeCloud).c_str());
        }

        for(int idC = 0; idC < nbCameras; ++idC){
            cloudsResSize[idC] = copy_camera_cloud__dc_video_player(dcPlayer, idC, cloudsData[idC].data(), static_cast<int>(cloudsData[idC].size()), true);
        }

        // std::puts(std::format("\tCurrent time: {}ms\n",current_time_ms__dc_video_player(dcPlayer)).c_str());
        // for(int idC = 0; idC < nbCameras; ++idC){
        //     std::puts(std::format("\t\tCam: {}, size cloud {}\n", idC, cloudsResSize[idC]).c_str());
        // }

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0/60.0/2.0)));

    }std::puts("Playing stopped:\n");


    std::puts("Delete DCPlayer\n");
    delete__dc_video_player(dcPlayer);
}

#include "dc_network_direct_player_export.hpp"

auto dc_network_direct_player_export_test() -> void{

    std::string networkSettingsFilePath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-manager/config/network/network_FLORIAN-PC.config";
    std::string deviceSettingsFilePath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-manager/config/settings/device_FLORIAN-PC_all.config";
    std::string filtersSettingsFilePath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-manager/config/settings/filters_FLORIAN-PC_all.config";
    std::string colorsSettingsFilePath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-manager/config/settings/color_FLORIAN-PC_all.config";
    std::string modelSettingsFilePath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-manager/config/calibration/model_FLORIAN-PC_all.config";

    std::puts("Create DCNetworkDirectPlayer\n");
    auto dcNetworkDirectPlayer = create__dc_network_direct_player();


    std::puts("Initialize\n");
    if(initialize__dc_network_direct_player(dcNetworkDirectPlayer,networkSettingsFilePath.c_str()) == 1){
        std::puts("Initialization successful\n");
    }else{
        std::puts("Initialization failure\n");
    }
    std::puts(std::format("Number of devices: {}\n",devices_nb__dc_network_direct_player(dcNetworkDirectPlayer)).c_str());


    std::puts("Connect to devices\n");
    connect_to_devices__dc_network_direct_player(dcNetworkDirectPlayer);
    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::puts("Devices connection state:\n");
    for(int idD = 0; idD < devices_nb__dc_network_direct_player(dcNetworkDirectPlayer); ++idD){
        if(is_device_connected__dc_network_direct_player(dcNetworkDirectPlayer, idD)){
            std::puts(std::format("\tDevice n°{} connected\n", idD).c_str());
        }else{
            std::puts(std::format("\tDevice n°{} not connected\n", idD).c_str());
        }
    }

    std::puts("Update settings\n");
    update_device_settings__dc_network_direct_player(dcNetworkDirectPlayer, deviceSettingsFilePath.c_str());
    update_color_settings__dc_network_direct_player(dcNetworkDirectPlayer, colorsSettingsFilePath.c_str());
    update_filters_settings__dc_network_direct_player(dcNetworkDirectPlayer, filtersSettingsFilePath.c_str());
    update_model_settings__dc_network_direct_player(dcNetworkDirectPlayer, modelSettingsFilePath.c_str());

    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::puts("Start reading\n");
    start_reading__dc_network_direct_player(dcNetworkDirectPlayer);

    int nbIterations = 1000;
    std::vector<std::vector<tool::cam::DCVertexMeshData>> data;
    data.resize(devices_nb__dc_network_direct_player(dcNetworkDirectPlayer));

    for(int idL = 0; idL < nbIterations; ++idL){

        update__dc_network_direct_player(dcNetworkDirectPlayer);

        for(int idC = 0; idC < devices_nb__dc_network_direct_player(dcNetworkDirectPlayer); ++idC){
            auto sizeVertices = current_frame_cloud_size__dc_network_direct_player(dcNetworkDirectPlayer, idC);
            if(data[idC].size() < sizeVertices){
                data[idC].resize(sizeVertices);
            }
            auto verticesCopied = copy_current_frame_vertices__dc_network_direct_player(dcNetworkDirectPlayer, idC, data[idC].data(), sizeVertices);
            std::puts(std::format("\t{} -> {} -> {} -> {}\n",
                idC, current_frame_id__dc_network_direct_player(dcNetworkDirectPlayer, idC), sizeVertices, verticesCopied).c_str());
        }

        // std::puts(std::format("Infos loop id {}: ", idL).c_str());
        // for(int idC = 0; idC < devices_nb__dc_network_direct_player(dcNetworkDirectPlayer); ++idC){
        //     std::puts(std::format("D:{} -> {} {} | ",
        //         idC,
        //         current_frame_id__dc_network_direct_player(dcNetworkDirectPlayer, idC),
        //         current_frame_cloud_size__dc_network_direct_player(dcNetworkDirectPlayer, idC)).c_str());
        // }
        // std::puts("\n");

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0/60.0/2.0)));
    }

    std::puts("Stop reading\n");
    stop_reading__dc_network_direct_player(dcNetworkDirectPlayer);

    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::puts("Disconnect from devices\n");
    disconnect_from_devices__dc_network_direct_player(dcNetworkDirectPlayer);

    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::puts("Delete DCNetworkDirectPlayer\n");
    delete__dc_network_direct_player(dcNetworkDirectPlayer);
}


int main(int, char *[]){

    dc_video_player_export_test();
    // dc_network_direct_player_export_test();

    return 0;
}



