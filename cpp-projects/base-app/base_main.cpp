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
#include <format>
#include <map>

// kinect4
#include <k4a/k4a.hpp>

// turbojpg
#include <turbojpeg.h>



// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"
#include "utility/io_file.hpp"
#include "camera/kinect2/k2_device.hpp"

#include "camera/dc_device_manager.hpp"

#include "data/integers_encoder.hpp"
#include "graphics/texture.hpp"
#include "io/cloud_io.hpp"
#include "camera/dc_player.hpp"
#include "camera/dc_frame_compressor.hpp"
#include "geometry/voxel_grid.hpp"

#include "exvr/ex_experiment.hpp"
// orbbec
#include "libobsensor/ObSensor.hpp"

#include "thirdparty/stb/stb_image_write.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace ob;
using namespace tool;
using namespace tool::camera;


// // frames processing thread
// void processFrames() {
//     std::vector<cv::Mat>                    mats;
//     cv::Mat                                 imuMat;
//     cv::Mat                                 rstMat;
//     std::vector<std::shared_ptr<ob::Frame>> frames;
//     while(!windowClose_) {
//         {
//             std::unique_lock<std::mutex> lk(srcFramesMtx_);
//             srcFramesCv_.wait(lk, [this] { return !srcFrames_.empty() || windowClose_; });
//             if(windowClose_) {
//                 break;
//             }
//             frames = srcFrames_;
//             srcFrames_.clear();
//         }
//         for(auto frame: frames) {
//             rstMat.release();
//             if(frame == nullptr) {
//                 mats.clear();
//                 imuMat.release();
//                 break;
//             }
//             if(frame->type() == OB_FRAME_COLOR) {
//                 auto videoFrame = frame->as<ob::VideoFrame>();
//                 switch(videoFrame->format()) {
//                 case OB_FORMAT_MJPG: {
//                     cv::Mat rawMat(1, videoFrame->dataSize(), CV_8UC1, videoFrame->data());
//                     rstMat = cv::imdecode(rawMat, 1);
//                 } break;
//                 case OB_FORMAT_NV21: {
//                     cv::Mat rawMat(videoFrame->height() * 3 / 2, videoFrame->width(), CV_8UC1, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_YUV2BGR_NV21);
//                 } break;
//                 case OB_FORMAT_YUYV:
//                 case OB_FORMAT_YUY2: {
//                     cv::Mat rawMat(videoFrame->height(), videoFrame->width(), CV_8UC2, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_YUV2BGR_YUY2);
//                 } break;
//                 case OB_FORMAT_RGB: {
//                     cv::Mat rawMat(videoFrame->height(), videoFrame->width(), CV_8UC3, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_RGB2BGR);
//                 } break;
//                 case OB_FORMAT_UYVY: {
//                     cv::Mat rawMat(videoFrame->height(), videoFrame->width(), CV_8UC2, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_YUV2BGR_UYVY);
//                 } break;
//                 default:
//                     break;
//                 }
//                 if(showInfo_ && !rstMat.empty()) {
//                     drawInfo(rstMat, videoFrame);
//                 }
//             }
//             else if(frame->type() == OB_FRAME_DEPTH) {
//                 auto videoFrame = frame->as<ob::VideoFrame>();
//                 if(videoFrame->format() == OB_FORMAT_Y16) {
//                     cv::Mat cvtMat;
//                     cv::Mat rawMat = cv::Mat(videoFrame->height(), videoFrame->width(), CV_16UC1, videoFrame->data());
//                     // depth frame pixel value multiply scale to get distance in millimeter
//                     float scale = videoFrame->as<ob::DepthFrame>()->getValueScale();

