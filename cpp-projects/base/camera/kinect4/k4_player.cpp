

#include "k4_player.hpp"

// local
#include "utility/logger.hpp"

using namespace tool::camera;

auto K4Player::set_video(const K4VolumetricVideo &video) -> void{

    m_videoResource = video;

    m_currentCompressedFrames.clear();
    m_currentCompressedFrames.resize(m_videoResource.nb_cameras());
    std::fill(std::begin(m_currentCompressedFrames), std::end(m_currentCompressedFrames), nullptr);

    m_currentFrames.clear();
    m_currentFrames.resize(m_videoResource.nb_cameras());
    std::fill(std::begin(m_currentFrames), std::end(m_currentFrames), nullptr);
    update_states();

    std::vector<K4Model> models;
    for(size_t ii = 0; ii < m_videoResource.nb_cameras(); ++ii){
        K4Model model;
        model.transformation = m_videoResource.get_transform(ii).conv<float>();
        models.push_back(model);
    }
    initialize_signal(std::move(models));
}

#include <iostream>
auto K4Player::display_infos() -> void{

    using namespace std::chrono;


    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){

        std::cout << "FROM idc " << idC << "\n";
        auto firstC = video()->get_camera_data(idC)->first_frame_capture_timestamp();//.value();
        for(size_t idC2 = 0; idC2 < video()->nb_cameras(); ++idC2){
            auto camD = video()->get_camera_data(idC2);
            auto diff = nanoseconds(camD->first_frame_capture_timestamp()/**.value()*/ - firstC);
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


auto K4Player::start_playing() -> void{
    if(m_sw.is_started()){
        return;
    }
    m_sw.start();

    update_states();
}

auto K4Player::stop_playing() -> void{
    if(!m_sw.is_started()){
        return;
    }
    m_sw.stop();

    update_states();
}

auto K4Player::current_time_ms() const noexcept -> double{
    return m_sw.ellapsed_milli_s();
}

auto K4Player::is_looping() const noexcept -> bool {
    return m_settings.doLoop;
}

auto K4Player::restart() -> void{
    set_current_time(0.0);
}

auto K4Player::set_current_time(double timeMs) -> void{


    if(timeMs > video()->duration_ms()){
        timeMs = video()->duration_ms();
    }

    m_states.currentTime = timeMs;
    m_sw.set_current_time(m_states.currentTime);

    update_states();
}

auto K4Player::update_time() -> void{

    auto cTime = current_time_ms();
    if(cTime > video()->duration_ms()){
        if(is_looping()){
            set_current_time(0.0);
        }else{
            stop_playing();
        }
    }

    for(size_t idC = 0; idC < m_videoResource.nb_cameras(); ++idC){

        if(m_videoResource.nb_frames(idC) == 0){
            continue;
        }

        if(auto idF = m_videoResource.closest_frame_id_from_time(idC, cTime); idF != -1){//.has_value()){
            m_currentCompressedFrames[idC] = m_videoResource.get_compressed_frame(idC, idF/**.value()*/).lock();
            update_states();
        }else{
//            Logger::error(idF.error());
            Logger::error("[K4Player::update_time] Invalid id for wanted time.");
        }
    }
}

auto K4Player::remove_until_current_frame() -> void{

    for(size_t ii = 0; ii < m_videoResource.nb_cameras(); ++ii){
        m_videoResource.remove_compressed_frames_until(ii, current_frame_id(ii));
    }

    set_current_time(0.0);    
}

auto K4Player::remove_after_current_frame() -> void{

    for(size_t ii = 0; ii < m_videoResource.nb_cameras(); ++ii){
        m_videoResource.remove_compressed_frames_after(ii, current_frame_id(ii));        
    }

    update_states();
}

auto K4Player::merge() -> void{
//    Logger::error("Not implemented.\n");
    merge_cameras(0.005f, {-20.f,-20.f,-20.f}, {+20.f,+20.f,+20.f});
}

auto K4Player::merge_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void{

    if(m_videoResource.nb_cameras() < 1){
        return;
    }

    m_videoResource.merge_all_cameras(voxelSize, minBound, maxBound);

//    m_videoResource.keep_only_one_camera(2);

    m_currentCompressedFrames = {nullptr};
    m_currentFrames = {nullptr};
    update_states();

    std::vector<K4Model> models(1);
    models.front().transformation = m_videoResource.get_transform(0).conv<float>();
    initialize_signal(std::move(models));
}

auto K4Player::update_settings(const K4PlayerSettings &playerS) noexcept -> void {
    m_settings = playerS;
}

auto K4Player::uncompress_frame(size_t idCamera, K4Frame &frame) -> bool{

    if(idCamera >= m_currentCompressedFrames.size()){
        // ...
        return false;
    }

    if(m_currentCompressedFrames[idCamera] == nullptr){
        // ...
        return false;
    }

    if(m_currentCompressedFrames[idCamera]->idCapture == frame.idCapture){
        return false;
    }

    return m_videoResource.uncompress_frame(idCamera, m_currentCompressedFrames[idCamera].get(), frame);
}

auto K4Player::current_frame_id(size_t idCamera) const -> size_t{
    size_t idF = 0;
    for(auto &frame : m_videoResource.get_camera_data(idCamera)->frames){
        if(frame->idCapture == current_frame_id_capture(idCamera)){
            return idF;
        }
        ++idF;
    }
    return 0;
}

auto K4Player::current_frame_id_capture(size_t idCamera) const -> size_t{
    if(m_currentCompressedFrames[idCamera] != nullptr){
        return m_currentCompressedFrames[idCamera]->idCapture;
    }
    return 0;
}

auto K4Player::is_playing() const -> bool{
    return m_sw.is_started();
}

auto K4Player::update_frames() -> void{

    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){

        if(video()->nb_frames(idC) == 0){
            continue;
        }

        if(m_currentFrames[idC] == nullptr){
            m_currentFrames[idC] = std::make_shared<K4Frame>();
        }

        if(uncompress_frame(idC, *m_currentFrames[idC])){
            new_frame_signal(idC, m_currentFrames[idC]);
        }
    }
}

auto K4Player::video() -> K4VolumetricVideo* {
    return &m_videoResource;
}

auto K4Player::load_from_file(std::string_view path) -> bool{

    auto res = m_videoResource.load_from_file(path);

    if(res){
        m_currentCompressedFrames.clear();
        m_currentCompressedFrames.resize(m_videoResource.nb_cameras());
        std::fill(std::begin(m_currentCompressedFrames), std::end(m_currentCompressedFrames), nullptr);

        m_currentFrames.clear();
        m_currentFrames.resize(m_videoResource.nb_cameras());
        std::fill(std::begin(m_currentFrames), std::end(m_currentFrames), nullptr);

        update_states();

        std::vector<K4Model> models;
        for(size_t ii = 0; ii < m_videoResource.nb_cameras(); ++ii){
            K4Model model;
            model.transformation = m_videoResource.get_transform(ii).conv<float>();
            models.push_back(model);
        }
        initialize_signal(std::move(models));
    }   

    return res;
}

auto K4Player::save_to_file(std::string_view path) -> bool{ return m_videoResource.save_to_file(path);}

auto K4Player::merge_before(K4VolumetricVideo &other) -> void{

    if(other.nb_cameras() != m_videoResource.nb_cameras()){
        Logger::error("Incompatible number of camreras.\n");
        return;
    }

    for(size_t ii = 0; ii < m_videoResource.nb_cameras(); ++ii){
        if(m_videoResource.get_camera_data(ii)->transform != other.get_camera_data(ii)->transform){
            Logger::warning("The video to be merged has different transforms.\n");
            break;
        }
    }

    // ...
    update_states();
}

auto K4Player::save_cloud_to_file(std::string_view path) -> bool{
    Logger::error("Not implemented.\n");
    return true;
}

auto K4Player::update_states() -> void{

    if(m_states.nbFrames.size() != m_videoResource.nb_cameras()){
        m_states.nbFrames.resize(m_videoResource.nb_cameras());
        m_states.currentFrames.resize(m_videoResource.nb_cameras());
    }

    for(size_t ii = 0; ii < m_videoResource.nb_cameras(); ++ii){
        m_states.nbFrames[ii] = m_videoResource.nb_frames(ii);
        m_states.currentFrames[ii] = current_frame_id_capture(ii);
    }

    m_states.isPlaying   = m_sw.is_started();
    m_states.duration    = video()->duration_ms();
    m_states.currentTime = current_time_ms();
    states_updated_signal(m_states);
}

