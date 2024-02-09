
/*******************************************************************************
** Toolset-dc-grabber                                                         **
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

// base
#include "utility/logger.hpp"
#include "utility/paths.hpp"

// local
#include "dcg_controller.hpp"

using namespace tool;

#include "thirdparty/taskflow/taskflow.hpp"

auto create_read_data_taskflow() -> tf::Taskflow{

    tf::Taskflow readDataTF;
    auto captureCT         = readDataTF.emplace([&](){return rand()%2;}).name("capture_frame");
    auto captureFailureT   = readDataTF.emplace([&](){std::cout << "capture_failure\n";}).name("capture_failure");
    auto captureSuccessT   = readDataTF.emplace([&](){std::cout << "capture_success\n";}).name("capture_success");
    auto readColorImageT   = readDataTF.emplace([&](){std::cout << "read_color_image\n";}).name("read_color_image");
    auto readDepthImageT   = readDataTF.emplace([&](){}).name("read_depth_image");
    auto readInfraImageT   = readDataTF.emplace([&](){}).name("read_infra_image");
    auto readBodytrackingT = readDataTF.emplace([&](){}).name("read_body_tracking");
    auto readAudio         = readDataTF.emplace([&](){}).name("read_audio");
    auto readIMU           = readDataTF.emplace([&](){}).name("read_imu");
    auto waitForDataCheckT = readDataTF.emplace([&](){}).name("wait_for_data");
    captureCT.precede(captureFailureT, captureSuccessT);
    captureSuccessT.precede(
        readColorImageT, readDepthImageT, readInfraImageT,
        readBodytrackingT, readAudio, readIMU
    );
    waitForDataCheckT.succeed(
        readColorImageT, readDepthImageT, readInfraImageT,
        readBodytrackingT, readAudio, readIMU
    );
    return readDataTF;
}

auto process_data_taskflow() -> tf::Taskflow{

    tf::Taskflow processDataTF;

    auto startProcessingDataT             = processDataTF.emplace([&](){}).name("start_processing_data");
    auto convertColorImageT               = processDataTF.emplace([&](){}).name("convert_color_image");
    auto resizeColorImageT                = processDataTF.emplace([&](){}).name("resize_color_image");

    auto generateCloudT                   = processDataTF.emplace([&](){}).name("generate_cloud");

    auto preprocessColorImageT            = processDataTF.emplace([&](){}).name("preprocess_color_image");
    auto preprocessDepthSizedColorImageT  = processDataTF.emplace([&](){}).name("preprocess_depth_sized_color_image");
    auto preprocessDepthImageT            = processDataTF.emplace([&](){}).name("preprocess_depth_image");
    auto preprocessInfraImageT            = processDataTF.emplace([&](){}).name("preprocess_infra_image");
    auto preprocessCloudImageT            = processDataTF.emplace([&](){}).name("preprocess_cloud_image");
    auto preprocessBodytrackingImageT     = processDataTF.emplace([&](){}).name("preprocess_body_tracking_image");

    startProcessingDataT.precede(
        convertColorImageT, generateCloudT,
        preprocessColorImageT,preprocessDepthImageT,
        preprocessInfraImageT,preprocessBodytrackingImageT
    );

    convertColorImageT.precede(resizeColorImageT);
    resizeColorImageT.precede(preprocessDepthSizedColorImageT);
    generateCloudT.precede(preprocessCloudImageT);

    auto filterDepthBasicT                  = processDataTF.emplace([&](){}).name("filter_depth_basic");
    auto filterDepthFromDepthSizedColorT    = processDataTF.emplace([&](){}).name("filter_depth_from_depth_sized_color");
    auto filterDepthFromInfraT              = processDataTF.emplace([&](){}).name("filter_depth_from_infra");
    auto filterDepthFromBodyTrackingT       = processDataTF.emplace([&](){}).name("filter_depth_from_body_tracking");
    auto filterDepthFromCloudT              = processDataTF.emplace([&](){}).name("filter_depth_from_cloud");
    auto filterDepthComplexT                = processDataTF.emplace([&](){}).name("filter_depth_complex");

    preprocessDepthImageT.precede(filterDepthBasicT);
    filterDepthFromBodyTrackingT.succeed(filterDepthBasicT,         preprocessBodytrackingImageT);
    filterDepthFromInfraT.succeed(filterDepthFromBodyTrackingT,     preprocessInfraImageT);
    filterDepthFromCloudT.succeed(filterDepthFromInfraT,            preprocessCloudImageT);
    filterDepthFromDepthSizedColorT.succeed(filterDepthFromCloudT,  preprocessDepthSizedColorImageT);
    filterDepthComplexT.succeed(filterDepthFromDepthSizedColorT);

    auto filterDepthSizedColorFromDepthT     = processDataTF.emplace([&](){}).name("filter_depth_sized_color_from_depth");
    auto filterInfraFromDepthT               = processDataTF.emplace([&](){}).name("filter_infra_from_depth");
    auto mixDepthSizedColorWithBodyTrackingT = processDataTF.emplace([&](){}).name("mix_depth_sized_color_with_body_tracking");
    auto mixInfraWithBodyTrackingT           = processDataTF.emplace([&](){}).name("mix_infra_with_body_tracking");

    filterDepthSizedColorFromDepthT.succeed(filterDepthComplexT);
    filterDepthSizedColorFromDepthT.precede(mixDepthSizedColorWithBodyTrackingT);

    filterInfraFromDepthT.succeed(filterDepthComplexT);
    filterInfraFromDepthT.precede(mixInfraWithBodyTrackingT);

    tf::Task updateCompressedFrameColor = processDataTF.emplace([&](){}).name("update_compressed_frame_color");
    tf::Task updateCompressedFrameDepthSizedColor = processDataTF.emplace([&](){}).name("update_compressed_frame_depth_sized_color");
    tf::Task updateCompressedFrameDepth = processDataTF.emplace([&](){}).name("update_compressed_frame_depth");
    tf::Task updateCompressedFrameInfra = processDataTF.emplace([&](){}).name("update_compressed_frame_infra");
    tf::Task updateCompressedFrameCloud = processDataTF.emplace([&](){}).name("update_compressed_frame_cloud");
    tf::Task updateCompressedFrameAudio = processDataTF.emplace([&](){}).name("update_compressed_frame_audio");
    tf::Task updateCompressedFrameIMU = processDataTF.emplace([&](){}).name("update_compressed_frame_imu");
    tf::Task updateCompressedFrameBodies = processDataTF.emplace([&](){}).name("update_compressed_frame_bodies");
    tf::Task updateCompressedFrameCalibration = processDataTF.emplace([&](){}).name("update_compressed_frame_calibration");
    tf::Task finalizeCompressionT = processDataTF.emplace([&](){}).name("finalize_compression");

    updateCompressedFrameColor.succeed(convertColorImageT);
    updateCompressedFrameDepthSizedColor.succeed(mixDepthSizedColorWithBodyTrackingT);
    updateCompressedFrameDepth.succeed(filterDepthComplexT);
    updateCompressedFrameInfra.succeed(mixInfraWithBodyTrackingT);
    updateCompressedFrameCloud.succeed(filterDepthComplexT);

    updateCompressedFrameAudio.succeed(startProcessingDataT);
    updateCompressedFrameCalibration.succeed(startProcessingDataT);
    updateCompressedFrameIMU.succeed(startProcessingDataT);
    updateCompressedFrameBodies.succeed(preprocessBodytrackingImageT);

    finalizeCompressionT.succeed(
        updateCompressedFrameColor, updateCompressedFrameDepthSizedColor, updateCompressedFrameDepth,
        updateCompressedFrameInfra,  updateCompressedFrameCloud, updateCompressedFrameCalibration,
        updateCompressedFrameAudio, updateCompressedFrameIMU, updateCompressedFrameBodies
    );

    return processDataTF;
}

auto create_process_frame_taskflow(tf::Taskflow &readDataTF, tf::Taskflow &processDataTF) -> tf::Taskflow{

    tf::Taskflow processFrameTF;
    auto startProcessingFrameT       = processFrameTF.emplace([&](){std::cout << "start_processing_frame\n";}).name("start_processing_frame");
    auto updateSettingsT             = processFrameTF.emplace([&](){}).name("update_settings");
    auto readingDataModuleT          = processFrameTF.composed_of(readDataTF).name("reading_data_module");
    auto checkDataCT                 = processFrameTF.emplace([&](){return rand()%2;}).name("check_data");
    auto successDataT                = processFrameTF.emplace([&](){std::cout << "success_data\n";}).name("success_data");
    auto failureDataT                = processFrameTF.emplace([&](){std::cout << "failure_data\n";}).name("failure_data");
    auto startProcessingDataModuleT  = processFrameTF.composed_of(processDataTF).name("processing_data_module");

    startProcessingFrameT.precede(updateSettingsT);
    updateSettingsT.precede(readingDataModuleT);
    readingDataModuleT.precede(checkDataCT);

    checkDataCT.precede(failureDataT, successDataT);
    successDataT.precede(startProcessingDataModuleT);

    return processFrameTF;
}

auto gen_grabbing_task_flow() -> void{

    tf::Executor executor(1);
    tf::Taskflow mainLoopTF;
    bool doLoop = true;

    size_t loopCounter = 0;

    auto startLoopT = mainLoopTF.emplace([&](){}).name("start_loop");
    auto endLoopT = mainLoopTF.emplace([&](){}).name("end_loop");
    auto doLoopCT = mainLoopTF.emplace([&](){
        doLoop = loopCounter < 10;
        std::cout << "LOOP ID : " << loopCounter << "\n";
        ++loopCounter;
        return doLoop ? 1 : 0;
    }).name("do_loop");

    auto startIterationLoopT = mainLoopTF.emplace([&](){
    }).name("start_iteration_loop");

    auto endIterationLoopT = mainLoopTF.emplace([&](){return 0;
    }).name("end_iteration_loop");

    // main loop
    auto readDataTF     = create_read_data_taskflow();
    auto proccessDataTF = process_data_taskflow();
    auto processFrameTF = create_process_frame_taskflow(readDataTF, proccessDataTF);
    auto processFrameT  = mainLoopTF.composed_of(processFrameTF).name("process_frame_module");

    startLoopT.precede(doLoopCT);
    doLoopCT.precede(endLoopT, startIterationLoopT);
    startIterationLoopT.precede(processFrameT);
    processFrameT.precede(endIterationLoopT);
    endIterationLoopT.precede(doLoopCT);
    mainLoopTF.dump(std::cout);
    // executor.run(mainLoopTF).wait();
}


int main(int argc, char *argv[]){


    gen_grabbing_task_flow();

    // init paths
    Paths::initialize(argv);

    Logger::message("Start DC-Grabber.\n");
    Logger::message(std::format("Args {}.\n", argc));
    // define grabber id from command line arg
    bool argsFound = false;
    size_t idLocalGrabber = 0;
    for(int ii = 0; ii < argc; ++ii){
        std::string arg = argv[ii];
        if(arg.starts_with("-i")){
            argsFound = true;
            idLocalGrabber = std::stoi(arg.substr(2,1));
        }
    }
    if(!argsFound){
        Logger::warning("No id argument found. ID set to '0'\n");
    }
    Logger::message(std::format("ID local grabber: {}\n", idLocalGrabber));

    // init controller
    DCGController controller;
    if(controller.initialize(idLocalGrabber)){
        controller.start();
        Logger::message("Exit Depth camera grabber.\n");
        return 0;
    }

    return -1;
}