//                     // threshold to 5.12m
//                     cv::threshold(rawMat, cvtMat, 5120.0f / scale, 0, cv::THRESH_TRUNC);
//                     cvtMat.convertTo(cvtMat, CV_8UC1, scale * 0.05);
//                     cv::applyColorMap(cvtMat, rstMat, cv::COLORMAP_JET);
//                 }
//                 if(showInfo_ && !rstMat.empty()) {
//                     drawInfo(rstMat, videoFrame);
//                 }
//             }
//             else if(frame->type() == OB_FRAME_IR || frame->type() == OB_FRAME_IR_LEFT || frame->type() == OB_FRAME_IR_RIGHT) {
//                 auto videoFrame = frame->as<ob::VideoFrame>();
//                 if(videoFrame->format() == OB_FORMAT_Y16) {
//                     cv::Mat cvtMat;
//                     cv::Mat rawMat = cv::Mat(videoFrame->height(), videoFrame->width(), CV_16UC1, videoFrame->data());
//                     rawMat.convertTo(cvtMat, CV_8UC1, 1.0 / 16.0f);
//                     cv::cvtColor(cvtMat, rstMat, cv::COLOR_GRAY2RGB);
//                 }
//                 else if(videoFrame->format() == OB_FORMAT_Y8) {
//                     cv::Mat rawMat = cv::Mat(videoFrame->height(), videoFrame->width(), CV_8UC1, videoFrame->data());
//                     cv::cvtColor(rawMat * 2, rstMat, cv::COLOR_GRAY2RGB);
//                 }
//                 else if(videoFrame->format() == OB_FORMAT_MJPG) {
//                     cv::Mat rawMat(1, videoFrame->dataSize(), CV_8UC1, videoFrame->data());
//                     rstMat = cv::imdecode(rawMat, 1);
//                     cv::cvtColor(rstMat * 2, rstMat, cv::COLOR_GRAY2RGB);
//                 }
//                 if(showInfo_ && !rstMat.empty()) {
//                     drawInfo(rstMat, videoFrame);
//                 }
//             }
//             else if(frame->type() == OB_FRAME_ACCEL) {
//                 if(imuMat.empty()) {
//                     imuMat = cv::Mat::zeros(640, 360, CV_8UC3);
//                 }
//                 auto        accelFrame = frame->as<ob::AccelFrame>();
//                 auto        value      = accelFrame->value();
//                 std::string str        = "Accel:";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 60), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" x=") + std::to_string(value.x) + "m/s^2";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 120), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" y=") + std::to_string(value.y) + "m/s^2";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 180), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" z=") + std::to_string(value.z) + "m/s^2";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 240), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 continue;
//             }
//             else if(frame->type() == OB_FRAME_GYRO) {
//                 if(imuMat.empty()) {
//                     imuMat = cv::Mat(640, 360, CV_8UC3);
//                 }
//                 auto        gyroFrame = frame->as<ob::GyroFrame>();
//                 auto        value     = gyroFrame->value();
//                 std::string str       = "Gyro:";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 300), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" x=") + std::to_string(value.x) + "rad/s";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 360), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" y=") + std::to_string(value.y) + "rad/s";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 420), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" z=") + std::to_string(value.z) + "rad/s";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 480), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 continue;
//             }
//             if(!rstMat.empty()) {
//                 mats.push_back(rstMat);
//             }
//         }

//         if(!imuMat.empty()) {
//             mats.push_back(imuMat);
//             imuMat.release();
//         }

//         if(!mats.empty()) {
//             std::lock_guard<std::mutex> lock(renderMatsMtx_);
//             renderMats_ = mats;
//             mats.clear();
//         }
//     }
// }

// Save colored point cloud data to ply
void saveRGBPointsToPly(std::shared_ptr<ob::Frame> frame, std::string fileName) {
    int   pointsSize = frame->dataSize() / sizeof(OBColorPoint);
    FILE *fp         = fopen(fileName.c_str(), "wb+");
    fprintf(fp, "ply\n");
    fprintf(fp, "format ascii 1.0\n");
    fprintf(fp, "element vertex %d\n", pointsSize);
    fprintf(fp, "property float x\n");
    fprintf(fp, "property float y\n");
    fprintf(fp, "property float z\n");
    fprintf(fp, "property uchar red\n");
    fprintf(fp, "property uchar green\n");
    fprintf(fp, "property uchar blue\n");
    fprintf(fp, "end_header\n");

    OBColorPoint *point = (OBColorPoint *)frame->data();
    for(int i = 0; i < pointsSize; i++) {
        fprintf(fp, "%.3f %.3f %.3f %d %d %d\n", point->x, point->y, point->z, (int)point->r, (int)point->g, (int)point->b);
        point++;
    }

    fflush(fp);
    fclose(fp);
}


