
#pragma once

// std
#include <iostream>
#include <vector>

// kinect4
#include <k4a/k4a.hpp>

// stb
#include "thirdparty/stb/stb_image_write.h"

// base
#include "camera/dc_device_manager.hpp"
#include "utility/benchmark.hpp"
#include "geometry/point3.hpp"
#include "io/cloud_io.hpp"


auto test_kinect4() -> void{

    using namespace tool;
    using namespace tool::cam;

    std::puts("### Test Azure kinectc.\n");

    DCDeviceManager dManager;

    tool::cam::DCDeviceSettings ds;

    // actions
    ds.actionsS.openDevice    = true;
    ds.actionsS.startReading  = true;
    // config
    ds.configS.typeDevice   = DCType::AzureKinect;
    ds.configS.idDevice     = 0;
    ds.configS.disableLED   = false;
    ds.configS.synchMode    = DCSynchronisationMode::Standalone;
    ds.configS.mode         = DCMode::AK_C1280x720_DI640x576_NV12_F30;
    ds.configS.delayBetweenColorAndDepthUsec = 0;
    ds.configS.synchronizeColorAndDepth = true;    
    // data
    // ds.dataS.generateRGBLocalFrame   = true;
    // ds.dataS.generateDepthLocalFrame = true;
    // ds.dataS.generateInfraLocalFrame = true;
    // ds.dataS.generateCloudLocal      = true;

    std::puts("### Open device.\n");
    dManager.update_device_settings(ds);

    int idC = 0;
    dManager.new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
        if(idC < 5){
            // std::cout << "frame " << frame->depthWidth << " " << frame->depthHeight << " " << frame->depthData.size() << " "<< frame->imageDepthData.size() << "\n";
            std::cout << "save cloud: " << frame->cloud.vertices.size() << " " << frame->cloud.has_vertices() << " " << frame->cloud.has_colors() << " " << frame->cloud.has_normals() << "\n";
            tool::io::CloudIO::save_cloud(std::format("D:/ak_cloud_{}.obj", idC++), frame->cloud);


            // geo::ColoredCloudData c;
            // auto &d = frame->depthData;


            // auto dRes    = depth_resolution(ds.configS.mode);
            // auto dWidth  = depth_width(dRes);
            // auto dHeight = depth_height(dRes);

            // // std::cout << "depth " << frame->depthData.size() << "\n";
            // c.vertices.resize(dWidth*dHeight);
            // size_t id = 0;
            // for(int ii = 0; ii < dHeight; ++ii){
            //         for(int jj = 0; jj < dWidth; ++jj){
            //         c.vertices[id] = {ii*00.1f, jj*0.01f, d[id]*0.001f};
            //         id++;
            //     }
            // }
            // tool::io::CloudIO::save_cloud(std::format("D:/ak2_cloud_{}.obj", idC++), c);

        }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ds.actionsS.startReading  = false;
    ds.actionsS.openDevice = false;

    std::puts("### Close device.\n");
    dManager.update_device_settings(ds);

    tool::Bench::display();
    std::puts("End tests.\n");
}


