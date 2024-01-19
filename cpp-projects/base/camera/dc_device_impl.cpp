
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

// local
#include "utility/benchmark.hpp"
#include "utility/time.hpp"
#include "utility/logger.hpp"

using namespace tool::geo;
using namespace tool::camera;

auto DCInfos::initialize(DCMode mode) -> void{

    idCapture               = 0;
    initialColorResolution         = color_resolution(mode);
    imageFormat             = image_format(mode);
    depthMode               = depth_resolution(mode);
    initialDepthResolution  = depth_resolution(mode);
    fps                     = framerate(mode);
    timeoutMs               = 0;
    switch (fps) {
    case DCFramerate::F30:
        timeoutMs = 40;
        break;
    case DCFramerate::F15:
        timeoutMs = 70;
        break;
    case DCFramerate::F5:
        timeoutMs = 250;
        break;
    default:
        timeoutMs = 40;
        break;
    }

    // retrieve colors dimensions
    if(initialColorResolution != DCColorResolution::OFF){
        colorWidth      = color_width(initialColorResolution);
        colorHeight     = color_height(initialColorResolution);
        colorSize       = colorWidth * colorHeight;
    }else{
        colorWidth      = 0;
        colorHeight     = 0;
        colorSize       = 0;
    }

    // retrieve depth dimensions
    auto dev = get_device(mode);
    if(dev == DCType::FemtoOrbbec){

        if(depthMode != DCDepthResolution::K4_OFF){
            if(initialColorResolution != DCColorResolution::OFF){
                // depth always aligned to color
                depthWidth      = colorWidth;
                depthHeight     = colorHeight;
                depthSize       = colorSize;

                auto depthRes   = depth_resolution(mode);
                infraWidth      = depth_width(depthRes);
                infraHeight     = depth_height(depthRes);
                infraSize       = infraWidth * infraHeight;

            }else{
                auto depthRes   = depth_resolution(mode);
                depthWidth      = depth_width(depthRes);
                depthHeight     = depth_height(depthRes);
                depthSize       = depthWidth * depthHeight;
                infraWidth      = depthWidth;
                infraHeight     = depthHeight;
                infraSize       = depthSize;
            }

        }else{
            depthWidth      = 0;
            depthHeight     = 0;
            depthSize       = 0;
            infraWidth      = 0;
            infraHeight     = 0;
            infraSize       = 0;
        }
    }else if(dev == DCType::AzureKinect){
        if(depthMode != DCDepthResolution::K4_OFF){
            auto depthRes   = depth_resolution(mode);
            depthWidth      = depth_width(depthRes);
            depthHeight     = depth_height(depthRes);
            depthSize       = depthWidth * depthHeight;
            infraWidth      = depthWidth;
            infraHeight     = depthHeight;
            infraSize       = depthSize;
        }else{
            depthWidth      = 0;
            depthHeight     = 0;
            depthSize       = 0;
            infraWidth      = 0;
            infraHeight     = 0;
            infraSize       = 0;
        }
    }

    std::cout << "COLOR " << colorWidth << " " << colorHeight << " " << colorSize << "\n";
    std::cout << "DEPTH " << depthWidth << " " << depthHeight << " " << depthSize << "\n";
    std::cout << "INFRA " << infraWidth << " " << infraHeight << " " << infraSize << "\n";
    std::cout << "FPS" << framerate_value(fps) << "\n";
    std::cout << "FORMAT: " << (int)imageFormat << "\n";

}

