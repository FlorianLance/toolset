
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

#include "dc_device_impl.hpp"

// std
#include <execution>
#include <iostream>

// opencv
#include <opencv2/imgproc.hpp>


#include "thirdparty/taskflow/taskflow.hpp"
// #include <taskflow/algorithm/for_each.hpp>
// #include <taskflow/algorithm/sort.hpp>

// local
#include "utility/benchmark.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"


using namespace tool::geo;
using namespace tool::cam;

auto DCDeviceImpl::initialize(const DCConfigSettings &newConfigS) -> void{

    settings.config = newConfigS;

    mInfos.initialize(settings.config.mode);
    fIndices.initialize(mInfos);
    fData.reset(mInfos);
    initialize_device_specific();
}

auto DCDeviceImpl::start_reading_thread() -> void{

    update_from_colors_settings();

    // start thread
    Logger::message("[DCDeviceImpl::start_reading] Start reading frames.\n");
    frameReaderT = std::make_unique<std::thread>(&DCDeviceImpl::read_frames, this);
}

struct TimeM{

    auto start(std::string_view id) -> void{
        auto sTime = tool::Time::nanoseconds_since_epoch();
        if(!times.contains(id)){
            locker.lock();
            times[id] = {sTime, std::chrono::nanoseconds{0}};
            locker.unlock();
        }else{
            times[id] = {sTime, std::chrono::nanoseconds{0}};
        }
    }

    auto end(std::string_view id) -> void{
        auto eTime = tool::Time::nanoseconds_since_epoch();
        if(times.contains(id)){
            std::get<1>(times[id]) = eTime;
        }
    }

    auto display() -> void{

        using namespace std::chrono;

        std::vector<std::pair<std::string_view, microseconds>> durations;
        locker.lock();
        for(const auto &time : times){
            durations.push_back({time.first, tool::Time::difference_micro_s(std::get<0>(time.second), std::get<1>(time.second))});
        }
        locker.unlock();

        std::sort(std::begin(durations), std::end(durations), [&](const std::pair<std::string_view, microseconds> &c1,  const std::pair<std::string_view, microseconds> &c2){
            return c1.second > c2.second;
        });

        std::cout << "DURATIONS:\n";
        for(const auto &duration : durations){
            if(duration.second.count() > 0){
                std::cout << std::format("[{}:{}] ", duration.first, duration.second.count());
            }
        }
        std::cout << "\n";

    }

    tool::s_umap<std::string_view, std::tuple<std::chrono::nanoseconds,std::chrono::nanoseconds>> times;
    std::mutex locker;
};

struct TimeF{

    TimeF(TimeM &t, std::string_view id) : m_id(id){
        m_t = &t;
        m_t->start(m_id);
    }
    ~TimeF(){
        m_t->end(m_id);
    }

    std::string_view m_id;
    TimeM *m_t = nullptr;
};

