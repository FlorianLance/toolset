
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


#include "dc_video.hpp"

// std
#include <fstream>
#include <format>

// local
#include "utility/logger.hpp"
#include "utility/io_fstream.hpp"
#include "geometry/voxel_grid.hpp"
#include "dc_frame_compressor.hpp"
#include "dc_frame_uncompressor.hpp"

#include "settings/dc_data_settings.hpp"

using namespace tool::cam;
using namespace std::chrono;

DCVideo &DCVideo::operator=(const DCVideo &other){

    m_camerasCompressedFrames = other.m_camerasCompressedFrames;
    for(size_t id = 0; id < other.m_camerasUncompressors.size(); ++id){
        m_camerasUncompressors.push_back(std::make_unique<DCFrameUncompressor>());
    }
    m_camerasTransforms = other.m_camerasTransforms;

    return *this;
}

auto DCVideo::initialize(size_t nbDevices) -> void{

    clean();

    m_camerasCompressedFrames.resize(nbDevices);
    m_camerasUncompressors.resize(nbDevices);
    for(auto &uncompressor : m_camerasUncompressors){
        uncompressor = std::make_unique<DCFrameUncompressor>();
    }    
    m_camerasTransforms  = std::vector<geo::Mat4d>(nbDevices, geo::Mat4d::identity());
}

auto DCVideo::clean() -> void{
    if(nb_cameras() > 0){
        m_camerasCompressedFrames.clear();
        m_camerasUncompressors.clear();
        m_camerasTransforms.clear();
    }
}

auto DCVideo::nb_cameras() const noexcept -> size_t{
    return m_camerasCompressedFrames.size();
}

auto DCVideo::count_frames_from_all_cameras() const noexcept -> size_t{
    size_t count = 0;
    for(const auto &cFrames : m_camerasCompressedFrames){
        count += cFrames.nb_frames();
    }
    return count;
}

auto DCVideo::uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idCamera, size_t idFrame, DCFrame &frame) -> bool{
    if(const auto camD = get_compressed_frames_ptr(idCamera)){
        if(auto cFrame = camD->get_frame_ptr(idFrame)){
            return uncompress_frame(gSettings, idCamera, cFrame, frame);
        }
    }
    return false;
}

auto DCVideo::uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idCamera, DCCompressedFrame *cFrame, DCFrame &frame) -> bool{
    if(auto unc = uncompressor(idCamera); unc != nullptr){
        return unc->uncompress(gSettings, cFrame, frame);
    }
    return false;
}

auto DCVideo::first_frame_received_timestamp() const noexcept -> std::int64_t{
    auto start = std::numeric_limits<std::int64_t>::max();
    bool found = false;
    for(const auto &cameraCompressedFrames : m_camerasCompressedFrames){
        if(auto camStartTS = cameraCompressedFrames.first_frame_received_timestamp(); camStartTS != -1){
            if(start > camStartTS){
                start = camStartTS;
                found = true;
            }
        }
    }
    if(found){
        return start;
    }
    return -1;
}

auto DCVideo::last_frame_received_timestamp() const noexcept -> std::int64_t{
    std::int64_t end = -1;
    bool found = false;
    for(const auto &cameraCompressedFrames : m_camerasCompressedFrames){
        if(auto camEndTS = cameraCompressedFrames.last_frame_received_timestamp(); camEndTS!=-1){
            if(end < camEndTS){
                end = camEndTS;
                found = true;
            }
        }
    }
    if(found){
        return end;
    }
    return -1;
}

auto DCVideo::camera_duration_ms(size_t idCamera) const noexcept -> double{
    if(idCamera >= nb_cameras()){
        return 0.0;
    }
    return m_camerasCompressedFrames[idCamera].duration_ms();
}

auto DCVideo::camera_first_frame_received_timestamp(size_t idCamera) const noexcept -> int64_t{
    if(idCamera >= nb_cameras()){
        return -1;
    }
    return m_camerasCompressedFrames[idCamera].first_frame_received_timestamp();
}

