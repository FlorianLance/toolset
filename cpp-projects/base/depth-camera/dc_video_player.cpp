

#include "dc_video_player.hpp"

// std
#include <execution>

// local
#include "utility/logger.hpp"
#include "utility/stop_watch.hpp"

using namespace tool::cam;

struct DCPlayerData{

    DCVideo video;
    std::vector<size_t> camerasFrameId;
    std::vector<std::shared_ptr<DCCompressedFrame>> camerasCompressedFrame;
    std::vector<std::shared_ptr<DCFrame>> camerasFrame;

    std::vector<size_t> ids;

    auto initialize(const DCVideo &nVideo) -> void{

        // copy video
        video = nVideo;

        size_t nbDevices = video.nb_cameras();
        camerasFrameId.resize(nbDevices);
        camerasCompressedFrame.resize(nbDevices);
        camerasFrame.resize(nbDevices);

        std::fill(std::begin(camerasFrameId),         std::end(camerasFrameId), 0);
        std::fill(std::begin(camerasCompressedFrame), std::end(camerasCompressedFrame), nullptr);
        std::fill(std::begin(camerasFrame),           std::end(camerasFrame), nullptr);
    }

    auto load_from_file(std::string_view path) -> bool{

        if(video.load_from_file(path)){
            size_t nbDevices = video.nb_cameras();
            camerasFrameId.resize(nbDevices);
            camerasCompressedFrame.resize(nbDevices);
            camerasFrame.resize(nbDevices);

            std::fill(std::begin(camerasFrameId),         std::end(camerasFrameId), 0);
            std::fill(std::begin(camerasCompressedFrame), std::end(camerasCompressedFrame), nullptr);

            for(auto &cameraFrame : camerasFrame){
                cameraFrame = std::make_shared<DCFrame>();
            }

            // std::cout << "VIDEO " << video.nb_cameras() << " " << video.duration_ms() << "\n";
            // for(size_t idC = 0; idC < video.nb_cameras(); ++idC){
            //     std::cout << "FROM idc " << idC << " " << video.nb_frames(idC) <<  "\n";
            //     auto firstC = video.get_compressed_frames_ptr(idC)->first_frame_received_timestamp();
            //     auto lastC = video.get_compressed_frames_ptr(idC)->last_frame_received_timestamp();
            //     std::cout << firstC << " " << lastC << " " << video.camera_duration_ms(idC) << "\n";

            //     for(size_t idF = 0; idF < video.nb_frames(idC); ++idF){
            //         if(auto camD = video.get_compressed_frame(idC, idF).lock()){
            //             std::cout << " -> " << camD->idCapture << " " << camD->afterCaptureTS << " " << camD->receivedTS << " " << camD->cloud_vertices_size() << " " <<
            //                 std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(camD->receivedTS-firstC)).count() << "ms\n";
            //         }
            //     }
            //     auto duration = video.duration_ms();
            //     for(int t = 0; t < duration; t += 50){
            //         std::cout << "T " << t << " " << video.closest_frame_id_from_time(idC, t) << "\n";
            //     }
            // }

            return true;
        }

        return false;
    }


    auto set_current_time(double timeMs) noexcept -> void{
        if(timeMs > video.duration_ms()){
            timeMs = video.duration_ms();
        }
        stopWatch.set_current_time(timeMs);
    }

    auto start_playing() noexcept -> void{

        if(stopWatch.is_started()){
            return;
        }
        stopWatch.start();
    }

    auto stop_playing() noexcept -> void{

        if(!stopWatch.is_started()){
            return;
        }
        stopWatch.stop();
    }

    auto is_started() const noexcept -> bool{
        return stopWatch.is_started();
    }

    auto current_time_ms() const noexcept  -> double{
        return stopWatch.ellapsed_milli_s();
    }

    auto current_frame_id(size_t idCamera) const noexcept -> size_t{
        if(idCamera < camerasFrameId.size()){
            return camerasFrameId[idCamera];
        }
        return 0;
    }