auto DCDeviceImpl::read_frames() -> void {


    // auto dev = dc_get_device(settings.config.mode);

    // start loop
    readFramesFromCameras = true;
    fTiming.reset();

    read_calibration();

    if(false)
    {
        TimeM timeM;


        tf::Executor executor(1);

        tf::Taskflow mainTaskFlow;
        std::unique_ptr<DCFrame> nFrame = nullptr;
        std::unique_ptr<DCCompressedFrame> nCompressedFrame = nullptr;

        tf::Task updateSettingsT =mainTaskFlow.emplace([&](){
           parametersM.lock();
           // update current settings
           cFiltersS = settings.filters;
           cDataS    = settings.data;
           cDelayS   = settings.delay;
           // reset timestamps
           fTiming.swap_local_timestamps();
           parametersM.unlock();

           update_from_data_settings();

        }).name("update_settings");

        tf::Task captureCT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "C"sv);
            auto ret = capture_frame(mInfos.timeout_ms()) ? 1 : 0;
            fTiming.update_local("after_capture"sv);
            return ret;
        }).name("capture_frame");
        tf::Task captureFailureT = mainTaskFlow.emplace([&](){}).name("capture_failure");
        tf::Task captureSuccessT = mainTaskFlow.emplace([&](){
            timeM.start("PRC");
        }).name("capture_success");

        tf::Task readAudioT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RA"sv);
            read_audio(cDataS.capture_audio() && mInfos.has_audio());
        }).name("read_audio");
        readAudioT.priority(tf::TaskPriority::LOW);

        tf::Task readImuT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RI"sv);
            read_IMU(cDataS.capture_imu());
        }).name("read_imu");
        readImuT.priority(tf::TaskPriority::LOW);

        tf::Task readBodiesT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RB"sv);
            read_body_tracking(cDataS.capture_body_tracking() && settings.config.btEnabled && mInfos.has_depth()); // TODO add other bodi
        }).name("read_bodies");
        readBodiesT.priority(tf::TaskPriority::LOW);

        tf::Task readColorImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RCI"sv);
            read_color_image(mInfos.has_color() && cDataS.capture_color());
        }).name("read_color_image");        

        tf::Task readDepthImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RDI"sv);
            read_depth_image(mInfos.has_depth() && cDataS.capture_depth());
        }).name("read_depth_image");

        tf::Task readInfraImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RIM"sv);
            read_infra_image(mInfos.has_infra() && cDataS.capture_infra());
        }).name("read_infra_image");

        tf::Task readDataCheckCT  = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RDC"sv);
            return check_data_validity() ? 1 : 0;
        }).name("read_data_check");
        tf::Task readDataFailureT = mainTaskFlow.emplace([&](){}).name("read_data_failure");
        tf::Task readDataSuccessT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RDS"sv);
            nCompressedFrame = std::make_unique<DCCompressedFrame>();
            nFrame           = std::make_unique<DCFrame>();
            update_infos(nFrame.get());
        }).name("read_data_success");

        tf::Task convertColorImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "CCI"sv);
            convert_color_image();
            fTiming.update_local("after_color_convert"sv);
        }).name("convert_color_image");

        tf::Task resizeColorImageToDepthSizeT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RCIDS"sv);
            resize_color_image_to_depth_size();
            fTiming.update_local("after_color_resize"sv);
        }).name("resize_color_image_to_depth_size");

        tf::Task generateCloudT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "GC"sv);
            generate_cloud(true);
            fTiming.update_local("after_cloud_generation"sv);
        }).name("generate_cloud");

        tf::Task filterDepthImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FDI"sv);
            filter_depth_image();
            fTiming.update_local("after_depth_filter"sv);
        }).name("filter_depth_image");

        tf::Task filterDepthSizedColorImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FDSCI"sv);
            filter_depth_sized_color_image();
            fTiming.update_local("after_color_filter"sv);
        }).name("filter_depth_sized_color_image");
        filterDepthSizedColorImageT.priority(tf::TaskPriority::LOW);

        tf::Task filterInfraImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FII"sv);
            filter_infrared_image();
            fTiming.update_local("after_infrared_filter"sv);
        }).name("filter_infrared_image");
        filterInfraImageT.priority(tf::TaskPriority::LOW);

        tf::Task filterCloudImage = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FCI"sv);
            filter_cloud_image();
            fTiming.update_local("after_cloud_filtering"sv);
        }).name("filter_cloud_image");

        tf::Task updateCompressedFrameColor = mainTaskFlow.emplace([&](){
            update_compressed_frame_color(nCompressedFrame.get());
        }).name("update_compressed_frame_color");
        tf::Task updateCompressedFrameDepthSizedColor = mainTaskFlow.emplace([&](){
            update_compressed_frame_depth_sized_color(nCompressedFrame.get());
        }).name("update_compressed_frame_depth_sized_color");
        tf::Task updateCompressedFrameDepth = mainTaskFlow.emplace([&](){
            update_compressed_frame_infos(nCompressedFrame.get());
            update_compressed_frame_depth(nCompressedFrame.get());
        }).name("update_compressed_frame_depth");
        tf::Task updateCompressedFrameInfra = mainTaskFlow.emplace([&](){
            update_compressed_frame_infra(nCompressedFrame.get());
        }).name("update_compressed_frame_infra");
        tf::Task updateCompressedFrameCloud = mainTaskFlow.emplace([&](){
            update_compressed_frame_cloud(nCompressedFrame.get());
        }).name("update_compressed_frame_cloud");
        tf::Task updateCompressedFrameAudio = mainTaskFlow.emplace([&](){
            update_compressed_frame_audio(nCompressedFrame.get());
        }).name("update_compressed_frame_audio");
        tf::Task updateCompressedFrameIMU = mainTaskFlow.emplace([&](){
            update_compressed_frame_imu(nCompressedFrame.get());
        }).name("update_compressed_frame_imu");
        tf::Task updateCompressedFrameBodies = mainTaskFlow.emplace([&](){
            update_compressed_frame_bodies(nCompressedFrame.get());
        }).name("update_compressed_frame_bodies");
        tf::Task updateCompressedFrameCalibration = mainTaskFlow.emplace([&](){
            update_compressed_frame_calibration(nCompressedFrame.get());
        }).name("update_compressed_frame_calibration");

        tf::Task finalizeCompressionT = mainTaskFlow.emplace([&](){
            frames.add_compressed_frame(std::move(nCompressedFrame));
            nCompressedFrame = nullptr;
            fTiming.update_local("after_compressing"sv);
        }).name("finalize_compression");

        tf::Task sendCompressedFrameT = mainTaskFlow.emplace([&](){

            // send with delay
            if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
                fTiming.update_local("after_compressed_frame_sending"sv);
            }
        }).name("send_compressed_frame");;


        tf::Task updateFrameColor = mainTaskFlow.emplace([&](){
            update_color(nFrame.get());
        }).name("update_frame_color");
        updateFrameColor.priority(tf::TaskPriority::NORMAL);
        tf::Task updateFrameDepth = mainTaskFlow.emplace([&](){
            update_depth(nFrame.get());
        }).name("update_depth");
        updateFrameDepth.priority(tf::TaskPriority::NORMAL);
        tf::Task updateFrameInfra = mainTaskFlow.emplace([&](){
            update_infra(nFrame.get());
        }).name("update_infra");
        tf::Task updateFrameDepthSizedColor = mainTaskFlow.emplace([&](){
            update_depth_sized_color(nFrame.get());
        }).name("update_depth_sized_color");
        // tf::Task updateFrameCloud = mainTaskFlow.emplace([&](){
        //     update_cloud(localData, nFrame.get());
        // }).name("update_cloud");
        tf::Task updateFrameCalibration = mainTaskFlow.emplace([&](){
            update_calibration(nFrame.get());
        }).name("update_calibration");
        tf::Task updateFrameAudio = mainTaskFlow.emplace([&](){
            update_audio(nFrame.get());
        }).name("update_audio");
        tf::Task updateFrameBodies = mainTaskFlow.emplace([&](){
            update_bodies(nFrame.get());
        }).name("update_bodies");
        tf::Task updateFrameIMU = mainTaskFlow.emplace([&](){
            update_imu(nFrame.get());
        }).name("update_imu");


        tf::Task finalizeLocalT = mainTaskFlow.emplace([&](){
            frames.add_frame(std::move(nFrame));
            nFrame = nullptr;
            fTiming.update_local("after_local_frame_creation"sv);
        }).name("finalize_local");
        tf::Task sendFrameT = mainTaskFlow.emplace([&](){
            if(auto frameToSend = frames.take_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_frame_signal(std::move(frameToSend));
                fTiming.update_local("after_frame_sending"sv);
            }
        }).name("send_frame");;


        tf::Task endTask = mainTaskFlow.emplace([&](){
            timeM.end("PRC");
        }).name("end");


        updateSettingsT.precede(captureCT);
        captureCT.precede(captureFailureT, captureSuccessT);
        captureSuccessT.precede(
            readColorImageT, readDepthImageT, readInfraImageT,
            readAudioT, readImuT, readBodiesT
        );
        readDataCheckCT.succeed(
            readColorImageT, readDepthImageT, readInfraImageT,
            readAudioT, readImuT, readBodiesT
        );
        readDataCheckCT.precede(readDataFailureT, readDataSuccessT);
        readDataSuccessT.precede(
            convertColorImageT, generateCloudT,
            // compressed
            updateCompressedFrameCalibration, updateCompressedFrameAudio,
            updateCompressedFrameIMU, updateCompressedFrameBodies,
            // local
            updateFrameCalibration,
            updateFrameAudio,
            updateFrameIMU,
            updateFrameBodies
        );
        convertColorImageT.precede(resizeColorImageToDepthSizeT);
        filterDepthImageT.succeed(resizeColorImageToDepthSizeT);

        filterDepthImageT.precede(filterDepthSizedColorImageT, filterInfraImageT);
        filterCloudImage.succeed(generateCloudT, filterDepthImageT);

        updateCompressedFrameColor.succeed(convertColorImageT);
        updateCompressedFrameDepthSizedColor.succeed(filterDepthSizedColorImageT);
        updateCompressedFrameDepth.succeed(filterCloudImage);
        updateCompressedFrameInfra.succeed(filterInfraImageT);
        updateCompressedFrameCloud.succeed(filterCloudImage);

        finalizeCompressionT.succeed(
            updateCompressedFrameColor, updateCompressedFrameDepthSizedColor, updateCompressedFrameDepth,
            updateCompressedFrameInfra,  updateCompressedFrameCloud, updateCompressedFrameCalibration,
            updateCompressedFrameAudio, updateCompressedFrameIMU, updateCompressedFrameBodies
        );
        sendCompressedFrameT.succeed(finalizeCompressionT);

        updateFrameColor.succeed(convertColorImageT);
        updateFrameDepth.succeed(filterCloudImage);
        updateFrameInfra.succeed(filterInfraImageT);
        updateFrameDepthSizedColor.succeed(filterDepthSizedColorImageT);

        finalizeLocalT.succeed(
            updateFrameCalibration, updateFrameAudio, updateFrameIMU, updateFrameBodies,
            updateFrameColor,  updateFrameDepth, updateFrameInfra, updateFrameDepthSizedColor
            // updateFrameCloud,
        );
        sendFrameT.succeed(finalizeLocalT);

        endTask.succeed(sendCompressedFrameT, sendFrameT);

        mainTaskFlow.dump(std::cout);


        while(readFramesFromCameras){
            // auto tStart = Time::nanoseconds_since_epoch();
            executor.run(mainTaskFlow).wait();

            mInfos.increment_capture_id();
            // timeM.display();
            // auto tTotal = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
            // std::cout << "total: " << tTotal.count() << "\n";
        }

        return;
    }

    // auto t1 = Time::milliseconds_since_epoch();

    while(readFramesFromCameras){

        parametersM.lock();
        // update current settings
        cFiltersS = settings.filters;
        cDataS    = settings.data;
        cDelayS   = settings.delay;
        // reset timestamps
        fTiming.swap_local_timestamps();
        parametersM.unlock();

        update_from_data_settings();

        // capture frame
        try {

            fTiming.update_local("before_capture"sv);
            bool frameSuccess = capture_frame(mInfos.timeout_ms());
            fTiming.update_local("after_capture"sv);

            if(!frameSuccess){ // likely timeout
                // std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }

        }   catch (std::exception &error) {
            Logger::error(std::format("[DCDeviceData::read_frames] Get capture error: {}\n", error.what()));
            readFramesFromCameras = false;
            break;
        }

        // read data
        read_audio(cDataS.capture_audio() && mInfos.has_audio());
        read_IMU(cDataS.capture_imu());
        read_body_tracking(cDataS.capture_body_tracking() && settings.config.btEnabled && mInfos.has_depth());

        read_color_image(mInfos.has_color() && cDataS.capture_color());
        read_depth_image(mInfos.has_depth() && cDataS.capture_depth());
        read_infra_image(mInfos.has_infra() && cDataS.capture_infra());

        fTiming.update_local("after_get_images"sv);

        if(!check_data_validity()){
            continue;
        }

        fTiming.compute_capture_framerate();

        // process
        convert_color_image();
        fTiming.update_local("after_color_convert"sv);
        
        resize_color_image_to_depth_size();
        fTiming.update_local("after_color_resize"sv);

        filter_depth_image();
        fTiming.update_local("after_depth_filter"sv);

        filter_depth_sized_color_image();
        fTiming.update_local("after_color_filter"sv);

        filter_infrared_image();
        fTiming.update_local("after_infrared_filter"sv);

        generate_cloud(true);//cDataS.generate_cloud_from_client() || cFiltersS.p1FMode != DCFiltersSettings::PlaneFilteringMode::None);
        fTiming.update_local("after_cloud_generation"sv);

        filter_cloud_image(); // TODO !!!!
        fTiming.update_local("after_cloud_filtering"sv);

        fTiming.update_local("after_processing"sv);

        if(!readFramesFromCameras){
            break;
        }

        // compressed frame
        if(sendData && cDataS.client.compression.has_data_to_compress()){

            // generate
            fTiming.update_local("before_compressing"sv);
            auto compressedFrame = compress_frame();
            frames.add_compressed_frame(std::move(compressedFrame));
            fTiming.update_local("after_compressing"sv);

            // send with delay
            if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
                fTiming.update_local("after_compressed_frame_sending"sv);
            }
        }

        // local frame
        if(cDataS.client.generation.has_data_to_generate()){

            // generate
            fTiming.update_local("before_local_frame_creation"sv);
            auto dFrame = create_local_frame();
            frames.add_frame(std::move(dFrame));
            fTiming.update_local("after_local_frame_creation"sv);

            // send with delay
            if(auto frameToSend = frames.take_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_frame_signal(std::move(frameToSend));
                fTiming.update_local("after_frame_sending"sv);
            }
        }

        mInfos.increment_capture_id();
    }
}

