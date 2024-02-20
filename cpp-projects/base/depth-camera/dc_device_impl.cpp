
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
#include "utility/logger.hpp"
#include "utility/time.hpp"

using namespace tool::geo;
using namespace tool::cam;

DCDeviceImpl::DCDeviceImpl(){

    // init tasks
    mainLoopTF = std::make_unique<tf::Taskflow>();
    auto startLoopT = mainLoopTF->emplace([&](){
        update_from_colors_settings();
        readFramesFromCameras = true;
        read_calibration();
    }).name("start_loop");

    auto endLoopT = mainLoopTF->emplace([&](){}).name("end_loop");

    auto doLoopCT = mainLoopTF->emplace([&](){
        return readFramesFromCameras ? 1 : 0;
    }).name("do_loop");

    auto startIterationLoopT = mainLoopTF->emplace([&](){
    }).name("start_iteration_loop");

    auto endIterationLoopT = mainLoopTF->emplace([&](){return 0;
    }).name("end_iteration_loop");

    // main loop
    readDataTF          = read_data_taskflow();
    processDataTF       = process_data_taskflow();
    processFrameTF      = process_frame_taskflow(*readDataTF, *processDataTF);
    auto processFrameT  = mainLoopTF->composed_of(*processFrameTF).name("process_frame_module");

    startLoopT.precede(doLoopCT);
    doLoopCT.precede(endLoopT, startIterationLoopT);
    startIterationLoopT.precede(processFrameT);
    processFrameT.precede(endIterationLoopT);
    endIterationLoopT.precede(doLoopCT);
    mainLoopTF->dump(std::cout);
}

auto DCDeviceImpl::initialize(const DCConfigSettings &newConfigS) -> void{

    settings.config = newConfigS;

    mInfos.initialize(settings.config.mode);
    fIndices.initialize(mInfos);
    fData.reset(mInfos);
    initialize_device_specific();
}

auto DCDeviceImpl::start_reading_thread() -> void{
    executor = std::make_unique<tf::Executor>();
    loopFuture = executor->run(*mainLoopTF);
}

auto DCDeviceImpl::stop_reading_thread() -> void{
    if(readFramesFromCameras){
        readFramesFromCameras = false;
        loopFuture.wait();
    }
}


auto DCDeviceImpl::convert_color_image() -> void{

    auto t = TimeF(timeM, "CONVERT_COLOR_IMAGE"sv);
    fData.color = {};

    if(!mInfos.has_color() || fData.rawColor.empty()){
        return;
    }

    if(mInfos.image_format() == DCImageFormat::NV12){

        cv::Mat yuvImage(static_cast<int>(mInfos.color_height() * 3 / 2), static_cast<int>(mInfos.color_width()), CV_8UC1);
        std::copy(fData.rawColor.data(), fData.rawColor.data() + mInfos.color_size(),  reinterpret_cast<std::byte*>(yuvImage.data)); // copy Y plane
        std::copy(fData.rawColor.data() + mInfos.color_size(), fData.rawColor.data() + mInfos.color_size() + mInfos.color_size()/2, reinterpret_cast<std::byte*>(yuvImage.data) + mInfos.color_size());

        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data());
        cv::cvtColor(yuvImage, convMat, cv::COLOR_YUV2RGBA_NV12);

    }else if(mInfos.image_format() == DCImageFormat::MJPG){

        auto success = jpegColorDecoder.decode(
            mInfos.color_width(), mInfos.color_height(), fData.rawColor,
            fData.convertedColorData
        );

        if(!success){
            Logger::error("DCDeviceImpl::convert_color_image: Uncompression error.\n");
            return;
        }
    }else if(mInfos.image_format() == DCImageFormat::BGRA){

        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_BGRA2RGBA);

    }else if(mInfos.image_format() == DCImageFormat::YUY2 ){

        //YUY2 is a 4:2:2 format, so there are 4 bytes per 'chunk' of data, and each 'chunk' represents 2 pixels.
        cv::Mat rawMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC2, fData.rawColor.data());
        cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data()); // ?
        cv::cvtColor(rawMat, convMat, cv::COLOR_YUV2RGBA_YUY2);

    }else{
        Logger::error("DCDeviceImpl::convert_color_image: Invalid color format.\n");
        return;
    }

    fData.color = fData.convertedColorData.span();
}

auto DCDeviceImpl::filter_depth_basic() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_BASIC"sv);

    // depth / width / height
    if(fData.depth.empty()){
        return;
    }

    auto dRange = mInfos.depth_range_mm();
    auto minW   = mInfos.depth_width()  * cFiltersS.minWidthF;
    auto maxW   = mInfos.depth_width()  * cFiltersS.maxWidthF;
    auto minH   = mInfos.depth_height() * cFiltersS.minHeightF;
    auto maxH   = mInfos.depth_height() * cFiltersS.maxHeightF;
    auto diffRange = dRange.y()-dRange.x();
    auto minD = dRange.x() + cFiltersS.minDepthF * diffRange;
    auto maxD = dRange.x() + cFiltersS.maxDepthF * diffRange;

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
        if( (ii < minW)  || (ii > maxW)  ||                   // width
            (jj < minH)  || (jj > maxH)  ||                   // height
            (currentDepth < minD) || (currentDepth > maxD) ){ // depth
            fData.depthMask[id] = 0;
            return;
        }
    });
}

auto DCDeviceImpl::filter_depth_from_depth_sized_color() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_FROM_DEPTH_SIZED_COLOR"sv);

    // invalidate depth from depth-sized color
    if(!cFiltersS.filterDepthWithColor){
        return;
    }
    if(fData.depth.empty()){
        return;
    }
    if(!fData.depthSizedColor.empty() && (fData.depthSizedColor.size() != fData.depth.size())){
        return;
    }

    auto hsvDiffColor = Convert::to_hsv(cFiltersS.filterColor);
    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const Pt3<size_t> &dIndex){
        size_t id = dIndex.x();
        auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);
        if((std::abs(hsv.h()- hsvDiffColor.h()) > cFiltersS.maxDiffColor.x()) ||
            (std::abs(hsv.s()- hsvDiffColor.s()) > cFiltersS.maxDiffColor.y()) ||
            (std::abs(hsv.v()- hsvDiffColor.v()) > cFiltersS.maxDiffColor.z())){
            fData.depthMask[id] = 0;
            return;
        }
    });
}

auto DCDeviceImpl::filter_depth_from_infra() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_FROM_INFRA"sv);

    // invalidate depth from infra
    if(!cFiltersS.filterDepthWithInfra){
        return;
    }
    if(fData.depth.empty()){
        return;
    }
    if(!fData.infra.empty() && (fData.infra.size() != fData.depth.size())){
        return;
    }

    // std::for_each(std::execution::par_unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const Pt3<size_t> &dIndex){
    //    // ...
    // });
}

auto DCDeviceImpl::filter_depth_from_cloud() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_FROM_CLOUD"sv);

    // invalidate depth from cloud geometry
    if(!cFiltersS.filterDepthWithCloud){
        return;
    }
    if(fData.depth.empty() || fData.depthCloud.empty()){
        return;
    }

    if(cFiltersS.p1FMode == PlaneFilteringMode::None && !cFiltersS.removeFromPointDistance){
        return; // do nothing
    }

    // plane filtering parameters
    auto p1             = cFiltersS.p1A*1000.f;
    auto p2             = cFiltersS.p1B*1000.f;
    auto p3             = cFiltersS.p1C*1000.f;
    geo::Pt3f meanPt    = (p1+p2+p3)/3.f;
    auto AB             = vec(p2,p1);
    auto AC             = vec(p3,p1);
    auto normalV        = cross(AB,AC);
    normalV             = normalize(normalV);

    geo::Pt3f pSphere   = cFiltersS.pSphere*1000.f;
    auto squareMaxDistanceFromPoint = cFiltersS.maxSphereDistance*cFiltersS.maxSphereDistance;

    std::for_each(std::execution::par_unseq, std::begin(fIndices.depthVertexCorrrespondance), std::end(fIndices.depthVertexCorrrespondance), [&](auto idC){

        auto idD = std::get<0>(idC);
        if(fData.depth[idD] == dc_invalid_depth_value){
            return;
        }

        if(cFiltersS.p1FMode != PlaneFilteringMode::None){
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
        }

        if(cFiltersS.removeFromPointDistance){
            if(square_norm(vec(pSphere,fData.depthCloud[idD].template conv<float>())) > squareMaxDistanceFromPoint){
                fData.depthMask[idD] = 0;
                return;
            }
        }
    });
}

