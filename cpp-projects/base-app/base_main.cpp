/*******************************************************************************
** Toolset-base                                                               **
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

// std
#include <iostream>
#include <filesystem>
#include <ostream>

// kinect4
#include <k4a/k4a.hpp>

// turbojpg
#include <turbojpeg.h>

// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"
#include "utility/io_file.hpp"
#include "camera/kinect2/k2_device.hpp"
#include "camera/kinect4/k4_device.hpp"
#include "camera/kinect4/k4_device_manager.hpp"

#include "data/integers_encoder.hpp"
#include "graphics/texture.hpp"
#include "files/cloud_io.hpp"
#include "camera/kinect4/k4_player.hpp"
#include "camera/kinect4/k4_frame_compressor.hpp"
#include "geometry/voxel_grid.hpp"

#include "exvr/ex_experiment.hpp"

using namespace tool;
using namespace tool::camera;

void test_k4device(){

    std::puts("### Init device.\n");
    tool::camera::K4Device device;
    std::puts("### Open device.\n");
    if(device.open(0)){
        tool::camera::K4ConfigSettings config;
        config.idDevice = 0;
        config.mode = tool::camera::K4Mode::Cloud_640x576_NV12;
        config.synchronizeColorAndDepth = true;
        config.delayBetweenColorAndDepthUsec = 0;
        config.synchMode = tool::camera::K4SynchronisationMode::Standalone;
        config.subordinateDelayUsec = 0;
        config.disableLED = false;

        std::puts("### Start cameras.\n");
        device.start_cameras(config);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::puts("### Stop cameras.\n");
        device.stop_cameras();

        std::puts("### Close device.\n");
        device.close();
    }else{
        std::puts("### Device opening failure.\n");
    }

    std::puts("End tests.\n");
    tool::Bench::display();
    std::puts("End tests.\n");
}


void test_raw_k4device(){

    // open device
    std::puts("### Open device.\n");
    k4a_device_t handle = nullptr;
    k4a_result_t result = k4a_device_open(0, &handle);
    if (result == K4A_RESULT_FAILED){
        std::puts("### Fail to open device.\n");
        return;
    }

    k4a_device_configuration_t configuration;
    configuration.color_format = k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_BGRA32;
    configuration.color_resolution = k4a_color_resolution_t::K4A_COLOR_RESOLUTION_720P;
    configuration.depth_delay_off_color_usec = 0;
    configuration.subordinate_delay_off_master_usec = 0;
    configuration.synchronized_images_only = false;
    configuration.camera_fps = k4a_fps_t::K4A_FRAMES_PER_SECOND_30;
    configuration.wired_sync_mode = k4a_wired_sync_mode_t::K4A_WIRED_SYNC_MODE_STANDALONE;
    configuration.depth_mode = k4a_depth_mode_t::K4A_DEPTH_MODE_NFOV_UNBINNED;

    std::puts("### Start cameras.\n");
    result = k4a_device_start_cameras(handle, &configuration);
    if (result == K4A_RESULT_SUCCEEDED){

        int countLoops = 100;


        for(int ii = 0; ii < countLoops; ++ii){

            k4a_capture_t capture = nullptr;

            std::puts("### Get capture.\n");
            auto wresult = k4a_device_get_capture(handle, &capture, 300);
            if(wresult == k4a_wait_result_t::K4A_WAIT_RESULT_SUCCEEDED){
                std::puts("SUCCEEDED.\n");

                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                std::puts("### Release capture.\n");
                k4a_capture_release(capture);

            }else if(wresult == k4a_wait_result_t::K4A_WAIT_RESULT_FAILED){
                std::puts("FAILED.\n");
            }else if(wresult == k4a_wait_result_t::K4A_WAIT_RESULT_TIMEOUT){
                std::puts("TIMEOUT.\n");
            }


        }

        std::puts("### Stop cameras.\n");
        k4a_device_stop_cameras(handle);

    }else{
        std::puts("### Fail to start device.\n");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::puts("### Close device.\n");
    k4a_device_close(handle);
    handle = nullptr;
}


auto test_compress_frame() -> void{

    K4DeviceManager m;

    m.initialize();
    m.update_device_list();

    K4DeviceSettings ds;
    ds.actionsS.openCamera  = true;
    ds.actionsS.startDevice = true;

    ds.configS.idDevice = 2;
    ds.configS.mode = K4Mode::Cloud_640x576_NV12;
    ds.configS.subordinateDelayUsec = 0;

    ds.dataS.sendCloud = true;
    ds.dataS.sendColor = true;

    m.update_settings(ds);

    K4FrameUncompressor uc;


    m.new_compressed_frame_signal.connect([&](std::shared_ptr<K4CompressedFrame> cFrame){
        std::cout << "cframe " << cFrame->idCapture << "\n";

        std::vector<std::int8_t> bufferToSend;
        size_t totalDataSizeBytes = cFrame->convert_to_data(bufferToSend);

//        std::cout << totalDataSizeBytes << "\n" << std::flush;
        auto cFrame2 = std::make_shared<K4CompressedFrame>();
        cFrame2->init_from_data(bufferToSend.data());

        K4Frame frame;
        uc.uncompress(cFrame2.get(), frame);

        std::cout << "frame " << frame.idCapture << "\n" << std::flush;
    });


    for(size_t ii = 0; ii < 100; ++ii){
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

}


void kinect2_test(){

//    using namespace std::chrono_literals;
//    camera::K2Device kinect;
//    if(!kinect.open(camera::K2FrameRequest::compressed_color_mesh)){
//        std::cerr << "Cannot init kinect 2\n";
//        return;
//    }

//    std::cout << "init\n";
//    std::this_thread::sleep_for(1000ms);
//    std::cout << "try to grab\n";

//    for(int ii = 0; ii < 10000; ++ii){
//        if(auto newFrame = kinect.get_kinect_data(); newFrame.has_value()){
//            std::cout << "-";
//        }else{
//            std::cout << "E";
//        }
//        std::this_thread::sleep_for((1000/33)*1ms);
//    }

//    std::cout << "close\n";
//    kinect.close();
}



int main(){

    using namespace std::chrono;

    K4Player player;
    player.load_from_file("D:/kvidtest.kvid");


    auto duration = player.video()->duration_ms();


    auto ffTs = duration_cast<milliseconds>(nanoseconds(player.video()->first_frame_capture_timestamp().value()));
    for(size_t idC = 0; idC < player.video()->nb_cameras(); ++idC){
        std::cout << player.video()->get_camera_data(idC)->frames.size() << "\n";
//        auto ff = duration_cast<milliseconds>(nanoseconds(player.video()->get_camera_data(idC)->first_frame_capture_timestamp().value()));
//        auto sec = duration_cast<milliseconds>(nanoseconds(player.video()->get_camera_data(idC)->frames[1]->afterCaptureTS));
//        std::cout << "IDC " << idC << " " << ffTs << " " << ff << " " << sec << "\n";
    }


    return 0;

    for(size_t ii = 0; ii < 10; ++ii){
        auto time = (ii / 10.0)* duration;

        for(size_t idC = 0; idC < player.video()->nb_cameras(); ++idC){
            auto id = player.video()->closest_frame_id_from_time(idC, time);
            std::cout << "time " << time << " idC " << idC << " " << id.value() << "\n";
        }
    }


    return 0;
    player.load_from_file("D:/_debug/adriel2.kvid");

    player.set_current_time(1000);
    player.update_time();
    player.remove_until_current_frame();
    player.update_time();

    player.merge_cameras(0.005f, {-20.f,-20.f,-20.f}, {+20.f,+20.f,+20.f});
    player.save_to_file("D:/--.kvid");


    duration = player.video()->duration_ms();

    for(size_t ii = 0; ii < 1000; ++ii){
        auto time = (ii / 1000.0)* duration;
        auto id = player.video()->closest_frame_id_from_time(0, time);
        if(id.has_value()){
            if(auto frame = player.video()->get_compressed_frame(0, id.value()).lock(); frame != nullptr){
                std::cout << "time " << time << " id " << id.value() << " " << frame->cloud_vertices_size() <<  "\n";
            }

        }else{
            std::cout << "ERROR " << id.error() << "\n";
        }
    }

    return 0;
}