auto DCVideo::camera_last_frame_received_timestamp(size_t idCamera) const noexcept -> int64_t{
    if(idCamera >= nb_cameras()){
        return -1;
    }
    return m_camerasCompressedFrames[idCamera].last_frame_received_timestamp();
}

auto DCVideo::duration_ms() const noexcept -> double {

    if(nb_cameras() == 0){
        return 0.0;
    }
    auto lfc = last_frame_received_timestamp();
    auto ffc = first_frame_received_timestamp();
    if(lfc != -1&& ffc != -1 ){
        return duration_cast<microseconds>(nanoseconds(lfc-ffc)).count()*0.001;
    }
    return 0.0;
}

auto DCVideo::get_timestamp_diff_time_ms(std::int64_t t1, std::int64_t t2) noexcept -> double{
    return duration_cast<microseconds>(nanoseconds(std::max(t1,t2)) - nanoseconds(std::min(t1,t2))).count() / 1000.0;
}

auto DCVideo::closest_frame_id_from_time(size_t idCamera, double timeMs) const noexcept -> std::int64_t{

    if(idCamera >= nb_cameras()){
        Logger::error("[DCVolumetricVideo::closest_frame_id_from_time] Camera id invalid.\n"sv);
        return -1;
    }

    if(nb_frames(idCamera) == 0){
        Logger::error("[DCVolumetricVideo::closest_frame_id_from_time] No frame available.\n"sv);
        return -1;
    }    

    auto framesPtr = get_compressed_frames_ptr(idCamera);
    if(framesPtr->nb_frames() == 1){
        return 0;
    }

    auto ffTS = nanoseconds(first_frame_received_timestamp());
    // init with first diff
    double prevDiff = std::abs(get_timestamp_diff_time_ms(ffTS.count(), camera_first_frame_received_timestamp(idCamera))-timeMs);

    for(size_t idFrame = 1; idFrame < framesPtr->nb_frames(); ++idFrame){        
        auto timeFrameMs = get_timestamp_diff_time_ms(ffTS.count(), framesPtr->frames[idFrame]->receivedTS);
        auto diffMs      = std::abs(timeFrameMs-timeMs);
        if(diffMs > prevDiff){
            return idFrame-1;
        }
        prevDiff = diffMs;
    }
    return framesPtr->nb_frames()-1;
}

auto DCVideo::nb_frames(size_t idCamera) const noexcept -> size_t{
    if(idCamera < nb_cameras()){
        return get_compressed_frames_ptr(idCamera)->nb_frames();
    }
    return 0;
}

auto DCVideo::min_nb_frames() const noexcept  -> size_t {

    if(m_camerasCompressedFrames.empty()){
        return 0;
    }

    size_t minFrames = m_camerasCompressedFrames.front().nb_frames();
    for(auto &frames : m_camerasCompressedFrames){
        if(minFrames > frames.nb_frames()){
            minFrames = frames.nb_frames();
        }
    }
    return minFrames;
}

auto DCVideo::get_compressed_frames_ptr(size_t idCamera) const noexcept -> const DCCompressedFrames*{
    if(idCamera < nb_cameras()){
        return &m_camerasCompressedFrames[idCamera];
    }
    Logger::error(std::format("[DCVolumetricVideo::get_compressed_frames_ptr] Invalid camera id: [{}], number of cameras available: [{}]\n", idCamera, nb_cameras()));
    return nullptr;
}

auto DCVideo::get_compressed_frame(size_t idCamera, size_t idFrame) -> std::weak_ptr<DCCompressedFrame>{
    if(const auto camD = get_compressed_frames_ptr(idCamera)){
        return camD->get_compressed_frame(idFrame);
    }
    return {};
}

auto DCVideo::remove_compressed_frames_until(size_t idCamera, size_t idFrame) -> void{
    if(idCamera < nb_cameras()){
        m_camerasCompressedFrames[idCamera].remove_frames_until(idFrame);
    }
}

auto DCVideo::remove_compressed_frames_after(size_t idCamera, size_t idFrame) -> void{
    if(idCamera < nb_cameras()){
        m_camerasCompressedFrames[idCamera].remove_frames_after(idFrame);
    }
}

