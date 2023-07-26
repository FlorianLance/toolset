
#include "imgui_k4_cloud_drawer.hpp"

using namespace tool::graphics;

auto K4CloudDrawer::initialize() -> void {

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

auto K4CloudDrawer::reset() -> void{

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

auto K4CloudDrawer::init_from_frame(std::shared_ptr<camera::K4Frame> frame) -> bool {

    if(lastFrameId == frame->idCapture){
        return false;
    }

    if(!frame->imageColorData.empty()){
        colorT.init_or_update_8ui(
            static_cast<GLsizei>(frame->colorWidth),
            static_cast<GLsizei>(frame->colorHeight), 4, reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()));
    }
    if(!frame->imageDepthData.empty()){
        depthT.init_or_update_8ui(
            static_cast<GLsizei>(frame->depthWidth),
            static_cast<GLsizei>(frame->depthHeight), 3, reinterpret_cast<std::uint8_t*>(frame->imageDepthData.data()));
    }
    if(!frame->imageInfraData.empty()){
        infraT.init_or_update_8ui(
            static_cast<GLsizei>(frame->infraWidth),
            static_cast<GLsizei>(frame->infraHeight), 3, reinterpret_cast<std::uint8_t*>(frame->imageInfraData.data()));
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

auto K4CloudDrawer::init_from_colored_cloud_data(const geo::ColoredCloudData &cloudData) -> bool {

//    if(cloudData.size() == 0 || cloudData.vertices.empty() || cloudData.colors.empty()){
//        return false;
//    }
    cpD.init(cloudData.size(), cloudData.vertices.get_data(), cloudData.colors.get_data());

    lastFrame   = nullptr;
    lastFrameId = -1;

    return true;
}