auto DCDeviceImpl::convert_color_image() -> void{

    fData.color = {};

    if(!mInfos.has_color() || fData.rawColor.empty()){
        return;
    }

    if(mInfos.image_format() == DCImageFormat::NV12){

        Bench::start("[DCDeviceImpl::convert_color_image] NV12");

        cv::Mat yuvImage(static_cast<int>(mInfos.color_height() * 3 / 2), static_cast<int>(mInfos.color_width()), CV_8UC1);
        std::copy(fData.rawColor.data(), fData.rawColor.data() + mInfos.color_size(),  reinterpret_cast<std::byte*>(yuvImage.data)); // copy Y plane
        std::copy(fData.rawColor.data() + mInfos.color_size(), fData.rawColor.data() + mInfos.color_size() + mInfos.color_size()/2, reinterpret_cast<std::byte*>(yuvImage.data) + mInfos.color_size());

        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data());
        cv::cvtColor(yuvImage, convMat, cv::COLOR_YUV2RGBA_NV12);

        Bench::stop();

    }else if(mInfos.image_format() == DCImageFormat::MJPG){

        Bench::start("[DCDeviceImpl::convert_color_image] MJPG");

        auto success = jpegColorDecoder.decode(
            mInfos.color_width(), mInfos.color_height(), fData.rawColor,
            fData.convertedColorData
        );

        Bench::stop();

        if(!success){
            Logger::error("DCDeviceImpl::convert_color_image: Uncompression error.\n");
            return;
        }
    }else if(mInfos.image_format() == DCImageFormat::BGRA){

        Bench::start("[DCDeviceImpl::convert_color_image] BGRA");

        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_BGRA2RGBA);

        Bench::stop();

    }else if(mInfos.image_format() == DCImageFormat::YUY2 ){

        //YUY2 is a 4:2:2 format, so there are 4 bytes per 'chunk' of data, and each 'chunk' represents 2 pixels.
        Bench::start("[DCDeviceImpl::convert_color_image] YUY2");

        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC2, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_YUV2RGBA_YUY2);

        Bench::stop();

    }else{
        Logger::error("DCDeviceImpl::convert_color_image: Invalid color format.\n");
        return;
    }

    fData.color = fData.convertedColorData.span();
}

auto DCDeviceImpl::filter_depth_image() -> void{

    if(fData.depth.empty()){
        return;
    }
    if(!fData.dephtSizedColor.empty() && (fData.dephtSizedColor.size() != fData.depth.size())){
        return;
    }
    if(!fData.infra.empty() && (fData.infra.size() != fData.depth.size())){
        return;
    }

    Bench::start("[DCDeviceData::filter_depth_image]");

    auto dRange = mInfos.depth_range_mm();
    auto minW   = mInfos.depth_width()  * cFiltersS.minWidthF;
    auto maxW   = mInfos.depth_width()  * cFiltersS.maxWidthF;
    auto minH   = mInfos.depth_height() * cFiltersS.minHeightF;
    auto maxH   = mInfos.depth_height() * cFiltersS.maxHeightF;

    auto diffRange = dRange.y()-dRange.x();
    auto minD = dRange.x() + cFiltersS.minDepthF * diffRange;
    auto maxD = dRange.x() + cFiltersS.maxDepthF * diffRange;
    auto hsvDiffColor = Convert::to_hsv(cFiltersS.filterColor);

    // reset depth mask
    std::fill(fData.depthMask.begin(), fData.depthMask.end(), 1);

    // depth/width/height/mask/color/infra filtering
    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const Pt3<size_t> &dIndex){

        size_t id = dIndex.x();
        size_t ii = dIndex.y();
        size_t jj = dIndex.z();

        const auto &currentDepth = fData.depth[id];

        // check validity
        if(currentDepth == dc_invalid_depth_value){
            fData.depthMask[id] = 0;
            return;
        }

        // depth filtering
        if( (ii < minW)  || (ii > maxW)  ||   // width
            (jj < minH)  || (jj > maxH) ||   // height
            (currentDepth < minD) || (currentDepth > maxD) ){           // depth
            fData.depthMask[id] = 0;
            return;
        }

        // color filtering
        if(!fData.dephtSizedColor.empty() && cFiltersS.filterDepthWithColor){

            auto hsv = Convert::to_hsv(fData.dephtSizedColor[id]);
            if((std::abs(hsv.h()- hsvDiffColor.h()) > cFiltersS.maxDiffColor.x()) ||
                (std::abs(hsv.s()- hsvDiffColor.s()) > cFiltersS.maxDiffColor.y()) ||
                (std::abs(hsv.v()- hsvDiffColor.v()) > cFiltersS.maxDiffColor.z())){
                fData.depthMask[id] = 0;
                return;
            }
        }

        // infrared filtering
        if(!fData.infra.empty() && cFiltersS.filterDepthWithInfra){
            // ...
        }

        // body tracking filtering
        if(!fData.bodiesIdMap.empty() && cFiltersS.filterDepthWithBodyTracking){
            // ...
        }

        // NFOV sans compartimentation 	640 x 576 	75°x°65 	0, 5, 15, 30 	0,5 - 3,86 m 	12,8 ms
        // NFOV 2x2 avec compartimentation (SW) 	320 x 288 	75°x°65 	0, 5, 15, 30 	0,5 - 5,46 m 	12,8 ms
        // WFOV 2x2 avec compartimentation 	512 x 512 	120°x°120 	0, 5, 15, 30 	0,25 - 2,88 m 	12,8 ms
        // WFOV sans compartimentation 	1024 x 1024 	120°x°120 	0, 5, 15 	0,25 - 2,21 m 	20,3 ms
        // Infrarouge passif 	1024 x 1024 	N/A 	0, 5, 15, 30 	N/A 	1,6 ms

        float vfow = 50.f;
        // deg_2_rad()

//         def is_point_in_fov(camera_position, camera_orientation, vfov, hfov, point):
// # Calculate direction vectors
//             top_left = np.array([np.tan(hfov/2), np.tan(vfov/2), -1])
//                        top_right = np.array([-np.tan(hfov/2), np.tan(vfov/2), -1])
//               bottom_left = np.array([np.tan(hfov/2), -np.tan(vfov/2), -1])
//               bottom_right = np.array([-np.tan(hfov/2), -np.tan(vfov/2), -1])

// # Transform direction vectors
//               rotation_matrix = get_rotation_matrix(camera_orientation)
//             top_left = np.dot(rotation_matrix, top_left)
//               top_right = np.dot(rotation_matrix, top_right)
//               bottom_left = np.dot(rotation_matrix, bottom_left)
//               bottom_right = np.dot(rotation_matrix, bottom_right)

// # Calculate dot products
//               dot_top_left = np.dot(top_left, point - camera_position)
//               dot_top_right = np.dot(top_right, point - camera_position)
//               dot_bottom_left = np.dot(bottom_left, point - camera_position)
//               dot_bottom_right = np.dot(bottom_right, point - camera_position)

// # Check if point is within FOV
//                   if dot_top_left > 0 and dot_top_right > 0 and dot_bottom_left > 0 and dot_bottom_right > 0:
//                                                                                        return True
//                                                                                        else:
//                                                                                               return False


    });

    if(cFiltersS.doLocalDiffFiltering){
        maximum_local_depth_difference(fIndices, fData.depth, cFiltersS.maxLocalDiff, DCConnectivity::Connectivity_2H);
    }

    // minimum neighbours filtering
    if(cFiltersS.doMinNeighboursFiltering){
        mininum_neighbours(cFiltersS.minNeighboursLoops, cFiltersS.nbMinNeighbours, DCConnectivity::Connectivity_4);
    }

    // erosion
    if(cFiltersS.doErosion){
        erode(cFiltersS.erosionLoops, DCConnectivity::Connectivity_8);
    }

    // keep only biggest cluster
    if(cFiltersS.keepOnlyBiggestCluster){
        keep_only_biggest_cluster();
    }else{
        fData.meanBiggestZoneId = 0;
    }


    if(cFiltersS.removeAfterClosestPoint){

        std::uint16_t minDist = std::numeric_limits<std::uint16_t>::max();
        std::for_each(std::execution::unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
            if(fData.depthMask[id] != 0){
                if(minDist > fData.depth[id]){
                    minDist = fData.depth[id];
                }
            }
        });

        std::uint16_t maxDist = minDist + static_cast<std::uint16_t>(1000.f * cFiltersS.maxDistanceAfterClosestPoint);
        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
            if(fData.depthMask[id] == 0){
                return;
            }

            if(fData.depth[id] > maxDist){
                fData.depthMask[id] = 0;
            }
        });
    }


    update_valid_depth_values();

    // for(auto &d : fData.depth){

    //     // fData.depthMask
    //     if(d != dc_invalid_depth_value){
    //         d = 3000;
    //     }else{
    //         d = 0;
    //     }

    // }

    Bench::stop();

}

