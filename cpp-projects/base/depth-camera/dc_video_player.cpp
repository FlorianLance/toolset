

#include "dc_video_player.hpp"

// std
#include <execution>

// local
#include "utility/logger.hpp"
#include "utility/stop_watch.hpp"


using namespace tool::cam;


struct DCVideoPlayer::Impl{

    DCVideo video;
    std::vector<size_t> camerasFrameId;
    std::vector<std::shared_ptr<DCDataFrame>> camerasDataFrame;
    std::vector<std::shared_ptr<DCFrame>> camerasFrame;
    std::vector<size_t> ids;

    auto initialize(const DCVideo &nVideo) -> void{

        // copy video
        video = nVideo;
        
        size_t nbDevices = video.nb_devices();
        camerasFrameId.resize(nbDevices);
        camerasDataFrame.resize(nbDevices);
        camerasFrame.resize(nbDevices);

        std::fill(std::begin(camerasFrameId),   std::end(camerasFrameId), 0);
        std::fill(std::begin(camerasDataFrame), std::end(camerasDataFrame), nullptr);
        std::fill(std::begin(camerasFrame),     std::end(camerasFrame), nullptr);

        ids.resize(1024*1024);
        std::iota(ids.begin(), ids.end(), 0);
    }

    auto load_from_file(std::string_view path) -> bool{

        if(video.load_from_file(path)){
            
            size_t nbDevices = video.nb_devices();
            camerasFrameId.resize(nbDevices);
            camerasDataFrame.resize(nbDevices);
            camerasFrame.resize(nbDevices);

            std::fill(std::begin(camerasFrameId),   std::end(camerasFrameId), 0);
            std::fill(std::begin(camerasDataFrame), std::end(camerasDataFrame), nullptr);

            for(auto &cameraFrame : camerasFrame){
                cameraFrame = std::make_shared<DCFrame>();
            }

            ids.resize(1024*1024);
            std::iota(ids.begin(), ids.end(), 0);

            return true;
        }

        return false;
    }