auto DCVideo::keep_only_one_camera(size_t idCamera) -> void{
    m_camerasUncompressors.resize(1);
    auto cameraFrames = std::move(m_camerasCompressedFrames[idCamera]);
    m_camerasCompressedFrames = {std::move(cameraFrames)};
}

auto DCVideo::keep_only_cameras_from_id(const std::vector<size_t> &ids) -> void{

    std::vector<std::unique_ptr<DCFrameUncompressor>> uncompressors;
    std::vector<DCCompressedFrames> framesPerCamera;

    for(const auto &id : ids){
        uncompressors.push_back(std::move(m_camerasUncompressors[id]));
        framesPerCamera.push_back(std::move(m_camerasCompressedFrames[id]));
    }
    std::swap(m_camerasUncompressors, uncompressors);
    std::swap(m_camerasCompressedFrames, framesPerCamera);
}

auto DCVideo::remove_all_cameras_compressed_frames() noexcept -> void{
    for(auto &frames : m_camerasCompressedFrames){
        frames.clean();
    }
}

auto DCVideo::remove_all_compressed_frames(size_t idCamera) noexcept -> void{
    if(idCamera < nb_cameras()){
        m_camerasCompressedFrames[idCamera].clean();
    }
}

auto DCVideo::replace_compressed_frame(size_t idCamera, size_t idFrame, std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(idCamera < nb_cameras()){
        if(idFrame < nb_frames(idCamera)){
            m_camerasCompressedFrames[idCamera].frames[idFrame] = std::move(frame);
        }
    }
}

auto DCVideo::get_transform(size_t idCamera) const -> tool::geo::Mat4d{
    if(idCamera < nb_cameras()){
        return m_camerasTransforms[idCamera];
    }
    Logger::error("[DCVolumetricVideo::get_transform] Invalid camera id: [{}], number of cameras available: [{}]\n", idCamera, nb_cameras());
    return geo::Mat4d::identity();
}

auto DCVideo::set_transform(size_t idCamera, geo::Mat4d tr) -> void{
    if(idCamera < nb_cameras()){
        m_camerasTransforms[idCamera] = tr;
        return;
    }
    Logger::error("[DCVolumetricVideo::set_transform]Invalid camera id: [{}], number of cameras available: [{}]\n", idCamera, nb_cameras());
}

auto DCVideo::add_compressed_frame(size_t idCamera, std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(idCamera >= nb_cameras()){
        m_camerasCompressedFrames.resize(idCamera+1);
    }

    m_camerasCompressedFrames[idCamera].add_compressed_frame(std::move(frame));
}

auto DCVideo::save_to_file(std::string_view path) -> bool{

    if(path.length() == 0){
        Logger::error("[DCVolumetricVideo::save_to_file] Empty path.\n");
        return false;
    }

    if(count_frames_from_all_cameras() == 0){
        Logger::error("[DCVolumetricVideo::save_to_file] No available frames to save.\n");
        return false;
    }

    std::ofstream file;
    file.open(path.data(), std::ios_base::binary);
    if(!file.is_open()){
        Logger::error(std::format("[DCVolumetricVideo::save_to_file] Cannot save compressed frames to {}.\n", path));
        return false;
    }
    file.exceptions(std::ofstream::badbit | std::ofstream::failbit);

    // write file
    bool success = false;
    try{
        write_file(file);
        success = true;
    }catch(const std::exception &e){
        Logger::error(std::format("[DCVolumetricVideo::save_to_file] Error happend during writing file [{}].\n", e.what()));
    }
    file.close();

    return success;
}

auto DCVideo::load_from_file(std::string_view path) -> bool{

    if(path.length() == 0){
        Logger::error("[DCVolumetricVideo::load_from_file] Empty path.\n");
        return false;
    }

    // open file
    std::ifstream file(path.data(), std::ios_base::binary);    
    if(!file.is_open()){
        Logger::error(std::format("[DCVolumetricVideo::load_from_file] Cannot open compressed frames file: [{}].\n", path));
        return false;
    }
    file.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

    // clean data
    remove_all_cameras_compressed_frames();

    // read file
    bool success = false;
    try{
        success = read_file(file);
    }catch(const std::exception &e){
        Logger::error(std::format("[DCVolumetricVideo::load_from_file] Error happend during reading file [{}].\n", e.what()));
        return false;
    }

    return success;
}

