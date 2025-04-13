
/*******************************************************************************
** Toolset-imgui-opengl-engine                                                **
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
using namespace tool::geo;

auto DCCloudDrawer::initialize() -> void {

    std::vector<std::uint8_t> reset4(4* 100*100, 50);

    // init textures
    colorT.init_or_update_8ui(100,100,4, reset4.data());
    depthSizedColorT.init_or_update_8ui(100,100,4, reset4.data());
    depthT.init_or_update_8ui(100,100,3, reset4.data());
    infraT.init_or_update_8ui(100,100,3, reset4.data());
    bodiesIdMapT.init_or_update_8ui(100, 100, 3, reset4.data());

    // init textures drawers
    colorD.init(&colorT);
    depthSizedColorD.init(&depthSizedColorT);
    depthD.init(&depthT);
    infraD.init(&infraT);
    bodiesIdMapD.init(&bodiesIdMapT);
    
    btJointCenterD.initialize(0.05f, 20, 20);
    btJointDirD.initialize(0.3f);

    std::vector<Pt3f> initData(1024*1024);
    cpD.initialize(true, initData, initData, initData);
    cpD.set_indice_count(0);

    frustumD.initialize(true);
    geo::OBB3<float> obb;
    oobLinesD.initialize(true, obb);
    planeFilteringLinesD.initialize(true);
}

auto DCCloudDrawer::reset() -> void{

    // reset frame
    lastFrameId = -1;
    lastFrame = nullptr;

    // reset bodies
    nbBodies = 0;
    jointsModels.clear();

    std::vector<std::uint8_t> reset4(4* 100*100, 50);
    colorT.init_or_update_8ui(100,100,4, reset4.data());
    depthSizedColorT.init_or_update_8ui(100,100,4, reset4.data());
    depthT.init_or_update_8ui(100,100,3, reset4.data());
    infraT.init_or_update_8ui(100,100,3, reset4.data());
    bodiesIdMapT.init_or_update_8ui(100,100,3, reset4.data());

    cpD.set_indice_count(0);
}

auto DCCloudDrawer::init_from_frame(std::shared_ptr<cam::DCFrame> frame) -> bool {

    if(lastFrameId == frame->idCapture){
        return false;
    }

    auto dr     = cam::dc_depth_resolution(frame->mode);
    auto range  = cam::dc_depth_range(dr);
    auto hFov   = dc_depth_h_fov(dr);
    auto vFov   = dc_depth_v_fov(dr);
    auto diff   = range.y() - range.x();
    frustumD.update(1.f*vFov, 1.f*hFov/vFov, range.x() + filtersS.minDepthF*diff, range.x() + filtersS.maxDepthF*diff);

    if(auto image = frame->image_buffer<ColorRGBA8>(cam::DCImageBufferType::DepthSizedColorRGBA8)){

        depthSizedColorT.init_or_update_8ui(
            static_cast<GLsizei>(image->width),
            static_cast<GLsizei>(image->height), 4, reinterpret_cast<uint8_t *>(image->get_data()));
    }

    if(auto image = frame->image_buffer<ColorRGBA8>(cam::DCImageBufferType::OriginalColorRGBA8)){

        colorT.init_or_update_8ui(
            static_cast<GLsizei>(image->width),
            static_cast<GLsizei>(image->height), 4, reinterpret_cast<uint8_t *>(image->get_data()));
    }

    if(auto image = frame->image_buffer<ColorRGB8>(cam::DCImageBufferType::DepthRGB8)){

        depthT.init_or_update_8ui(
            static_cast<GLsizei>(image->width),
            static_cast<GLsizei>(image->height), 3, reinterpret_cast<uint8_t *>(image->get_data()));
    }

    if(auto image = frame->image_buffer<ColorRGB8>(cam::DCImageBufferType::InfraredRGB8)){

        infraT.init_or_update_8ui(
            static_cast<GLsizei>(image->width),
            static_cast<GLsizei>(image->height), 3, reinterpret_cast<uint8_t *>(image->get_data()));
    }
    
    if(auto image = frame->image_buffer<ColorRGB8>(cam::DCImageBufferType::BodiesIdMapRGB8)){

        bodiesIdMapT.init_or_update_8ui(
            static_cast<GLsizei>(image->width),
            static_cast<GLsizei>(image->height), 3, reinterpret_cast<uint8_t *>(image->get_data()));
    }

    if(auto cloud = frame->volume_buffer<ColorCloud>(cam::DCVolumeBufferType::ColoredCloud)){
        if(cloud->is_valid()){
            cpD.update(*cloud);
            cpD.set_indice_count(cloud->size());
        }
    }

    if(auto bodiesSkeleton = frame->data_buffer(cam::DCDataBufferType::BodiesSkeleton)){

        nbBodies = bodiesSkeleton->size()/sizeof(cam::DCBody);
        if(jointsModels.size() != nbBodies){
            jointsModels.resize(nbBodies);
        }

        auto btData = reinterpret_cast<cam::DCBody*>(bodiesSkeleton->get_data());

        for(size_t ii = 0; ii < nbBodies; ++ii){
            for(size_t jj = 0; jj < btData[ii].skeleton.joints.size(); ++jj){
                const auto &j = btData[ii].skeleton.joints[jj];

                auto e =euler_angles(j.orientation)*d180_PI<float>;
                jointsModels[ii][jj] = std::make_tuple(j.confidence,
                    geo::transform(
                        {{1.f,1.f,1.f}},
                        // {{-e.x(),-e.y(),e.z()}},
                        // {{e.x(),e.z(),e.y()}},
                        // {{e.z(),e.x(),e.y()}},
                        {{-e.z(),-e.y(),e.x()}},
                        j.position*0.001f
                    )
                );
            }
        }

    }else{
        nbBodies = 0;
    }

    // update last frame
    lastFrameId = frame->idCapture;
    lastFrame   = frame;

    // updateTS = frame->afterCaptureTS;
    // Log::message(std::format("[U:{}]", Time::difference_micro_s(std::chrono::nanoseconds(frame->afterCaptureTS), Time::nanoseconds_since_epoch())));

    return true;
}

auto DCCloudDrawer::init_from_colored_cloud_data(const geo::ColorCloud &cloudData) -> bool {    

    cpD.update(cloudData);
    cpD.set_indice_count(cloudData.vertices.size());

    lastFrame   = nullptr;
    lastFrameId = -1;

    return true;
}


//    if(k4M->parameters.captureAudio){
//        for(size_t idFrame = 0; idFrame < currentData->audioFramesCount; ++idFrame){
//            for(size_t idChannel = 0; idChannel < channelsData2.size(); ++idChannel){
//                //                        if(idChannel == 0){
//                //                            Log::message(std::to_string(currentData->audioChannelsData[idChannel][idFrame]) + " ");
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
