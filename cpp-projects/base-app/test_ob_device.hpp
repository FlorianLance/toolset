
#pragma once

// std
#include <iostream>
#include <format>

// opencv
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// base
#include "camera/dc_device_manager.hpp"
#include "utility/benchmark.hpp"

// local
#include "test_ob_utility.hpp"
#include "io/cloud_io.hpp"


auto test_raw_orbbec_femto() -> void{


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

auto test_orbbec_femto() -> void {

    using namespace tool;
    using namespace tool::camera;
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