auto DCVideo::uncompressor(size_t idCamera) noexcept -> DCFrameUncompressor*{
    if(idCamera < m_camerasUncompressors.size()){
        return m_camerasUncompressors[idCamera].get();
    }
    return nullptr;
}

auto DCVideo::merge_all_cameras(const DCFrameGenerationSettings &gSettings, float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void{

    if(m_camerasCompressedFrames.empty()){
        return;
    }

    if(m_camerasCompressedFrames.front().frames.empty()){
        return;
    }

    for(const auto &cData : m_camerasCompressedFrames){
        if(!cData.check_same_mode_for_every_frame()){
            Logger::error("[DCVolumetricVideo::merge_all_cameras] Mode inconstancie accros cameras frames\n");
            return;
        }
    }
    auto mode = m_camerasCompressedFrames.front().first_frame_ptr()->mode;
    for(const auto &cData : m_camerasCompressedFrames){
        if(cData.first_frame_ptr()->mode != mode){
            Logger::error("[DCVolumetricVideo::merge_all_cameras] Mode inconstancie accros cameras frames\n");
            return;
        }
    }

    auto c0FirstFrameTS = m_camerasCompressedFrames.front().frames.front()->receivedTS;

    DCFrameCompressor compressor;
    for(size_t idF = 0; idF < nb_frames(0); ++idF){

        auto c0Frame = m_camerasCompressedFrames.front().frames[idF].get();
        auto c0Time  = c0Frame->receivedTS;
        auto c0TimeMs= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(c0Time - c0FirstFrameTS));

        DCFrame final;
        if(!uncompress_frame(gSettings, 0, idF, final)){
            continue;
        }

        final.mode            = DCMode::Merged;
        final.idCapture       = c0Frame->idCapture;
        final.afterCaptureTS  = c0Frame->afterCaptureTS;
        final.receivedTS      = c0Frame->receivedTS;

        final.rgbaColor.reset();
        final.rgbDepth.reset();
        final.rgbInfra.reset();
        final.depth.reset();
        final.infra.reset();

        geo::VoxelGrid grid(voxelSize, minBound, maxBound);        
        grid.add_cloud(final.cloud, m_camerasTransforms.front().conv<float>());

        for(size_t jj = 1; jj < nb_cameras(); ++jj){

            size_t idF = 0;
            if(auto id = closest_frame_id_from_time(jj, static_cast<double>(c0TimeMs.count())); id != -1){
                idF = id;
            }else{
                continue;
            }

            DCFrame current;
            if(!uncompress_frame(gSettings, jj, idF, current)){
                continue;
            }

            grid.add_cloud(current.cloud, m_camerasTransforms[jj].conv<float>());
        }
        grid.compute_grid();
        grid.convert_to_cloud(final.cloud);

        // compress frame
        //m_camerasCompressedFrames.front().frames[idF] = compressor.compress(final, 90);
    }

    m_camerasTransforms.front() = geo::Mat4d::identity();

    m_camerasCompressedFrames.resize(1);
    m_camerasUncompressors.resize(1);
}

auto DCVideo::merge_cameras_frame_id(const DCFrameGenerationSettings &gSettings, size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, tool::geo::ColoredCloudData &cloud) -> void{

    if(idFrame >= min_nb_frames()){
        // ...
        return ;
    }

    geo::VoxelGrid grid(sizeVoxel, minBound, maxBound);

    DCFrame frame;
    for(size_t ii = 0; ii < nb_cameras(); ++ii){        
        uncompress_frame(gSettings, ii, idFrame, frame);
        grid.add_cloud(frame.cloud, get_transform(ii).conv<float>());
    }
    grid.compute_grid();
    grid.convert_to_cloud(cloud);
}