    auto update_current_compressed_frames(double timeMs) -> void{
        for(size_t idC = 0; idC < video.nb_cameras(); ++idC){
            if(auto idF = video.closest_frame_id_from_time(idC, timeMs); idF != -1){
                if(idF != camerasFrameId[idC]){ 
                    camerasCompressedFrame[idC] = video.get_compressed_frame(idC, idF).lock();
                    camerasFrameId[idC]         = idF;
                }
            }
        }
    }

    auto uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idCamera) -> bool{
        if(idCamera < camerasFrameId.size()){
            if(camerasCompressedFrame[idCamera] != nullptr){

                if(camerasFrame[idCamera] == nullptr){
                    camerasFrame[idCamera] = std::make_shared<DCFrame>();
                }

                if(camerasCompressedFrame[idCamera]->idCapture != camerasFrame[idCamera]->idCapture){
                    return video.uncompress_frame(gSettings, idCamera, camerasCompressedFrame[idCamera].get(), *camerasFrame[idCamera]);
                }
            }
        }
        return false;
    }


    auto remove_empty_cameras() -> void{
        tool::Logger::error("[DCPlayerData::remove_empty_cameras] Not implemented.\n");

        // for(size_t ii = 0; ii < i->data.video.nb_cameras(); ++ii){
        //     Logger::message(std::format("\n\ncam {} nb frames {}\n",ii, i->data.video.nb_frames(ii)));

        //     for(size_t jj = 0; jj < i->data.video.nb_frames(ii); ++jj){
        //         auto nbV = i->data.video.get_compressed_frames_ptr(ii)->valid_vertices_count(ii);
        //         Logger::message(std::format(" {}",nbV));
        //     }
        // }

        // size_t idC = 0;
        // for(const auto &ccf : i->data.camerasCompressedFrame){
        //     if(ccf != nullptr){
        //         Logger::message(std::format(" cam {} size {} \n", idC, ccf->cloud_vertices_size()));
        //     }
        //     idC++;
        // }

        // std::vector<size_t> idCamerasToKeep;
        // for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        //     if(i->videoResource.nb_frames(ii) > 0){
        //         Logger::message(std::format("keep camera {}",ii));
        //         idCamerasToKeep.push_back(ii);
        //     }
        // }
        // i->videoResource.keep_only_cameras_from_id(idCamerasToKeep);

        // i->currentCompressedFrames.resize(idCamerasToKeep.size());
        // for(auto &ccf : i->currentCompressedFrames){
        //     ccf = nullptr;
        // }
        // i->currentFrames.resize(idCamerasToKeep.size());
        // for(auto &cf : i->currentFrames){
        //     cf = nullptr;
        // }
        // update_states();

        // std::vector<DCModelSettings> models;
        // for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        //     DCModelSettings model;
        //     model.transformation = i->videoResource.get_transform(ii).conv<float>();
        //     models.push_back(model);
        // }
        // initialize_signal(std::move(models));

        return;

    }


    auto merge_before(DCVideo &other) -> void{

        tool::Logger::error("[DCPlayerData::merge_before] Not implemented.\n");

        // if(other.nb_cameras() != video.nb_cameras()){
        //     tool::Logger::error("[DCPlayerData::merge_before] Incompatible number of cameras.\n");
        //     return;
        // }

        // for(size_t idCamera = 0; idCamera < video.nb_cameras(); ++idCamera){
        //     if(video.get_transform(idCamera)  != other.get_transform(idCamera)){
        //         tool::Logger::warning("[DCPlayerData::merge_before] The video to be merged has different transforms.\n");
        //         break;
        //     }
        // }

        // ...
    }

private:

    tool::StopWatch stopWatch;
};

struct DCVideoPlayer::Impl{
    DCPlayerData data;
    DCVideoPlayerStates states;
    DCVideoPlayerSettings settings;    
};

DCVideoPlayer::DCVideoPlayer(): i(std::make_unique<Impl>()){
}

DCVideoPlayer::~DCVideoPlayer(){
}

