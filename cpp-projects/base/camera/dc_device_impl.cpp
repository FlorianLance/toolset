
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

// local
#include "utility/benchmark.hpp"
#include "utility/logger.hpp"

using namespace tool::geo;
using namespace tool::cam;

auto DCDeviceImpl::initialize() -> void{

    mInfos.initialize(settings.config.mode);
    fIndices.initialize(mInfos);
    fData.reset();
    
    if(mInfos.has_depth()){
        depthMask.resize(mInfos.depth_size());
        filteringMask.resize(mInfos.depth_size());
        depthFiltering.resize(mInfos.depth_size());
        zonesId.resize(mInfos.depth_size());
    }else{
        depthMask.clear();
        filteringMask.clear();
        depthFiltering.clear();
        zonesId.clear();
    }

    if(mInfos.has_color()){
        convertedColorData.resize(mInfos.rgba_color_size_bytes());
    }else{
        convertedColorData.clear();
    }

    initialize_device_specific();
}


auto DCDeviceImpl::start_reading_thread() -> void{

    update_camera_from_colors_settings();

    // start thread
    Logger::message("[DCDeviceImpl::start_reading] Start reading frames.\n");
    frameReaderT = std::make_unique<std::thread>(&DCDeviceImpl::read_frames, this);
}

auto DCDeviceImpl::read_frames() -> void {

    auto dev = dc_get_device(settings.config.mode);

    // start loop
    readFramesFromCameras = true;
    fTiming.reset();

    while(readFramesFromCameras){

        parametersM.lock();
        auto cFiltersS = settings.filters;
        auto cDataS    = settings.data;
        auto cDelayS   = settings.delay;
        fTiming.swap_local_timestamps();
        parametersM.unlock();

        // read data from device
        try {

            fTiming.update_local("before_capture"sv);
            bool success = capture_frame(mInfos.timeout_ms());
            fTiming.update_local("after_capture"sv);

            if(!success){
                // timeout
                continue;
            }

            if(cDataS.captureAudio){
                read_from_microphones();
            }

            if(cDataS.captureIMU){
                read_from_imu();
            }

            if(cDataS.captureBodies){
                read_bodies();
            }

            // timing.compute_capture_framerate();

        }   catch (std::exception &error) {
            Logger::error(std::format("[DCDeviceData::read_frames] Get capture error: {}\n", error.what()));
            readFramesFromCameras = false;
            break;
        }

        // get images
        read_color_image();
        read_depth_image();
        read_infra_image();
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

        filter_depth_image(cFiltersS);
        fTiming.update_local("after_depth_filter"sv);

        filter_depth_sized_color_image(cFiltersS);
        fTiming.update_local("after_color_filter"sv);

        filter_infrared_image(cFiltersS);
        fTiming.update_local("after_infrared_filter"sv);       

        if(cDataS.generateCloudLocal || cFiltersS.p1FMode != DCFiltersSettings::PlaneFilteringMode::None){

            generate_cloud();
            fTiming.update_local("after_cloud_generation"sv);

            // filter from geometry
            filter_cloud_image(cFiltersS);
            fTiming.update_local("after_cloud_filtering"sv);
        }
        fTiming.update_local("after_processing"sv);

        if(!readFramesFromCameras){
            break;
        }

        bool hasDataToSend = false;
        if(dev == DCType::FemtoBolt || dev == DCType::FemtoMega){
            hasDataToSend = cDataS.has_data_to_send() || cDataS.sendIMU || cDataS.sendBodies;
        }else if (dev == DCType::AzureKinect){
            hasDataToSend = cDataS.has_data_to_send() || cDataS.sendIMU || cDataS.sendAudio || cDataS.sendBodies;
        }

        // compressed frame
        if(sendData && hasDataToSend){

            // generate
            auto compressedFrame = compress_frame(cFiltersS,cDataS);
            fTiming.update_local("after_compressing"sv);
            frames.add_compressed_frame(std::move(compressedFrame));

            // send with delay
            if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
                fTiming.update_local("after_compressed_frame_sending"sv);
            }
        }

        // local frame
        if(cDataS.generateRGBLocalFrame || cDataS.generateDepthLocalFrame || cDataS.generateInfraLocalFrame || cDataS.generateCloudLocal){

            // generate
            auto dFrame = create_local_frame(cDataS);
            fTiming.update_local("after_local_frame_creation"sv);
            frames.add_frame(std::move(dFrame));

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
        std::copy(fData.rawColor.data(), fData.rawColor.data() + mInfos.color_size(), yuvImage.data); // copy Y plane
        std::copy(fData.rawColor.data() + mInfos.color_size(), fData.rawColor.data() + mInfos.color_size() + mInfos.color_size()/2, yuvImage.data + mInfos.color_size());

        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, convertedColorData.data());
        cv::cvtColor(yuvImage, convMat, cv::COLOR_YUV2RGBA_NV12);

        Bench::stop();

    }else if(mInfos.image_format() == DCImageFormat::MJPG){

        Bench::start("[DCDeviceImpl::convert_color_image] MJPG");
        auto success = fUncompressor.uncompress_jpeg_data(
            mInfos.color_width(),
            mInfos.color_height(),
            DCFrameUncompressor::ColorFormat::RGBA,
            fData.rawColor.size_bytes(),
            reinterpret_cast<uint8_t*>(fData.rawColor.data()),
            reinterpret_cast<uint8_t*>(convertedColorData.data())
        );
        Bench::stop();

        if(!success){
            Logger::error("DCDeviceImpl::convert_color_image: Uncompression error.\n");
            return;
        }
    }else if(mInfos.image_format() == DCImageFormat::BGRA){

        Bench::start("[DCDeviceImpl::convert_color_image] BGRA");

        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, convertedColorData.data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_BGRA2RGBA);

        Bench::stop();

    }else if(mInfos.image_format() == DCImageFormat::YUY2 ){

        //YUY2 is a 4:2:2 format, so there are 4 bytes per 'chunk' of data, and each 'chunk' represents 2 pixels.
        Bench::start("[DCDeviceImpl::convert_color_image] YUY2");

        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC2, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, convertedColorData.data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_YUV2RGBA_YUY2);

        Bench::stop();

    }else{
        Logger::error("DCDeviceImpl::convert_color_image: Invalid color format.\n");
        return;
    }

    fData.color =  std::span<tool::ColorRGBA8>{
        reinterpret_cast<tool::ColorRGBA8*>(convertedColorData.data()),
        mInfos.color_size()
    };
}