auto DCDeviceImpl::filter_depth_from_body_tracking() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_FROM_BODY_TRACKING"sv);
    // invalidate depth from body tracking
    if(fData.depth.empty()){
        return;
    }

    // filter cloud from body tracking positions
    // filter cloud from body tracking map
    // if(fData.depth.empty() || fData.depthCloud.empty()){
    //     return;
    // }
}

auto DCDeviceImpl::filter_depth_complex() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_COMPLEX"sv);
    if(fData.depth.empty()){
        return;
    }

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

    // remove after closest point
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
}





auto DCDeviceImpl::update_valid_depth_values() -> void{

    if(fData.depth.empty()){
        return;
    }

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

auto DCDeviceImpl::filter_depth_sized_color_from_depth() -> void{

    auto t = TimeF(timeM, "FILTER_DEPTH_SIZED_COLOR_FROM_DEPTH"sv);

    if(fData.depthSizedColor.empty() || fData.depth.empty()){
        return;
    }

    if(fData.depth.size() != fData.depthSizedColor.size()){
        return;
    }

    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
        if(cFiltersS.invalidateColorFromDepth){
            if(fData.depth[id] == dc_invalid_depth_value){
                fData.depthSizedColor[id] = ColorRGBA8{dc_invalid_color_value};
            }
            // else{
            //     if(cFiltersS.keepOnlyBiggestCluster){
            //         // colorBuffer[meanBiggestZoneId] = ColorRGBA8{255,0,0,255};// TODO ?
            //     }
            // }
        }

        // if(writeBodies){

        //     if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
        //         fData.depthSizedColor[id] = ColorRGBA8{255,0,0,255};
        //     }
        // }
    });
}

auto DCDeviceImpl::filter_infra_from_depth() -> void{

    auto t = TimeF(timeM, "FILTER_INFRA_FROM_DEPTH"sv);

    if(fData.infra.empty() || fData.depth.empty()){
        return;
    }

    if(fData.depth.size() != fData.infra.size()){
        return;
    }

    std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
        if(cFiltersS.invalidateInfraFromDepth){
            if(fData.depth[id] == dc_invalid_depth_value){
                fData.infra[id] = dc_invalid_infra_value;
            }
        }

        // if(writeBodies){
        //     if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
        //         fData.infra[id] = 16000;
        //     }
        // }
    });
}


auto DCDeviceImpl::update_frame_color() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_COLOR"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.color.empty() && cDataS.client.generation.colorImage){
        frame->rgbaColor.width  = mInfos.color_width();
        frame->rgbaColor.height = mInfos.color_height();
        frame->rgbaColor.resize(mInfos.color_size());
        std::copy(fData.color.begin(), fData.color.end(), frame->rgbaColor.begin());
    }else{
        frame->rgbaColor.reset();
    }
}

auto DCDeviceImpl::update_frame_depth_sized_color() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_DEPTH_SIZED_COLOR"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.depthSizedColor.empty() && cDataS.client.generation.depthSizedColorImage){
        frame->rgbaDepthSizedColor.width  = mInfos.depth_width();
        frame->rgbaDepthSizedColor.height = mInfos.depth_height();
        frame->rgbaDepthSizedColor.resize(mInfos.depth_size());
        std::copy(fData.depthSizedColor.begin(), fData.depthSizedColor.end(), frame->rgbaDepthSizedColor.begin());
    }else{
        frame->rgbaDepthSizedColor.reset();
    }
}

auto DCDeviceImpl::update_frame_depth() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_DEPTH"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.depth.empty() && cDataS.client.generation.depth){

        frame->depth.width  = mInfos.depth_width();
        frame->depth.height = mInfos.depth_height();
        frame->depth.resize(mInfos.depth_size());
        std::copy(fData.depth.begin(), fData.depth.end(), frame->depth.begin());

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

            frame->rgbDepth.width  = mInfos.depth_width();
            frame->rgbDepth.height = mInfos.depth_height();
            frame->rgbDepth.resize(mInfos.depth_size());

            std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){

                if(fData.depth[id] == dc_invalid_depth_value){
                    frame->rgbDepth[id] = ColorRGB8{};
                    return;
                }

                float vF = (static_cast<float>(fData.depth[id]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);

                auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                frame->rgbDepth[id] = ColorRGB8{
                    static_cast<std::uint8_t>(255*col.x()),
                    static_cast<std::uint8_t>(255*col.y()),
                    static_cast<std::uint8_t>(255*col.z())
                };
            });
        }else{
            frame->rgbDepth.reset();
        }

    }else{
        frame->depth.reset();
        frame->rgbDepth.reset();
    }
}

auto DCDeviceImpl::update_frame_infra() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_INFRA"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.infra.empty() && cDataS.client.generation.infra){

        frame->infra.width  = mInfos.infra_width();
        frame->infra.height = mInfos.infra_height();
        frame->infra.resize(mInfos.infra_size());
        std::copy(fData.infra.begin(), fData.infra.end(), frame->infra.begin());

        if(cDataS.client.generation.infraImage){

            frame->rgbInfra.width  = mInfos.infra_width();
            frame->rgbInfra.height = mInfos.infra_height();
            frame->rgbInfra.resize(mInfos.infra_size());

            const float max = 2000;
            std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){

                float vF = static_cast<float>(fData.infra[id]);
                if(vF > max){
                    vF = max;
                }
                vF/=max;
                frame->rgbInfra[id] = ColorRGB8{
                    static_cast<std::uint8_t>(255*vF),
                    static_cast<std::uint8_t>(255*vF),
                    static_cast<std::uint8_t>(255*vF)
                };
            });
        }else{
            frame->rgbInfra.reset();
        }

    }else{
        frame->infra.reset();
        frame->rgbInfra.reset();
    }
}

auto DCDeviceImpl::update_frame_cloud() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_CLOUD"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.depth.empty() && !fData.depthCloud.empty() && cDataS.client.generation.cloud){

        frame->cloud.vertices.resize(fData.validDepthValues);
        frame->cloud.colors.resize(fData.validDepthValues);
        frame->cloud.normals.resize(fData.validDepthValues);

        bool addColors = !fData.depthSizedColor.empty() && cDataS.client.generation.cloudColorMode == CloudColorMode::FromDepthSizedColorImage;
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
            frame->cloud.vertices[idV] = fData.depthCloud[idD].template conv<float>()*0.001f;

            if(addColors){
                frame->cloud.colors[idV] = geo::Pt3f{
                    static_cast<float>(fData.depthSizedColor[idD].r()),
                    static_cast<float>(fData.depthSizedColor[idD].g()),
                    static_cast<float>(fData.depthSizedColor[idD].b())
                }/255.f;
            }else{

                float vF = (static_cast<float>(fData.depth[idD]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                frame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            }

            // A B C
            // D I E
            // F G H
            const auto &idN   = fIndices.neighbours8Depth1D[idD];
            const auto &idDVC = fIndices.depthVertexCorrrespondance;
            const auto &v     = frame->cloud.vertices;
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
            frame->cloud.normals[idV] = normalize(normal);
        });

    }else{
        frame->cloud.clear();
    }
}

auto DCDeviceImpl::update_frame_imu() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_IMU"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.binaryIMU.empty() && cDataS.client.generation.imu){
        frame->imu.resize(fData.binaryIMU.size());
        std::copy(fData.binaryIMU.begin(), fData.binaryIMU.end(), frame->imu.begin());
    }else{
        frame->imu.clear();
    }
}

auto DCDeviceImpl::update_frame_bodies() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_BODIES"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.bodies.empty() && cDataS.client.generation.bodyTracking){
        frame->bodyTracking.resize(fData.bodies.size());
        std::copy(fData.bodies.begin(), fData.bodies.end(), frame->bodyTracking.begin());
    }else{
        frame->bodyTracking.clear();
    }


    // bodies id map
    if(!fData.bodiesIdMap.empty() && cDataS.client.generation.bodyIdMapImage){
        frame->grayBodiesIdMap.width  = mInfos.depth_width();
        frame->grayBodiesIdMap.height = mInfos.depth_height();
        frame->grayBodiesIdMap.resize(mInfos.depth_size());
        std::copy(fData.bodiesIdMap.begin(), fData.bodiesIdMap.end(), frame->grayBodiesIdMap.begin());
    }else{
        frame->grayBodiesIdMap.clear();
    }
}

