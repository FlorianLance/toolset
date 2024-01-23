

#include "dc_player.hpp"

// std
#include <iostream>

// local
#include "utility/logger.hpp"
#include "utility/stop_watch.hpp"

using namespace tool::cam;

struct DCPlayer::Impl{

    StopWatch sw;
    std::vector<std::shared_ptr<DCCompressedFrame>> currentCompressedFrames;
    std::vector<std::shared_ptr<DCFrame>> currentFrames;

    DCPlayerStates states;
    DCPlayerSettings settings;
    DCVolumetricVideo videoResource;
};

DCPlayer::DCPlayer(): i(std::make_unique<Impl>()){

}

DCPlayer::~DCPlayer(){

}


auto DCPlayer::set_video(const DCVolumetricVideo &video) -> void{

    i->videoResource = video;

    i->currentCompressedFrames.clear();
    i->currentCompressedFrames.resize(i->videoResource.nb_cameras());
    std::fill(std::begin(i->currentCompressedFrames), std::end(i->currentCompressedFrames), nullptr);

    i->currentFrames.clear();
    i->currentFrames.resize(i->videoResource.nb_cameras());
    std::fill(std::begin(i->currentFrames), std::end(i->currentFrames), nullptr);
    update_states();

    std::vector<DCModelSettings> models;
    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        DCModelSettings model;
        model.transformation = i->videoResource.get_transform(ii).conv<float>();
        models.push_back(model);
    }
    initialize_signal(std::move(models));
}

auto DCPlayer::display_infos() -> void{

    using namespace std::chrono;


    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){

        std::cout << "FROM idc " << idC << "\n";
        auto firstC = video()->get_camera_data(idC)->first_frame_capture_timestamp();
        for(size_t idC2 = 0; idC2 < video()->nb_cameras(); ++idC2){
            auto camD = video()->get_camera_data(idC2);
            auto diff = nanoseconds(camD->first_frame_capture_timestamp() - firstC);
            std::cout << "  Camera " << idC2 << " mdiff: " <<  duration_cast<milliseconds>(diff) << " "<< camD->first_frame_capture_timestamp()/**.value()*/ << " " << camD->last_frame_capture_timestamp()/**.value()*/ << "\n";
        }
    }

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


auto DCPlayer::start_playing() -> void{
    if(i->sw.is_started()){
        return;
    }
    i->sw.start();

    update_states();
}

auto DCPlayer::stop_playing() -> void{
    if(!i->sw.is_started()){
        return;
    }
    i->sw.stop();

    update_states();
}

auto DCPlayer::current_time_ms() const noexcept -> double{
    return i->sw.ellapsed_milli_s();
}

auto DCPlayer::is_looping() const noexcept -> bool {
    return i->settings.doLoop;
}

auto DCPlayer::restart() -> void{
    set_current_time(0.0);
}

auto DCPlayer::set_current_time(double timeMs) -> void{


    if(timeMs > video()->duration_ms()){
        timeMs = video()->duration_ms();
    }

    i->states.currentTime = timeMs;
    i->sw.set_current_time(i->states.currentTime);

    update_states();
}

auto DCPlayer::update_time() -> void{

    auto cTime = current_time_ms();
    if(cTime > video()->duration_ms()){
        if(is_looping()){
            set_current_time(0.0);
        }else{
            stop_playing();
        }
    }

    for(size_t idC = 0; idC < i->videoResource.nb_cameras(); ++idC){

        if(i->videoResource.nb_frames(idC) == 0){
            continue;
        }

        if(auto idF = i->videoResource.closest_frame_id_from_time(idC, cTime); idF != -1){
            i->currentCompressedFrames[idC] = i->videoResource.get_compressed_frame(idC, idF).lock();
            update_states();
        }else{
            Logger::error("[DCPlayer::update_time] Invalid id for wanted time.");
        }
    }
}

auto DCPlayer::remove_until_current_frame() -> void{

    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        i->videoResource.remove_compressed_frames_until(ii, current_frame_id(ii));
    }

    set_current_time(0.0);    
}

auto DCPlayer::remove_after_current_frame() -> void{

    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        i->videoResource.remove_compressed_frames_after(ii, current_frame_id(ii));
    }

    update_states();
}

auto DCPlayer::merge() -> void{
    Logger::error("MERGE.\n");
    remove_empty_cameras();
//    merge_cameras(0.005f, {-20.f,-20.f,-20.f}, {+20.f,+20.f,+20.f});
}

auto DCPlayer::merge_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void{

    if(i->videoResource.nb_cameras() < 1){
        return;
    }

    i->videoResource.merge_all_cameras(voxelSize, minBound, maxBound);

//    i->videoResource.keep_only_one_camera(2);

    i->currentCompressedFrames = {nullptr};
    i->currentFrames = {nullptr};
    update_states();

    std::vector<DCModelSettings> models(1);
    models.front().transformation = i->videoResource.get_transform(0).conv<float>();
    initialize_signal(std::move(models));
}

