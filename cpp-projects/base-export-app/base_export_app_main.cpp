
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

#include "utility/stop_watch.hpp"

using namespace tool;
using namespace tool::geo;

struct CloudData{
    std::vector<Pt3f> positions;
    std::vector<Pt3f> colors;
    std::vector<Vec3f> normals;
    size_t verticesCopied = 0;

    auto resize(size_t size) -> void{
        positions.resize(size);
        colors.resize(size);
        normals.resize(size);
    }
};

#include "io/cloud_io.hpp"
#include <iostream>
auto dc_video_player_export_test() -> void{

    geo::ColoredCloudData cloud;
    std::vector<geo::Pt3f> positions;
    std::vector<geo::Pt3f> normals;
    std::vector<geo::Pt3f> colors;

    for(int ii = 0; ii < 1; ++ii){

        std::puts(std::format("create {}\n", ii).c_str());
        auto dcPlayer = create__dc_video_player();

        std::puts("Load from file\n");
        if(load_from_file__dc_video_player(dcPlayer, "E:/27-05-24.kvid") == 1){
            std::puts("Loading successful\n");
        }else{
            std::puts("Loading failure\n");
        }

        set_player_settings__dc_video_player(dcPlayer, 1, -1.f, -1.f);
        start_video__dc_video_player(dcPlayer);
        set_current_time__dc_video_player(dcPlayer, 3000.f);

        for(int jj = 0; jj < 4; ++jj){
            update__dc_video_player(dcPlayer);

            int vvc= get_current_frame_valid_vertices_count__dc_video_player(dcPlayer, jj);
            cloud.resize(vvc, true);
            copy_camera_cloud_vfx__dc_video_player(dcPlayer, jj, cloud.vertices.get_data(), cloud.colors.get_data(), cloud.normals.get_data(), vvc, true);

            std::puts(std::format("Current time {} size {} \n", dcPlayer->current_time_ms(), vvc).c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));


            geo::ColoredCloudData cloudN;
            cloudN.resize(cloud.size()*1);
            for(int kk = 0; kk < cloud.size(); ++kk){

                for(int ll = 0; ll < 1; ++ll){
                    cloudN.vertices[ll*cloud.size() + kk] = cloud.vertices[kk] + cloud.normals[kk] * (ll * 0.1f);
                    cloudN.colors[ll*cloud.size() + kk] = cloud.normals[kk];
                }
            }


            io::CloudIO::save_cloud(std::format("E:/cloud_{}.obj", jj), cloud);
            io::CloudIO::save_cloud(std::format("E:/cloud_normals1_{}.obj", jj), cloudN);

            // for(size_t idV = 0; idV < cloud.size(); ++idV){
            //     cloud.vertices[idV] += cloud.normals[idV] * 0.3f;
            // }
            // io::CloudIO::save_cloud(std::format("E:/cloud_normals1_{}.obj", jj), cloud);

            // for(size_t idV = 0; idV < cloud.size(); ++idV){
            //     cloud.vertices[idV] += cloud.normals[idV] * 1.3f;
            // }
            // io::CloudIO::save_cloud(std::format("E:/cloud_normals2_{}.obj", jj), cloud);
        }

        stop_video__dc_video_player(dcPlayer);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::puts("delete\n");
        delete__dc_video_player(dcPlayer);
    }
    return;

    std::puts("Create DCPlayer\n");
    auto dcPlayer = create__dc_video_player();

    std::puts("Load from file\n");
    if(load_from_file__dc_video_player(dcPlayer, "E:/16-05-24.kvid") == 1){
        std::puts("Loading successful\n");
    }else{
        std::puts("Loading failure\n");
    }

    std::puts("Set settings\n");
    set_player_settings__dc_video_player(dcPlayer, 0, -1.f, -1.f);

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

    std::vector<CloudData> cloudsData;
    cloudsData.resize(nbCameras);

    std::vector<size_t> cloudsResSize;
    cloudsResSize.resize(nbCameras);

    std::puts("Start playing:\n");
    start_video__dc_video_player(dcPlayer);


    tool::StopWatch sw;
    sw.start();
    while(is_playing__dc_video_player(dcPlayer)){

        update__dc_video_player(dcPlayer);

        for(int idC = 0; idC < nbCameras; ++idC){
            int sizeCloud = get_current_frame_valid_vertices_count__dc_video_player(dcPlayer, idC);
            cloudsData[idC].resize(sizeCloud);
            std::puts(std::format("cloud:[{}] frame_id:[{}] size:[{}]\n", idC, current_frame_id__dc_video_player(dcPlayer, idC), sizeCloud).c_str());
        }

        for(int idC = 0; idC < nbCameras; ++idC){
            cloudsData[idC].verticesCopied = copy_camera_cloud_vfx__dc_video_player(dcPlayer, idC, cloudsData[idC].positions.data(), cloudsData[idC].colors.data(), cloudsData[idC].normals.data(), cloudsData[idC].positions.size(), 1);
            std::puts(std::format("cloud:[{}] vertices copied:[{}] \n", idC, cloudsData[idC].verticesCopied).c_str());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0/60.0/2.0)));

        if(sw.ellapsed_s() > 5){
            stop_video__dc_video_player(dcPlayer);
        }

    }std::puts("Playing stopped:\n");


    std::puts("Delete DCPlayer\n");
    delete__dc_video_player(dcPlayer);
}