auto DCIndices::initialize(const DCInfos &infos) -> void{

    // set color indices
    if(infos.initialColorResolution != DCColorResolution::OFF){

        colors1D.resize(infos.colorSize);
        std::iota(std::begin(colors1D), std::end(colors1D), 0);
    }else{
        colors1D.clear();
    }

    // set depth indices
    if(infos.depthMode != DCDepthResolution::K4_OFF){

        // depthMask.resize(depthSize);
        // filteringMask.resize(depthSize);
        // depthFiltering.resize(depthSize);
        // zonesId.resize(depthSize);        
        infras1D.resize(infos.infraSize);
        std::iota(std::begin(infras1D), std::end(infras1D), 0);

        depths1D.resize(infos.depthSize);
        std::iota(std::begin(depths1D), std::end(depths1D), 0);

        depthVertexCorrrespondance.resize(infos.depthSize);

        depthsSortedCorrespondanceNoBorders.resize((infos.depthWidth-2)*(infos.depthHeight-2));


        depths3D.resize(infos.depthSize);

        depths1DNoBorders.clear();
        depths1DNoBorders.reserve((infos.depthWidth-2)*(infos.depthHeight-2));

        neighbours4Depth1D.clear();
        neighbours4Depth1D.reserve(infos.depthSize);

        neighbours8Depth1D.clear();
        neighbours8Depth1D.reserve(infos.depthSize);

        size_t id = 0;
        for(size_t ii = 0; ii < infos.depthHeight; ++ii){
            for(size_t jj = 0; jj < infos.depthWidth; ++jj){

                depths3D[id] = {id,jj,ii};

                // A B C
                // D I E
                // F G H
                std::int32_t idA = -1;
                std::int32_t idD = -1;
                std::int32_t idF = -1;
                std::int32_t idC = -1;
                std::int32_t idE = -1;
                std::int32_t idH = -1;
                std::int32_t idB = -1;
                std::int32_t idG = -1;

                bool notOnLeft   = jj > 0;
                bool notOnRight  = jj < infos.depthWidth - 1;
                bool notOnTop    = ii > 0;
                bool notOnBottom = ii < infos.depthHeight-1;

                if(notOnLeft){
                    idD = id - 1;
                    if(notOnTop){
                        idA = id - infos.depthWidth-1;
                    }
                    if(notOnBottom){
                        idF = id + infos.depthWidth-1;
                    }
                }
                if(notOnRight){
                    idE = id + 1;
                    if(notOnTop){
                        idC = id - infos.depthWidth + 1;
                    }
                    if(notOnBottom){
                        idH = id + infos.depthWidth + 1;
                    }
                }
                if(notOnTop){
                    idB = id - infos.depthWidth;
                }
                if(notOnBottom){
                    idG = id + infos.depthWidth;
                }

                neighbours4Depth1D.push_back({idB,idD,idE,idG});
                neighbours8Depth1D.push_back({idA,idB,idC,idD,idE,idF,idG,idH});

                if(notOnLeft && notOnRight && notOnTop && notOnBottom){
                    depths1DNoBorders.push_back(id);
                }
                ++id;
            }
        }
    }else{
        depths1D.clear();
        depths1DNoBorders.clear();
        neighbours4Depth1D.clear();
        neighbours8Depth1D.clear();
        depthVertexCorrrespondance.clear();
        depthsSortedCorrespondanceNoBorders.clear();
        depths3D.clear();
    }
}



auto DCTiming::reset() -> void{
    colorImageTS    = std::chrono::nanoseconds{0};
    depthImageTS    = std::chrono::nanoseconds{0};
    infraredImageTS = std::chrono::nanoseconds{0};
    capturesTimes.clear();
    timestamps.clear();
    localTimestamps.clear();
}

auto DCTiming::swap_local_timestamps() -> void{
    for(auto &ts : localTimestamps){
        timestamps[ts.first]    = ts.second;
        ts.second               = std::nullopt;
    }
}

auto DCTiming::update_local(std::string_view name) -> void{
    localTimestamps[name] = Time::nanoseconds_since_epoch();
}

auto DCTiming::get_local(std::string_view name) const -> std::chrono::nanoseconds{
    if(localTimestamps.contains(name)){
        if(localTimestamps.at(name).has_value()){
            return localTimestamps.at(name).value();
        }
    }
    return std::chrono::nanoseconds{0};
}