auto DCVideo::merge_cameras_frame_id(const DCFrameGenerationSettings &gSettings, size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, DCFrame &frame) -> void{

    frame = DCFrame();
    if(idFrame >= min_nb_frames()){
        // ...
        return;
    }

    geo::VoxelGrid grid(sizeVoxel, minBound, maxBound);
    for(size_t ii = 0; ii < nb_cameras(); ++ii){
        DCFrame uFrame;
        uncompress_frame(gSettings, ii, idFrame, uFrame);
        grid.add_cloud(uFrame.cloud, get_transform(ii).conv<float>());

        if(ii == 0){
            frame.mode           = DCMode::Merged;
            frame.idCapture      = uFrame.idCapture;
            frame.afterCaptureTS = uFrame.afterCaptureTS;
            frame.receivedTS     = uFrame.receivedTS;

            frame.imu        = uFrame.imu; // merge imu ?
            frame.audioFrames    = uFrame.audioFrames; // merge audio ?
            frame.bodyTracking         = uFrame.bodyTracking; // merge bodies ?
        }
    }
    grid.compute_grid();
    merge_cameras_frame_id(gSettings, idFrame, sizeVoxel, minBound, maxBound, frame.cloud);
}


auto DCVideo::total_audio_frames_size(size_t idCamera) const -> size_t{
    if(idCamera < m_camerasCompressedFrames.size()){
        size_t total = 0;
        for(const auto &frame : m_camerasCompressedFrames[idCamera].frames){
            for(const auto &audioChannel : frame->audioFrames){
                total += audioChannel.size();
            }
        }
        return total;
    }
    Logger::error("[DCVolumetricVideo::total_audio_frames_size] Invalid camera id.\n");
    return 0;
}

auto DCVideo::get_audio_samples_all_channels(size_t idCamera, std::vector<std::vector<float> > &audioBuffer) -> void{

    auto camData = &m_camerasCompressedFrames[idCamera];

    size_t samplesCount = 0;
    for(const auto &frame : camData->frames){
        if(frame != nullptr){
            samplesCount += frame->audioFrames.size();
        }
    }

    if(samplesCount == 0){
        Logger::error("");
        return;
    }

    audioBuffer.resize(7);
    for(auto &channelAudioBuffer : audioBuffer){
        channelAudioBuffer.reserve(samplesCount);
    }

    for(const auto &frame : camData->frames){
        if(frame != nullptr){
            for(size_t idChannel = 0; idChannel < 7; ++idChannel){
                for(const auto &channelsData : frame->audioFrames){
                    audioBuffer[idChannel].push_back(channelsData[idChannel]);
                }
            }
        }
    }
}

auto DCVideo::get_audio_samples_all_channels(size_t idCamera, std::vector<float> &audioBuffer) -> void{

    auto camData = &m_camerasCompressedFrames[idCamera];

    size_t samplesCount = 0;
    for(const auto &frame : camData->frames){
        samplesCount += frame->audioFrames.size();
    }

    if(samplesCount == 0){
        Logger::error("");
        return;
    }

    audioBuffer.resize(samplesCount*7);

    size_t id = 0;
    for(const auto &frame : camData->frames){
        for(const auto &channelsData : frame->audioFrames){
            for(size_t idChannel = 0; idChannel < 7; ++idChannel){
                audioBuffer[id++] = channelsData[idChannel];
            }
        }
    }
}