// Save point cloud data to ply
void savePointsToPly(std::shared_ptr<ob::Frame> frame, std::string fileName) {
    int   pointsSize = frame->dataSize() / sizeof(OBPoint);
    FILE *fp         = fopen(fileName.c_str(), "wb+");
    fprintf(fp, "ply\n");
    fprintf(fp, "format ascii 1.0\n");
    fprintf(fp, "element vertex %d\n", pointsSize);
    fprintf(fp, "property float x\n");
    fprintf(fp, "property float y\n");
    fprintf(fp, "property float z\n");
    fprintf(fp, "end_header\n");

    OBPoint *point = (OBPoint *)frame->data();
    for(int i = 0; i < pointsSize; i++) {
        fprintf(fp, "%.3f %.3f %.3f\n", point->x, point->y, point->z);
        point++;
    }

    fflush(fp);
    fclose(fp);
}



auto test_raw_orbbec() -> void{


    ob::Context::setLoggerSeverity(OB_LOG_SEVERITY_WARN);

    std::cout << "test_raw_orbbec: " << std::endl;

    // ob::Context context;

    // Create a pipeline with default device
    ob::Pipeline pipe;


    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

    std::cout << "Lost color profiles: " << std::endl;
    auto colorStreamProfileList = pipe.getStreamProfileList(OB_SENSOR_COLOR);
    std::cout << "count: " << colorStreamProfileList->count() << std::endl;

    std::cout << "get color profile: " << std::endl;
    auto profile = colorStreamProfileList->getProfile(
        OB_PROFILE_DEFAULT
    );
    // auto colorProfile = profile->as<ob::VideoStreamProfile>();
    auto colorProfile = colorStreamProfileList->getVideoStreamProfile(
        1280,
        720,
        OBFormat::OB_FORMAT_NV12,
        30
    );
    std::cout << "enable color stream: " << std::endl;
    config->enableStream(colorProfile);

    auto depthProfileList = pipe.getD2CDepthProfileList(colorProfile, ALIGN_D2C_SW_MODE);
    auto depthProfile = depthProfileList->getVideoStreamProfile(
        640, // OB_WIDTH_ANY,  // 640,
        576, // OB_HEIGHT_ANY, // 576,
        OBFormat::OB_FORMAT_Y16, //OBFormat::OB_FORMAT_Y16,
        colorProfile->fps()//30
    );
    std::cout << "enable depth stream: " << std::endl;
    config->enableStream(depthProfile);

    auto infraProfileList = pipe.getStreamProfileList(OB_SENSOR_IR);
    auto infraProfile = infraProfileList->getVideoStreamProfile(
        640, // OB_WIDTH_ANY,  // 640,
        576, // OB_HEIGHT_ANY, // 576,
        OBFormat::OB_FORMAT_Y16, //OBFormat::OB_FORMAT_Y16,
        colorProfile->fps()//30
    );
    std::cout << "enable infra stream: " << std::endl;
    config->enableStream(infraProfile);





    config->setAlignMode(ALIGN_D2C_SW_MODE);

    // Start the pipeline with config
    pipe.start(config);

    std::cout << "point cloud:\n";
    ob::PointCloudFilter pointCloud;
    auto cameraParam = pipe.getCameraParam();
    pointCloud.setCameraParam(cameraParam);

    // Create a window for rendering, and set the resolution of the window
    // Window app("DepthViewer", depthProfile->width(), depthProfile->height());

    // 1024 x 1024 : 0.25m – 2.21m
    // 512  x 512  : 0.25m –2.88m
    // 640  x 576  : 0.5m – 3.86m
    // 320  x 288  : 0.5m – 5.46m


    int idFr = 0;
    int maxNbFrames = 3;

    std::cout << "loop:\n";
    cv::Mat rstMat;
    while(idFr < maxNbFrames) {

        // Wait for up to 100ms for a frameset in blocking mode.
        auto frameSet = pipe.waitForFrames(100);
        if(frameSet == nullptr) {
            continue;
        }

        auto colorFrame = frameSet->colorFrame();
        auto depthFrame = frameSet->depthFrame();
        auto infraFrame = frameSet->irFrame();


        uint32_t  cwidth  = 0;
        uint32_t  cheight = 0;
        if(colorFrame){
            cwidth  = colorFrame->width();
            cheight = colorFrame->height();
        }

        uint32_t  iwidth  = 0;
        uint32_t  iheight = 0;
        if(infraFrame){
            iwidth  = infraFrame->width();
            iheight = infraFrame->height();
        }

        uint32_t  dwidth  = 0;
        uint32_t  dheight = 0;
        if(infraFrame){
            dwidth  = depthFrame->width();
            dheight = depthFrame->height();
        }

        std::cout << "width d: " << dwidth  << " height d: " << dheight << " ";
        std::cout << "width c: " << cwidth  << " height c: " << cheight << " ";
        std::cout << "width i: " << iwidth  << " height i: " << iheight << " ";

        // depth
        {

            uint16_t *data   = (uint16_t *)depthFrame->data();
            int valid = 0;
            int invalid = 0;

            int min = 50000000;
            int max = 0;
            for(int ii = 0; ii < dwidth*dheight; ++ii){
                if(min > data[ii]){
                    min = data[ii];
                }
                if(max < data[ii]){
                    max = data[ii];
                }

                if(data[ii] == 0){
                    invalid++;
                }else{
                    valid++;
                }
            }
            std::cout << "min " << min  << " max " << max << " ";
            std::cout << "valid: " << valid << " invalid " << invalid << "\n";

            // write depth map
            {
                cv::Mat cvtMat;
                cv::Mat rawMat = cv::Mat(depthFrame->height(), depthFrame->width(), CV_16UC1, depthFrame->data());
                // depth frame pixel value multiply scale to get distance in millimeter
                float scale = depthFrame->getValueScale();
                // threshold to 5.12m
                cv::threshold(rawMat, cvtMat, 5120.0f / scale, 0, cv::THRESH_TRUNC);
                cvtMat.convertTo(cvtMat, CV_8UC1, scale * 0.05);
                cv::applyColorMap(cvtMat, rstMat, cv::COLORMAP_JET);
                cv::imwrite(std::format("D:/ob_img_{}.png", idFr).c_str(),rstMat);
            }
        }

        if(depthFrame != nullptr && colorFrame != nullptr){




            auto depthValueScale = depthFrame->getValueScale();
            std::cout << "depthValueScale " << depthValueScale << "\n";
            pointCloud.setPositionDataScaled(depthValueScale);
            pointCloud.setCreatePointFormat(OB_FORMAT_RGB_POINT);

            try {
                // Generate a colored point cloud and save it
                std::cout << "Save RGBD PointCloud ply file..." << std::endl;
                std::shared_ptr<ob::Frame> frame = pointCloud.process(frameSet);

                int pointsSize = frame->dataSize() / sizeof(OBPoint);
                std::cout << "points size: " << pointsSize << "\n";
                std::cout << "depth map size: " << dwidth * dheight << "\n";


                saveRGBPointsToPly(frame, std::format("D:/rgb_cloud_{}.ply", idFr));
                // savePointsToPly(frame, std::format("D:/rgb_cloud_{}.ply", idFr));
                std::cout << "RGBPoints.ply Saved" << std::endl;
            }
            catch(std::exception &e) {
                std::cout << "Get point cloud failed" << std::endl;
            }
        }

        ++idFr;
    }

    // Stop the pipeline
    pipe.stop();

    return;
}