auto DCVideoPlayer::set_video(const DCVideo &video) -> void{

    i->data.initialize(video);
    update_states();

    // retrieve models and send them
    std::vector<DCModelSettings> models;
    for(size_t ii = 0; ii < i->data.video.nb_cameras(); ++ii){
        DCModelSettings model;
        model.transformation = i->data.video.get_transform(ii).conv<float>();
        models.push_back(model);
    }
    initialize_signal(std::move(models));
}

auto DCVideoPlayer::display_infos() -> void{
    // using namespace std::chrono;
    // for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){

    //     std::cout << "FROM idc " << idC << "\n";
    //     auto firstC = video()->get_compressed_frames_ptr(idC)->first_frame_received_timestamp();
    //     for(size_t idC2 = 0; idC2 < video()->nb_cameras(); ++idC2){
    //         auto camD = video()->get_compressed_frames_ptr(idC2);
    //         auto diff = nanoseconds(camD->first_frame_received_timestamp() - firstC);
    //         std::cout << "  Camera " << idC2 << " mdiff: " <<  duration_cast<milliseconds>(diff) << " "<< camD->first_frame_received_timestamp()/**.value()*/ << " " << camD->last_frame_received_timestamp()/**.value()*/ << "\n";
    //     }
    // }

//    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){
//        auto camD = video()->get_camera_data(idC);
//        auto first = camD->first_frame_capture_timestamp().value();
//        std::cout <<  "CAMERA " << idC << "\n\n";
//        for(size_t idF = 0; idF < camD->nb_frames(); ++idF){
//            auto ts = camD->get_compressed_frame(idF).lock()->afterCaptureTS;
//            auto diffN = nanoseconds(ts-first);
//            auto diffN2 = nanoseconds(ts - firstC);
//            std::cout << duration_cast<milliseconds>(diffN) << " " << duration_cast<milliseconds>(diffN2) << " | ";
//        }
//        std::cout << "\n";
//    }
}

auto DCVideoPlayer::start_playing() noexcept -> void{
    i->data.start_playing();
    update_states();
}

auto DCVideoPlayer::stop_playing() noexcept -> void{
    i->data.stop_playing();
    update_states();
}

auto DCVideoPlayer::current_time_ms() const noexcept -> double{
    return i->data.current_time_ms();
}

auto DCVideoPlayer::is_looping() const noexcept -> bool {
    return i->settings.doLoop;
}

auto DCVideoPlayer::restart() noexcept -> void{
    i->data.set_current_time(0.0);
    update_states();
}

auto DCVideoPlayer::set_current_time(double timeMs) noexcept -> void{
    i->data.set_current_time(timeMs);
    update_states();
}

auto DCVideoPlayer::update() -> void{

    // update time
    auto cTime = current_time_ms();
    if(cTime > video()->duration_ms()){
        if(is_looping()){
            cTime = 0.0;
            i->data.set_current_time(cTime);
        }else{
            i->data.stop_playing();
        }
    }

    // store current ids
    std::vector<size_t> currentCamerasFrameId = i->data.camerasFrameId;

    // retrieve current frames to uncompress
    i->data.update_current_compressed_frames(cTime);

    // uncompress frames
    std::vector<bool> uncompressResults(video()->nb_cameras(), false);
    std::vector<size_t> ids(video()->nb_cameras());
    std::iota(ids.begin(), ids.end(), 0);
    std::for_each(std::execution::par_unseq, std::begin(ids), std::end(ids), [&](size_t id){
        if(video()->nb_frames(id) != 0){
            if(i->data.current_frame_id(id) == 0 || (i->data.current_frame_id(id) != currentCamerasFrameId[id])){
                uncompressResults[id] = i->data.uncompress_frame(i->settings.generation, id);
            }
        }
    });

    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){

        if(uncompressResults[idC]){
            auto cloudSize = i->data.camerasFrame[idC]->cloud.size();
            if(i->data.ids.size() < cloudSize){
                i->data.ids.resize(cloudSize);
                std::iota(std::begin(i->data.ids), std::end(i->data.ids), 0);
            }

            new_frame_signal(idC, i->data.camerasFrame[idC]);
        }
    }

    update_states();
}