auto DCDeviceImpl::filter_depth_image(const DCFiltersSettings &filtersS) -> void{

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
    auto minW   = mInfos.depth_width()  * filtersS.minWidthF;
    auto maxW   = mInfos.depth_width()  * filtersS.maxWidthF;
    auto minH   = mInfos.depth_height() * filtersS.minHeightF;
    auto maxH   = mInfos.depth_height() * filtersS.maxHeightF;

    auto diffRange = dRange.y()-dRange.x();
    auto minD = dRange.x() + filtersS.minDepthF * diffRange;
    auto maxD = dRange.x() + filtersS.maxDepthF * diffRange;
    auto hsvDiffColor = Convert::to_hsv(filtersS.filterColor);

    // reset depth mask
    std::fill(depthMask.begin(), depthMask.end(), 1);

    // depth/width/height/mask/color/infra filtering
    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const Pt3<size_t> &dIndex){

        size_t id = dIndex.x();
        size_t ii = dIndex.y();
        size_t jj = dIndex.z();

        const auto &currentDepth = fData.depth[id];

        // check validity
        if(currentDepth == dc_invalid_depth_value){
            depthMask[id] = 0;
            return;
        }

        // depth filtering
        if( (ii < minW)  || (ii > maxW)  ||   // width
            (jj < minH)  || (jj > maxH) ||   // height
            (currentDepth < minD) || (currentDepth > maxD) ){           // depth
            depthMask[id] = 0;
            return;
        }

        // color filtering
        if(!fData.dephtSizedColor.empty() && filtersS.filterDepthWithColor){

            auto hsv = Convert::to_hsv(fData.dephtSizedColor[id]);
            if((std::abs(hsv.h()- hsvDiffColor.h()) > filtersS.maxDiffColor.x()) ||
                (std::abs(hsv.s()- hsvDiffColor.s()) > filtersS.maxDiffColor.y()) ||
                (std::abs(hsv.v()- hsvDiffColor.v()) > filtersS.maxDiffColor.z())){
                depthMask[id] = 0;
                return;
            }
        }

        // infrared filtering
        // ...
    });

    if(filtersS.doLocalDiffFiltering){
        maximum_local_depth_difference(fIndices, fData.depth, filtersS.maxLocalDiff, DCConnectivity::Connectivity_4);
    }

    // minimum neighbours filtering
    if(filtersS.doMinNeighboursFiltering){
        mininum_neighbours(filtersS.minNeighboursLoops, filtersS.nbMinNeighbours, DCConnectivity::Connectivity_4);
    }

    // erosion
    if(filtersS.doErosion){
        erode(filtersS.erosionLoops, DCConnectivity::Connectivity_8);
    }

    // keep only biggest cluster
    if(filtersS.keepOnlyBiggestCluster){
        keep_only_biggest_cluster();
    }else{
        meanBiggestZoneId = 0;
    }

    update_valid_depth_values();

    Bench::stop();

}