auto DCDeviceImpl::update_frame_calibration() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_CALIBRATION"sv);
    if(frame == nullptr){
        return;
    }

    if(!fData.binaryCalibration.empty() && cDataS.client.generation.calibration){
        frame->calibration.resize(fData.binaryCalibration.size());
        std::copy(fData.binaryCalibration.begin(), fData.binaryCalibration.end(), frame->calibration.begin());
    }else{
        frame->calibration.clear();
    }
}


auto DCDeviceImpl::update_frame_audio() -> void{

    auto t = TimeF(timeM, "UPDATE_FRAME_AUDIO"sv);
    if(frame == nullptr){
        return;
    }

    if(cDataS.client.generation.audio && (fData.audioChannels.first != 0) && !fData.audioChannels.second.empty()){
        size_t nbFrames = fData.audioChannels.second.size() / fData.audioChannels.first;
        frame->audioFrames.resize(nbFrames);
        auto audioFrom = fData.audioChannels.second.data();
        std::copy(audioFrom, audioFrom + fData.audioChannels.second.size(), reinterpret_cast<float*>(frame->audioFrames.data()));
    }else{
        frame->audioFrames.clear();
    }
}

auto DCDeviceImpl::update_compressed_frame_color() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_COLOR"sv);
    if(cFrame == nullptr){
        return;
    }

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
}

auto DCDeviceImpl::update_compressed_frame_depth_sized_color() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_DEPH_SIZED_COLOR"sv);
    if(cFrame == nullptr){
        return;
    }

    if(!fData.depthSizedColor.empty() && cDataS.client.compression.depthSizedColor){

        if(!jpegDepthSizedColorEncoder.encode(
                mInfos.depth_width(),
                mInfos.depth_height(),
                fData.depthSizedColor,
                cFrame->jpegRGBA8DepthSizedColor,
                cDataS.client.compression.jpegCompressionRate
            )){
            cFrame->jpegRGBA8DepthSizedColor.reset();
        }

    }else{
        cFrame->jpegRGBA8DepthSizedColor.reset();
    }
}

auto DCDeviceImpl::update_compressed_frame_depth() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_DEPTH"sv);
    if(cFrame == nullptr){
        return;
    }

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
}

auto DCDeviceImpl::update_compressed_frame_infra() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_INFRA"sv);
    if(cFrame == nullptr){
        return;
    }

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
}

auto DCDeviceImpl::update_compressed_frame_calibration() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_CALIBRATION"sv);
    if(cFrame == nullptr){
        return;
    }

    if(!fData.binaryCalibration.empty() && cDataS.client.compression.calibration){
        cFrame->calibration.resize(fData.binaryCalibration.size());
        std::copy(fData.binaryCalibration.begin(), fData.binaryCalibration.end(), cFrame->calibration.begin());
    }else{
        cFrame->calibration.clear();
    }
}


auto DCDeviceImpl::update_compressed_frame_cloud() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_CLOUD"sv);
    if(cFrame == nullptr){
        return;
    }

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

        bool addColors = !fData.depthSizedColor.empty() && cDataS.client.compression.cloudColorMode == CloudColorMode::FromDepthSizedColorImage;

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
                processedCloudData8[6*fData.validDepthValues + idV] = fData.depthSizedColor[idV].r();
                processedCloudData8[7*fData.validDepthValues + idV] = fData.depthSizedColor[idV].g();
                processedCloudData8[8*fData.validDepthValues + idV] = fData.depthSizedColor[idV].b();
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
}

auto DCDeviceImpl::update_compressed_frame_audio() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_AUDIO"sv);
    if(cFrame == nullptr){
        return;
    }

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
}

auto DCDeviceImpl::update_compressed_frame_imu() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_IMU"sv);
    if(cFrame == nullptr){
        return;
    }

    if(!fData.binaryIMU.empty() && cDataS.client.compression.imu){
        cFrame->imu.resize(fData.binaryIMU.size());
        std::copy(fData.binaryIMU.begin(), fData.binaryIMU.end(), cFrame->imu.begin());
    }else{
        cFrame->imu.clear();
    }
}

auto DCDeviceImpl::update_compressed_frame_bodies() -> void{

    auto t = TimeF(timeM, "UPDATE_COMPRESSED_FRAME_BODIES"sv);
    if(cFrame == nullptr){
        return;
    }

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

auto DCDeviceImpl::get_duration_ms(std::string_view id) -> std::optional<std::chrono::milliseconds>{
    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    if(times.contains(id)){
        return tool::Time::to_ms(times[id].lastDiff);
    }
    return std::nullopt;
}

auto DCDeviceImpl::get_duration_micro_s(std::string_view id) -> std::optional<std::chrono::microseconds>{
    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    if(times.contains(id)){
        return tool::Time::to_micro_s(times[id].lastDiff);
    }
    return std::nullopt;
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

auto DCDeviceImpl::read_data_taskflow() -> std::unique_ptr<tf::Taskflow>{

    auto readDataTF = std::make_unique<tf::Taskflow>();
    auto captureCT = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "CAPTURE_FRAME"sv);
        captureSuccess = capture_frame(mInfos.timeout_ms());
        return captureSuccess ? 1 : 0;
    }).name("capture_frame");

    auto captureFailureT   = readDataTF->emplace([&](){
    }).name("capture_failure");

    auto captureSuccessT   = readDataTF->emplace([&](){
        timeM.start("READ_IMAGES"sv);
    }).name("capture_success");

    auto readColorImageT   = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "READ_COLOR_IMAGE"sv);
        read_color_image(mInfos.has_color() && cDataS.capture_color());
    }).name("read_color_image");

    auto readDepthImageT   = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "READ_DEPTH_IMAGE"sv);
        read_depth_image(mInfos.has_depth() && cDataS.capture_depth());
    }).name("read_depth_image");

    auto readInfraImageT   = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "READ_INFRA_IMAGE"sv);
        read_infra_image(mInfos.has_infra() && cDataS.capture_infra());
    }).name("read_infra_image");

    auto readBodytrackingT = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "READ_BODY_TRACKING"sv);
        read_body_tracking(cDataS.capture_body_tracking() && settings.config.btEnabled && mInfos.has_depth());
    }).name("read_body_tracking");

    auto readAudioT = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "READ_AUDIO"sv);
        read_audio(cDataS.capture_audio() && mInfos.has_audio());
    }).name("read_audio");

    auto readIMUT = readDataTF->emplace([&](){
        auto t = TimeF(timeM, "READ_IMU"sv);
        read_IMU(cDataS.capture_imu());
    }).name("read_imu");

    auto waitForDataCheckT = readDataTF->emplace([&](){
        timeM.end("READ_IMAGES"sv);
        dataIsValid = check_data_validity();
    }).name("wait_for_data");

    captureCT.precede(captureFailureT, captureSuccessT);
    captureSuccessT.precede(
        readColorImageT, readDepthImageT, readInfraImageT,
        readBodytrackingT, readAudioT, readIMUT
    );
    waitForDataCheckT.succeed(
        readColorImageT, readDepthImageT, readInfraImageT,
        readBodytrackingT, readAudioT, readIMUT
    );
    return readDataTF;
}