auto DCVideoPlayer::remove_until_current_frame() -> void{

    for(size_t idC = 0; idC < i->data.video.nb_cameras(); ++idC){
        i->data.video.remove_compressed_frames_until(idC, current_frame_id(idC));
    }    
    i->data.set_current_time(0.0);
    update_states();
}

auto DCVideoPlayer::remove_after_current_frame() -> void{

    for(size_t idC = 0; idC < i->data.video.nb_cameras(); ++idC){
        i->data.video.remove_compressed_frames_after(idC, current_frame_id(idC));
    }
    update_states();
}

auto DCVideoPlayer::merge() -> void{
    Logger::error("[DCPlayer::merge] Not implemented.\n");
    // remove_empty_cameras();
//    merge_cameras(0.005f, {-20.f,-20.f,-20.f}, {+20.f,+20.f,+20.f});
    // update_states();
}

auto DCVideoPlayer::merge_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void{

    if(i->data.video.nb_cameras() < 1){
        return;
    }

    i->data.video.merge_all_cameras(i->settings.generation, voxelSize, minBound, maxBound);
    i->data.camerasCompressedFrame = {nullptr};
    i->data.camerasFrame = {nullptr};

    update_states();

    std::vector<DCModelSettings> models(1);
    models.front().transformation = i->data.video.get_transform(0).conv<float>();
    initialize_signal(std::move(models));
}

auto DCVideoPlayer::remove_empty_cameras() -> void{
    i->data.remove_empty_cameras();
    update_states();
}

auto DCVideoPlayer::update_settings(const DCVideoPlayerSettings &playerS) noexcept -> void {
    i->settings = playerS;
}

auto DCVideoPlayer::current_frame_id(size_t idCamera) const -> size_t{
    return i->data.current_frame_id(idCamera);
}

auto DCVideoPlayer::current_compressed_frame_cloud_size(size_t idCamera) -> size_t{

    if(idCamera < i->data.camerasCompressedFrame.size()){
        if(i->data.camerasCompressedFrame[idCamera] != nullptr){
            return i->data.camerasCompressedFrame[idCamera]->validVerticesCount;
        }
    }
    return 0;
}

auto DCVideoPlayer::current_frame_cloud_size(size_t idCamera) -> size_t{

    if(idCamera < i->data.camerasFrame.size()){
        if(i->data.camerasFrame[idCamera] != nullptr){
            return i->data.camerasFrame[idCamera]->cloud.size();
        }
    }
    return 0;
}


auto DCVideoPlayer::current_frame(size_t idCamera) -> std::shared_ptr<DCFrame>{
    if(idCamera < i->data.camerasFrame.size()){
        return i->data.camerasFrame[idCamera];
    }
    return nullptr;
}

auto DCVideoPlayer::current_frames_total_cloud_size() -> size_t{
    size_t totalNbVertices = 0;
    for(size_t idD = 0; idD < video()->nb_cameras(); ++idD){
        totalNbVertices += current_frame_cloud_size(idD);
    }
    return totalNbVertices;
}

auto DCVideoPlayer::copy_current_cloud(size_t idCamera, std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t{
    if(auto frame = current_frame(idCamera); frame != nullptr){
        auto verticesCountToCopy = std::min(frame->cloud.size(), vertices.size());

        auto tr = video()->get_transform(idCamera).conv<float>();

        if(applyModelTransform){
            std::for_each(std::execution::par_unseq, std::begin(i->data.ids), std::begin(i->data.ids) + verticesCountToCopy, [&](size_t id){
                const auto &pt = frame->cloud.vertices[id];
                vertices[id].pos = geo::Pt3f(tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz());
                vertices[id].col = geo::Pt4<std::uint8_t>{
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].x()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].y()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].z()),
                    255
                };
            });
        }else{
            std::for_each(std::execution::par_unseq, std::begin(i->data.ids), std::begin(i->data.ids) + verticesCountToCopy, [&](size_t id){
                vertices[id].pos = frame->cloud.vertices[id];
                vertices[id].col = geo::Pt4<std::uint8_t>{
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].x()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].y()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].z()),
                    255
                };
            });
        }
        return verticesCountToCopy;
    }
    return 0;
}