auto DCDeviceImpl::filter_depth_sized_color_image() -> void{

    if(fData.dephtSizedColor.empty()){
        return;
    }

    if(!fData.depth.empty() && (fData.depth.size() != fData.dephtSizedColor.size())){
        return;
    }

    if(!fData.infra.empty() && (fData.infra.size() != fData.dephtSizedColor.size())){
        return;
    }

    bool writeBodies = (fData.bodiesIdMap.size() == fData.dephtSizedColor.size()) && true; // TODO

    Bench::start("[DCDeviceData::filter_color_image]");

    if(!fData.depth.empty()){

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
            if(cFiltersS.invalidateColorFromDepth){
                if(fData.depth[id] == dc_invalid_depth_value){
                    fData.dephtSizedColor[id] = ColorRGBA8{dc_invalid_color_value};
                }else{
                    if(cFiltersS.keepOnlyBiggestCluster){
                        // colorBuffer[meanBiggestZoneId] = ColorRGBA8{255,0,0,255};// TODO ?
                    }                                       
                }                                
            }

            if(writeBodies){

                if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    fData.dephtSizedColor[id] = ColorRGBA8{255,0,0,255};
                }
            }
        });
    }

    Bench::stop();
}

auto DCDeviceImpl::filter_infrared_image() -> void{

    if(fData.infra.empty()){
        return;
    }

    if(!fData.dephtSizedColor.empty() && (fData.dephtSizedColor.size() != fData.infra.size())){
        return;
    }

    if(!fData.depth.empty() && (fData.depth.size() != fData.infra.size())){
        return;
    }

    bool writeBodies = (fData.bodiesIdMap.size() == fData.infra.size()) && true; // TODO

    Bench::start("[DCDeviceData::filter_infrared_image]");
    if(!fData.depth.empty()){
        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
            if(cFiltersS.invalidateInfraFromDepth){
                if(fData.depth[id] == dc_invalid_depth_value){
                    fData.infra[id] = dc_invalid_infra_value;
                }
            }

            if(writeBodies){
                if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    fData.infra[id] = 16000;
                }
            }
        });
    }
    Bench::stop();
}

auto DCDeviceImpl::filter_cloud_image() -> void{

    if(fData.depthCloud.empty() || fData.depth.empty() || !cFiltersS.filterDepthWithCloud){
        return;
    }

    if(cFiltersS.p1FMode != PlaneFilteringMode::None){

        auto p1             = cFiltersS.p1A*1000.f;
        auto p2             = cFiltersS.p1B*1000.f;
        auto p3             = cFiltersS.p1C*1000.f;
        geo::Pt3f meanPt    = (p1+p2+p3)/3.f;
        auto AB             = vec(p2,p1);
        auto AC             = vec(p3,p1);
        auto normalV        = cross(AB,AC);
        normalV             = normalize(normalV);

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depthVertexCorrrespondance), std::end(fIndices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(fData.depth[idD] == dc_invalid_depth_value){
                return;
            }

            if(dot(normalV,vec(meanPt, fData.depthCloud[idD].template conv<float>())) < 0){
                if(cFiltersS.p1FMode == PlaneFilteringMode::Above){
                    fData.depthMask[idD] = 0;
                    return;
                }
            }else{
                if(cFiltersS.p1FMode == PlaneFilteringMode::Below){
                    fData.depthMask[idD] = 0;
                    return;
                }
            }
        });
    }

    update_valid_depth_values();
}

auto DCDeviceImpl::update_valid_depth_values() -> void{

    // count valid depth values
    fData.validDepthValues = 0;
    std::for_each(std::execution::unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
        if(fData.depthMask[id] == 0){
            fData.depth[id] = dc_invalid_depth_value;
            fIndices.depthVertexCorrrespondance[id] = {id, -1};
        }else{
            fIndices.depthVertexCorrrespondance[id] = {id, fData.validDepthValues};
            fData.validDepthValues++;
        }
    });
}

auto DCDeviceImpl::create_local_frame() -> std::unique_ptr<DCFrame>{

    tool::Bench::start("[DCDeviceImpl::create_local_frame]");

    auto dFrame  = std::make_unique<DCFrame>();
    update_infos(dFrame.get());
    update_color(dFrame.get());
    update_depth_sized_color(dFrame.get());
    update_depth(dFrame.get());
    update_infra(dFrame.get());
    update_cloud(dFrame.get());
    update_bodies(dFrame.get());
    update_calibration(dFrame.get());
    update_imu(dFrame.get());
    update_audio(dFrame.get());

    tool::Bench::stop();

    return dFrame;
}