auto DCDeviceImpl::process_data_taskflow()  -> std::unique_ptr<tf::Taskflow>{

    auto processDataTF = std::make_unique<tf::Taskflow>();
    auto startProcessingDataT = processDataTF->emplace([&](){
        timeM.start("PROCESSING_DATA"sv);
        if(cDataS.client.generation.has_data_to_generate()){
            frame = std::make_shared<DCFrame>();
        }else{
            frame = nullptr;
        }
        if(cDataS.client.compression.has_data_to_compress()){
            cFrame = std::make_shared<DCCompressedFrame>();
        }else{
            cFrame = nullptr;
        }
    }).name("start_processing_data");
    auto convertColorImageT = processDataTF->emplace([&](){        
        convert_color_image();
    }).name("convert_color_image");

    auto resizeColorImageT = processDataTF->emplace([&](){
        auto t = TimeF(timeM, "RESIZE_COLOR_IMAGE"sv);
        resize_color_image_to_depth_size();
    }).name("resize_color_image");

    auto generateCloudT = processDataTF->emplace([&](){
        auto t = TimeF(timeM, "GENERATE_CLOUD"sv);
        generate_cloud(cDataS.generate_cloud_from_client() || cFiltersS.filterDepthWithCloud);
    }).name("generate_cloud");

    auto preprocessColorImageT = processDataTF->emplace([&](){        
        preprocess_color_image();
    }).name("preprocess_color_image");

    auto preprocessDepthSizedColorImageT = processDataTF->emplace([&](){
        preprocess_depth_sized_color_image();
    }).name("preprocess_depth_sized_color_image");

    auto preprocessDepthImageT = processDataTF->emplace([&](){
        preprocess_depth_image();
    }).name("preprocess_depth_image");

    auto preprocessInfraImageT = processDataTF->emplace([&](){
        preprocess_infra_image();
    }).name("preprocess_infra_image");

    auto preprocessCloudImageT = processDataTF->emplace([&](){
        preprocess_cloud_image();
    }).name("preprocess_cloud_image");

    auto preprocessBodytrackingImageT     = processDataTF->emplace([&](){
        preprocess_body_tracking_image();
    }).name("preprocess_body_tracking_image");

    startProcessingDataT.precede(
        convertColorImageT, generateCloudT,
        preprocessColorImageT,preprocessDepthImageT,
        preprocessInfraImageT,preprocessBodytrackingImageT
    );

    convertColorImageT.precede(resizeColorImageT);
    resizeColorImageT.precede(preprocessDepthSizedColorImageT);
    generateCloudT.precede(preprocessCloudImageT);

    auto filterDepthBasicT = processDataTF->emplace([&](){
        timeM.start("FILTER_DEPTH"sv);
        filter_depth_basic();
    }).name("filter_depth_basic");

    auto filterDepthFromDepthSizedColorT = processDataTF->emplace([&](){        
        filter_depth_from_depth_sized_color();
    }).name("filter_depth_from_depth_sized_color");

    auto filterDepthFromInfraT = processDataTF->emplace([&](){        
        filter_depth_from_infra();
    }).name("filter_depth_from_infra");

    auto filterDepthFromBodyTrackingT = processDataTF->emplace([&](){        
        filter_depth_from_body_tracking();
    }).name("filter_depth_from_body_tracking");

    auto filterDepthFromCloudT = processDataTF->emplace([&](){        
        filter_depth_from_cloud();
    }).name("filter_depth_from_cloud");

    auto filterDepthComplexT = processDataTF->emplace([&](){        
        filter_depth_complex();
        update_valid_depth_values();
        timeM.end("FILTER_DEPTH"sv);
    }).name("filter_depth_complex");

    preprocessDepthImageT.precede(filterDepthBasicT);
    filterDepthFromBodyTrackingT.succeed(filterDepthBasicT,         preprocessBodytrackingImageT);
    filterDepthFromInfraT.succeed(filterDepthFromBodyTrackingT,     preprocessInfraImageT);
    filterDepthFromCloudT.succeed(filterDepthFromInfraT,            preprocessCloudImageT);
    filterDepthFromDepthSizedColorT.succeed(filterDepthFromCloudT,  preprocessDepthSizedColorImageT);
    filterDepthComplexT.succeed(filterDepthFromDepthSizedColorT);

    auto filterDepthSizedColorFromDepthT = processDataTF->emplace([&](){        
        filter_depth_sized_color_from_depth();
    }).name("filter_depth_sized_color_from_depth");

    auto filterInfraFromDepthT = processDataTF->emplace([&](){        
        filter_infra_from_depth();
    }).name("filter_infra_from_depth");

    auto mixDepthSizedColorWithBodyTrackingT = processDataTF->emplace([&](){        
        mix_depth_sized_color_with_body_tracking();
    }).name("mix_depth_sized_color_with_body_tracking");

    auto mixInfraWithBodyTrackingT = processDataTF->emplace([&](){
        mix_infra_with_body_tracking();
    }).name("mix_infra_with_body_tracking");

    filterDepthSizedColorFromDepthT.succeed(filterDepthComplexT);
    filterDepthSizedColorFromDepthT.precede(mixDepthSizedColorWithBodyTrackingT);

    filterInfraFromDepthT.succeed(filterDepthComplexT);
    filterInfraFromDepthT.precede(mixInfraWithBodyTrackingT);

    auto updateCompressedFrameColorT = processDataTF->emplace([&](){
        update_compressed_frame_color();
    }).name("update_compressed_frame_color");

    auto updateCompressedFrameDepthSizedColorT = processDataTF->emplace([&](){
        update_compressed_frame_depth_sized_color();
    }).name("update_compressed_frame_depth_sized_color");

    auto updateCompressedFrameDepthT = processDataTF->emplace([&](){
        update_compressed_frame_depth();
    }).name("update_compressed_frame_depth");

    auto updateCompressedFrameInfraT = processDataTF->emplace([&](){
        update_compressed_frame_infra();
    }).name("update_compressed_frame_infra");

    auto updateCompressedFrameCloudT = processDataTF->emplace([&](){
        update_compressed_frame_cloud();
    }).name("update_compressed_frame_cloud");

    auto updateCompressedFrameAudioT = processDataTF->emplace([&](){
        update_compressed_frame_audio();
    }).name("update_compressed_frame_audio");

    auto updateCompressedFrameIMUT = processDataTF->emplace([&](){
        update_compressed_frame_imu();
    }).name("update_compressed_frame_imu");

    auto updateCompressedFrameBodiesT = processDataTF->emplace([&](){
        update_compressed_frame_bodies();
    }).name("update_compressed_frame_bodies");

    auto updateCompressedFrameCalibrationT = processDataTF->emplace([&](){
        update_compressed_frame_calibration();
    }).name("update_compressed_frame_calibration");

    auto finalizedCompressedFrameT = processDataTF->emplace([&](){
        auto t = TimeF(timeM, "FINALIZE_COMPRESSED_FRAME"sv);
        if(cFrame != nullptr){

            // set infos
            cFrame->idDevice           = static_cast<std::uint8_t>(settings.config.idDevice);
            cFrame->idCapture          = static_cast<std::int32_t>(mInfos.id_capture());
            cFrame->afterCaptureTS     = timeM.get_end("CAPTURE_FRAME"sv).count();
            cFrame->receivedTS         = cFrame->afterCaptureTS; // default init received TS with after capture TS
            cFrame->mode               = settings.config.mode;
            cFrame->validVerticesCount = fData.validDepthValues;

            // add frame
            frames.add_compressed_frame(std::move(cFrame));
            cFrame = nullptr;
        }
    }).name("finalize_compressed_frame");

    auto sendCompressedFrameT  = processDataTF->emplace([&](){
        auto t = TimeF(timeM, "SEND_COMPRESSED_FRAME"sv);
        if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), cDelayS.delayMs)){            
            dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
            // auto t1 = Time::nanoseconds_since_epoch();
            // auto t2 = timeM.get_end("CAPTURE_FRAME"sv);
            // std::cout << "-> " << Time::to_micro_s(t1-t2) << "\n";
        }

    }).name("send_compressed_frame");

    updateCompressedFrameColorT.succeed(convertColorImageT);
    updateCompressedFrameDepthSizedColorT.succeed(mixDepthSizedColorWithBodyTrackingT);
    updateCompressedFrameDepthT.succeed(filterDepthComplexT);
    updateCompressedFrameInfraT.succeed(mixInfraWithBodyTrackingT);
    updateCompressedFrameCloudT.succeed(filterDepthComplexT);

    updateCompressedFrameAudioT.succeed(startProcessingDataT);
    updateCompressedFrameCalibrationT.succeed(startProcessingDataT);
    updateCompressedFrameIMUT.succeed(startProcessingDataT);
    updateCompressedFrameBodiesT.succeed(preprocessBodytrackingImageT);

    finalizedCompressedFrameT.succeed(
        updateCompressedFrameColorT, updateCompressedFrameDepthSizedColorT, updateCompressedFrameDepthT,
        updateCompressedFrameInfraT,  updateCompressedFrameCloudT, updateCompressedFrameCalibrationT,
        updateCompressedFrameAudioT, updateCompressedFrameIMUT, updateCompressedFrameBodiesT
    );
    sendCompressedFrameT.succeed(finalizedCompressedFrameT);

    auto updateFrameColorT = processDataTF->emplace([&](){
        update_frame_color();
    }).name("update_frame_color");

    auto updateFrameDepthSizedColorT = processDataTF->emplace([&](){
        update_frame_depth_sized_color();
    }).name("update_frame_depth_sized_color");

    auto updateFrameDepthT = processDataTF->emplace([&](){
        update_frame_depth();
    }).name("update_frame_depth");

    auto updateFrameInfraT = processDataTF->emplace([&](){
        update_frame_infra();
    }).name("update_frame_infra");

    auto updateFrameCloudT = processDataTF->emplace([&](){
        update_frame_cloud();
    }).name("update_frame_cloud");

    auto updateFrameAudioT = processDataTF->emplace([&](){
        update_frame_audio();
    }).name("update_compressed_frame_audio");

    auto updateFrameIMUT = processDataTF->emplace([&](){
        update_frame_imu();
    }).name("update_frame_imu");

    auto updateFrameBodiesT = processDataTF->emplace([&](){
        update_frame_bodies();
    }).name("update_frame_bodies");

    auto updateFrameCalibrationT = processDataTF->emplace([&](){
        update_frame_calibration();
    }).name("update_frame_calibration");

    auto finalizedFrameT = processDataTF->emplace([&](){
        auto t = TimeF(timeM, "FINALIZE_FRAME"sv);
        if(frame != nullptr){

            // set infos
            frame->idCapture       = static_cast<std::int32_t>(mInfos.id_capture());
            frame->afterCaptureTS  = timeM.get_end("CAPTURE_FRAME"sv).count();
            frame->receivedTS      = frame->afterCaptureTS;  // default init received TS with after capture TS
            frame->mode            = settings.config.mode;

            frames.add_frame(std::move(frame));
            frame = nullptr;
        }
    }).name("finalize_frame");

    auto sendFrameT  = processDataTF->emplace([&](){
        auto t = TimeF(timeM, "SEND_FRAME"sv);
        if(auto frameToSend = frames.take_frame_with_delay(timeM.get_end("CAPTURE_FRAME"sv), cDelayS.delayMs)){
            dcDevice->new_frame_signal(std::move(frameToSend));
        }
    }).name("send_frame");

    updateFrameColorT.succeed(convertColorImageT);
    updateFrameDepthSizedColorT.succeed(mixDepthSizedColorWithBodyTrackingT);
    updateFrameDepthT.succeed(filterDepthComplexT);
    updateFrameInfraT.succeed(mixInfraWithBodyTrackingT);
    updateFrameCloudT.succeed(filterDepthComplexT);

    updateFrameAudioT.succeed(startProcessingDataT);
    updateFrameCalibrationT.succeed(startProcessingDataT);
    updateFrameIMUT.succeed(startProcessingDataT);
    updateFrameBodiesT.succeed(preprocessBodytrackingImageT);

    finalizedFrameT.succeed(
        updateFrameColorT, updateFrameDepthSizedColorT, updateFrameDepthT,
        updateFrameInfraT,  updateFrameCloudT, updateFrameCalibrationT,
        updateFrameAudioT, updateFrameIMUT, updateFrameBodiesT
    );
    sendFrameT.succeed(finalizedFrameT);

    auto endProcessingDataT = processDataTF->emplace([&](){
        timeM.end("PROCESSING_DATA"sv);
    }).name("end_processing_data");

    endProcessingDataT.succeed(sendCompressedFrameT, sendFrameT);

    return processDataTF;
}