    auto reset_playing() noexcept -> void{
        stopWatch.reset();
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

    auto set_current_time(double timeMs) noexcept -> void{
        if(timeMs > video.duration_ms()){
            timeMs = video.duration_ms();
        }
        if(timeMs < 0.0){
            timeMs = 0.0;
        }
        stopWatch.set_current_time(timeMs);
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

    auto update_current_data_frame_from_time(double timeMs) -> void{
        for(size_t idC = 0; idC < video.nb_devices(); ++idC){
            if(auto idF = video.closest_frame_id_from_time(idC, timeMs); idF != -1){
                if(idF != camerasFrameId[idC]){
                    camerasDataFrame[idC] = video.get_data_frame(idC, idF).lock();
                    camerasFrameId[idC]   = idF;
                }
            }
        }
    }

    auto generate_current_frame(const DCFrameGenerationSettings &gSettings, size_t idCamera) -> bool{

        if(idCamera < camerasFrameId.size()){
            if(camerasDataFrame[idCamera] != nullptr){

                if(camerasFrame[idCamera] == nullptr){
                    camerasFrame[idCamera] = std::make_shared<DCFrame>();
                }

                if(camerasDataFrame[idCamera]->idCapture != camerasFrame[idCamera]->idCapture){
                    return video.generate_frame(gSettings, idCamera, camerasDataFrame[idCamera].get(), *camerasFrame[idCamera]);
                }
            }
        }
        return false;
    }

    auto remove_empty_cameras() -> void{
        tool::Log::error("[DCVideoPlayer::remove_empty_cameras] Not implemented.\n");

        // for(size_t ii = 0; ii < i->video.nb_cameras(); ++ii){
        //     Log::message(std::format("\n\ncam {} nb frames {}\n",ii, i->video.nb_frames(ii)));

        //     for(size_t jj = 0; jj < i->video.nb_frames(ii); ++jj){
        //         auto nbV = i->video.get_compressed_frames_ptr(ii)->valid_vertices_count(ii);
        //         Log::message(std::format(" {}",nbV));
        //     }
        // }

        // size_t idC = 0;
        // for(const auto &ccf : i->camerasCompressedFrame){
        //     if(ccf != nullptr){
        //         Log::message(std::format(" cam {} size {} \n", idC, ccf->cloud_vertices_size()));
        //     }
        //     idC++;
        // }

        // std::vector<size_t> idCamerasToKeep;
        // for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        //     if(i->videoResource.nb_frames(ii) > 0){
        //         Log::message(std::format("keep camera {}",ii));
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
        static_cast<void>(other);
        tool::Log::error("[DCVideoPlayer::merge_before] Not implemented.\n");

        // if(other.nb_cameras() != video.nb_cameras()){
        //     tool::Log::error("[DCPlayerData::merge_before] Incompatible number of cameras.\n");
        //     return;
        // }

        // for(size_t idCamera = 0; idCamera < video.nb_cameras(); ++idCamera){
        //     if(video.get_transform(idCamera)  != other.get_transform(idCamera)){
        //         tool::Log::warning("[DCPlayerData::merge_before] The video to be merged has different transforms.\n");
        //         break;
        //     }
        // }

        // ...
    }

private:

    tool::StopWatch stopWatch;
};

DCVideoPlayer::DCVideoPlayer(): i(std::make_unique<Impl>()){
}

DCVideoPlayer::~DCVideoPlayer(){
}

auto DCVideoPlayer::set_video(const DCVideo &video) -> void{

    // stop playing current video
    stop_video();

    // init data with new video
    i->initialize(video);

    // reset custom times
    settings.startTimeMs = -1.0;
    settings.endTimeMs   = -1.0;

    // update player states
    states.isStarted = false;
    update_states();

    // retrieve models and send them
    std::vector<DCModelSettings> models;
    for(size_t ii = 0; ii < i->video.nb_devices(); ++ii){
        DCModelSettings model;
        model.transformation = i->video.get_transform(ii).conv<float>();
        models.push_back(model);
    }
    initialize_signal(std::move(models));
}


auto DCVideoPlayer::is_started() const noexcept -> bool{
    return states.isStarted;
}

auto DCVideoPlayer::is_playing() const noexcept -> bool{
    return states.isPlaying;
}

auto DCVideoPlayer::start_video() noexcept -> void{

    if(is_started() && !is_playing()){
        unpause_video();
        return;
    }

    states.isStarted = true;

    i->start_playing();
    if(start_time_ms() > 0.0){
        i->set_current_time(start_time_ms());
    }

    update_states();
}

auto DCVideoPlayer::stop_video() noexcept -> void{

    states.isStarted = false;
    i->reset_playing();
    update_states();
}

auto DCVideoPlayer::pause_video() noexcept -> void{

    if(!is_started()){
        return;
    }

    i->stop_playing();
    update_states();
}

auto DCVideoPlayer::unpause_video() noexcept -> void{

    if(!is_started()){
        return;
    }

    i->start_playing();
    update_states();
}

auto DCVideoPlayer::go_to_start_time() noexcept -> void{
    i->set_current_time(start_time_ms());
    update_states();
}

auto DCVideoPlayer::go_to_end_time() noexcept -> void{
    i->set_current_time(end_time_ms());
    update_states();
}

auto DCVideoPlayer::is_looping() const noexcept -> bool {
    return settings.doLoop;
}

auto DCVideoPlayer::duration_ms() const noexcept -> double{
    return i->video.duration_ms();
}

auto DCVideoPlayer::start_time_ms() const noexcept -> double{
    if((settings.startTimeMs > 0.0) && (settings.startTimeMs < duration_ms())){
        return settings.startTimeMs;
    }
    return 0.0;
}

auto DCVideoPlayer::end_time_ms() const noexcept -> double{
    if((settings.endTimeMs > 0.0) && (settings.endTimeMs < duration_ms())){
        return settings.endTimeMs;
    }
    return duration_ms();
}


auto DCVideoPlayer::set_current_time(double timeMs) noexcept -> void{

    if(timeMs < start_time_ms()){
        timeMs = start_time_ms();
    }

    if(timeMs > end_time_ms()){
        timeMs = end_time_ms();
    }

    i->set_current_time(timeMs);
    update_states();
}

auto DCVideoPlayer::current_time_ms() const noexcept -> double{
    return i->current_time_ms();
}

auto DCVideoPlayer::update() -> void{

    // update time
    auto cTime = current_time_ms();

    if(cTime > end_time_ms()){
        if(is_looping()){
            go_to_start_time();
        }else{
            go_to_end_time();
        }
    }

    // store current ids
    std::vector<size_t> currentCamerasFrameId = i->camerasFrameId;

    // retrieve current frames to uncompress
    i->update_current_data_frame_from_time(cTime);

    // uncompress frames
    std::vector<bool> uncompressResults(video()->nb_devices(), false);
    std::vector<size_t> ids(video()->nb_devices());
    std::iota(ids.begin(), ids.end(), 0);

    StopWatch sw;
    sw.start();
    std::for_each(std::execution::par_unseq, std::begin(ids), std::end(ids), [&](size_t id){
        if(video()->nb_frames(id) != 0){
            if(i->current_frame_id(id) == 0 || (i->current_frame_id(id) != currentCamerasFrameId[id])){
                uncompressResults[id] = i->generate_current_frame(settings.generation, id);
            }
        }
    });
    
    for(size_t idC = 0; idC < video()->nb_devices(); ++idC){

        if(uncompressResults[idC]){

            size_t cloudSize = 0;
            if(i->camerasFrame[idC]->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
                cloudSize = i->camerasFrame[idC]->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)->size();
            }

            if(i->ids.size() != cloudSize){
                i->ids.resize(cloudSize);
                std::iota(std::begin(i->ids), std::end(i->ids), 0);
            }

            new_frame_signal(idC, i->camerasFrame[idC]);
        }
    }

    update_states();
}

auto DCVideoPlayer::remove_until_current_frame() -> void{
    
    for(size_t idC = 0; idC < i->video.nb_devices(); ++idC){
        i->video.remove_data_frames_until(idC, current_frame_id(idC));
    }    
    i->set_current_time(0.0);
    update_states();
}

auto DCVideoPlayer::remove_after_current_frame() -> void{
    
    for(size_t idC = 0; idC < i->video.nb_devices(); ++idC){
        i->video.remove_data_frames_after(idC, current_frame_id(idC));
    }
    update_states();
}

auto DCVideoPlayer::merge() -> void{
    Log::error("[DCVideoPlayer::merge].\n");
    merge_cameras(0.0025f, {-2.f,-2.f,-2.f}, {+4.f,+4.f,+4.f});
}

auto DCVideoPlayer::merge_cameras(float voxelSize, tool::geo::Pt3f origin, tool::geo::Pt3f size) -> void{
    
    if(i->video.nb_devices() < 1){
        return;
    }
    
    i->video.merge_all_devices(settings.generation, voxelSize, origin, size);
    i->camerasDataFrame = {nullptr};
    i->camerasFrame = {nullptr};

    update_states();

    std::vector<DCModelSettings> models(1);
    models.front().transformation = geo::Mat4f::identity();// i->video.get_transform(0).conv<float>();
    initialize_signal(std::move(models));
}

auto DCVideoPlayer::remove_empty_cameras() -> void{
    i->remove_empty_cameras();
    update_states();
}

auto DCVideoPlayer::update_settings(const DCVideoPlayerSettings &playerS) noexcept -> void {
    settings = playerS;
}

auto DCVideoPlayer::current_frame_id(size_t idCamera) const -> size_t{
    return i->current_frame_id(idCamera);
}

auto DCVideoPlayer::current_compressed_frame_cloud_size(size_t idCamera) -> size_t{

    if(idCamera < i->camerasDataFrame.size()){
        if(i->camerasDataFrame[idCamera] != nullptr){
            return i->camerasDataFrame[idCamera]->validVerticesCount;
        }
    }
    return 0;
}

auto DCVideoPlayer::current_frame_cloud_size(size_t idCamera) -> size_t{

    if(idCamera < i->camerasFrame.size()){
        if(i->camerasFrame[idCamera] != nullptr){
            if(i->camerasFrame[idCamera]->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
                return i->camerasFrame[idCamera]->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)->size();
            }
        }
    }
    return 0;
}

