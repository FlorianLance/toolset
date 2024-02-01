

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "dc_frame.hpp"

// local
#include "utility/time.hpp"
#include "utility/vector.hpp"

using namespace tool;
using namespace tool::cam;

auto DCFrameData::reset() -> void{

    rawColor        = {};
    color           = {};
    dephtSizedColor = {};
    depth           = {};
    infra           = {};
    depthCloud      = {};
    bodiesIdDepth   = {};
    audioChannels   = {0,{}};
    imuSample       = std::nullopt;
    bodies.clear();
}

auto DCFrameIndices::initialize(const DCModeInfos &infos) -> void{

    // set color indices
    if(infos.has_color()){
        colors1D.resize(infos.color_size());
        std::iota(std::begin(colors1D), std::end(colors1D), 0);
    }else{
        colors1D.clear();
    }

    // set depth indices
    if(infos.has_depth()){

        // resize
        // # 1D
        depths1D.resize(infos.depth_size());
        depths1DNoBorders.clear();
        depths1DNoBorders.reserve((infos.depth_width()-2)*(infos.depth_height()-2));
        neighbours4Depth1D.clear();
        neighbours4Depth1D.reserve(infos.depth_size());
        neighbours8Depth1D.clear();
        neighbours8Depth1D.reserve(infos.depth_size());
        // # 3D
        depths3D.resize(infos.depth_size());
        // # correspondance
        depthVertexCorrrespondance.resize(infos.depth_size());
        // depthsSortedCorrespondanceNoBorders.resize((infos.depth_width()-2)*(infos.depth_height()-2));

        // fill
        std::iota(std::begin(depths1D), std::end(depths1D), 0);
        size_t id = 0;
        for(size_t ii = 0; ii < infos.depth_height(); ++ii){
            for(size_t jj = 0; jj < infos.depth_width(); ++jj){

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
                bool notOnRight  = jj < infos.depth_width() - 1;
                bool notOnTop    = ii > 0;
                bool notOnBottom = ii < infos.depth_height()-1;

                if(notOnLeft){
                    idD = id - 1;
                    if(notOnTop){
                        idA = id - infos.depth_width()-1;
                    }
                    if(notOnBottom){
                        idF = id + infos.depth_width()-1;
                    }
                }
                if(notOnRight){
                    idE = id + 1;
                    if(notOnTop){
                        idC = id - infos.depth_width() + 1;
                    }
                    if(notOnBottom){
                        idH = id + infos.depth_width() + 1;
                    }
                }
                if(notOnTop){
                    idB = id - infos.depth_width();
                }
                if(notOnBottom){
                    idG = id + infos.depth_width();
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
        // # 1D
        depths1D.clear();
        depths1DNoBorders.clear();
        neighbours4Depth1D.clear();
        neighbours8Depth1D.clear();
        // # 3D
        depths3D.clear();
        // # correspondance
        depthVertexCorrrespondance.clear();
        // depthsSortedCorrespondanceNoBorders.clear();
    }
}


auto DCFrameTiming::reset() -> void{
    capturesTimes.clear();
    timestamps.clear();
    localTimestamps.clear();
}

auto DCFrameTiming::swap_local_timestamps() -> void{
    for(auto &ts : localTimestamps){
        timestamps[ts.first]    = ts.second;
        ts.second               = std::nullopt;
    }
}

auto DCFrameTiming::update_local(std::string_view name) -> void{
    localTimestamps[name] = Time::nanoseconds_since_epoch();
}

auto DCFrameTiming::get_local(std::string_view name) const -> std::chrono::nanoseconds{
    if(localTimestamps.contains(name)){
        if(localTimestamps.at(name).has_value()){
            return localTimestamps.at(name).value();
        }
    }
    return std::chrono::nanoseconds{0};
}

auto DCFrameTiming::compute_capture_framerate() -> void{

    // if(!localTimestamps.contains("after_capture"sv)){
    if(!localTimestamps.contains("after_get_images"sv)){
        return;
    }

    // auto aftertCatpureTs = localTimestamps["after_capture"sv].value();
    auto captureTs = localTimestamps["after_get_images"sv].value();
    capturesTimes.push_back(captureTs);
    bool foundT = false;
    size_t idT = 0;
    for(size_t ii = 0; ii < capturesTimes.size(); ++ii){
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(captureTs - capturesTimes[ii]);
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

auto DCFrameTiming::get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::milliseconds>{
    if(timestamps.contains(from) && timestamps.contains(to)){
        if(timestamps.at(from).has_value() && timestamps.at(to).has_value()){
            return std::chrono::duration_cast<std::chrono::milliseconds>(timestamps.at(to).value()-timestamps.at(from).value());
        }
        return std::nullopt;
    }
    return std::nullopt;
}

auto DCFrameTiming::get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::microseconds>{
    if(timestamps.contains(from) && timestamps.contains(to)){
        if(timestamps.at(from).has_value() && timestamps.at(to).has_value()){
            return std::chrono::duration_cast<std::chrono::microseconds>(timestamps.at(to).value()-timestamps.at(from).value());
        }
        return std::nullopt;
    }
    return std::nullopt;
}