auto DCDeviceImpl::process_frame_taskflow(tf::Taskflow &readDataTF, tf::Taskflow &processDataTF) -> std::unique_ptr<tf::Taskflow>{

    auto processFrameTF = std::make_unique<tf::Taskflow>();

    auto startProcessingFrameT       = processFrameTF->emplace([&](){
        timeM.start("PROCESS_FRAME"sv);
        fData.reset_spans();
    }).name("start_processing_frame");

    auto updateSettingsT             = processFrameTF->emplace([&](){
        parametersM.lock();
        {
            cFiltersS = settings.filters;
            cDataS    = settings.data;
            cDelayS   = settings.delay;
            times     = timeM.times;
        }
        parametersM.unlock();
        update_from_data_settings();

    }).name("update_settings");

    auto readingDataModuleT = processFrameTF->composed_of(readDataTF).name("reading_data_module");

    auto checkDataCT = processFrameTF->emplace([&](){
        return captureSuccess && dataIsValid;
    }).name("check_data");

    auto failureDataT = processFrameTF->emplace([&](){
        if(captureSuccess){
            mInfos.increment_capture_id();
        }
        timeM.end("PROCESS_FRAME"sv);
    }).name("failure_data");

    auto startProcessingDataModuleT  = processFrameTF->composed_of(processDataTF).name("processing_data_module");

    auto endProcessingFrameT       = processFrameTF->emplace([&](){
        mInfos.increment_capture_id();
    timeM.end("PROCESS_FRAME"sv);
    }).name("end_processing_frame");

    startProcessingFrameT.precede(updateSettingsT);
    updateSettingsT.precede(readingDataModuleT);
    readingDataModuleT.precede(checkDataCT);

    checkDataCT.precede(failureDataT, startProcessingDataModuleT);

    endProcessingFrameT.succeed(startProcessingDataModuleT);

    return processFrameTF;
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





// if(false)
// {
//     TimeM timeM;


//     tf::Executor executor(1);

//     tf::Taskflow mainTaskFlow;
//     std::unique_ptr<DCFrame> nFrame = nullptr;
//     std::unique_ptr<DCCompressedFrame> nCompressedFrame = nullptr;

//     tf::Task updateSettingsT =mainTaskFlow.emplace([&](){
//                                                parametersM.lock();
//                                                // update current settings
//                                                cFiltersS = settings.filters;
//                                                cDataS    = settings.data;
//                                                cDelayS   = settings.delay;
//                                                // reset timestamps
//                                                fTiming.swap_local_timestamps();
//                                                parametersM.unlock();

//                                                update_from_data_settings();

//                                            }).name("update_settings");

//

//     tf::Task convertColorImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "CCI"sv);
//                                                   convert_color_image();
//                                                   fTiming.update_local("after_color_convert"sv);
//                                               }).name("convert_color_image");

//     tf::Task resizeColorImageToDepthSizeT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "RCIDS"sv);
//                                                             resize_color_image_to_depth_size();
//                                                             fTiming.update_local("after_color_resize"sv);
//                                                         }).name("resize_color_image_to_depth_size");

//     tf::Task generateCloudT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "GC"sv);
//                                               generate_cloud(true);
//                                               fTiming.update_local("after_cloud_generation"sv);
//                                           }).name("generate_cloud");

//     tf::Task filterDepthImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FDI"sv);
//                                                  filter_depth_image();
//                                                  fTiming.update_local("after_depth_filter"sv);
//                                              }).name("filter_depth_image");

//     tf::Task filterDepthSizedColorImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FDSCI"sv);
//                                                            filter_depth_sized_color_image();
//                                                            fTiming.update_local("after_color_filter"sv);
//                                                        }).name("filter_depth_sized_color_image");
//     filterDepthSizedColorImageT.priority(tf::TaskPriority::LOW);

//     tf::Task filterInfraImageT = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FII"sv);
//                                                  filter_infrared_image();
//                                                  fTiming.update_local("after_infrared_filter"sv);
//                                              }).name("filter_infrared_image");
//     filterInfraImageT.priority(tf::TaskPriority::LOW);

//     tf::Task filterCloudImage = mainTaskFlow.emplace([&](){ auto t = TimeF(timeM, "FCI"sv);
//                                                 filter_cloud_image();
//                                                 fTiming.update_local("after_cloud_filtering"sv);
//                                             }).name("filter_cloud_image");

//     tf::Task updateCompressedFrameColor = mainTaskFlow.emplace([&](){
//                                                           update_compressed_frame_color(nCompressedFrame.get());
//                                                       }).name("update_compressed_frame_color");
//     tf::Task updateCompressedFrameDepthSizedColor = mainTaskFlow.emplace([&](){
//                                                                     update_compressed_frame_depth_sized_color(nCompressedFrame.get());
//                                                                 }).name("update_compressed_frame_depth_sized_color");
//     tf::Task updateCompressedFrameDepth = mainTaskFlow.emplace([&](){
//                                                           update_compressed_frame_infos(nCompressedFrame.get());
//                                                           update_compressed_frame_depth(nCompressedFrame.get());
//                                                       }).name("update_compressed_frame_depth");
//     tf::Task updateCompressedFrameInfra = mainTaskFlow.emplace([&](){
//                                                           update_compressed_frame_infra(nCompressedFrame.get());
//                                                       }).name("update_compressed_frame_infra");
//     tf::Task updateCompressedFrameCloud = mainTaskFlow.emplace([&](){
//                                                           update_compressed_frame_cloud(nCompressedFrame.get());
//                                                       }).name("update_compressed_frame_cloud");
//     tf::Task updateCompressedFrameAudio = mainTaskFlow.emplace([&](){
//                                                           update_compressed_frame_audio(nCompressedFrame.get());
//                                                       }).name("update_compressed_frame_audio");
//     tf::Task updateCompressedFrameIMU = mainTaskFlow.emplace([&](){
//                                                         update_compressed_frame_imu(nCompressedFrame.get());
//                                                     }).name("update_compressed_frame_imu");
//     tf::Task updateCompressedFrameBodies = mainTaskFlow.emplace([&](){
//                                                            update_compressed_frame_bodies(nCompressedFrame.get());
//                                                        }).name("update_compressed_frame_bodies");
//     tf::Task updateCompressedFrameCalibration = mainTaskFlow.emplace([&](){
//                                                                 update_compressed_frame_calibration(nCompressedFrame.get());
//                                                             }).name("update_compressed_frame_calibration");

//     tf::Task finalizeCompressionT = mainTaskFlow.emplace([&](){
//                                                     frames.add_compressed_frame(std::move(nCompressedFrame));
//                                                     nCompressedFrame = nullptr;
//                                                     fTiming.update_local("after_compressing"sv);
//                                                 }).name("finalize_compression");

//     tf::Task sendCompressedFrameT = mainTaskFlow.emplace([&](){

//                                                     // send with delay
//                                                     if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
//                                                         dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
//                                                         fTiming.update_local("after_compressed_frame_sending"sv);
//                                                     }
//                                                 }).name("send_compressed_frame");;


//     tf::Task updateFrameColor = mainTaskFlow.emplace([&](){
//                                                 update_color(nFrame.get());
//                                             }).name("update_frame_color");
//     updateFrameColor.priority(tf::TaskPriority::NORMAL);
//     tf::Task updateFrameDepth = mainTaskFlow.emplace([&](){
//                                                 update_depth(nFrame.get());
//                                             }).name("update_depth");
//     updateFrameDepth.priority(tf::TaskPriority::NORMAL);
//     tf::Task updateFrameInfra = mainTaskFlow.emplace([&](){
//                                                 update_infra(nFrame.get());
//                                             }).name("update_infra");
//     tf::Task updateFrameDepthSizedColor = mainTaskFlow.emplace([&](){
//                                                           update_depth_sized_color(nFrame.get());
//                                                       }).name("update_depth_sized_color");
//     // tf::Task updateFrameCloud = mainTaskFlow.emplace([&](){
//     //     update_cloud(localData, nFrame.get());
//     // }).name("update_cloud");
//     tf::Task updateFrameCalibration = mainTaskFlow.emplace([&](){
//                                                       update_calibration(nFrame.get());
//                                                   }).name("update_calibration");
//     tf::Task updateFrameAudio = mainTaskFlow.emplace([&](){
//                                                 update_audio(nFrame.get());
//                                             }).name("update_audio");
//     tf::Task updateFrameBodies = mainTaskFlow.emplace([&](){
//                                                  update_bodies(nFrame.get());
//                                              }).name("update_bodies");
//     tf::Task updateFrameIMU = mainTaskFlow.emplace([&](){
//                                               update_imu(nFrame.get());
//                                           }).name("update_imu");


//     tf::Task finalizeLocalT = mainTaskFlow.emplace([&](){
//                                               frames.add_frame(std::move(nFrame));
//                                               nFrame = nullptr;
//                                               fTiming.update_local("after_local_frame_creation"sv);
//                                           }).name("finalize_local");
//     tf::Task sendFrameT = mainTaskFlow.emplace([&](){
//                                           if(auto frameToSend = frames.take_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
//                                               dcDevice->new_frame_signal(std::move(frameToSend));
//                                               fTiming.update_local("after_frame_sending"sv);
//                                           }
//                                       }).name("send_frame");;


//     tf::Task endTask = mainTaskFlow.emplace([&](){
//                                        timeM.end("PRC");
//                                    }).name("end");


//     updateSettingsT.precede(captureCT);
//     captureCT.precede(captureFailureT, captureSuccessT);
//     captureSuccessT.precede(
//         readColorImageT, readDepthImageT, readInfraImageT,
//         readAudioT, readImuT, readBodiesT
//         );
//     readDataCheckCT.succeed(
//         readColorImageT, readDepthImageT, readInfraImageT,
//         readAudioT, readImuT, readBodiesT
//         );
//     readDataCheckCT.precede(readDataFailureT, readDataSuccessT);
//     readDataSuccessT.precede(
//         convertColorImageT, generateCloudT,
//         // compressed
//         updateCompressedFrameCalibration, updateCompressedFrameAudio,
//         updateCompressedFrameIMU, updateCompressedFrameBodies,
//         // local
//         updateFrameCalibration,
//         updateFrameAudio,
//         updateFrameIMU,
//         updateFrameBodies
//         );
//     convertColorImageT.precede(resizeColorImageToDepthSizeT);
//     filterDepthImageT.succeed(resizeColorImageToDepthSizeT);

//     filterDepthImageT.precede(filterDepthSizedColorImageT, filterInfraImageT);
//     filterCloudImage.succeed(generateCloudT, filterDepthImageT);

//     updateCompressedFrameColor.succeed(convertColorImageT);
//     updateCompressedFrameDepthSizedColor.succeed(filterDepthSizedColorImageT);
//     updateCompressedFrameDepth.succeed(filterCloudImage);
//     updateCompressedFrameInfra.succeed(filterInfraImageT);
//     updateCompressedFrameCloud.succeed(filterCloudImage);

//     finalizeCompressionT.succeed(
//         updateCompressedFrameColor, updateCompressedFrameDepthSizedColor, updateCompressedFrameDepth,
//         updateCompressedFrameInfra,  updateCompressedFrameCloud, updateCompressedFrameCalibration,
//         updateCompressedFrameAudio, updateCompressedFrameIMU, updateCompressedFrameBodies
//         );
//     sendCompressedFrameT.succeed(finalizeCompressionT);

//     updateFrameColor.succeed(convertColorImageT);
//     updateFrameDepth.succeed(filterCloudImage);
//     updateFrameInfra.succeed(filterInfraImageT);
//     updateFrameDepthSizedColor.succeed(filterDepthSizedColorImageT);

//     finalizeLocalT.succeed(
//         updateFrameCalibration, updateFrameAudio, updateFrameIMU, updateFrameBodies,
//         updateFrameColor,  updateFrameDepth, updateFrameInfra, updateFrameDepthSizedColor
//         // updateFrameCloud,
//         );
//     sendFrameT.succeed(finalizeLocalT);

//     endTask.succeed(sendCompressedFrameT, sendFrameT);

//     mainTaskFlow.dump(std::cout);


//     while(readFramesFromCameras){
//         // auto tStart = Time::nanoseconds_since_epoch();
//         executor.run(mainTaskFlow).wait();

//         mInfos.increment_capture_id();
//         // timeM.display();
//         // auto tTotal = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
//         // std::cout << "total: " << tTotal.count() << "\n";
//     }

//     return;
// }



// auto DCDeviceImpl::filter_depth_image() -> void{

//     filter_depth_basic();
//     filter_depth_from_depth_sized_color();
//     filter_depth_from_infra();
//     filter_depth_from_cloud();
//     filter_depth_from_body_tracking();
//     filter_depth_complex();
//     update_valid_depth_values();
//     return;

//     if(fData.depth.empty()){
//         return;
//     }
//     if(!fData.depthSizedColor.empty() && (fData.depthSizedColor.size() != fData.depth.size())){
//         return;
//     }
//     if(!fData.infra.empty() && (fData.infra.size() != fData.depth.size())){
//         return;
//     }

//     Bench::start("[DCDeviceData::filter_depth_image]");

//     auto dRange = mInfos.depth_range_mm();
//     auto minW   = mInfos.depth_width()  * cFiltersS.minWidthF;
//     auto maxW   = mInfos.depth_width()  * cFiltersS.maxWidthF;
//     auto minH   = mInfos.depth_height() * cFiltersS.minHeightF;
//     auto maxH   = mInfos.depth_height() * cFiltersS.maxHeightF;

//     auto diffRange = dRange.y()-dRange.x();
//     auto minD = dRange.x() + cFiltersS.minDepthF * diffRange;
//     auto maxD = dRange.x() + cFiltersS.maxDepthF * diffRange;
//     auto hsvDiffColor = Convert::to_hsv(cFiltersS.filterColor);

//     // reset depth mask
//     std::fill(fData.depthMask.begin(), fData.depthMask.end(), 1);

//     // depth/width/height/mask/color/infra filtering
//     std::for_each(std::execution::par_unseq, std::begin(fIndices.depths3D), std::end(fIndices.depths3D), [&](const Pt3<size_t> &dIndex){

//         size_t id = dIndex.x();
//         size_t ii = dIndex.y();
//         size_t jj = dIndex.z();

//         const auto &currentDepth = fData.depth[id];

//         // check validity
//         if(currentDepth == dc_invalid_depth_value){
//             fData.depthMask[id] = 0;
//             return;
//         }

//         // depth filtering
//         if( (ii < minW)  || (ii > maxW)  ||   // width
//             (jj < minH)  || (jj > maxH) ||   // height
//             (currentDepth < minD) || (currentDepth > maxD) ){           // depth
//             fData.depthMask[id] = 0;
//             return;
//         }

//         // color filtering
//         if(!fData.depthSizedColor.empty() && cFiltersS.filterDepthWithColor){

//             auto hsv = Convert::to_hsv(fData.depthSizedColor[id]);
//             if((std::abs(hsv.h()- hsvDiffColor.h()) > cFiltersS.maxDiffColor.x()) ||
//                 (std::abs(hsv.s()- hsvDiffColor.s()) > cFiltersS.maxDiffColor.y()) ||
//                 (std::abs(hsv.v()- hsvDiffColor.v()) > cFiltersS.maxDiffColor.z())){
//                 fData.depthMask[id] = 0;
//                 return;
//             }
//         }

//         // infrared filtering
//         if(!fData.infra.empty() && cFiltersS.filterDepthWithInfra){
//             // ...
//         }

//         // body tracking filtering
//         if(!fData.bodiesIdMap.empty() && cFiltersS.filterDepthWithBodyTracking){
//             // ...
//         }

//         // NFOV sans compartimentation 	640 x 576 	75°x°65 	0, 5, 15, 30 	0,5 - 3,86 m 	12,8 ms
//         // NFOV 2x2 avec compartimentation (SW) 	320 x 288 	75°x°65 	0, 5, 15, 30 	0,5 - 5,46 m 	12,8 ms
//         // WFOV 2x2 avec compartimentation 	512 x 512 	120°x°120 	0, 5, 15, 30 	0,25 - 2,88 m 	12,8 ms
//         // WFOV sans compartimentation 	1024 x 1024 	120°x°120 	0, 5, 15 	0,25 - 2,21 m 	20,3 ms
//         // Infrarouge passif 	1024 x 1024 	N/A 	0, 5, 15, 30 	N/A 	1,6 ms

//         float vfow = 50.f;
//         // deg_2_rad()

// //         def is_point_in_fov(camera_position, camera_orientation, vfov, hfov, point):
// // # Calculate direction vectors
// //             top_left = np.array([np.tan(hfov/2), np.tan(vfov/2), -1])
// //                        top_right = np.array([-np.tan(hfov/2), np.tan(vfov/2), -1])
// //               bottom_left = np.array([np.tan(hfov/2), -np.tan(vfov/2), -1])
// //               bottom_right = np.array([-np.tan(hfov/2), -np.tan(vfov/2), -1])

// // # Transform direction vectors
// //               rotation_matrix = get_rotation_matrix(camera_orientation)
// //             top_left = np.dot(rotation_matrix, top_left)
// //               top_right = np.dot(rotation_matrix, top_right)
// //               bottom_left = np.dot(rotation_matrix, bottom_left)
// //               bottom_right = np.dot(rotation_matrix, bottom_right)

// // # Calculate dot products
// //               dot_top_left = np.dot(top_left, point - camera_position)
// //               dot_top_right = np.dot(top_right, point - camera_position)
// //               dot_bottom_left = np.dot(bottom_left, point - camera_position)
// //               dot_bottom_right = np.dot(bottom_right, point - camera_position)

// // # Check if point is within FOV
// //                   if dot_top_left > 0 and dot_top_right > 0 and dot_bottom_left > 0 and dot_bottom_right > 0:
// //                                                                                        return True
// //                                                                                        else:
// //                                                                                               return False


//     });

//     if(cFiltersS.doLocalDiffFiltering){
//         maximum_local_depth_difference(fIndices, fData.depth, cFiltersS.maxLocalDiff, DCConnectivity::Connectivity_2H);
//     }

//     // minimum neighbours filtering
//     if(cFiltersS.doMinNeighboursFiltering){
//         mininum_neighbours(cFiltersS.minNeighboursLoops, cFiltersS.nbMinNeighbours, DCConnectivity::Connectivity_4);
//     }

//     // erosion
//     if(cFiltersS.doErosion){
//         erode(cFiltersS.erosionLoops, DCConnectivity::Connectivity_8);
//     }

//     // keep only biggest cluster
//     if(cFiltersS.keepOnlyBiggestCluster){
//         keep_only_biggest_cluster();
//     }else{
//         fData.meanBiggestZoneId = 0;
//     }


//     if(cFiltersS.removeAfterClosestPoint){

//         std::uint16_t minDist = std::numeric_limits<std::uint16_t>::max();
//         std::for_each(std::execution::unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
//             if(fData.depthMask[id] != 0){
//                 if(minDist > fData.depth[id]){
//                     minDist = fData.depth[id];
//                 }
//             }
//         });

//         std::uint16_t maxDist = minDist + static_cast<std::uint16_t>(1000.f * cFiltersS.maxDistanceAfterClosestPoint);
//         std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
//             if(fData.depthMask[id] == 0){
//                 return;
//             }

//             if(fData.depth[id] > maxDist){
//                 fData.depthMask[id] = 0;
//             }
//         });
//     }


//     update_valid_depth_values();

//     // for(auto &d : fData.depth){

//     //     // fData.depthMask
//     //     if(d != dc_invalid_depth_value){
//     //         d = 3000;
//     //     }else{
//     //         d = 0;
//     //     }

//     // }

//     Bench::stop();

// }



// auto DCDeviceImpl::filter_depth_sized_color_image() -> void{

//     if(fData.depthSizedColor.empty()){
//         return;
//     }

//     if(!fData.depth.empty() && (fData.depth.size() != fData.depthSizedColor.size())){
//         return;
//     }

//     if(!fData.infra.empty() && (fData.infra.size() != fData.depthSizedColor.size())){
//         return;
//     }

//     bool writeBodies = (fData.bodiesIdMap.size() == fData.depthSizedColor.size()) && true; // TODO

//     Bench::start("[DCDeviceData::filter_color_image]");

//     if(!fData.depth.empty()){

//         std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
//             if(cFiltersS.invalidateColorFromDepth){
//                 if(fData.depth[id] == dc_invalid_depth_value){
//                     fData.depthSizedColor[id] = ColorRGBA8{dc_invalid_color_value};
//                 }else{
//                     if(cFiltersS.keepOnlyBiggestCluster){
//                         // colorBuffer[meanBiggestZoneId] = ColorRGBA8{255,0,0,255};// TODO ?
//                     }
//                 }
//             }

//             if(writeBodies){

//                 if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
//                     fData.depthSizedColor[id] = ColorRGBA8{255,0,0,255};
//                 }
//             }
//         });
//     }

//     Bench::stop();
// }


// auto DCDeviceImpl::filter_infrared_image() -> void{

//     if(fData.infra.empty()){
//         return;
//     }

//     if(!fData.depthSizedColor.empty() && (fData.depthSizedColor.size() != fData.infra.size())){
//         return;
//     }

//     if(!fData.depth.empty() && (fData.depth.size() != fData.infra.size())){
//         return;
//     }

//     bool writeBodies = (fData.bodiesIdMap.size() == fData.infra.size()) && true; // TODO

//     Bench::start("[DCDeviceData::filter_infrared_image]");
//     if(!fData.depth.empty()){
//         std::for_each(std::execution::par_unseq, std::begin(fIndices.depths1D), std::end(fIndices.depths1D), [&](size_t id){
//             if(cFiltersS.invalidateInfraFromDepth){
//                 if(fData.depth[id] == dc_invalid_depth_value){
//                     fData.infra[id] = dc_invalid_infra_value;
//                 }
//             }

//             if(writeBodies){
//                 if(fData.bodiesIdMap[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
//                     fData.infra[id] = 16000;
//                 }
//             }
//         });
//     }
//     Bench::stop();
// }

// auto DCDeviceImpl::filter_cloud_image() -> void{

//     if(fData.depthCloud.empty() || fData.depth.empty() || !cFiltersS.filterDepthWithCloud){
//         return;
//     }

//     if(cFiltersS.p1FMode != PlaneFilteringMode::None){

//         auto p1             = cFiltersS.p1A*1000.f;
//         auto p2             = cFiltersS.p1B*1000.f;
//         auto p3             = cFiltersS.p1C*1000.f;
//         geo::Pt3f meanPt    = (p1+p2+p3)/3.f;
//         auto AB             = vec(p2,p1);
//         auto AC             = vec(p3,p1);
//         auto normalV        = cross(AB,AC);
//         normalV             = normalize(normalV);

//         std::for_each(std::execution::par_unseq, std::begin(fIndices.depthVertexCorrrespondance), std::end(fIndices.depthVertexCorrrespondance), [&](auto idC){

//             auto idD = std::get<0>(idC);
//             if(fData.depth[idD] == dc_invalid_depth_value){
//                 return;
//             }

//             if(dot(normalV,vec(meanPt, fData.depthCloud[idD].template conv<float>())) < 0){
//                 if(cFiltersS.p1FMode == PlaneFilteringMode::Above){
//                     fData.depthMask[idD] = 0;
//                     return;
//                 }
//             }else{
//                 if(cFiltersS.p1FMode == PlaneFilteringMode::Below){
//                     fData.depthMask[idD] = 0;
//                     return;
//                 }
//             }
//         });
//     }

//     update_valid_depth_values();
// }



// auto DCDeviceImpl::read_frames() -> void {

//     // start loop
//     readFramesFromCameras = true;
//     fTiming.reset();

//     read_calibration();

//     // auto t1 = Time::milliseconds_since_epoch();

//     while(readFramesFromCameras){

//         parametersM.lock();
//         // update current settings
//         cFiltersS = settings.filters;
//         cDataS    = settings.data;
//         cDelayS   = settings.delay;
//         // reset timestamps
//         fTiming.swap_local_timestamps();
//         parametersM.unlock();

//         update_from_data_settings();

//         // capture frame
//         try {

//             fTiming.update_local("before_capture"sv);
//             bool frameSuccess = capture_frame(mInfos.timeout_ms());
//             fTiming.update_local("after_capture"sv);

//             if(!frameSuccess){ // likely timeout
//                 // std::this_thread::sleep_for(std::chrono::milliseconds(5));
//                 continue;
//             }

//         }   catch (std::exception &error) {
//             Logger::error(std::format("[DCDeviceData::read_frames] Get capture error: {}\n", error.what()));
//             readFramesFromCameras = false;
//             break;
//         }

//         // read data
//         read_audio(cDataS.capture_audio() && mInfos.has_audio());
//         read_IMU(cDataS.capture_imu());
//         read_body_tracking(cDataS.capture_body_tracking() && settings.config.btEnabled && mInfos.has_depth());

//         read_color_image(mInfos.has_color() && cDataS.capture_color());
//         read_depth_image(mInfos.has_depth() && cDataS.capture_depth());
//         read_infra_image(mInfos.has_infra() && cDataS.capture_infra());

//         fTiming.update_local("after_get_images"sv);

//         if(!check_data_validity()){
//             continue;
//         }

//         fTiming.compute_capture_framerate();

//         // process
//         convert_color_image();
//         fTiming.update_local("after_color_convert"sv);

//         resize_color_image_to_depth_size();
//         fTiming.update_local("after_color_resize"sv);

//         generate_cloud(cDataS.generate_cloud_from_client() || cFiltersS.filterDepthWithCloud);
//         fTiming.update_local("after_cloud_generation"sv);

//         filter_depth_basic();
//         filter_depth_from_depth_sized_color();
//         filter_depth_from_infra();
//         filter_depth_from_cloud();
//         filter_depth_from_body_tracking();
//         filter_depth_complex();
//         update_valid_depth_values();
//         fTiming.update_local("after_depth_filter"sv);

//         filter_depth_sized_color_from_depth();
//         fTiming.update_local("after_color_filter"sv);

//         filter_infra_from_depth();
//         fTiming.update_local("after_infrared_filter"sv);

//         fTiming.update_local("after_processing"sv);

//         if(!readFramesFromCameras){
//             break;
//         }

//         // compressed frame
//         if(cDataS.client.compression.has_data_to_compress()){

//             // generate
//             fTiming.update_local("before_compressing"sv);
//             auto compressedFrame = compress_frame();
//             frames.add_compressed_frame(std::move(compressedFrame));
//             fTiming.update_local("after_compressing"sv);

//             // send with delay
//             if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
//                 dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
//                 fTiming.update_local("after_compressed_frame_sending"sv);
//             }
//         }

//         // local frame
//         if(cDataS.client.generation.has_data_to_generate()){

//             // generate
//             fTiming.update_local("before_local_frame_creation"sv);
//             auto dFrame = create_local_frame();
//             frames.add_frame(std::move(dFrame));
//             fTiming.update_local("after_local_frame_creation"sv);

//             // send with delay
//             if(auto frameToSend = frames.take_frame_with_delay(fTiming.get_local("after_capture"sv), cDelayS.delayMs)){
//                 dcDevice->new_frame_signal(std::move(frameToSend));
//                 fTiming.update_local("after_frame_sending"sv);
//             }
//         }

//         mInfos.increment_capture_id();
//     }
// }


// auto DCDeviceImpl::create_local_frame() -> std::unique_ptr<DCFrame>{

//     tool::Bench::start("[DCDeviceImpl::create_local_frame]");

//     auto dFrame  = std::make_unique<DCFrame>();
//     update_frame_infos(dFrame.get());
//     update_frame_color(dFrame.get());
//     update_frame_depth_sized_color(dFrame.get());
//     update_frame_depth(dFrame.get());
//     update_frame_infra(dFrame.get());
//     update_frame_cloud(dFrame.get());
//     update_frame_bodies(dFrame.get());
//     update_frame_calibration(dFrame.get());
//     update_frame_imu(dFrame.get());
//     update_frame_audio(dFrame.get());

//     tool::Bench::stop();

//     return dFrame;
// }



// auto DCDeviceImpl::compress_frame() -> std::unique_ptr<DCCompressedFrame>{

//     tool::Bench::start("[DCDeviceImpl::compress_frame]");

//     auto cFrame = std::make_unique<DCCompressedFrame>();
//     update_compressed_frame_infos(cFrame.get());
//     update_compressed_frame_color(cFrame.get());
//     update_compressed_frame_depth_sized_color(cFrame.get());
//     update_compressed_frame_depth(cFrame.get());
//     update_compressed_frame_infra(cFrame.get());
//     update_compressed_frame_cloud(cFrame.get());
//     update_compressed_frame_audio(cFrame.get());
//     update_compressed_frame_imu(cFrame.get());
//     update_compressed_frame_bodies(cFrame.get());
//     update_compressed_frame_calibration(cFrame.get());

//     tool::Bench::stop();

//     return cFrame;
// }