#include "io/cloud_io.hpp"

// // Save point cloud data to ply
// void savePointsToPly(std::shared_ptr<ob::Frame> frame, std::string fileName) {
//     int   pointsSize = frame->dataSize() / sizeof(OBPoint);
//     FILE *fp         = fopen(fileName.c_str(), "wb+");
//     fprintf(fp, "ply\n");
//     fprintf(fp, "format ascii 1.0\n");
//     fprintf(fp, "element vertex %d\n", pointsSize);
//     fprintf(fp, "property float x\n");
//     fprintf(fp, "property float y\n");
//     fprintf(fp, "property float z\n");
//     fprintf(fp, "end_header\n");

//     OBPoint *point = (OBPoint *)frame->data();
//     for(int i = 0; i < pointsSize; i++) {
//         fprintf(fp, "%.3f %.3f %.3f\n", point->x, point->y, point->z);
//         point++;
//     }

//     fflush(fp);
//     fclose(fp);
// }


auto test_femto_orbbec() -> void {

    std::puts("### Test femto orbbec.\n");

    DCDeviceManager dManager;

    tool::camera::DCDeviceSettings ds;

    // actions
    ds.actionsS.openDevice     = true;
    ds.actionsS.startReading  = true;
    // config
    ds.configS.typeDevice   = DCType::FemtoOrbbec;
    ds.configS.idDevice     = 0;
    ds.configS.disableLED   = false;
    ds.configS.synchMode    = DCSynchronisationMode::Standalone;
    ds.configS.mode         = DCMode::FO_CLOUD_C1280x720_DI640x576_NV12_F30;
    ds.configS.delayBetweenColorAndDepthUsec = 0;
    ds.configS.synchronizeColorAndDepth = true;
    // data
    ds.dataS.generateRGBLocalFrame   = true;
    ds.dataS.generateDepthLocalFrame = true;
    ds.dataS.generateInfraLocalFrame = true;
    ds.dataS.generateCloudLocal      = true;


    std::puts("### Open device.\n");
    dManager.update_device_settings(ds);

    int idC = 0;
    dManager.new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
        std::cout << "save cloud: " << frame->cloud.vertices.size() << " " << frame->cloud.has_vertices() << " " << frame->cloud.has_colors() << " " << frame->cloud.has_normals() << "\n";
        tool::io::CloudIO::save_cloud(std::format("D:/cloud_{}.obj", idC++), frame->cloud);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));



    ds.actionsS.startReading  = false;
    ds.actionsS.openDevice = false;

    std::puts("### Close device.\n");
    dManager.update_device_settings(ds);

    tool::Bench::display();
    std::puts("End tests.\n");
}