auto DCVideoPlayer::current_frame(size_t idCamera) -> std::shared_ptr<DCFrame>{
    if(idCamera < i->camerasFrame.size()){
        return i->camerasFrame[idCamera];
    }
    return nullptr;
}

auto DCVideoPlayer::current_frames_total_cloud_size() -> size_t{
    size_t totalNbVertices = 0;
    for(size_t idD = 0; idD < video()->nb_devices(); ++idD){
        totalNbVertices += current_frame_cloud_size(idD);
    }
    return totalNbVertices;
}

auto DCVideoPlayer::frame_cloud_size(size_t idCamera, size_t idFrame) -> size_t{

    // i->video->
    // if(idCamera < i->camerasFrame.size()){
    //     if(i->camerasFrame[idCamera] != nullptr){
    //         if(i->camerasFrame[idCamera]->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
    //             return i->camerasFrame[idCamera]->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)->size();
    //         }
    //     }
    // }

    return {};
}

auto DCVideoPlayer::copy_current_cloud(size_t idCamera, std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t{

    if(auto frame = current_frame(idCamera); frame != nullptr){

        if(auto cloud = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)){

            auto verticesCountToCopy = std::min(cloud->size(), vertices.size());
            auto tr = video()->get_transform(idCamera).conv<float>();

            if(applyModelTransform){
                std::for_each(std::execution::par_unseq, std::begin(i->ids), std::begin(i->ids) + verticesCountToCopy, [&](size_t id){
                    const auto &pt = cloud->vertices[id];
                    vertices[id].pos = geo::Pt3f(tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz());
                    vertices[id].col = geo::Pt4<std::uint8_t>{
                        static_cast<std::uint8_t>(255.f* cloud->colors[id].x()),
                        static_cast<std::uint8_t>(255.f* cloud->colors[id].y()),
                        static_cast<std::uint8_t>(255.f* cloud->colors[id].z()),
                        255
                    };
                });
            }else{
                std::for_each(std::execution::par_unseq, std::begin(i->ids), std::begin(i->ids) + verticesCountToCopy, [&](size_t id){
                    vertices[id].pos = cloud->vertices[id];
                    vertices[id].col = geo::Pt4<std::uint8_t>{
                        static_cast<std::uint8_t>(255.f* cloud->colors[id].x()),
                        static_cast<std::uint8_t>(255.f* cloud->colors[id].y()),
                        static_cast<std::uint8_t>(255.f* cloud->colors[id].z()),
                        255
                    };
                });
            }
            return verticesCountToCopy;
        }
    }
    return 0;
}