auto DCDeviceImpl::update_infos(DCFrame *dFrame) -> void{
    dFrame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
    dFrame->afterCaptureTS  = fTiming.get_local("after_capture"sv).count();
    dFrame->receivedTS      = dFrame->afterCaptureTS;  // default init received TS with after capture TS
    dFrame->mode            = settings.config.mode;
}

auto DCDeviceImpl::update_color(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_color]");

    if(!fData.color.empty() && cDataS.client.generation.colorImage){
        dFrame->rgbaColor.width  = mInfos.color_width();
        dFrame->rgbaColor.height = mInfos.color_height();
        dFrame->rgbaColor.resize(mInfos.color_size());
        std::copy(fData.color.begin(), fData.color.end(), dFrame->rgbaColor.begin());
    }else{
        dFrame->rgbaColor.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_depth_sized_color(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_depth_sized_color]");

    if(!fData.dephtSizedColor.empty() && cDataS.client.generation.depthSizedColorImage){
        dFrame->rgbaDepthSizedColor.width  = mInfos.depth_width();
        dFrame->rgbaDepthSizedColor.height = mInfos.depth_height();
        dFrame->rgbaDepthSizedColor.resize(mInfos.depth_size());
        std::copy(fData.dephtSizedColor.begin(), fData.dephtSizedColor.end(), dFrame->rgbaDepthSizedColor.begin());
    }else{
        dFrame->rgbaDepthSizedColor.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_depth(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_depth]");

    if(!fData.depth.empty() && cDataS.client.generation.depth){

        dFrame->depth.width  = mInfos.depth_width();
        dFrame->depth.height = mInfos.depth_height();
        dFrame->depth.resize(mInfos.depth_size());
        std::copy(fData.depth.begin(), fData.depth.end(), dFrame->depth.begin());

        if(cDataS.client.generation.depthImage){

            const auto dRange = mInfos.depth_range_mm();
            const auto diff   = dRange.y() - dRange.x();
            static constexpr std::array<Pt3f,5> depthGradient ={
                Pt3f{0.f,0.f,1.f},
                {0.f,1.f,1.f},
                {0.f,1.f,0.f},
                {1.f,1.f,0.f},
                {1.f,0.f,0.f},
            };

            dFrame->rgbDepth.width  = mInfos.depth_width();
            dFrame->rgbDepth.height = mInfos.depth_height();
            dFrame->rgbDepth.resize(mInfos.depth_size());

            std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){

                if(fData.depth[id] == dc_invalid_depth_value){
                    dFrame->rgbDepth[id] = ColorRGB8{};
                    return;
                }

                float vF = (static_cast<float>(fData.depth[id]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);

                auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                dFrame->rgbDepth[id] = ColorRGB8{
                    static_cast<std::uint8_t>(255*col.x()),
                    static_cast<std::uint8_t>(255*col.y()),
                    static_cast<std::uint8_t>(255*col.z())
                };
            });
        }else{
            dFrame->rgbDepth.reset();
        }

    }else{
        dFrame->depth.reset();
        dFrame->rgbDepth.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_infra(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_infra]");

    if(!fData.infra.empty() && cDataS.client.generation.infra){

        dFrame->infra.width  = mInfos.infra_width();
        dFrame->infra.height = mInfos.infra_height();
        dFrame->infra.resize(mInfos.infra_size());
        std::copy(fData.infra.begin(), fData.infra.end(), dFrame->infra.begin());

        if(cDataS.client.generation.infraImage){

            dFrame->rgbInfra.width  = mInfos.infra_width();
            dFrame->rgbInfra.height = mInfos.infra_height();
            dFrame->rgbInfra.resize(mInfos.infra_size());

            const float max = 2000;
            std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){

                float vF = static_cast<float>(fData.infra[id]);
                if(vF > max){
                    vF = max;
                }
                vF/=max;
                dFrame->rgbInfra[id] = ColorRGB8{
                    static_cast<std::uint8_t>(255*vF),
                    static_cast<std::uint8_t>(255*vF),
                    static_cast<std::uint8_t>(255*vF)
                };
            });
        }else{
            dFrame->rgbInfra.reset();
        }

    }else{
        dFrame->infra.reset();
        dFrame->rgbInfra.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_cloud(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_cloud]");

    if(!fData.depth.empty() && !fData.depthCloud.empty() && cDataS.client.generation.cloud){

        dFrame->cloud.vertices.resize(fData.validDepthValues);
        dFrame->cloud.colors.resize(fData.validDepthValues);
        dFrame->cloud.normals.resize(fData.validDepthValues);

        bool addColors = !fData.dephtSizedColor.empty() && cDataS.client.generation.cloudColorMode == CloudColorMode::FromDepthSizedColorImage;
        const auto dRange = mInfos.depth_range_mm();
        const auto diff   = dRange.y() - dRange.x();
        static constexpr std::array<Pt3f,5> depthGradient ={
            Pt3f{0.f,0.f,1.f},
            {0.f,1.f,1.f},
            {0.f,1.f,0.f},
            {1.f,1.f,0.f},
            {1.f,0.f,0.f},
        };

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depthVertexCorrrespondance), std::end(fIndices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(fData.depth[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);
            dFrame->cloud.vertices[idV] = fData.depthCloud[idD].template conv<float>()*0.001f;

            if(addColors){
                dFrame->cloud.colors[idV] = geo::Pt3f{
                    static_cast<float>(fData.dephtSizedColor[idD].r()),
                    static_cast<float>(fData.dephtSizedColor[idD].g()),
                    static_cast<float>(fData.dephtSizedColor[idD].b())
                }/255.f;
            }else{

                float vF = (static_cast<float>(fData.depth[idD]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                dFrame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            }

            // A B C
            // D I E
            // F G H
            const auto &idN   = fIndices.neighbours8Depth1D[idD];
            const auto &idDVC = fIndices.depthVertexCorrrespondance;
            const auto &v     = dFrame->cloud.vertices;
            Vec3f normal{};

            const auto &vId = v[idV];
            if(idN[0] != -1 && std::get<1>(idDVC[idN[0]]) != -1){
                if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vId x vIA
                    normal += cross(v[std::get<1>(idDVC[idN[3]])] - vId, v[std::get<1>(idDVC[idN[0]])] - vId);
                }
                if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIA x vIB
                    normal += cross(v[std::get<1>(idDVC[idN[0]])] - vId, v[std::get<1>(idDVC[idN[1]])] - vId);
                }
            }
            if(idN[2] != -1 && std::get<1>(idDVC[idN[2]]) != -1){
                if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIB x vIC
                    normal += cross(v[std::get<1>(idDVC[idN[1]])] - vId, v[std::get<1>(idDVC[idN[2]])] - vId);
                }
                if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIC x vIE
                    normal += cross(v[std::get<1>(idDVC[idN[2]])] - vId, v[std::get<1>(idDVC[idN[4]])] - vId);
                }
            }
            if(idN[7] != -1 && std::get<1>(idDVC[idN[7]]) != -1){
                if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIE x vIH
                    normal += cross(v[std::get<1>(idDVC[idN[4]])] - vId, v[std::get<1>(idDVC[idN[7]])] - vId);
                }
                if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIH x vIG
                    normal += cross(v[std::get<1>(idDVC[idN[7]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
                }
            }
            if(idN[5] != -1 && std::get<1>(idDVC[idN[5]]) != -1){
                if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIG x vIF
                    normal += cross(v[std::get<1>(idDVC[idN[5]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
                }
                if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vIF x vID
                    normal += cross(v[std::get<1>(idDVC[idN[6]])] - vId, v[std::get<1>(idDVC[idN[3]])] - vId);
                }
            }
            dFrame->cloud.normals[idV] = normalize(normal);
        });

    }else{
        dFrame->cloud.clear();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_imu(DCFrame *dFrame) -> void{

    if(!fData.binaryIMU.empty() && cDataS.client.generation.imu){
        dFrame->imu.resize(fData.binaryIMU.size());
        std::copy(fData.binaryIMU.begin(), fData.binaryIMU.end(), dFrame->imu.begin());
    }else{
        dFrame->imu.clear();
    }
}

auto DCDeviceImpl::update_bodies(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_bodies]");

    if(!fData.bodies.empty() && cDataS.client.generation.bodyTracking){
        dFrame->bodyTracking.resize(fData.bodies.size());
        std::copy(fData.bodies.begin(), fData.bodies.end(), dFrame->bodyTracking.begin());
    }else{
        dFrame->bodyTracking.clear();
    }


    // bodies id map
    if(!fData.bodiesIdMap.empty() && cDataS.client.generation.bodyIdMapImage){
        dFrame->grayBodiesIdMap.width  = mInfos.depth_width();
        dFrame->grayBodiesIdMap.height = mInfos.depth_height();
        dFrame->grayBodiesIdMap.resize(mInfos.depth_size());
        std::copy(fData.bodiesIdMap.begin(), fData.bodiesIdMap.end(), dFrame->grayBodiesIdMap.begin());
    }else{
        dFrame->grayBodiesIdMap.clear();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_calibration(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_calibration]");

    if(!fData.binaryCalibration.empty() && cDataS.client.generation.calibration){
        dFrame->calibration.resize(fData.binaryCalibration.size());
        std::copy(fData.binaryCalibration.begin(), fData.binaryCalibration.end(), dFrame->calibration.begin());
    }else{
        dFrame->calibration.clear();
    }

    Bench::stop();
}

auto DCDeviceImpl::compress_frame() -> std::unique_ptr<DCCompressedFrame>{

    tool::Bench::start("[DCDeviceImpl::compress_frame]");

    auto cFrame = std::make_unique<DCCompressedFrame>();
    update_compressed_frame_infos(cFrame.get());
    update_compressed_frame_color(cFrame.get());
    update_compressed_frame_depth_sized_color(cFrame.get());
    update_compressed_frame_depth(cFrame.get());
    update_compressed_frame_infra(cFrame.get());
    update_compressed_frame_cloud(cFrame.get());
    update_compressed_frame_audio(cFrame.get());
    update_compressed_frame_imu(cFrame.get());
    update_compressed_frame_bodies(cFrame.get());
    update_compressed_frame_calibration(cFrame.get());

    tool::Bench::stop();

    return cFrame;
}

auto DCDeviceImpl::update_audio(DCFrame *dFrame) -> void{

    Bench::start("[DCDeviceImpl::update_audio]");

    if(cDataS.client.generation.audio && (fData.audioChannels.first != 0) && !fData.audioChannels.second.empty()){
        size_t nbFrames = fData.audioChannels.second.size() / fData.audioChannels.first;
        dFrame->audioFrames.resize(nbFrames);
        auto audioFrom = fData.audioChannels.second.data();
        std::copy(audioFrom, audioFrom + fData.audioChannels.second.size(), reinterpret_cast<float*>(dFrame->audioFrames.data()));
    }else{
        dFrame->audioFrames.clear();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_infos(DCCompressedFrame *cFrame) -> void{
    cFrame->idDevice           = static_cast<std::uint8_t>(settings.config.idDevice);
    cFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());    
    cFrame->afterCaptureTS     = fTiming.get_local("after_capture"sv).count();
    cFrame->receivedTS         = cFrame->afterCaptureTS; // default init received TS with after capture TS
    cFrame->mode               = settings.config.mode;
    cFrame->validVerticesCount = fData.validDepthValues;
}

auto DCDeviceImpl::update_compressed_frame_color(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_color]");
    if(!fData.color.empty() && cDataS.client.compression.color){
        if(!jpegColorEncoder.encode(
                mInfos.color_width(),
                mInfos.color_height(),
                fData.color,
                cFrame->jpegRGBA8Color,
                cDataS.client.compression.jpegCompressionRate
            )){
            cFrame->jpegRGBA8Color.reset();
        }
    }else{
        cFrame->jpegRGBA8Color.reset();
    }
    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_depth_sized_color(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_depth_sized_color]");

    if(!fData.dephtSizedColor.empty() && cDataS.client.compression.depthSizedColor){

        if(!jpegDepthSizedColorEncoder.encode(
                mInfos.depth_width(),
                mInfos.depth_height(),
                fData.dephtSizedColor,
                cFrame->jpegRGBA8DepthSizedColor,
                cDataS.client.compression.jpegCompressionRate
            )){
            cFrame->jpegRGBA8DepthSizedColor.reset();
        }

    }else{
        cFrame->jpegRGBA8DepthSizedColor.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_depth(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_depth]");

    if(!fData.depth.empty() && cDataS.client.compression.depth){

        fastPForDepthEncoder.encode(
            mInfos.depth_width(),
            mInfos.depth_height(),
            fData.depth,
            cFrame->fpfDepth
        );

    }else{
        cFrame->fpfDepth.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_infra(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_infra]");

    if(!fData.infra.empty() && cDataS.client.compression.infra){

        fastPForInfraEncoder.encode(
            mInfos.infra_width(),
            mInfos.infra_height(),
            fData.infra,
            cFrame->fpfInfra
        );

    }else{
        cFrame->fpfInfra.reset();
    }

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_calibration(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_calibration]");

    if(!fData.binaryCalibration.empty() && cDataS.client.compression.calibration){
        cFrame->calibration.resize(fData.binaryCalibration.size());
        std::copy(fData.binaryCalibration.begin(), fData.binaryCalibration.end(), cFrame->calibration.begin());
    }else{
        cFrame->calibration.clear();
    }

    Bench::stop();
}

auto DCDeviceImpl::read_frames2() -> void {

    tf::Executor executor;
    tf::Taskflow mainTF;
    bool doLoop = true;

    auto startLoopT = mainTF.emplace([&](){

    }).name("start_loop");
    auto endLoopT = mainTF.emplace([&](){

    }).name("end_loop");

    auto loopCT = mainTF.emplace([&](){
        return doLoop ? 1 : 0;
    }).name("loop");

    auto startReadingFrameT = mainTF.emplace([&](){

    }).name("start_reading_frame");

    auto endReadingFrameCT = mainTF.emplace([&](){
        return 0;
    }).name("end_reading_frame");


    loopCT.precede(endLoopT, startReadingFrameT);
    endReadingFrameCT.succeed(startReadingFrameT);

    // int i;

    // auto [init, cond, body, back, done] = taskflow.emplace(
    //     [&](){ std::cout << "i=0\n"; i=0; },
    //     [&](){ std::cout << "while i<5\n"; return i < 5 ? 0 : 1; },
    //     [&](){ std::cout << "i++=" << i++ << '\n'; },
    //     [&](){ std::cout << "back\n"; return 0; },
    //     [&](){ std::cout << "done\n"; }
    //     );

    // init.precede(cond);
    // cond.precede(body, done);
    // body.precede(back);
    // back.precede(cond);

}


auto DCDeviceImpl::update_compressed_frame_cloud(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_cloud]");

    if(!fData.depthCloud.empty() && !fData.depth.empty() && cDataS.client.compression.cloud){

        Buffer<std::uint16_t> processedCloudData;

        // resize cloud data
        // data mapping: XXYYZZRGB0
        size_t cloudVerticesBufferSize              = fData.validDepthValues*5;
        size_t rest                                 = cloudVerticesBufferSize % 128;
        size_t paddeCloudVerticesBufferPaddedSize   = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
        if(processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
            processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
        }
        std::fill(processedCloudData.begin(), processedCloudData.end(), 0);

        auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(processedCloudData.get_data());

        bool addColors = !fData.dephtSizedColor.empty() && cDataS.client.compression.cloudColorMode == CloudColorMode::FromDepthSizedColorImage;

        const auto dRange = mInfos.depth_range_mm();
        const auto diff   = dRange.y() - dRange.x();
        static constexpr std::array<Pt3f,5> depthGradient ={
            Pt3f{0.f,0.f,1.f},
            {0.f,1.f,1.f},
            {0.f,1.f,0.f},
            {1.f,1.f,0.f},
            {1.f,0.f,0.f},
        };

        // fill data
        std::for_each(std::execution::par_unseq, std::begin(fIndices.depthVertexCorrrespondance), std::end(fIndices.depthVertexCorrrespondance), [&](const auto &idC){

            auto idD = std::get<0>(idC);
            if(fData.depth[idD] == dc_invalid_depth_value){
                return;
            }
            auto idV = std::get<1>(idC);

            processedCloudData[idV]                             = static_cast<std::uint16_t>(static_cast<std::int32_t>(fData.depthCloud[idV].x())+4096); // X
            processedCloudData[fData.validDepthValues   + idV]  = static_cast<std::uint16_t>(static_cast<std::int32_t>(fData.depthCloud[idV].y())+4096); // Y
            processedCloudData[2*fData.validDepthValues + idV]  = static_cast<std::uint16_t>(fData.depthCloud[idV].z());                                 // Z
            if(addColors){
                processedCloudData8[6*fData.validDepthValues + idV] = fData.dephtSizedColor[idV].r();
                processedCloudData8[7*fData.validDepthValues + idV] = fData.dephtSizedColor[idV].g();
                processedCloudData8[8*fData.validDepthValues + idV] = fData.dephtSizedColor[idV].b();
            }else{
                float vF = (static_cast<float>(fData.depth[idD]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                auto colorF = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                processedCloudData8[6*fData.validDepthValues + idV] = static_cast<std::uint8_t>(255.f * colorF.x());
                processedCloudData8[7*fData.validDepthValues + idV] = static_cast<std::uint8_t>(255.f * colorF.y());
                processedCloudData8[8*fData.validDepthValues + idV] = static_cast<std::uint8_t>(255.f * colorF.z());
            }
        });

        if(!fastPForCloudEncoder.encode(processedCloudData.byte_span(), cFrame->fpfColoredCloud)){
            cFrame->fpfColoredCloud = {};
        }

    }else{
        cFrame->fpfColoredCloud = {};
    }

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_audio(DCCompressedFrame *cFrame) -> void{

    Bench::start("[DCDeviceImpl::update_compressed_frame_audio]");

    if((fData.audioChannels.first != 0) && !fData.audioChannels.second.empty() && cDataS.client.compression.audio){

        size_t audioSize = fData.audioChannels.second.size() / fData.audioChannels.first;
        cFrame->audioFrames.resize(audioSize);
        std::copy(
            fData.audioChannels.second.begin(), fData.audioChannels.second.end(),
            reinterpret_cast<float*>(cFrame->audioFrames.data())
        );

    }else{
        cFrame->audioFrames = {};
    }

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_imu(DCCompressedFrame *cFrame) -> void{

    if(!fData.binaryIMU.empty() && cDataS.client.compression.imu){
        cFrame->imu.resize(fData.binaryIMU.size());
        std::copy(fData.binaryIMU.begin(), fData.binaryIMU.end(), cFrame->imu.begin());
    }else{
        cFrame->imu.clear();
    }
}

auto DCDeviceImpl::update_compressed_frame_bodies(DCCompressedFrame *cFrame) -> void{

    // body tracking
    if(!fData.bodies.empty() && cDataS.client.compression.bodyTracking){
        cFrame->bodyTracking.resize(fData.bodies.size());
        std::copy(fData.bodies.begin(), fData.bodies.end(), cFrame->bodyTracking.begin());
    }else{
        cFrame->bodyTracking.clear();
    }

    // bodies id map
    if(!fData.bodiesIdMap.empty() && cDataS.client.compression.bodyIdMap){

        if(!jpegBodiesIdEncoder.encode(
            mInfos.depth_width(),
            mInfos.depth_height(),
            fData.bodiesIdMap,
            cFrame->jpegG8BodiesIdMap,
            cDataS.client.compression.jpegCompressionRate
        )){
            cFrame->jpegG8BodiesIdMap.reset();
        }
    }else{
        cFrame->jpegG8BodiesIdMap.reset();
    }
}

auto DCDeviceImpl::stop_reading_thread() -> void{

    // wait for reading thread to finish
    if(readFramesFromCameras){
        readFramesFromCameras = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // stop thread    
    if(frameReaderT != nullptr){
        if(frameReaderT->joinable()){
            frameReaderT->join();
        }
        frameReaderT = nullptr;
    }
}

auto DCDeviceImpl::check_data_validity() -> bool {

    if(mInfos.has_color() && settings.data.capture_color()){
        if(fData.rawColor.empty()){
            return false;
        }
    }

    if(mInfos.has_depth() && settings.data.capture_depth()){
        if(fData.depth.empty()){
            return false;
        }
    }

    if(mInfos.has_infra() && settings.data.capture_infra()){
        if(fData.infra.empty()){
            return false;
        }
    }

    return true;
}


auto DCDeviceImpl::maximum_local_depth_difference(const DCFrameIndices &ids, std::span<uint16_t> depthBuffer, float max, DCConnectivity connectivity) -> void{

    std::fill(fData.filteringMask.begin(), fData.filteringMask.end(), 0);

    std::for_each(std::execution::par_unseq, std::begin(ids.depths1DNoBorders), std::end(ids.depths1DNoBorders), [&](size_t id){

        if(fData.depthMask[id] == 0){
            return;
        }

        float meanDiff = 0;
        float currDepth = depthBuffer[id];
        size_t count = 0;
        if(connectivity == DCConnectivity::Connectivity_2H){
            for(auto cId : ids.neighbours2HDepth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }else if(connectivity == DCConnectivity::Connectivity_2V){
            for(auto cId : ids.neighbours2VDepth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }else if(connectivity == DCConnectivity::Connectivity_4){
            for(auto cId : ids.neighbours4Depth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }else if(connectivity == DCConnectivity::Connectivity_8){
            for(auto cId : ids.neighbours8Depth1D[id]){
                if(fData.depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }
        fData.filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);
    });

    for(size_t ii = 0; ii < fData.filteringMask.size(); ++ii){
        if(fData.filteringMask[ii] == 0){
            fData.depthMask[ii] = 0;
        }
    }
}

auto DCDeviceImpl::keep_only_biggest_cluster() -> void{

    // empty zones
    std::fill(fData.zonesId.begin(), fData.zonesId.end(), 0);

    size_t currentZoneId = 1;
    int biggestZone = -1;
    size_t sizeBiggestZone = 0;

    auto depthWidth  = mInfos.depth_width();
    auto depthHeight = mInfos.depth_height();
    auto depthSize   = mInfos.depth_size();

    std::for_each(std::execution::unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t pt){

        if(fData.zonesId[pt] != 0){
            return;
        }

        if(fData.depthMask[pt] == 0){
            return;
        }

        size_t zoneId = currentZoneId;
        std::queue<size_t> ids;
        ids.push(pt);

        // count new zone
        size_t count = 0;
        while(ids.size() > 0){
            size_t id = ids.front();
            ids.pop();

            if(fData.zonesId[id] != 0){
                continue;
            }

            if(fData.depthMask[id] == 0){
                continue;
            }

            fData.zonesId[id] = static_cast<int>(zoneId);
            count++;

            if(id >= 1){
                ids.push(id-1);

                if(id >= depthWidth-1){

                    ids.push(id-depthWidth+1);

                    if(id >= depthWidth){

                        ids.push(id-depthWidth);

                        if(id >= depthWidth + 1){
                            ids.push(id-depthWidth-1);
                        }
                    }
                }
            }

            if(id < depthSize-1){

                ids.push(id+1);

                if(id < depthSize - depthWidth+1){
                    ids.push(id+depthWidth-1);

                    if(id < depthSize -depthWidth){
                        ids.push(id+depthWidth);

                        if(id < depthSize -depthWidth-1){
                            ids.push(id+depthWidth+1);
                        }
                    }
                }
            }
        }

        if(count != 0){
            if(count > sizeBiggestZone){
                sizeBiggestZone = count;
                biggestZone = static_cast<int>(currentZoneId);
            }
            currentZoneId++;
        }
    });

    if(biggestZone != -1){
        Pt2<size_t> meanBiggestZone = {0,0};
        size_t count = 0;
        std::for_each(std::execution::unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const auto &pt){
            if(fData.zonesId[pt.x()] != biggestZone){
                fData.depthMask[pt.x()] = 0;
            }

            if(fData.depthMask[pt.x()] == 1){
                meanBiggestZone.x() += pt.y();
                meanBiggestZone.y() += pt.z();
                ++count;
            }
        });

        meanBiggestZone /= count;
        fData.meanBiggestZoneId = meanBiggestZone.y() * depthWidth + meanBiggestZone.x();
    }

}

auto DCDeviceImpl::mininum_neighbours(uint8_t nbLoops, uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void{

    for(std::uint8_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        // reset filtering mask
        std::fill(fData.filteringMask.begin(), fData.filteringMask.end(), 0);

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1DNoBorders), std::end(fIndices.depths1DNoBorders), [&](size_t id){

            if(fData.depthMask[id] == 0){
                return;
            }

            std::uint8_t count = 0;
            if(connectivity == DCConnectivity::Connectivity_4){
                for(auto cId : fIndices.neighbours4Depth1D[id]){

                    if(fData.depthMask[cId] == 1){
                        ++count;
                    }
                }
                fData.filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : fIndices.neighbours8Depth1D[id]){

                    if(fData.depthMask[cId] == 1){
                        ++count;
                    }
                }
                fData.filteringMask[id] = count == 8 ? 1 : 0;
            }

            fData.filteringMask[id] = (count < nbMinNeighbours) ? 1 : 0;
        });

        for(size_t ii = 0; ii < fData.filteringMask.size(); ++ii){
            if(fData.filteringMask[ii] == 1){
                fData.depthMask[ii] = 0;
            }
        }
    }
}

auto DCDeviceImpl::erode(uint8_t nbLoops, DCConnectivity connectivity) -> void{

    for(size_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        std::fill(fData.filteringMask.begin(), fData.filteringMask.end(), 0);

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1DNoBorders), std::end(fIndices.depths1DNoBorders), [&](size_t id){

            if(fData.depthMask[id] == 0){
                return;
            }

            std::uint8_t count = 0;
            if(connectivity == DCConnectivity::Connectivity_4){
                for(auto cId : fIndices.neighbours4Depth1D[id]){

                    if(fData.depthMask[cId] == 1){
                        ++count;
                    }
                }
                fData.filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : fIndices.neighbours8Depth1D[id]){

                    if(fData.depthMask[cId] == 1){
                        ++count;
                    }
                }
                fData.filteringMask[id] = count == 8 ? 1 : 0;
            }
        });

        for(size_t ii = 0; ii < fData.filteringMask.size(); ++ii){
            if(fData.filteringMask[ii] == 0){
                fData.depthMask[ii] = 0;
            }
        }
    }
}

auto DCDeviceImpl::set_data_settings(const DCDataSettings &dataS) -> void {
    parametersM.lock();
    settings.data = dataS;
    parametersM.unlock();
}

auto DCDeviceImpl::set_filters_settings(const DCFiltersSettings &filtersS) -> void{
    parametersM.lock();
    settings.filters = filtersS;
    parametersM.unlock();
}

auto DCDeviceImpl::set_color_settings(const DCColorSettings &colorS) -> void{
    settings.color = colorS;
    update_from_colors_settings();
}

auto DCDeviceImpl::set_delay_settings(const DCDelaySettings &delayS) -> void{
    parametersM.lock();
    settings.delay = delayS;
    parametersM.unlock();
}

auto DCDeviceImpl::get_duration_between_ms(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::milliseconds>{

    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    return fTiming.get_duration_between_ms(from, to);
}

auto DCDeviceImpl::get_duration_between_micro_s(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::microseconds>{

    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    return fTiming.get_duration_between_micro_s(from, to);
}



auto DCFramesBuffer::add_frame(std::shared_ptr<DCFrame> frame) -> void{
    // store frame
    auto afterCaptureTS = std::chrono::nanoseconds(frame->afterCaptureTS);
    frames.push_back(std::make_tuple(afterCaptureTS, std::move(frame)));
}

auto DCFramesBuffer::add_compressed_frame(std::shared_ptr<DCCompressedFrame> cFrame) -> void{
    // store frame
    auto afterCaptureTS = std::chrono::nanoseconds(cFrame->afterCaptureTS);
    compressedFrames.push_back(std::make_tuple(afterCaptureTS, std::move(cFrame)));
}

auto DCFramesBuffer::take_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, int64_t delayMs) -> std::shared_ptr<DCFrame>{

    // check delay
    using namespace std::chrono;
    bool foundFrame = false;
    size_t idFrame = 0;
    for(size_t ii = 0; ii < frames.size(); ++ii){
        auto diff = duration_cast<milliseconds>(afterCaptureTS - std::get<0>(frames[ii]));
        if(diff.count() >= delayMs){
            foundFrame = true;
            idFrame = ii;
        }else{
            break;
        }
    }

    if(foundFrame){
        auto frame = std::move(frames[idFrame]);
        tool::erase_range(frames, 0, idFrame + 1);
        return std::get<1>(frame);
    }
    return nullptr;
}

auto DCFramesBuffer::get_compressed_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, int64_t delayMs) -> std::shared_ptr<DCCompressedFrame>{

    // check delay
    using namespace std::chrono;
    bool foundFrame = false;
    size_t idFrame = 0;
    for(size_t ii = 0; ii < compressedFrames.size(); ++ii){
        auto diff = duration_cast<milliseconds>(afterCaptureTS - std::get<0>(compressedFrames[ii]));
        if(diff.count() >= delayMs){
            foundFrame = true;
            idFrame = ii;
        }else{
            break;
        }
    }

    if(foundFrame){
        auto cFrame = std::move(compressedFrames[idFrame]);
        tool::erase_range(compressedFrames, 0, idFrame + 1);
        return std::get<1>(cFrame);
    }
    return nullptr;
}


// #include "thirdparty/stb/stb_image_write.h"

// auto DCDeviceImpl::debug_save_images(std::string parentPath) -> void{

//     parentPath += std::format("/dev_{}_",mInfos.id_capture()%10);

//     if(!fData.color.empty()){
//         std::cout << std::format("write color image [{},{},{}] [{},{}]\n",mInfos.color_width(),mInfos.color_height(),mInfos.color_size(), fData.color.size(), fData.color.size_bytes()) << "\n";
//         stbi_write_png(std::format("{}_color.png", parentPath).c_str(), mInfos.color_width(), mInfos.color_height(), 4, fData.color.data(), mInfos.color_width()*4);
//     }

//     if(!fData.dephtSizedColor.empty()){
//         std::cout << std::format("write depth sized color image [{},{},{}] [{},{}]\n",mInfos.depth_width(),mInfos.depth_height(),mInfos.depth_size(), fData.dephtSizedColor.size(), fData.dephtSizedColor.size_bytes()) << "\n";
//         stbi_write_png(std::format("{}_depth_sized_color.png", parentPath).c_str(), mInfos.depth_width(), mInfos.depth_height(), 4, fData.dephtSizedColor.data(), mInfos.depth_width()*4);
//     }
// }