auto DCDeviceImpl::filter_depth_sized_color_image(const DCFiltersSettings &filtersS) -> void{

    if(fData.dephtSizedColor.empty()){
        return;
    }

    if(!fData.depth.empty() && (fData.depth.size() != fData.dephtSizedColor.size())){
        return;
    }

    if(!fData.infra.empty() && (fData.infra.size() != fData.dephtSizedColor.size())){
        return;
    }

    bool writeBodies = (fData.bodiesIdDepth.size() == fData.dephtSizedColor.size()) && true; // TODO

    Bench::start("[DCDeviceData::filter_color_image]");

    if(!fData.depth.empty()){

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
            if(filtersS.invalidateColorFromDepth){
                if(fData.depth[id] == dc_invalid_depth_value){
                    fData.dephtSizedColor[id] = ColorRGBA8{dc_invalid_color_value};
                }else{
                    if(filtersS.keepOnlyBiggestCluster){
                        // colorBuffer[meanBiggestZoneId] = ColorRGBA8{255,0,0,255};// TODO ?
                    }                                       
                }                                
            }

            if(writeBodies){
                if(fData.bodiesIdDepth[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    fData.dephtSizedColor[id] = ColorRGBA8{255,0,0,255};
                }
            }
        });
    }

    Bench::stop();
}

auto DCDeviceImpl::filter_infrared_image(const DCFiltersSettings &filtersS) -> void{

    if(fData.infra.empty()){
        return;
    }

    if(!fData.dephtSizedColor.empty() && (fData.dephtSizedColor.size() != fData.infra.size())){
        return;
    }

    if(!fData.depth.empty() && (fData.depth.size() != fData.infra.size())){
        return;
    }

    bool writeBodies = (fData.bodiesIdDepth.size() == fData.infra.size()) && true; // TODO

    Bench::start("[DCDeviceData::filter_infrared_image]");
    if(!fData.depth.empty()){
        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
            if(filtersS.invalidateInfraFromDepth){
                if(fData.depth[id] == dc_invalid_depth_value){
                    fData.infra[id] = dc_invalid_infra_value;
                }
            }

            if(writeBodies){
                if(fData.bodiesIdDepth[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    fData.infra[id] = 16000;
                }
            }
        });
    }
    Bench::stop();
}

auto DCDeviceImpl::update_valid_depth_values() -> void{

    // count valid depth values
    validDepthValues = 0;
    std::for_each(std::execution::unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
        if(depthMask[id] == 0){
            fData.depth[id] = dc_invalid_depth_value;
            fIndices.depthVertexCorrrespondance[id] = {id, -1};
        }else{
            fIndices.depthVertexCorrrespondance[id] = {id, validDepthValues};
            validDepthValues++;
        }
    });
}