auto DCVideoPlayer::copy_current_cloud(size_t idCamera, std::span<geo::Pt3f> positions, std::span<geo::Pt3f> colors) -> size_t{

    if(auto frame = current_frame(0); frame != nullptr){
        if(auto cloud = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)){
            auto verticesCountToCopy = std::min(cloud->size(), positions.size());
            std::for_each(std::execution::par_unseq, std::begin(i->ids), std::begin(i->ids) + verticesCountToCopy, [&](size_t id){
                const auto &pt = cloud->vertices[id];;
                positions[id] = geo::Pt3f{pt.x(), pt.y(), pt.z()};
                positions[id].x() *= -1.f;
                const auto &col = cloud->colors[id];
                colors[id] = {
                    col.x(), col.y(), col.z()
                };
            });
            return verticesCountToCopy;
        }
    }
    return 0;
}

auto DCVideoPlayer::copy_current_cloud(size_t idCamera, std::span<geo::Pt3f> positions, std::span<geo::Pt3f> colors, std::span<geo::Pt3f> normals, bool applyModelTransform) -> size_t{

    if(auto frame = current_frame(idCamera); frame != nullptr){

        if(auto cloud = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)){

            auto verticesCountToCopy = std::min(cloud->size(), positions.size());
            auto tr = video()->get_transform(idCamera).conv<float>();

            if(applyModelTransform){
                std::for_each(std::execution::par_unseq, std::begin(i->ids), std::begin(i->ids) + verticesCountToCopy, [&](size_t id){
                    const auto &pt = cloud->vertices[id];
                    positions[id] = tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz();
                    positions[id].x() *= -1.f;
                    const auto &col = cloud->colors[id];
                    colors[id] = {
                        col.x(), col.y(), col.z()
                    };
                    const auto &norm = cloud->normals[id];
                    normals[id] = normalize(tr.multiply_vector(geo::Pt4f{norm.x(), norm.y(), norm.z(), 1.f}).xyz());
                    normals[id].x() *= -1.f;
                });
            }else{
                std::for_each(std::execution::par_unseq, std::begin(i->ids), std::begin(i->ids) + verticesCountToCopy, [&](size_t id){
                    const auto &pt = cloud->vertices[id];;
                    positions[id] = geo::Pt3f{pt.x(), pt.y(), pt.z()};
                    positions[id].x() *= -1.f;
                    const auto &col = cloud->colors[id];
                    colors[id] = {
                        col.x(), col.y(), col.z()
                    };
                    normals[id] = cloud->normals[id];
                    normals[id].x() *= -1.f;
                });
            }
            return verticesCountToCopy;
        }
    }
    return 0;
}