auto test_raw_k4device() -> void{

    using namespace tool;
    using namespace tool::geo;

    for(int ii = 0; ii < 4; ++ii){

        // open device
        // std::puts("### Open device.\n");
        std::cout << "open device " << ii << "\"n";
        k4a_device_t handle = nullptr;
        k4a_result_t result = k4a_device_open(ii, &handle);
        if (result == K4A_RESULT_FAILED){
            std::puts("### Fail to open device.\n");
            return;
        }

        k4a_device_configuration_t configuration;
        configuration.color_format                      = k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_NV12;
        configuration.color_resolution                  = k4a_color_resolution_t::K4A_COLOR_RESOLUTION_720P;
        configuration.depth_delay_off_color_usec        = 0;
        configuration.subordinate_delay_off_master_usec = 0;
        configuration.synchronized_images_only          = true;
        configuration.camera_fps                        = k4a_fps_t::K4A_FRAMES_PER_SECOND_30;
        configuration.wired_sync_mode                   = k4a_wired_sync_mode_t::K4A_WIRED_SYNC_MODE_STANDALONE;
        configuration.depth_mode                        = k4a_depth_mode_t::K4A_DEPTH_MODE_NFOV_UNBINNED;

        k4a::calibration calib;
        result = k4a_device_get_calibration(handle, configuration.depth_mode, configuration.color_resolution, &calib);
        if(result != K4A_RESULT_SUCCEEDED){
            std::puts("### Fail to get calibration.\n");
            return;
        }

        const auto &ce = calib.extrinsics;
        // std::cout << "K4A_CALIBRATION_TYPE_NUM " <<K4A_CALIBRATION_TYPE_NUM << "\n";
        // for(int ii = 0; ii < K4A_CALIBRATION_TYPE_NUM; ++ii){
        //     for(int jj = 0; jj < K4A_CALIBRATION_TYPE_NUM; ++jj){
        //         const auto &tr = calib.extrinsics[ii][jj].translation;
        //         const auto &rot = calib.extrinsics[ii][jj].rotation;
        //         std::cout << "[" << ii << " " << jj << "]\n" << tr[0] << " " << tr[1] << " " << tr[2] << "\n";
        //         for(int kk = 0; kk < 9; ++kk){
        //             std::cout << rot[kk] << " ";
        //         }
        //         std::cout << "\n\n";
        //     }
        // }

        const auto &cccc = calib.color_camera_calibration;
        std::cout << "calib.color_camera_calibration\n";
        // std::cout << " metric radius: " << cccc.metric_radius << "\n";
        // std::cout << " resolution_width: " << cccc.resolution_width << "\n";
        // std::cout << " resolution_height: " << cccc.resolution_height << "\n";
        // std::cout << " intrinsics.parameter_count: " << cccc.intrinsics.parameter_count << "\n";
        // std::cout << " intrinsics.type: " << cccc.intrinsics.type << "\n";

        const auto &cp = cccc.intrinsics.parameters.param;
        std::cout << "cx " << cp.cx << "\n";
        std::cout << "cu " << cp.cy << "\n";
        std::cout << "fx " << cp.fx << "\n";
        std::cout << "fy " << cp.fy << "\n";
        std::cout << "k1 " << cp.k1 << "\n";
        std::cout << "k2 " << cp.k2 << "\n";
        std::cout << "k3 " << cp.k3 << "\n";
        std::cout << "k4 " << cp.k4 << "\n";
        std::cout << "k5 " << cp.k5 << "\n";
        std::cout << "k6 " << cp.k6 << "\n";
        std::cout << "codx " << cp.codx << "\n";
        std::cout << "cody " << cp.cody << "\n";
        std::cout << "p2 " << cp.p2 << "\n";
        std::cout << "p1 " << cp.p1 << "\n";
        std::cout << "metric_radius " << cp.metric_radius << "\n";

        const auto &dccc = calib.depth_camera_calibration;
        std::cout << "calib.depth_camera_calibration\n";
        // std::cout << " metric radius: " << dccc.metric_radius << "\n";
        // std::cout << " resolution_width: " << dccc.resolution_width << "\n";
        // std::cout << " resolution_height: " << dccc.resolution_height << "\n";
        // std::cout << " intrinsics.parameter_count: " << dccc.intrinsics.parameter_count << "\n";
        // std::cout << " intrinsics.type: " << dccc.intrinsics.type << "\n";

        const auto &dp = dccc.intrinsics.parameters.param;
        std::cout << "cx " << dp.cx << "\n";
        std::cout << "cu " << dp.cy << "\n";
        std::cout << "fx " << dp.fx << "\n";
        std::cout << "fy " << dp.fy << "\n";
        std::cout << "k1 " << dp.k1 << "\n";
        std::cout << "k2 " << dp.k2 << "\n";
        std::cout << "k3 " << dp.k3 << "\n";
        std::cout << "k4 " << dp.k4 << "\n";
        std::cout << "k5 " << dp.k5 << "\n";
        std::cout << "k6 " << dp.k6 << "\n";
        std::cout << "codx " << dp.codx << "\n";
        std::cout << "cody " << dp.cody << "\n";
        std::cout << "p2 " << dp.p2 << "\n";
        std::cout << "p1 " << dp.p1 << "\n";
        std::cout << "metric_radius " << dp.metric_radius << "\n";


        std::puts("### Close device.\n");
        k4a_device_close(handle);

    }

    return;





    // std::puts("### Start cameras.\n");
    // result = k4a_device_start_cameras(handle, &configuration);

    // if (result == K4A_RESULT_SUCCEEDED){

    //     int countLoops = 1;


    //     for(int ii = 0; ii < countLoops; ++ii){

    //         k4a_capture_t capture = nullptr;

    //         std::puts("### Get capture.\n");
            // auto wresult = k4a_device_get_capture(handle, &capture, 300);
    //         if(wresult == k4a_wait_result_t::K4A_WAIT_RESULT_SUCCEEDED){
    //             std::puts("SUCCEEDED.\n");

    //             std::this_thread::sleep_for(std::chrono::milliseconds(100));


    //             k4a_image_t depthImg = k4a_capture_get_depth_image(capture);
    //             auto dw = k4a_image_get_width_pixels(depthImg);
    //             auto dh = k4a_image_get_height_pixels(depthImg);
    //             auto depthBuffer = reinterpret_cast<std::int16_t*>(k4a_image_get_buffer(depthImg));


    //             std::vector<geo::Pt3<std::uint8_t>> img;
    //             img.resize(dw*dh);

    //             if(countLoops % 10 == 0){
    //                 std::cout << "WRITE\n";
    //                 for(int ii = 0; ii < dw*dh; ++ii){
    //                     std::uint8_t dv;
    //                     if(depthBuffer[ii] != 0){
    //                         dv = 255;
    //                     }else{
    //                         dv = 0;
    //                     }
    //                     // auto dv = static_cast<std::uint8_t>(data[ii] % 255);
    //                     img[ii] = {
    //                         dv,dv,dv
    //                     };
    //                 }
    //                 std::cout << "->WRITE\n";
    //                 int res = stbi_write_png(std::format("D:/k4_img_{}.png", ii).c_str(), dw, dh, 3, img.data(), dw* sizeof(std::uint8_t)*3);
    //                 std::cout << "->->WRITE " << res << "\n";
    //             }


    //             std::puts("### Release capture.\n");
    //             k4a_image_release(depthImg);
    //             k4a_capture_release(capture);

    //         }else if(wresult == k4a_wait_result_t::K4A_WAIT_RESULT_FAILED){
    //             std::puts("FAILED.\n");
    //         }else if(wresult == k4a_wait_result_t::K4A_WAIT_RESULT_TIMEOUT){
    //             std::puts("TIMEOUT.\n");
    //         }


    //     }

    //     std::puts("### Stop cameras.\n");
    //     k4a_device_stop_cameras(handle);

    // }else{
    //     std::puts("### Fail to start device.\n");
    // }

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // std::puts("### Close device.\n");
    // k4a_device_close(handle);
    // handle = nullptr;
}