#include <iostream>
auto DCVideo::read_file(std::ifstream &file) -> bool{

    // read mode
    std::int8_t videoType;
    read(videoType, file);

    std::cout << "VIDEO TYPE" << (int)videoType << "\n";
    if(videoType == 0){
        read_legacy_full_video_file(file);
        return true;
    }else if(videoType == 1){
        read_legacy_cloud_video_file(file);
        return true;
    }else if(videoType != 2){
        Logger::error("[DCVolumetricVideo::read_file] Invalid video type.\n");
        return false;
    }    

    // read nb of cameras
    std::int8_t nbCameras;
    read(nbCameras, file);    
    initialize(nbCameras);

    // read infos per camera
    std::int32_t nbFrames;
    size_t currentC = 0;
    for(size_t idC = 0; idC < m_camerasCompressedFrames.size(); ++idC){

        // read nb frames
        read(nbFrames, file);

        if(nbFrames > 0){

            auto &cameraCompressedFrames = m_camerasCompressedFrames[currentC];
            cameraCompressedFrames.frames.reserve(nbFrames);

            for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
                cameraCompressedFrames.frames.push_back(std::make_shared<DCCompressedFrame>());
            }

            // calibration matrix
            read_array(m_camerasTransforms[currentC].array.data(), file, 16);

            ++currentC;
        }else{
            geo::Mat4d transform;
            read_array(transform.array.data(), file, 16);
        }
    }
    m_camerasCompressedFrames.resize(currentC);

    // read frames
    for(auto &cameraData : m_camerasCompressedFrames){
        for(auto &frame : cameraData.frames){
            double timeMsNotUsed;
            read(timeMsNotUsed, file); // read time ms
            frame->init_from_file_stream(file);
        }
    }

    // remove empty cameras
    // TODO: ...

    return true;
}

auto DCVideo::write_file(std::ofstream &file) -> void{

    // write video type
    std::int8_t videoType = 2;
    write(videoType, file);                                                                         // std::int8_t

    // write nb of cameras
    write(static_cast<std::int8_t>(m_camerasCompressedFrames.size()), file);                        // std::int8_t

    // write infos per camera
    for(size_t idCamera = 0; idCamera < nb_cameras(); ++idCamera){
        // nb frames
        write(static_cast<std::int32_t>(m_camerasCompressedFrames[idCamera].frames.size()), file);  // std::int32_t * cameras count
        // calibration matrix
        write_array(m_camerasTransforms[idCamera].array.data(), file, 16);                          // double * 16
    }

    // writes frames
    for(const auto &cameraCompressedFrames : m_camerasCompressedFrames){
        for(const auto &cFrame : cameraCompressedFrames.frames){
            double timeMsNotUsed = 0.0;
            write(timeMsNotUsed, file);
            cFrame->write_to_file_stream(file);
        }
    }
}

auto DCVideo::read_legacy_cloud_video_file(std::ifstream &file) -> void{

    // read nb of cameras
    std::int8_t nbCameras;
    read(nbCameras, file);
    initialize(nbCameras);

    // read infos per camera
    std::int32_t nbFrames;
    for(size_t idCamera = 0; idCamera < nb_cameras(); ++idCamera){

        // read nb frames
        read(nbFrames, file);

        // create frames
        m_camerasCompressedFrames[idCamera].frames.reserve(nbFrames);
        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
            m_camerasCompressedFrames[idCamera].frames.push_back(std::make_shared<DCCompressedFrame>());
        }

        // calibration matrix
        read_array(m_camerasTransforms[idCamera].array.data(), file, 16);
    }

    // read frames
    for(auto &cameraData : m_camerasCompressedFrames){
        for(auto &frame : cameraData.frames){
            // read frame
            frame->init_legacy_cloud_frame_from_file_stream(file);
        }
    }
}

auto DCVideo::read_legacy_full_video_file(std::ifstream &file) -> void{

    // read nb of cameras
    std::int8_t nbCameras;
    read(nbCameras, file);
    initialize(nbCameras);

    // read infos per camera
    std::int32_t nbFrames;
    for(size_t idCamera = 0; idCamera < nb_cameras(); ++idCamera){

        // read nb frames
        read(nbFrames, file);

        // create frames
        m_camerasCompressedFrames[idCamera].frames.reserve(nbFrames);
        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
            m_camerasCompressedFrames[idCamera].frames.push_back(std::make_shared<DCCompressedFrame>());
        }

        // calibration matrix
        read_array(m_camerasTransforms[idCamera].array.data(), file, 16);
    }

    // read frames
    for(auto &cameraData : m_camerasCompressedFrames){
        for(auto &frame : cameraData.frames){

            std::int32_t idFrame;
            std::int64_t timestamp;

            // # read frame info
            read(idFrame, file);
            read(timestamp, file);
            frame->afterCaptureTS = timestamp;
            frame->receivedTS     = timestamp;

            // read frame            
            frame->init_legacy_full_frame_from_file_stream(file);
        }
    }
}