auto DCVideoPlayer::copy_all_current_clouds(std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t{

    std::vector<size_t> startId;
    size_t totalNbVertices = 0;
    for(size_t idD = 0; idD < video()->nb_cameras(); ++idD){
        if(startId.empty()){
            startId.push_back(0);
        }else{
            startId.push_back(current_frame_cloud_size(idD-1));
        }
        totalNbVertices += current_frame_cloud_size(idD);
    }

    if(vertices.size() < totalNbVertices){
        return 0;
    }

    for(size_t idD = 0; idD < video()->nb_cameras(); ++idD){
        auto start = startId[idD];
        auto tr = video()->get_transform(idD).conv<float>();
        auto frame = current_frame(idD);

        if(i->data.ids.size() < frame->cloud.size()){
            i->data.ids.resize(frame->cloud.size());
            std::iota(std::begin(i->data.ids), std::end(i->data.ids), 0);
        }

        std::for_each(std::execution::par_unseq, std::begin(i->data.ids), std::begin(i->data.ids) + frame->cloud.size(), [&](size_t idV){

            const auto &pt = frame->cloud.vertices[idV];
            if(applyModelTransform){
                vertices[start + idV].pos = geo::Pt3f(tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz());
            }else{
                vertices[start + idV].pos = pt;
            }
            vertices[start + idV].col = geo::Pt4<std::uint8_t>{
                static_cast<std::uint8_t>(255.f*frame->cloud.colors[idV].x()),
                static_cast<std::uint8_t>(255.f*frame->cloud.colors[idV].y()),
                static_cast<std::uint8_t>(255.f*frame->cloud.colors[idV].z()),
                255
            };
        });
    }

    return totalNbVertices;
}

auto DCVideoPlayer::is_playing() const noexcept -> bool{
    return i->data.is_started();
}

auto DCVideoPlayer::video() -> DCVideo* {
    return &i->data.video;
}

auto DCVideoPlayer::load_from_file(std::string_view path) -> bool{

    if(i->data.load_from_file(path)){

        restart();

        // retrieve models and send them
        std::vector<DCModelSettings> models;
        for(size_t ii = 0; ii < i->data.video.nb_cameras(); ++ii){
            DCModelSettings model;
            model.transformation = i->data.video.get_transform(ii).conv<float>();
            models.push_back(model);
        }
        initialize_signal(std::move(models));

        return true;
    }

    return false;
}

auto DCVideoPlayer::save_to_file(std::string_view path) -> bool{
    return i->data.video.save_to_file(path);
}

auto DCVideoPlayer::merge_before(DCVideo &other) -> void{
    i->data.merge_before(other);
    update_states();
}

auto DCVideoPlayer::save_cloud_to_file(std::string_view path) -> bool{

    Logger::error("[DCPlayer::save_cloud_to_file] Not implemented.\n");
    // size_t idC = 0;
    // for(const auto &ccf : i->data.camerasCompressedFrame){
    //     if(ccf != nullptr){
    //         Logger::message(std::format(" cam {} size {} \n", idC, ccf->cloud_vertices_size()));
    //     }
    //     idC++;
    // }

    return true;
}

auto DCVideoPlayer::update_states() noexcept -> void{

    if(i->states.nbFrames.size() != i->data.video.nb_cameras()){
        i->states.nbFrames.resize(i->data.video.nb_cameras());
        i->states.currentFrames.resize(i->data.video.nb_cameras());
    }

    for(size_t ii = 0; ii < i->data.video.nb_cameras(); ++ii){
        i->states.nbFrames[ii]      = i->data.video.nb_frames(ii);
        i->states.currentFrames[ii] = current_frame_id(ii);
    }

    i->states.isPlaying   = i->data.is_started();
    i->states.duration    = video()->duration_ms();
    i->states.currentTime = current_time_ms();

    states_updated_signal(i->states);
}