auto DCTiming::compute_capture_framerate() -> void{

    if(!localTimestamps.contains("after_capture"sv)){
        return;
    }

    auto aftertCatpureTs = localTimestamps["after_capture"sv].value();
    capturesTimes.push_back(aftertCatpureTs);
    bool foundT = false;
    size_t idT = 0;
    for(size_t ii = 0; ii < capturesTimes.size(); ++ii){
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(aftertCatpureTs - capturesTimes[ii]);
        if(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() > 5000){
            foundT = true;
            idT = ii;
        }else{
            break;
        }
    }
    if(foundT){
        tool::erase_range(capturesTimes, 0, idT + 1);
    }
    nbCapturePerSecond = capturesTimes.size()/5.f;
}

auto DCTiming::get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::milliseconds>{
    if(timestamps.contains(from) && timestamps.contains(to)){
        if(timestamps.at(from).has_value() && timestamps.at(to).has_value()){
            return std::chrono::duration_cast<std::chrono::milliseconds>(timestamps.at(to).value()-timestamps.at(from).value());
        }
        return std::nullopt;
    }
    return std::nullopt;
}

auto DCTiming::get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::microseconds>{
    if(timestamps.contains(from) && timestamps.contains(to)){
        if(timestamps.at(from).has_value() && timestamps.at(to).has_value()){
            return std::chrono::duration_cast<std::chrono::microseconds>(timestamps.at(to).value()-timestamps.at(from).value());
        }
        return std::nullopt;
    }
    return std::nullopt;
}