#include "dc_network_direct_player_export.hpp"

auto dc_network_direct_player_export_test() -> void{

    std::string networkSettingsFilePath = "D:/DEV/ALIAD/ALIADTracking/toolset/cpp-projects/_build/bin/dc-manager/config/network/network_FLORIAN-PC.config";
    std::string deviceSettingsFilePath = "D:/DEV/ALIAD/ALIADTracking/toolset/cpp-projects/_build/bin/dc-manager/config/settings/device_FLORIAN-PC_all.config";
    std::string filtersSettingsFilePath = "D:/DEV/ALIAD/ALIADTracking/toolset/cpp-projects/_build/bin/dc-manager/config/settings/filters_FLORIAN-PC_all.config";
    std::string colorsSettingsFilePath = "D:/DEV/ALIAD/ALIADTracking/toolset/cpp-projects/_build/bin/dc-manager/config/settings/color_FLORIAN-PC_all.config";
    std::string modelSettingsFilePath = "D:/DEV/ALIAD/ALIADTracking/toolset/cpp-projects/_build/bin/dc-manager/config/calibration/model_FLORIAN-PC_all.config";

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


    int nbDevices = devices_nb__dc_network_direct_player(dcNetworkDirectPlayer);

    std::atomic_bool readData = true;
    std::thread rThread([&](){
        std::puts("start thread");
        while(readData){
            for(int idD = 0; idD < nbDevices; ++idD){
                std::puts(std::format("read data {}\n", idD).c_str());
                read_network_data__dc_network_direct_player(dcNetworkDirectPlayer, idD);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        std::puts("end thread");
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // std::puts("Devices connection state:\n");
    // for(int idD = 0; idD < nbDevices; ++idD){
    //     if(is_device_connected__dc_network_direct_player(dcNetworkDirectPlayer, idD)){
    //         std::puts(std::format("\tDevice n°{} connected\n", idD).c_str());
    //     }else{
    //         std::puts(std::format("\tDevice n°{} not connected\n", idD).c_str());
    //     }
    // }

    std::puts("Update settings\n");
    update_device_settings__dc_network_direct_player(dcNetworkDirectPlayer, deviceSettingsFilePath.c_str());
    // update_color_settings__dc_network_direct_player(dcNetworkDirectPlayer, colorsSettingsFilePath.c_str());
    // update_filters_settings__dc_network_direct_player(dcNetworkDirectPlayer, filtersSettingsFilePath.c_str());
    // update_model_settings__dc_network_direct_player(dcNetworkDirectPlayer, modelSettingsFilePath.c_str());

    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));


    return;
    std::puts("Start reading\n");
    // start_reading__dc_network_direct_player(dcNetworkDirectPlayer);

    int nbIterations = 1000;
    std::vector<std::vector<tool::cam::DCVertexMeshData>> data;
    data.resize(devices_nb__dc_network_direct_player(dcNetworkDirectPlayer));

    for(int idL = 0; idL < nbIterations; ++idL){

        // update__dc_network_direct_player(dcNetworkDirectPlayer);

        for(int idC = 0; idC < devices_nb__dc_network_direct_player(dcNetworkDirectPlayer); ++idC){
            auto sizeVertices = current_frame_cloud_size__dc_network_direct_player(dcNetworkDirectPlayer, idC);
            if(data[idC].size() < sizeVertices){
                data[idC].resize(sizeVertices);
            }
            auto verticesCopied = copy_current_frame_vertices__dc_network_direct_player(dcNetworkDirectPlayer, idC, data[idC].data(), sizeVertices, true);
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
    readData = false;
    rThread.join();
    // stop_reading__dc_network_direct_player(dcNetworkDirectPlayer);

    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::puts("Disconnect from devices\n");
    disconnect_from_devices__dc_network_direct_player(dcNetworkDirectPlayer);

    std::puts("Wait...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::puts("Delete DCNetworkDirectPlayer\n");
    delete__dc_network_direct_player(dcNetworkDirectPlayer);
}

#include "json.hpp"
using json = nlohmann::json;

auto test(const cam::DCFiltersSettings &f1) -> json{
    return json{
        {"min_widht_f",     f1.minWidthF},
        {"max_widht_f",     f1.maxWidthF},
        {"min_height_f",    f1.minHeightF},
        {"max_height_f",    f1.maxHeightF},
        {"min_depth_f",     f1.minDepthF},
        {"max_depth_f",     f1.maxDepthF}
    };
}

auto test_json() -> void{
    cam::DCFiltersSettings f1;
    std::cout << "b1:" << f1.minDepthF << "\n";
    f1.minDepthF = 0.5f;

    auto f1Str = f1.convert_to_json_str();

    cam::DCFiltersSettings f2;
    f2.init_from_json_str(f1Str);
    std::cout << "b2:" << f2.minDepthF << "\n";

    auto bin2 = f2.convert_to_json_binary();

    std::cout << "size " << bin2.size() << "\n";

    cam::DCFiltersSettings f3;
    f3.init_from_json_binary(bin2);

    std::cout << "b3:" << f3.minDepthF << "\n";

    f3.save_to_json_str_file("D:/json_test.json");

    cam::DCFiltersSettings f4;
    f4.load_from_file("D:/json_test.json");

    std::cout << "b4:" << f4.minDepthF << "\n";


    f4.save_to_json_binary_file("D:/json_test.bson");

    cam::DCFiltersSettings f5;
    f5.load_from_file("D:/json_test.bson");
    std::cout << "b5:" << f5.minDepthF << "\n";

    return;


    auto jsonF = test(f1);

    // json j2
    //     =  json{
    //              {"min_widht_f",     f1.minWidthF},
    //              {"max_widht_f",     f1.maxWidthF},
    //              {"min_height_f",    f1.minHeightF},
    //              {"max_height_f",    f1.maxHeightF},
    //              {"min_depth_f",     f1.minDepthF},
    //              {"max_depth_f",     f1.maxDepthF}
    //             };

    // auto b1 = json::to_bjdata(j2);
    // auto b2 = json::to_bjdata(j2);
    auto b3 = json::to_bson(jsonF);

    json fff(json::from_bson(json::to_bson(jsonF)).dump(4));


    std::cout << "jsonF " << jsonF.dump(4) << "\n" << json::from_bson(b3).dump(4) << "\n" << fff.dump(4) << "\n";

    // f1.minDepthF = 0.5;

    auto str = f1.convert_to_json_str();
    std::cout << "str " << str << "\n";

    f1.convert_to_json_binary();

    return;
}

#include "depth-camera/dc_device_impl.hpp"
#include "depth-camera/impl/azure_kinect_device_impl.hpp"

#include "thirdparty/taskflow/taskflow.hpp"
#include <execution>



#include "utility/logger.hpp"


auto test_dev() -> void{

    // cam::DCDevice dev(tool::cam::DCType::AzureKinect);
    // if(dev.open(0)){
    //     cam::DCConfigSettings cs;
    //     cs.idDevice = 0;
    //     cs.typeDevice = tool::cam::DCType::AzureKinect;
    //     dev.start_reading(cs);
    // }

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // dev.stop_reading();
    // dev.close();


    cam::AzureKinectDeviceImpl device;
    if(device.open(0)){

        cam::DCConfigSettings cs;
        cs.idDevice = 0;
        cs.typeDevice = tool::cam::DCType::AzureKinect;

        if(device.start(cs)){
            device.start_reading_thread();
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    device.stop_reading_thread();
    device.stop();
    device.close();
}

#include "utility/time.hpp"

auto test_multi_device() -> void{



    std::vector<std::unique_ptr<cam::DCDeviceImpl>> devices;

    std::mutex lock;

    std::vector<std::shared_ptr<cam::DCCompressedFrame>> frames;
    frames.resize(4, nullptr);

    for(size_t idD = 0; idD < 4; ++idD){
        devices.push_back(std::make_unique<cam::AzureKinectDeviceImpl>());

        cam::DCDeviceSettings ds;
        ds.apply_remote_grabber_profile();

        Logger::message("open\n");
        if(devices.back()->open(idD)){

            ds.configS.idDevice = idD;
            ds.configS.typeDevice = tool::cam::DCType::AzureKinect;

            Logger::message("start\n");
            if(devices.back()->start(ds.configS)){

                devices.back()->new_compressed_frame_signal.connect([&, idD](std::shared_ptr<cam::DCCompressedFrame> cFrame){
                    frames[idD] = std::move(cFrame);
                });

                devices.back()->loop_initialization();
            }

            // cam::DCDataSettings dataS;
            // dataS.

            // devices.back()->set_data_settings(dataS);
            // devices.back()->set_colors_settings();
        }
    }

    // auto executor = std::make_unique<tf::Executor>();
    // auto tk = std::make_unique<tf::Taskflow>();


    for(size_t idF = 0; idF < 1000; ++idF){
        // std::cout << "id " << idF << "\n";

        auto t1 = Time::milliseconds_since_epoch();
        std::for_each(std::execution::par_unseq, std::begin(devices), std::end(devices), [&](std::unique_ptr<cam::DCDeviceImpl> &device){
            device->read_settings();
            device->read_images();


            device->process_data();
        });
        auto t2 = Time::milliseconds_since_epoch();
        std::cout << "time1 " << idF << " " << Time::difference_ms(t1, t2).count() << std::endl;

        // std::for_each(std::execution::par_unseq, std::begin(devices), std::end(devices), [&](std::unique_ptr<cam::DCDeviceImpl> &device){
        //     // device->process_data();
        //     // device->init_frames();
        //     // device->resize_and_convert();
        //     // device->preprocess();
        //     // device->filter();
        //     // device->update_compressed_frame();
        //     // device->update_frame();
        // });
        // std::cout << "time2 " << idF << " " << Time::difference_ms(t2, Time::milliseconds_since_epoch()).count() << std::endl;

        // int idC= 0;
        // std::for_each(std::execution::seq, std::begin(devices), std::end(devices), [&](std::unique_ptr<cam::DCDeviceImpl> &device){

        //     std::cout << "cam "<< idC << " " << device->get_duration_micro_s("UPDATE_FRAME")->count() << "\n";
        //     ++idC;
        // });

        for(const auto &f : frames){
            if(f){
                std::cout << f->idCapture << " ";
            }
        }
        std::cout << std::flush;
    }


    // auto captureCT = tk->emplace([&](){


    // });




    std::this_thread::sleep_for(std::chrono::milliseconds(10000));


    for(auto &device : devices){
        device->stop_reading_thread();
    }

    for(auto &device : devices){
        device->stop();
    }

    for(auto &device : devices){
        device->close();
    }

    return;
}

int main(int, char *[]){

    std::cout << "start\n";
    Logger::init("./");
    Logger::get()->message_signal.connect([&](std::string message){
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        std::cerr << error;
    });


    // test_dev();
    test_multi_device();


    // cam::DCFiltersSettings f2;
    // f2.init_from_json_binary(binary);
    // // binary
    // // auto str    = f.convert_to_json_str();

    // std::cout << "b:" << f2.minDepthF << "\n";

    // auto str = f2.convert_to_json_str();

    // std::cout << "str:" << str << "\n";

    // json v(str);
    // std::cout << "-> " << v.dump() << "\n\n";

    // for (auto& element : v) {
    //     std::cout << element << '\n';
    // }



    // std::cout << "->->->\n";
    // for (json::iterator it = v.begin(); it != v.end(); ++it) {
    //     std::cout << it.key() << " : " << it.value() << "\n";
    // }

    return 0;


    // std::string_view sstr = str;
    // json v2(sstr);
    // std::cout << "-> " << v2.dump() << "\n\n";



    // std::cout << "cccc " << v2.contains("min_widht_f") << "\n";
    // std::cout << "cccc " << v2.contains("max_widht_f") << "\n";
    // std::cout << "cccc " << v2.contains("min_height_f") << "\n";

    // cam::DCFiltersSettings f3;
    // f3.init_from_json_str(str);



    // std::cout << "str: " << f3.minDepthF << "\n";

    return 0;
    // tool::StopWatch sw;

    // sw.start();

    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // std::puts(std::format("elapsed {} {} {} {}\n", sw.ellapsed_milli_s(), sw.first_start_ms(), sw.last_start_ms(), sw.total_ms()).c_str());

    // sw.set_current_time(50.0);
    // std::puts(std::format("elapsed {} {} {} {}\n", sw.ellapsed_milli_s(), sw.first_start_ms(), sw.last_start_ms(), sw.total_ms()).c_str());
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // std::puts(std::format("elapsed {} {} {} {}\n", sw.ellapsed_milli_s(), sw.first_start_ms(), sw.last_start_ms(), sw.total_ms()).c_str());

    // sw.stop();
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // sw.start();
    // std::puts(std::format("elapsed {} {} {} {}\n", sw.ellapsed_milli_s(), sw.first_start_ms(), sw.last_start_ms(), sw.total_ms()).c_str());
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // sw.stop();
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // std::puts(std::format("elapsed {} {} {} {}\n", sw.ellapsed_milli_s(), sw.first_start_ms(), sw.last_start_ms(), sw.total_ms()).c_str());
    // sw.set_current_time(200.0);
    // std::puts(std::format("elapsed {} {} {} {}\n", sw.ellapsed_milli_s(), sw.first_start_ms(), sw.last_start_ms(), sw.total_ms()).c_str());

    dc_video_player_export_test();

    return 0;
    // dc_video_player_export_test();
    dc_network_direct_player_export_test();

    return 0;
}



