
/*******************************************************************************
** Toolset-3d-engine                                                          **
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

#include "imgui_dc_cloud_drawer.hpp"

using namespace tool::graphics;

auto DCCloudDrawer::initialize() -> void {

    std::vector<std::uint8_t> reset4(4* 100*100, 50);
    std::vector<std::uint8_t> reset3(3* 100*100, 50);
    colorT.init_or_update_8ui(100,100,4, reset4.data());
    depthT.init_or_update_8ui(100,100,3, reset3.data());
    infraT.init_or_update_8ui(100,100,3, reset3.data());

    colorD.init(&colorT);
    depthD.init(&depthT);
    infraD.init(&infraT);

    spD.init(0.05f, 20,  20, {});
}

auto DCCloudDrawer::reset() -> void{

    // reset frame
    lastFrameId = -1;
    lastFrame = nullptr;

    // reset bodies
    nbBodies = 0;
    jointsModels.clear();

    std::vector<std::uint8_t> reset4(4* 100*100, 50);
    std::vector<std::uint8_t> reset3(3* 100*100, 50);
    colorT.init_or_update_8ui(100,100,4, reset4.data());
    depthT.init_or_update_8ui(100,100,3, reset3.data());
    infraT.init_or_update_8ui(100,100,3, reset3.data());

    // clean cloud drawer
    cpD.clean();
}

auto DCCloudDrawer::init_from_frame(std::shared_ptr<cam::DCFrame> frame) -> bool {

    if(lastFrameId == frame->idCapture){
        return false;
    }

    if(!frame->rgbaDepthSizedColor.empty()){
        colorT.init_or_update_8ui(
            static_cast<GLsizei>(frame->rgbaDepthSizedColor.width),
            static_cast<GLsizei>(frame->rgbaDepthSizedColor.height), 4, reinterpret_cast<uint8_t *>(frame->rgbaDepthSizedColor.get_data()));
    } else if(!frame->rgbaColor.empty()){
        colorT.init_or_update_8ui(
            static_cast<GLsizei>(frame->rgbaColor.width),
            static_cast<GLsizei>(frame->rgbaColor.height), 4, reinterpret_cast<uint8_t *>(frame->rgbaColor.get_data()));
    }
    
    if(!frame->rgbDepth.empty()){
        depthT.init_or_update_8ui(
            static_cast<GLsizei>(frame->rgbDepth.width),
            static_cast<GLsizei>(frame->rgbDepth.height), 3, reinterpret_cast<uint8_t *>(frame->rgbDepth.get_data()));
    }
    
    if(!frame->rgbInfra.empty()){
        infraT.init_or_update_8ui(
            static_cast<GLsizei>(frame->rgbInfra.width),
            static_cast<GLsizei>(frame->rgbInfra.height), 3, reinterpret_cast<uint8_t *>(frame->rgbInfra.get_data()));
    }

    if(frame->cloud.is_valid()){
        cpD.init(
            frame->cloud.size(),
            frame->cloud.vertices.get_data(),
            frame->cloud.colors.get_data(),
            !frame->cloud.normals.empty() ? frame->cloud.normals.get_data() : nullptr
        );
    }

    nbBodies = frame->bodies.size();
    if(jointsModels.size() < nbBodies){
        jointsModels.resize(nbBodies);
    }
    for(size_t ii = 0; ii < nbBodies; ++ii){
        for(size_t jj = 0; jj < frame->bodies[ii].skeleton.joints.size(); ++jj){
            const auto &j = frame->bodies[ii].skeleton.joints[jj];
            jointsModels[ii][jj] = std::make_tuple(j.good_confidence(),
                geo::transform(
                    {{1.f,1.f,1.f}},
                    euler_angles(j.orientation)*d180_PI<float>,
                    j.position*0.001f
                )
            );
        }
    }

    // update last frame
    lastFrameId = frame->idCapture;
    lastFrame   = frame;

    return true;
    //    if(k4M->parameters.captureAudio){
    //        for(size_t idFrame = 0; idFrame < currentData->audioFramesCount; ++idFrame){
    //            for(size_t idChannel = 0; idChannel < channelsData2.size(); ++idChannel){
    //                //                        if(idChannel == 0){
    //                //                            Logger::message(std::to_string(currentData->audioChannelsData[idChannel][idFrame]) + " ");
    //                //                        }
    //                channelsData2[idChannel].push_back(currentData->audioChannelsData[idChannel][idFrame]);
    //            }
    //        }

    //        for(size_t idChannel = 0; idChannel < channelsData2.size(); ++idChannel){
    //            if(channelsData2[idChannel].size() > 50000){
    //                channelsData2[idChannel].erase(channelsData2[idChannel].begin(), channelsData2[idChannel].begin() + (channelsData2[idChannel].size() - 50000));
    //            }
    //        }
    //    }
}

auto DCCloudDrawer::init_from_colored_cloud_data(const geo::ColoredCloudData &cloudData) -> bool {

//    if(cloudData.size() == 0 || cloudData.vertices.empty() || cloudData.colors.empty()){
//        return false;
//    }
    cpD.init(cloudData.size(), cloudData.vertices.get_data(), cloudData.colors.get_data());

    lastFrame   = nullptr;
    lastFrameId = -1;

    return true;
}