auto DCDeviceImpl::update_infos(DCFrame *dFrame) -> void{
    dFrame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
    dFrame->afterCaptureTS  = fTiming.get_local("after_capture"sv).count();
    dFrame->receivedTS      = dFrame->afterCaptureTS;  // default init received TS with after capture TS
    dFrame->mode            = settings.config.mode;
}

auto DCDeviceImpl::update_color(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(fData.color.empty() || !dataS.generateRGBLocalFrame){
        dFrame->colorWidth     = 0;
        dFrame->colorHeight    = 0;
        dFrame->imageColorData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_color]");

    dFrame->colorWidth  = mInfos.color_width();
    dFrame->colorHeight = mInfos.color_height();
    dFrame->imageColorData.resize(mInfos.color_size());

    std::copy(
        reinterpret_cast<std::int8_t*>(fData.color.data()),
        reinterpret_cast<std::int8_t*>(fData.color.data()) + fData.color.size_bytes(),
        reinterpret_cast<std::int8_t*>(dFrame->imageColorData.data())
    );

    Bench::stop();
}

auto DCDeviceImpl::update_depth_sized_color(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(fData.dephtSizedColor.empty() || !dataS.generateRGBLocalFrame){
        dFrame->depthSizedColorWidth     = 0;
        dFrame->depthSizedColorHeight    = 0;
        dFrame->depthSizedImageColorData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_depth_sized_color]");

    dFrame->depthSizedColorWidth  = mInfos.depth_width();
    dFrame->depthSizedColorHeight = mInfos.depth_height();
    dFrame->depthSizedImageColorData.resize(mInfos.depth_size());

    std::copy(
        reinterpret_cast<std::int8_t*>(fData.dephtSizedColor.data()),
        reinterpret_cast<std::int8_t*>(fData.dephtSizedColor.data()) + fData.dephtSizedColor.size_bytes(),
        reinterpret_cast<std::int8_t*>(dFrame->depthSizedImageColorData.data())
    );

    Bench::stop();
}

auto DCDeviceImpl::update_depth(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(fData.depth.empty() || !dataS.generateDepthLocalFrame){
        dFrame->depthWidth     = 0;
        dFrame->depthHeight    = 0;
        dFrame->imageDepthData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_depth]");

    dFrame->depthWidth  = mInfos.depth_width();
    dFrame->depthHeight = mInfos.depth_height();
    dFrame->imageDepthData.resize(mInfos.depth_size());

    dFrame->depthData.resize(mInfos.depth_size());
    std::copy(fData.depth.begin(), fData.depth.end(), dFrame->depthData.begin());

    const auto dRange = mInfos.depth_range_mm();
    const auto diff   = dRange.y() - dRange.x();
    static constexpr std::array<Pt3f,5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){

        if(fData.depth[id] == dc_invalid_depth_value){
            dFrame->imageDepthData[id] = geo::Pt3<std::uint8_t>{};
            return;
        }

        float vF = (static_cast<float>(fData.depth[id]) - dRange.x())/diff;
        float intPart;
        float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
        size_t idG = static_cast<size_t>(intPart);

        auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
        dFrame->imageDepthData[id] = {
            static_cast<std::uint8_t>(255*col.x()),
            static_cast<std::uint8_t>(255*col.y()),
            static_cast<std::uint8_t>(255*col.z())
        };
    });

    Bench::stop();
}

auto DCDeviceImpl::update_infra(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(fData.infra.empty() || !dataS.generateInfraLocalFrame){
        dFrame->infraWidth     = 0;
        dFrame->infraHeight    = 0;
        dFrame->imageInfraData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_infra]");

    dFrame->infraWidth  = mInfos.infra_width();
    dFrame->infraHeight = mInfos.infra_height();
    dFrame->imageInfraData.resize(mInfos.infra_size());

    const float max = 2000;
    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){

        float vF = static_cast<float>(fData.infra[id]);
        if(vF > max){
            vF = max;
        }
        vF/=max;
        dFrame->imageInfraData[id] = {
            static_cast<std::uint8_t>(255*vF),
            static_cast<std::uint8_t>(255*vF),
            static_cast<std::uint8_t>(255*vF)
        };
    });

    Bench::stop();
}