auto DCDeviceImpl::initialize() -> void{

    infos.initialize(settings.config.mode);
    indices.initialize(infos);

    if(infos.depthMode != DCDepthResolution::K4_OFF){
        depthMask.resize(infos.depthSize);
        filteringMask.resize(infos.depthSize);
        depthFiltering.resize(infos.depthSize);
        zonesId.resize(infos.depthSize);
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

    auto dev = get_device(settings.config.mode);

    // start loop
    readFramesFromCameras = true;
    timing.reset();

    while(readFramesFromCameras){

        parametersM.lock();
        auto cFiltersS = settings.filters;
        auto cDataS    = settings.data;
        auto cDelayS   = settings.delay;
        timing.swap_local_timestamps();
        parametersM.unlock();

        // read data from device
        try {

            timing.update_local("before_capture"sv);
            bool success = capture_frame(infos.timeoutMs);
            timing.update_local("after_capture"sv);

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

            if(!success){
                continue;
            }

            if(cDataS.captureBodies){
                read_bodies();
            }

            timing.compute_capture_framerate();

        }   catch (std::exception &error) {
            Logger::error(std::format("[DCDeviceData::read_frames] Get capture error: {}\n", error.what()));
            readFramesFromCameras = false;
            break;
        }


        // get images
        read_color_image();
        read_depth_image();
        read_infra_image();
        timing.update_local("after_get_images"sv);

        // process
        convert_color_image();
        timing.update_local("after_color_convert"sv);

        resize_images();
        timing.update_local("after_color_resize"sv);

        filter_depth_image(cFiltersS,depth_data(), color_data(), infra_data());
        timing.update_local("after_depth_filter"sv);

        filter_color_image(cFiltersS);
        timing.update_local("after_color_filter"sv);

        filter_infrared_image(cFiltersS);
        timing.update_local("after_infrared_filter"sv);

        if(cDataS.generateCloudLocal || cFiltersS.p1FMode != DCFiltersSettings::PlaneFilteringMode::None){
            generate_cloud();
            timing.update_local("after_cloud_generation"sv);

            // filter from geometry
            filter_cloud_image(cFiltersS);
            timing.update_local("after_cloud_filtering"sv);
        }
        timing.update_local("after_processing"sv);

        if(!readFramesFromCameras){
            break;
        }

        bool hasDataToSend = false;
        if(dev == DCType::FemtoOrbbec){
            hasDataToSend = cDataS.has_data_to_send() || cDataS.sendIMU || cDataS.sendBodies;
        }else if (dev == DCType::AzureKinect){
            hasDataToSend = cDataS.has_data_to_send() || cDataS.sendIMU || cDataS.sendAudio || cDataS.sendBodies;
        }

        // compressed frame
        if(sendData && hasDataToSend){

            // generate
            auto compressedFrame = compress_frame(cFiltersS,cDataS);
            timing.update_local("after_compressing"sv);
            frames.add_compressed_frame(std::move(compressedFrame));

            // send with delay
            if(auto compressedFrameToSend = frames.get_compressed_frame_with_delay(timing.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_compressed_frame_signal(std::move(compressedFrameToSend));
                timing.update_local("after_compressed_frame_sending"sv);
            }
        }

        // local frame
        if(cDataS.generateRGBLocalFrame || cDataS.generateDepthLocalFrame || cDataS.generateInfraLocalFrame || cDataS.generateCloudLocal){

            // generate
            auto dFrame = create_local_frame(cDataS);
            timing.update_local("after_local_frame_creation"sv);
            frames.add_frame(std::move(dFrame));

            // send with delay
            if(auto frameToSend = frames.take_frame_with_delay(timing.get_local("after_capture"sv), cDelayS.delayMs)){
                dcDevice->new_frame_signal(std::move(frameToSend));
                timing.update_local("after_frame_sending"sv);
            }
        }
        infos.idCapture++;
    }
}

auto DCDeviceImpl::filter_depth_image(const DCFiltersSettings &filtersS, std::span<uint16_t> depthBuffer, std::span<ColorRGBA8> colorBuffer, std::span<uint16_t> infraBuffer) -> void{

    static_cast<void>(infraBuffer); // not used yet

    if(depthBuffer.empty()){
        return;
    }

    if(!colorBuffer.empty() && (colorBuffer.size() != depthBuffer.size())){
        return;
    }

    // Bench::start("[DCDeviceData::filter_depth_image]");

    const auto dRange = depth_range(settings.config.mode)*1000.f;

    auto minW = infos.depthWidth  * filtersS.minWidthF;
    auto maxW = infos.depthWidth  * filtersS.maxWidthF;
    auto minH = infos.depthHeight * filtersS.minHeightF;
    auto maxH = infos.depthHeight * filtersS.maxHeightF;

    auto diffRange = dRange.y()-dRange.x();
    auto minD = dRange.x() + filtersS.minDepthF * diffRange;
    auto maxD = dRange.x() + filtersS.maxDepthF * diffRange;
    auto hsvDiffColor = Convert::to_hsv(filtersS.filterColor);

    // reset depth mask
    std::fill(depthMask.begin(), depthMask.end(), 1);

    // depth/width/height/mask/color/infra filtering
    std::for_each(std::execution::par_unseq, std::begin(indices.depths3D), std::end(indices.depths3D), [&](const Pt3<size_t> &dIndex){

        size_t id = dIndex.x();
        size_t ii = dIndex.y();
        size_t jj = dIndex.z();

        const auto &currentDepth = depthBuffer[id];

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
        if(!colorBuffer.empty() && filtersS.filterDepthWithColor){

            auto hsv = Convert::to_hsv(colorBuffer[id]);
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
        maximum_local_depth_difference(depthBuffer, filtersS.maxLocalDiff, Connectivity::Connectivity_4);
    }

    // minimum neighbours filtering
    if(filtersS.doMinNeighboursFiltering){
        mininum_neighbours(filtersS.minNeighboursLoops, filtersS.nbMinNeighbours, Connectivity::Connectivity_4);
    }

    // erosion
    if(filtersS.doErosion){
        erode(filtersS.erosionLoops, Connectivity::Connectivity_8);
    }

    // keep only biggest cluster
    if(filtersS.keepOnlyBiggestCluster){
        keep_only_biggest_cluster();
    }else{
        meanBiggestZoneId = 0;
    }



    // count valid depth values
    validDepthValues = 0;
    for_each(std::execution::unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){
        if(depthMask[id] == 0){
            depthBuffer[id] = dc_invalid_depth_value;
            indices.depthVertexCorrrespondance[id] = {id, -1};
        }else{
            indices.depthVertexCorrrespondance[id] = {id, validDepthValues};
            validDepthValues++;
        }
    });
    // Bench::stop();


}

auto DCDeviceImpl::filter_color_image(const DCFiltersSettings &filtersS) -> void{

    std::span<ColorRGBA8> colorBuffer = color_data();
    std::span<uint16_t> infraBuffer = infra_data();
    std::span<uint16_t> depthBuffer = depth_data();
    std::span<uint8_t> bodiesBuffer = bodies_index_data();

    if(colorBuffer.empty()){
        return;
    }

    if(!depthBuffer.empty() && (depthBuffer.size() != colorBuffer.size())){
        return;
    }

    if(!infraBuffer.empty() && (infraBuffer.size() != colorBuffer.size())){
        return;
    }

    bool writeBodies = (bodiesBuffer.size() == colorBuffer.size()) && true; // TODO

    Bench::start("[DCDeviceData::filter_color_image]");

    if(!depthBuffer.empty()){

        std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){
            if(filtersS.invalidateColorFromDepth){
                if(depthBuffer[id] == dc_invalid_depth_value){
                    colorBuffer[id] = ColorRGBA8{dc_invalid_color_value};
                }else{
                    if(filtersS.keepOnlyBiggestCluster){
                        // colorBuffer[meanBiggestZoneId] = ColorRGBA8{255,0,0,255};// TODO ?
                    }                                       
                }                                
            }

            if(writeBodies){
                if(bodiesBuffer[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    colorBuffer[id] = ColorRGBA8{255,0,0,255};
                }
            }
        });
    }

    Bench::stop();
}

auto DCDeviceImpl::filter_infrared_image(const DCFiltersSettings &filtersS) -> void{

    std::span<ColorRGBA8> colorBuffer = color_data();
    std::span<uint16_t> infraBuffer = infra_data();
    std::span<uint16_t> depthBuffer = depth_data();
    std::span<uint8_t> bodiesBuffer = bodies_index_data();


    if(infraBuffer.empty()){
        return;
    }

    if(!colorBuffer.empty() && (colorBuffer.size() != infraBuffer.size())){
        return;
    }

    if(!depthBuffer.empty() && (depthBuffer.size() != infraBuffer.size())){
        return;
    }

    bool writeBodies = (bodiesBuffer.size() == infraBuffer.size()) && true; // TODO

    Bench::start("[DCDeviceData::filter_infrared_image]");
    if(!depthBuffer.empty()){
        std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){
            if(filtersS.invalidateInfraFromDepth){
                if(depthBuffer[id] == dc_invalid_depth_value){
                    infraBuffer[id] = dc_invalid_infra_value;
                }
            }


            if(writeBodies){
                if(bodiesBuffer[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    infraBuffer[id] = 16000;
                }
            }
        });
    }
    Bench::stop();
}

auto DCDeviceImpl::update_valid_depth_values() -> void{

    auto depthBuffer  = depth_data();

    // count valid depth values
    validDepthValues = 0;
    for_each(std::execution::unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){
        if(depthMask[id] == 0){
            depthBuffer[id] = dc_invalid_depth_value;
            indices.depthVertexCorrrespondance[id] = {id, -1};
        }else{
            indices.depthVertexCorrrespondance[id] = {id, validDepthValues};
            validDepthValues++;
        }
    });
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



auto DCDeviceImpl::maximum_local_depth_difference(std::span<uint16_t> depthBuffer, float max, Connectivity connectivity) -> void{

    std::fill(filteringMask.begin(),filteringMask.end(), 0);

    std::for_each(std::execution::par_unseq, std::begin(indices.depths1DNoBorders), std::end(indices.depths1DNoBorders), [&](size_t id){

        if(depthMask[id] == 0){
            return;
        }

        float meanDiff = 0;
        float currDepth = depthBuffer[id];
        size_t count = 0;
        if(connectivity == Connectivity::Connectivity_4){
            for(auto cId : indices.neighbours4Depth1D[id]){
                if(depthMask[cId] == 1){
                    meanDiff += abs(depthBuffer[cId]-currDepth);
                    ++count;
                }
            }
        }else{
            for(auto cId : indices.neighbours8Depth1D[id]){
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
    for_each(std::execution::unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t pt){

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

                if(id >= infos.depthWidth-1){

                    ids.push(id-infos.depthWidth+1);

                    if(id >= infos.depthWidth){

                        ids.push(id-infos.depthWidth);

                        if(id >= infos.depthWidth + 1){
                            ids.push(id-infos.depthWidth-1);
                        }
                    }
                }
            }

            if(id < infos.depthSize-1){

                ids.push(id+1);

                if(id < infos.depthSize - infos.depthWidth+1){
                    ids.push(id+infos.depthWidth-1);

                    if(id < infos.depthSize -infos.depthWidth){
                        ids.push(id+infos.depthWidth);

                        if(id < infos.depthSize -infos.depthWidth-1){
                            ids.push(id+infos.depthWidth+1);
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
        std::for_each(std::execution::unseq, std::begin(indices.depths3D), std::end(indices.depths3D), [&](const auto &pt){
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
        meanBiggestZoneId = meanBiggestZone.y() * infos.depthWidth + meanBiggestZone.x();
    }

}

auto DCDeviceImpl::mininum_neighbours(uint8_t nbLoops, uint8_t nbMinNeighbours, Connectivity connectivity) -> void{

    for(std::uint8_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        // reset filtering mask
        std::fill(filteringMask.begin(),filteringMask.end(), 0);

        std::for_each(std::execution::par_unseq, std::begin(indices.depths1DNoBorders), std::end(indices.depths1DNoBorders), [&](size_t id){

            if(depthMask[id] == 0){
                return;
            }

            std::uint8_t count = 0;
            if(connectivity == Connectivity::Connectivity_4){
                for(auto cId : indices.neighbours4Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : indices.neighbours8Depth1D[id]){

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

auto DCDeviceImpl::erode(uint8_t nbLoops, Connectivity connectivity) -> void{


    for(size_t numLoop = 0; numLoop < nbLoops; ++numLoop){

        std::fill(filteringMask.begin(),filteringMask.end(), 0);

        std::for_each(std::execution::par_unseq, std::begin(indices.depths1DNoBorders), std::end(indices.depths1DNoBorders), [&](size_t id){

            if(depthMask[id] == 0){
                return;
            }

            uint8_t count = 0;
            if(connectivity == Connectivity::Connectivity_4){
                for(auto cId : indices.neighbours4Depth1D[id]){

                    if(depthMask[cId] == 1){
                        ++count;
                    }
                }
                filteringMask[id] = count == 4 ? 1 : 0;
            }else{
                for(auto cId : indices.neighbours8Depth1D[id]){

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
    return timing.get_duration_between_ms(from, to);
}

auto DCDeviceImpl::get_duration_between_micro_s(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::microseconds>{

    std::unique_lock<std::mutex> lock(parametersM, std::try_to_lock);
    if(!lock.owns_lock()){
        return std::nullopt;
    }
    return timing.get_duration_between_micro_s(from, to);
}



auto DCFrames::add_frame(std::shared_ptr<DCFrame> frame) -> void{
    // store frame
    auto afterCaptureTS = std::chrono::nanoseconds(frame->afterCaptureTS);
    frames.push_back(std::make_tuple(afterCaptureTS, std::move(frame)));
}

auto DCFrames::add_compressed_frame(std::shared_ptr<DCCompressedFrame> cFrame) -> void{
    // store frame
    auto afterCaptureTS = std::chrono::nanoseconds(cFrame->afterCaptureTS);
    compressedFrames.push_back(std::make_tuple(afterCaptureTS, std::move(cFrame)));
}

auto DCFrames::take_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, int64_t delayMs) -> std::shared_ptr<DCFrame>{

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

auto DCFrames::get_compressed_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, int64_t delayMs) -> std::shared_ptr<DCCompressedFrame>{

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