auto test_kinect4() -> void{

    std::puts("### Test femto orbbec.\n");

    DCDeviceManager dManager;

    tool::camera::DCDeviceSettings ds;

    // actions
    ds.actionsS.openDevice = true;
    ds.actionsS.startReading  = true;
    // config
    ds.configS.typeDevice   = DCType::AzureKinect;
    ds.configS.idDevice     = 0;
    ds.configS.disableLED   = false;
    ds.configS.synchMode    = DCSynchronisationMode::Standalone;
    ds.configS.mode         = DCMode::K4_CLOUD_C1280x720_DI640x576_NV12_F30;
    ds.configS.delayBetweenColorAndDepthUsec = 0;
    ds.configS.synchronizeColorAndDepth = true;

    std::puts("### Open device.\n");
    dManager.update_device_settings(ds);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    ds.actionsS.startReading  = false;
    ds.actionsS.openDevice = false;

    std::puts("### Close device.\n");
    dManager.update_device_settings(ds);

    tool::Bench::display();
    std::puts("End tests.\n");
}


auto test_raw_k4device() -> void{

    // open device
    std::puts("### Open device.\n");
    k4a_device_t handle = nullptr;
    k4a_result_t result = k4a_device_open(1, &handle);
    if (result == K4A_RESULT_FAILED){
        std::puts("### Fail to open device.\n");
        return;
    }

    k4a_device_configuration_t configuration;
    configuration.color_format                      = k4a_image_format_t::K4A_IMAGE_FORMAT_COLOR_MJPG;
    configuration.color_resolution                  = k4a_color_resolution_t::K4A_COLOR_RESOLUTION_720P;
    configuration.depth_delay_off_color_usec        = 0;
    configuration.subordinate_delay_off_master_usec = 0;
    configuration.synchronized_images_only          = true;
    configuration.camera_fps                        = k4a_fps_t::K4A_FRAMES_PER_SECOND_30;
    configuration.wired_sync_mode                   = k4a_wired_sync_mode_t::K4A_WIRED_SYNC_MODE_STANDALONE;
    configuration.depth_mode                        = k4a_depth_mode_t::K4A_DEPTH_MODE_NFOV_UNBINNED;

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


                k4a_image_t depthImg = k4a_capture_get_depth_image(capture);
                auto dw = k4a_image_get_width_pixels(depthImg);
                auto dh = k4a_image_get_height_pixels(depthImg);
                auto depthBuffer = reinterpret_cast<std::int16_t*>(k4a_image_get_buffer(depthImg));


                std::vector<geo::Pt3<std::uint8_t>> img;
                img.resize(dw*dh);

                if(countLoops % 10 == 0){
                    std::cout << "WRITE\n";
                    for(int ii = 0; ii < dw*dh; ++ii){
                        std::uint8_t dv;
                        if(depthBuffer[ii] != 0){
                            dv = 255;
                        }else{
                            dv = 0;
                        }
                        // auto dv = static_cast<std::uint8_t>(data[ii] % 255);
                        img[ii] = {
                            dv,dv,dv
                        };
                    }
                    std::cout << "->WRITE\n";
                    int res = stbi_write_png(std::format("D:/k4_img_{}.png", ii).c_str(), dw, dh, 3, img.data(), dw* sizeof(std::uint8_t)*3);
                    std::cout << "->->WRITE " << res << "\n";
                }


                std::puts("### Release capture.\n");
                k4a_image_release(depthImg);
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

    DCDeviceManager m;



    DCDeviceSettings ds;
    ds.actionsS.startReading  = true;
    ds.actionsS.openDevice = true;

    ds.configS.idDevice = 2;
    ds.configS.mode = DCMode::K4_CLOUD_C1280x720_DI640x576_NV12_F30;
    ds.configS.subordinateDelayUsec = 0;

    ds.dataS.sendCloud = true;
    ds.dataS.sendColor = true;
    
    m.update_device_settings(ds);

    DCFrameUncompressor uc;


    m.new_compressed_frame_signal.connect([&](std::shared_ptr<DCCompressedFrame> cFrame){
        std::cout << "cframe " << cFrame->idCapture << "\n";

        size_t totalDataSizeBytes = cFrame->total_data_size();

        std::vector<std::int8_t> bufferToSend;
        bufferToSend.resize(totalDataSizeBytes);

        size_t offset = 0;
        cFrame->write_to_data(bufferToSend.data(), offset, bufferToSend.size());

        auto cFrame2 = std::make_shared<DCCompressedFrame>();
        offset = 0;
        cFrame2->init_from_data(bufferToSend.data(), offset, totalDataSizeBytes);

        DCFrame frame;
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


#include <span>

int main(){

    // test_raw_orbbec();
    test_femto_orbbec();
    // std::vector<int> abc = {1,2,3};


    // std::span<int> s1(abc.data(), abc.size());

    // for(const auto& v : s1){
    //     std::cout << v << "\n";
    // }

    // std::span<int> s2{};
    // std::cout << s2.empty() << "\n";
    // std::span<int> s3;

    // std::cout << "end\n";
    // return 0;

    // test_raw_k4device();
    // test_raw_orbbec();
    // test_orbbec();
    return 0;

    using namespace std::chrono;

    DCPlayer player;
    player.load_from_file("D:/kvidtest.kvid");


    auto duration = player.video()->duration_ms();


    auto ffTs = duration_cast<milliseconds>(nanoseconds(player.video()->first_frame_capture_timestamp()));//.value()));
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
            std::cout << "time " << time << " idC " << idC << " " << id/**.value()*/ << "\n";
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
        if(id != -1){//.has_value()){
            if(auto frame = player.video()->get_compressed_frame(0, id/**.value()*/).lock(); frame != nullptr){
                std::cout << "time " << time << " id " << id/**.value()*/ << " " << frame->cloud_vertices_size() <<  "\n";
            }

        }else{
            std::cout << "ERROR " << id/**.error()*/ << "\n";
        }
    }

    return 0;
}