auto DCDeviceImpl::update_cloud(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(fData.depth.empty() || fData.depthCloud.empty() || !dataS.generateCloudLocal){
        return;
    }

    Bench::start("[DCDeviceImpl::update_cloud]");

    const auto dRange = mInfos.depth_range_mm();
    const auto diff   = dRange.y() - dRange.x();

    static constexpr std::array<Pt3f,5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    dFrame->cloud.vertices.resize(validDepthValues);
    dFrame->cloud.colors.resize(validDepthValues);
    dFrame->cloud.normals.resize(validDepthValues);

    std::for_each(std::execution::par_unseq, std::begin(fIndices.depthVertexCorrrespondance), std::end(fIndices.depthVertexCorrrespondance), [&](auto idC){

        auto idD = std::get<0>(idC);
        if(fData.depth[idD] == dc_invalid_depth_value){
            return;
        }

        auto idV = std::get<1>(idC);
        dFrame->cloud.vertices[idV]= geo::Pt3f{
            static_cast<float>(-fData.depthCloud[idD].x()),
            static_cast<float>(-fData.depthCloud[idD].y()),
            static_cast<float>( fData.depthCloud[idD].z())
        }*0.001f;

        if(!fData.dephtSizedColor.empty()){
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

    Bench::stop();
}

auto DCDeviceImpl::update_imu(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(dataS.captureIMU && fData.imuSample.has_value()){
        dFrame->imuSample = fData.imuSample.value();
    }else{
        dFrame->imuSample = std::nullopt;
    }
}

auto DCDeviceImpl::update_bodies(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(dataS.captureBodies){
        dFrame->bodies = fData.bodies;
    }else{
        dFrame->bodies = {};
    }
}

auto DCDeviceImpl::update_audio(const DCDataSettings &dataS, DCFrame *dFrame) -> void{

    if(!dataS.captureAudio || (fData.audioChannels.first == 0) || fData.audioChannels.second.empty()){
        return;
    }

    Bench::start("[DCDeviceImpl::update_audio]");

    size_t nbFrames = fData.audioChannels.second.size() / fData.audioChannels.first;
    dFrame->audioFrames.resize(nbFrames);
    auto audioFrom = fData.audioChannels.second.data();
    std::copy(audioFrom, audioFrom + fData.audioChannels.second.size(), reinterpret_cast<float*>(dFrame->audioFrames.data()));

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_infos(DCCompressedFrame *cFrame) -> void{
    cFrame->mode               = settings.config.mode;
    cFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());
    cFrame->afterCaptureTS     = fTiming.get_local("after_capture"sv).count();
    cFrame->receivedTS         = cFrame->afterCaptureTS;  // default init received TS with after capture TS
    cFrame->validVerticesCount = validDepthValues;
}

auto DCDeviceImpl::update_compressed_frame_color(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void{

    if(fData.color.empty() || !dataS.sendColor){
        cFrame->colorWidth       = 0;
        cFrame->colorHeight      = 0;
        cFrame->encodedColorData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_compressed_frame_color]");

    fCompressor.add_color(
        mInfos.color_width(),
        mInfos.color_height(),
        4,
        reinterpret_cast<uint8_t*>(fData.color.data()),
        filtersS.jpegCompressionRate,
        cFrame
    );

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_depth_sized_color(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void{

    if(fData.dephtSizedColor.empty() || !dataS.sendColor){
        cFrame->depthSizedColorWidth       = 0;
        cFrame->depthSizedColorHeight      = 0;
        cFrame->encodedDepthSizedColorData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_compressed_frame_depth_sized_color]");

    fCompressor.add_depth_sized_color(
        mInfos.depth_width(),
        mInfos.depth_height(),
        4,
        reinterpret_cast<uint8_t*>(fData.dephtSizedColor.data()),
        filtersS.jpegCompressionRate,
        cFrame
    );

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_depth(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void{

    if(fData.depth.empty() || !dataS.sendDepth){
        cFrame->depthWidth       = 0;
        cFrame->depthHeight      = 0;
        cFrame->encodedDepthData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_compressed_frame_depth]");

    fCompressor.add_depth(
        mInfos.depth_width(),
        mInfos.depth_height(),
        reinterpret_cast<std::uint16_t*>(fData.depth.data()),
        cFrame
    );

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_infra(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void{

    if(fData.infra.empty() || !dataS.sendInfra){
        cFrame->infraWidth       = 0;
        cFrame->infraHeight      = 0;
        cFrame->encodedInfraData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_compressed_frame_infra]");

    fCompressor.add_infra(
        mInfos.infra_width(),
        mInfos.infra_height(),
        reinterpret_cast<std::uint16_t*>(fData.infra.data()),
        cFrame
    );

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_cloud(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void{

    if(fData.depthCloud.empty() || fData.depth.empty() || fData.dephtSizedColor.empty() || !dataS.sendCloud){
        cFrame->encodedCloudVerticesData = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_compressed_frame_cloud]");

    fCompressor.add_cloud(
        fData.depth,
        fData.dephtSizedColor,
        fData.depthCloud,
        cFrame->encodedCloudVerticesData
    );

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_audio(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void{

    if((fData.audioChannels.first == 0) || fData.audioChannels.second.empty() || !dataS.sendAudio){
        cFrame->audioFrames = {};
        return;
    }

    Bench::start("[DCDeviceImpl::update_compressed_frame_audio]");

    size_t audioSize = fData.audioChannels.second.size() / fData.audioChannels.first;
    cFrame->audioFrames.resize(audioSize);
    std::copy(
        fData.audioChannels.second.begin(), fData.audioChannels.second.end(),
        reinterpret_cast<float*>(cFrame->audioFrames.data())
    );

    Bench::stop();
}

auto DCDeviceImpl::update_compressed_frame_imu(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void{
    if(!dataS.sendIMU || !fData.imuSample.has_value()){
        cFrame = {};
        return;
    }
    cFrame->imuSample = fData.imuSample;
}

auto DCDeviceImpl::update_compressed_frame_bodies(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void{

    if(!dataS.sendBodies){
        return;
    }
    // ...
}

#include "thirdparty/stb/stb_image_write.h"

auto DCDeviceImpl::debug_save_images(std::string parentPath) -> void{

    parentPath += std::format("/dev_{}_",mInfos.id_capture()%10);

    if(!fData.color.empty()){
        std::cout << std::format("write color image [{},{},{}] [{},{}]\n",mInfos.color_width(),mInfos.color_height(),mInfos.color_size(), fData.color.size(), fData.color.size_bytes()) << "\n";
        stbi_write_png(std::format("{}_color.png", parentPath).c_str(), mInfos.color_width(), mInfos.color_height(), 4, fData.color.data(), mInfos.color_width()*4);
    }

    if(!fData.dephtSizedColor.empty()){
        std::cout << std::format("write depth sized color image [{},{},{}] [{},{}]\n",mInfos.depth_width(),mInfos.depth_height(),mInfos.depth_size(), fData.dephtSizedColor.size(), fData.dephtSizedColor.size_bytes()) << "\n";
        stbi_write_png(std::format("{}_depth_sized_color.png", parentPath).c_str(), mInfos.depth_width(), mInfos.depth_height(), 4, fData.dephtSizedColor.data(), mInfos.depth_width()*4);
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

    if(mInfos.has_color()){
        if(fData.rawColor.empty()){
            return false;
        }
    }

    if(mInfos.has_depth()){
        if(fData.depth.empty()){
            return false;
        }
    }

    if(dc_has_infrared(settings.config.mode)){
        if(fData.infra.empty()){
            return false;
        }
    }

    return true;
}


auto DCDeviceImpl::maximum_local_depth_difference(const DCFrameIndices &ids, std::span<uint16_t> depthBuffer, float max, DCConnectivity connectivity) -> void{

    std::fill(filteringMask.begin(),filteringMask.end(), 0);

    std::for_each(std::execution::par_unseq, std::begin(ids.depths1DNoBorders), std::end(ids.depths1DNoBorders), [&](size_t id){

        if(depthMask[id] == 0){
            return;
        }

        float meanDiff = 0;
        float currDepth = depthBuffer[id];
        size_t count = 0;
        if(connectivity == DCConnectivity::Connectivity_4){
            for(auto cId : ids.neighbours4Depth1D[id]){
                if(depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }else{
            for(auto cId : ids.neighbours8Depth1D[id]){
                if(depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }
        filteringMask[id] = (count == 0) ? 0 : ((1.*meanDiff/count < max) ? 1 : 0);

    });

    for(size_t ii = 0; ii < filteringMask.size(); ++ii){
        if(filteringMask[ii] == 0){
            depthMask[ii] = 0;
        }
    }
}

auto DCDeviceImpl::keep_only_biggest_cluster() -> void{

    // empty zones
    std::fill(zonesId.begin(), zonesId.end(), 0);

    size_t currentZoneId = 1;
    int biggestZone = -1;
    size_t sizeBiggestZone = 0;

    auto depthWidth  = mInfos.depth_width();
    auto depthHeight = mInfos.depth_height();
    auto depthSize   = mInfos.depth_size();

    std::for_each(std::execution::unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t pt){

        if(zonesId[pt] != 0){
            return;
        }

        if(depthMask[pt] == 0){
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

            if(zonesId[id] != 0){
                continue;
            }

            if(depthMask[id] == 0){
                continue;
            }

            zonesId[id] = static_cast<int>(zoneId);
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
            if(zonesId[pt.x()] != biggestZone){
                depthMask[pt.x()] = 0;
            }

            if(depthMask[pt.x()] == 1){
                meanBiggestZone.x() += pt.y();
                meanBiggestZone.y() += pt.z();
                ++count;
            }
        });

        meanBiggestZone /= count;
        meanBiggestZoneId = meanBiggestZone.y() * depthWidth + meanBiggestZone.x();
    }

}

auto DCDeviceImpl::mininum_neighbours(uint8_t nbLoops, uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void{

    for(std::uint8_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        // reset filtering mask
        std::fill(filteringMask.begin(),filteringMask.end(), 0);

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1DNoBorders), std::end(fIndices.depths1DNoBorders), [&](size_t id){

            if(depthMask[id] == 0){
                return;
            }

            std::uint8_t count = 0;
            if(connectivity == DCConnectivity::Connectivity_4){
                for(auto cId : fIndices.neighbours4Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : fIndices.neighbours8Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 8 ? 1 : 0;
            }

            filteringMask[id] = (count < nbMinNeighbours) ? 1 : 0;
        });

        for(size_t ii = 0; ii < filteringMask.size(); ++ii){
            if(filteringMask[ii] == 1){
                depthMask[ii] = 0;
            }
        }
    }
}

auto DCDeviceImpl::erode(uint8_t nbLoops, DCConnectivity connectivity) -> void{


    for(size_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        std::fill(filteringMask.begin(),filteringMask.end(), 0);

        std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1DNoBorders), std::end(fIndices.depths1DNoBorders), [&](size_t id){

            if(depthMask[id] == 0){
                return;
            }

            uint8_t count = 0;
            if(connectivity == DCConnectivity::Connectivity_4){
                for(auto cId : fIndices.neighbours4Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : fIndices.neighbours8Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 8 ? 1 : 0;
            }
        });

        for(size_t ii = 0; ii < filteringMask.size(); ++ii){
            if(filteringMask[ii] == 0){
                depthMask[ii] = 0;
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
    update_camera_from_colors_settings();
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