auto DCVideoPlayer::copy_all_current_clouds(std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t{

    std::vector<size_t> startId;
    size_t totalNbVertices = 0;
    for(size_t idD = 0; idD < video()->nb_devices(); ++idD){
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
    
    for(size_t idD = 0; idD < video()->nb_devices(); ++idD){
        auto start = startId[idD];
        auto tr = video()->get_transform(idD).conv<float>();
        auto frame = current_frame(idD);

        if(auto cloud = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)){

            if(i->ids.size() < cloud->size()){
                i->ids.resize(cloud->size());
                std::iota(std::begin(i->ids), std::end(i->ids), 0);
            }

            std::for_each(std::execution::par_unseq, std::begin(i->ids), std::begin(i->ids) + cloud->size(), [&](size_t idV){

                const auto &pt = cloud->vertices[idV];
                if(applyModelTransform){
                    vertices[start + idV].pos = geo::Pt3f(tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz());
                }else{
                    vertices[start + idV].pos = pt;
                }
                vertices[start + idV].col = geo::Pt4<std::uint8_t>{
                    static_cast<std::uint8_t>(255.f*cloud->colors[idV].x()),
                    static_cast<std::uint8_t>(255.f*cloud->colors[idV].y()),
                    static_cast<std::uint8_t>(255.f*cloud->colors[idV].z()),
                    255
                };
            });
        }
    }

    return totalNbVertices;
}


auto DCVideoPlayer::video() -> DCVideo* {
    return &i->video;
}

auto DCVideoPlayer::load_from_file(std::string_view path) -> bool{

    if(i->load_from_file(path)){

        states.fileName = path;

        go_to_start_time();

        // retrieve models and send them
        std::vector<DCModelSettings> models;
        for(size_t ii = 0; ii < i->video.nb_devices(); ++ii){
            DCModelSettings model;
            model.transformation = i->video.get_transform(ii).conv<float>();
            models.push_back(model);
        }

        // send models
        initialize_signal(std::move(models));

        return true;
    }

    return false;
}

auto DCVideoPlayer::save_to_file(std::string_view path) -> bool{
    return i->video.save_to_file(path);
}

auto DCVideoPlayer::merge_before(DCVideo &other) -> void{
    i->merge_before(other);
    update_states();
}

auto DCVideoPlayer::update_states() noexcept -> void{
    
    if(states.nbFrames.size() != i->video.nb_devices()){
        states.nbFrames.resize(i->video.nb_devices());
        states.currentFrames.resize(i->video.nb_devices());
    }
    
    for(size_t ii = 0; ii < i->video.nb_devices(); ++ii){
        states.nbFrames[ii]      = i->video.nb_frames(ii);
        states.currentFrames[ii] = current_frame_id(ii);
    }

    states.isPlaying   = i->is_started();
    states.duration    = video()->duration_ms();
    states.currentTime = current_time_ms();
}

auto DCVideoPlayer::save_cloud_to_file(std::string_view path) -> bool{
    static_cast<void>(path);
    Log::error("[DCVideoPlayer::save_cloud_to_file] Not implemented.\n");
    // ...
    return true;
}


auto DCVideoPlayer::display_infos() -> void{
    
    Log::error("[DCVideoPlayer::display_infos] Not implemented.\n");

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