auto DCPlayer::remove_empty_cameras() -> void{

    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        Logger::message(std::format("\n\ncam {} nb frames {}\n",ii, i->videoResource.nb_frames(ii)));


        for(size_t jj = 0; jj < i->videoResource.nb_frames(ii); ++jj){
            auto nbV = i->videoResource.get_camera_data(ii)->valid_vertices_count(ii);
            Logger::message(std::format(" {}",nbV));
        }
    }

    size_t idC = 0;
    for(const auto &ccf : i->currentCompressedFrames){
        if(ccf != nullptr){
            Logger::message(std::format(" cam {} size {} \n", idC, ccf->cloud_vertices_size()));
        }
        idC++;
    }

    return;

    std::vector<size_t> idCamerasToKeep;
    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        if(i->videoResource.nb_frames(ii) > 0){
            Logger::message(std::format("keep camera {}",ii));
            idCamerasToKeep.push_back(ii);
        }
    }   
    i->videoResource.keep_only_cameras_from_id(idCamerasToKeep);

    i->currentCompressedFrames.resize(idCamerasToKeep.size());
    for(auto &ccf : i->currentCompressedFrames){
        ccf = nullptr;
    }
    i->currentFrames.resize(idCamerasToKeep.size());
    for(auto &cf : i->currentFrames){
        cf = nullptr;
    }
    update_states();

    std::vector<DCModelSettings> models;
    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        DCModelSettings model;
        model.transformation = i->videoResource.get_transform(ii).conv<float>();
        models.push_back(model);
    }
    initialize_signal(std::move(models));
}

auto DCPlayer::update_settings(const DCPlayerSettings &playerS) noexcept -> void {
    i->settings = playerS;
}

auto DCPlayer::uncompress_frame(size_t idCamera, DCFrame &frame) -> bool{

    if(idCamera >= i->currentCompressedFrames.size()){
        // ...
        return false;
    }

    if(i->currentCompressedFrames[idCamera] == nullptr){
        // ...
        return false;
    }

    if(i->currentCompressedFrames[idCamera]->idCapture == frame.idCapture){
        return false;
    }

    return i->videoResource.uncompress_frame(idCamera, i->currentCompressedFrames[idCamera].get(), frame);
}

auto DCPlayer::current_frame_id(size_t idCamera) const -> size_t{
    size_t idF = 0;
    for(auto &frame : i->videoResource.get_camera_data(idCamera)->frames){
        if(frame->idCapture == current_frame_id_capture(idCamera)){
            return idF;
        }
        ++idF;
    }
    return 0;
}

auto DCPlayer::current_frame_id_capture(size_t idCamera) const -> size_t{
    if(i->currentCompressedFrames[idCamera] != nullptr){
        return i->currentCompressedFrames[idCamera]->idCapture;
    }
    return 0;
}

auto DCPlayer::is_playing() const -> bool{
    return i->sw.is_started();
}

auto DCPlayer::update_frames() -> void{

    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){

        if(video()->nb_frames(idC) == 0){
            continue;
        }

        if(i->currentFrames[idC] == nullptr){
            i->currentFrames[idC] = std::make_shared<DCFrame>();
        }

        if(uncompress_frame(idC, *i->currentFrames[idC])){
            new_frame_signal(idC, i->currentFrames[idC]);
        }
    }
}

auto DCPlayer::video() -> DCVolumetricVideo* {
    return &i->videoResource;
}

auto DCPlayer::load_from_file(std::string_view path) -> bool{

    auto res = i->videoResource.load_from_file(path);

    if(res){
        i->currentCompressedFrames.clear();
        i->currentCompressedFrames.resize(i->videoResource.nb_cameras());
        std::fill(std::begin(i->currentCompressedFrames), std::end(i->currentCompressedFrames), nullptr);

        i->currentFrames.clear();
        i->currentFrames.resize(i->videoResource.nb_cameras());
        std::fill(std::begin(i->currentFrames), std::end(i->currentFrames), nullptr);

        update_states();

        std::vector<DCModelSettings> models;
        for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
            DCModelSettings model;
            model.transformation = i->videoResource.get_transform(ii).conv<float>();
            models.push_back(model);
        }
        initialize_signal(std::move(models));
    }   

    return res;
}

auto DCPlayer::save_to_file(std::string_view path) -> bool{ return i->videoResource.save_to_file(path);}

auto DCPlayer::merge_before(DCVolumetricVideo &other) -> void{

    if(other.nb_cameras() != i->videoResource.nb_cameras()){
        Logger::error("[DCPlayer::merge_before] Incompatible number of cameras.\n");
        return;
    }

    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        if(i->videoResource.get_camera_data(ii)->transform != other.get_camera_data(ii)->transform){
            Logger::warning("[DCPlayer::merge_before] The video to be merged has different transforms.\n");
            break;
        }
    }

    // ...
    update_states();
}

auto DCPlayer::save_cloud_to_file(std::string_view path) -> bool{
    Logger::error("[DCPlayer::save_cloud_to_file] Not implemented.\n");

    size_t idC = 0;
    for(const auto &ccf : i->currentCompressedFrames){
        if(ccf != nullptr){
            Logger::message(std::format(" cam {} size {} \n", idC, ccf->cloud_vertices_size()));
        }
        idC++;
    }

    return true;
}

auto DCPlayer::update_states() -> void{

    if(i->states.nbFrames.size() != i->videoResource.nb_cameras()){
        i->states.nbFrames.resize(i->videoResource.nb_cameras());
        i->states.currentFrames.resize(i->videoResource.nb_cameras());
    }

    for(size_t ii = 0; ii < i->videoResource.nb_cameras(); ++ii){
        i->states.nbFrames[ii] = i->videoResource.nb_frames(ii);
        i->states.currentFrames[ii] = current_frame_id_capture(ii);
    }

    i->states.isPlaying   = i->sw.is_started();
    i->states.duration    = video()->duration_ms();
    i->states.currentTime = current_time_ms();
    states_updated_signal(i->states);
}

