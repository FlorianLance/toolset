
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


#include "dc_volumetric_video.hpp"

// std
#include <fstream>
#include <ranges>
#include <format>

// local
#include "utility/logger.hpp"
#include "utility/io_fstream.hpp"
#include "geometry/voxel_grid.hpp"

#include "dc_frame_compressor.hpp"
#include "dc_frame_uncompressor.hpp"

#include "camera/kinect4/k4_compressed_frame.hpp"
//#include "camera/orbbec/orbbec_frame_uncompressor.hpp"

using namespace tool::camera;


DCVolumetricVideo &DCVolumetricVideo::operator=(const DCVolumetricVideo &other){

    m_framesPerCamera = other.m_framesPerCamera;
    for(size_t id = 0; id < other.m_uncompressors.size(); ++id){
        m_uncompressors.push_back(std::make_unique<DCFrameUncompressor>());
    }

    return *this;
}

auto DCVolumetricVideo::initialize(size_t nbDevices) -> void{

    if(m_framesPerCamera.size() > 0){
        Logger::error("[DCVolumetricVideo::initialize] Video must be cleaned before initialization.\n");
        return;
    }

    m_framesPerCamera.reserve(nbDevices);
    for(size_t id = 0; id < nbDevices; ++id){
        m_uncompressors.push_back(std::make_unique<DCFrameUncompressor>());
    }
}

auto DCVolumetricVideo::nb_cameras() const noexcept -> size_t{
    return m_framesPerCamera.size();
}

auto DCVolumetricVideo::count_frames_from_all_cameras() const noexcept -> size_t{
    size_t count = 0;
    for(const auto &cFrames : m_framesPerCamera){
        count += cFrames.nb_frames();
    }
    return count;
}

auto DCVolumetricVideo::uncompress_frame(size_t idCamera, size_t idFrame, DCFrame &frame) -> bool{
    if(const auto camD = get_camera_data(idCamera)){
        if(auto cFrame = camD->get_frame_ptr(idFrame)){
            return uncompress_frame(idCamera, cFrame, frame);
        }
    }
    return false;
}

auto DCVolumetricVideo::uncompress_frame(size_t idCamera, DCCompressedFrame *cFrame, DCFrame &frame) -> bool{
    if(auto unc = uncompressor(idCamera); unc != nullptr){
        return unc->uncompress(cFrame, frame);
    }
    return false;
}

auto DCVolumetricVideo::first_frame_capture_timestamp() const noexcept -> std::int64_t{
    auto start = std::numeric_limits<std::int64_t>::max();
    bool found = false;
    for(const auto &cFrame : m_framesPerCamera){
        if(auto camStart = cFrame.first_frame_capture_timestamp(); camStart != -1){
            if(start > camStart){
                start = camStart;
                found = true;
            }
        }
    }
    if(found){
        return start;
    }
    return -1;
}

auto DCVolumetricVideo::last_frame_capture_timestamp() const noexcept -> std::int64_t{
    std::int64_t end = -1;
    bool found = false;
     for(const auto &cFrame : m_framesPerCamera){
        if(auto camEnd = cFrame.last_frame_capture_timestamp(); camEnd!=-1){
            if(end < camEnd){
                end = camEnd;
                found = true;
            }
        }
    }
    if(found){
        return end;
    }
    return -1;
}

auto DCVolumetricVideo::duration_ms() const noexcept -> double {
    using namespace std::chrono;
    auto lfc = last_frame_capture_timestamp();
    auto ffc = first_frame_capture_timestamp();
    if(lfc != -1&& ffc != -1 ){
        return duration_cast<microseconds>(nanoseconds(lfc-ffc)).count()*0.001;
    }
    return 0.0;
}

auto DCVolumetricVideo::get_timestamp_diff_time_ms(std::int64_t t1, std::int64_t t2) noexcept -> double{
    using namespace std::chrono;
    return duration_cast<microseconds>(nanoseconds(std::max(t1,t2)) - nanoseconds(std::min(t1,t2))).count() / 1000.0;
}


auto DCVolumetricVideo::closest_frame_id_from_time(size_t idCamera, double timeMs) const noexcept -> std::int64_t{

    if(idCamera >= nb_cameras()){
        Logger::error("[DCVolumetricVideo::closest_frame_id_from_time]Camera id invalid."sv);
        return -1;
    }

    if(nb_frames(idCamera) == 0){
        Logger::error("[DCVolumetricVideo::closest_frame_id_from_time] No frame available."sv);
        return -1;
    }

    using namespace std::chrono;

    auto ffTs = nanoseconds(first_frame_capture_timestamp());
    size_t idFrame = 0;
    double prevDiff = std::numeric_limits<double>::max();

    for(const auto& frame : get_camera_data(idCamera)->frames){
        auto timeFrameMs = get_timestamp_diff_time_ms(ffTs.count(), frame->afterCaptureTS);
        auto diffMs = std::abs(timeFrameMs-timeMs);
        if(diffMs > prevDiff){
            return idFrame;
        }
        prevDiff = diffMs;
        ++idFrame;
    }

    return idFrame-1;
}

auto DCVolumetricVideo::nb_frames(size_t idCamera) const noexcept -> size_t{
    if(idCamera < nb_cameras()){
        return get_camera_data(idCamera)->nb_frames();
    }
    return 0;
}

auto DCVolumetricVideo::total_nb_frames() const noexcept -> size_t{
    size_t totalNbFrames = 0;
    for(const auto &cameraFrames : m_framesPerCamera){
        totalNbFrames += cameraFrames.nb_frames();
    }
    return totalNbFrames;
}

auto DCVolumetricVideo::min_nb_frames() const noexcept  -> size_t {

    if(m_framesPerCamera.empty()){
        return 0;
    }

    size_t minFrames = m_framesPerCamera.front().nb_frames();
    for(auto &frames : m_framesPerCamera){
        if(minFrames > frames.nb_frames()){
            minFrames = frames.nb_frames();
        }
    }
    return minFrames;
}

auto DCVolumetricVideo::get_camera_data(size_t idCamera) const noexcept -> const DCCameraData*{
    if(idCamera < nb_cameras()){
        return &m_framesPerCamera[idCamera];
    }
    Logger::error(std::format("[DCVolumetricVideo::get_camera_data] Invalid camera id: [{}], number of cameras available: [{}]\n", idCamera, nb_cameras()));
    return nullptr;
}

auto DCVolumetricVideo::get_compressed_frame(size_t idCamera, size_t idFrame) -> std::weak_ptr<DCCompressedFrame>{
    if(const auto camD = get_camera_data(idCamera)){
        return camD->get_compressed_frame(idFrame);
    }
    return {};
}

auto DCVolumetricVideo::remove_compressed_frames_until(size_t idCamera, size_t idFrame) -> void{
    if(idCamera < nb_cameras()){
        m_framesPerCamera[idCamera].remove_frames_until(idFrame);
    }
}

auto DCVolumetricVideo::remove_compressed_frames_after(size_t idCamera, size_t idFrame) -> void{
    if(idCamera < nb_cameras()){
        m_framesPerCamera[idCamera].remove_frames_after(idFrame);
    }
}

auto DCVolumetricVideo::keep_only_one_camera(size_t idCamera) -> void{
    m_uncompressors.resize(1);
    auto cameraFrames = std::move(m_framesPerCamera[idCamera]);
    m_framesPerCamera = {std::move(cameraFrames)};
}

auto DCVolumetricVideo::keep_only_cameras_from_id(const std::vector<size_t> &ids) -> void{

    std::vector<std::unique_ptr<DCFrameUncompressor>> uncompressors;
    std::vector<DCCameraData> framesPerCamera;

    for(const auto &id : ids){
        uncompressors.push_back(std::move(m_uncompressors[id]));
        framesPerCamera.push_back(std::move(m_framesPerCamera[id]));
    }
    std::swap(m_uncompressors, uncompressors);
    std::swap(m_framesPerCamera, framesPerCamera);
}

auto DCVolumetricVideo::clean_all_cameras_compressed_frames() noexcept -> void{
    for(auto &frames : m_framesPerCamera){
        frames.clean();
    }
}

auto DCVolumetricVideo::clean_camera_compressed_frames(size_t idCamera) noexcept -> void{
    if(idCamera < nb_cameras()){
        m_framesPerCamera[idCamera].clean();
    }
}

auto DCVolumetricVideo::get_transform(size_t idCamera) const -> tool::geo::Mat4d{
    if(idCamera < nb_cameras()){
        return m_framesPerCamera[idCamera].transform;
    }
    Logger::error("[DCVolumetricVideo::get_transform] Invalid camera id: [{}], number of cameras available: [{}]\n", idCamera, nb_cameras());
    return geo::Mat4d::identity();
}

auto DCVolumetricVideo::set_transform(size_t idCamera, geo::Mat4d tr) -> void{
    if(idCamera < nb_cameras()){
        m_framesPerCamera[idCamera].transform = tr;
        return;
    }
    Logger::error("[DCVolumetricVideo::set_transform]Invalid camera id: [{}], number of cameras available: [{}]\n", idCamera, nb_cameras());
}

auto DCVolumetricVideo::add_compressed_frame(size_t idCamera, std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(idCamera >= nb_cameras()){
        m_framesPerCamera.resize(idCamera+1);
    }

    m_framesPerCamera[idCamera].add_compressed_frame(std::move(frame));
}

auto DCVolumetricVideo::save_to_file(std::string_view path) -> bool{

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

auto DCVolumetricVideo::load_from_file(std::string_view path) -> bool{

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
    clean_all_cameras_compressed_frames();

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

auto DCVolumetricVideo::uncompressor(size_t idCamera) noexcept -> DCFrameUncompressor*{
    if(idCamera < m_uncompressors.size()){
        return m_uncompressors[idCamera].get();
    }
    return nullptr;
}

auto DCVolumetricVideo::merge_all_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void{

    if(m_framesPerCamera.empty()){
        return;
    }

    if(m_framesPerCamera.front().frames.empty()){
        return;
    }

    auto c0FirstFrameTS = m_framesPerCamera.front().frames.front()->afterCaptureTS;

    DCFrameCompressor compressor;
    for(size_t idF = 0; idF < nb_frames(0); ++idF){

        auto c0Frame = m_framesPerCamera.front().frames[idF].get();
        auto c0Time  = c0Frame->afterCaptureTS;
        auto c0TimeMs= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(c0Time - c0FirstFrameTS));

        DCFrame final;
        if(!uncompress_frame(0, idF, final)){
            continue;
        }

        final.mode            = DCMode::Merged;
        final.idCapture       = c0Frame->idCapture;
        final.afterCaptureTS  = c0Frame->afterCaptureTS;
        final.colorWidth      = 0;
        final.colorHeight     = 0;
        final.depthWidth      = 0;
        final.depthHeight     = 0;
        final.infraWidth      = 0;
        final.infraHeight     = 0;        
        final.imageColorData.clear();
        final.depthData.clear();
        final.imageDepthData.clear();
        final.infraData.clear();
        final.imageInfraData.clear();

        geo::VoxelGrid grid(voxelSize, minBound, maxBound);
        grid.add_cloud(final.cloud, m_framesPerCamera.front().transform.conv<float>());

        for(size_t jj = 1; jj < nb_cameras(); ++jj){

            size_t idF = 0;
            if(auto id = closest_frame_id_from_time(jj, c0TimeMs.count()); id != -1){
                idF = id;
            }else{
                continue;
            }

            DCFrame current;
//            std::cout << "uncompress_frame " << c0TimeMs.count() << " " <<  jj << " " << idF << "\n";
            if(!uncompress_frame(jj, idF, current)){
                continue;
            }

            grid.add_cloud(current.cloud, m_framesPerCamera[jj].transform.conv<float>());
        }
        grid.compute_grid();
        grid.convert_to_cloud(final.cloud);

        // compress frame
        auto cFrame = std::make_unique<K4CompressedFrame>();
        compressor.add_frame(final, 90, cFrame.get());
        m_framesPerCamera.front().frames[idF] = std::move(cFrame);
    }

    m_framesPerCamera.front().transform = geo::Mat4d::identity();

    m_framesPerCamera.resize(1);
    m_uncompressors.resize(1);
}

auto DCVolumetricVideo::merge_cameras_clouds(size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound) -> tool::geo::ColoredCloudData{

    if(idFrame >= min_nb_frames()){
        // ...
        return {};
    }

    geo::VoxelGrid grid(sizeVoxel, minBound, maxBound);

    DCFrame frame;
    for(size_t ii = 0; ii < nb_cameras(); ++ii){        
        uncompress_frame(ii, idFrame, frame);
        grid.add_cloud(frame.cloud, get_transform(ii).conv<float>());
    }
    grid.compute_grid();

    geo::ColoredCloudData cloud;
    grid.convert_to_cloud(cloud);

    return cloud;
}

auto DCVolumetricVideo::merge_cameras_clouds(size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, DCFrame &frame) -> void{

    frame = DCFrame();
    if(idFrame >= min_nb_frames()){
        // ...
        return;
    }

    geo::VoxelGrid grid(sizeVoxel, minBound, maxBound);
    for(size_t ii = 0; ii < nb_cameras(); ++ii){
        DCFrame uFrame;
        uncompress_frame(ii, idFrame, uFrame);
        grid.add_cloud(uFrame.cloud, get_transform(ii).conv<float>());

        if(ii == 0){
            frame.mode           = DCMode::Merged;
            frame.idCapture      = uFrame.idCapture;
            frame.afterCaptureTS = uFrame.afterCaptureTS;
            frame.imuSample      = uFrame.imuSample;
            frame.audioFrames    = uFrame.audioFrames;
            frame.bodies         = uFrame.bodies;
        }
    }
    grid.compute_grid();
    frame.cloud = merge_cameras_clouds(idFrame, sizeVoxel, minBound, maxBound);
}


auto DCVolumetricVideo::total_audio_frames_size(size_t idCamera) const -> size_t{
    if(idCamera < m_framesPerCamera.size()){
        size_t total = 0;
        for(const auto &frame : m_framesPerCamera[idCamera].frames){
            for(const auto &audioChannel : frame->audioFrames){
                total += audioChannel.size();
            }
        }
        return total;
    }
    Logger::error("[DCVolumetricVideo::total_audio_frames_size] Invalid camera id.\n");
    return 0;
}

auto DCVolumetricVideo::get_audio_samples_all_channels(size_t idCamera, std::vector<std::vector<float> > &audioBuffer) -> void{

    auto camData = &m_framesPerCamera[idCamera];

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

auto DCVolumetricVideo::get_audio_samples_all_channels(size_t idCamera, std::vector<float> &audioBuffer) -> void{

    auto camData = &m_framesPerCamera[idCamera];

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

//#include <iostream>
auto DCVolumetricVideo::read_file(std::ifstream &file) -> bool{

    // read mode
    std::int8_t videoType;
    read(videoType, file);

//    std::cout << "VIDEO TYPE" << (int)videoType << "\n";
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
//    std::cout << "nbCameras : " << nbCameras << "\n";

    // read infos per camera
    std::int32_t nbFrames;
    size_t currentC = 0;
    for(size_t idC = 0; idC < m_framesPerCamera.size(); ++idC){

        // read nb frames
        read(nbFrames, file);

        if(nbFrames > 0){
            auto &cameraData = m_framesPerCamera[currentC];

            cameraData.frames.reserve(nbFrames);

            for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
                cameraData.frames.push_back(std::make_shared<K4CompressedFrame>());
            }

            // calibration matrix
            read_array(cameraData.transform.array.data(), file, 16);

            ++currentC;
        }else{
            geo::Mat4d transform;
            read_array(transform.array.data(), file, 16);
        }
    }
    m_framesPerCamera.resize(currentC);

//    for(auto &cameraData : m_framesPerCamera){

//        // read nb frames
//        read(nbFrames, file);
//        std::cout << "nbFrames : " << nbFrames << "\n";

//        // create frames
//        cameraData.frames.reserve(nbFrames);
//        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
//            cameraData.frames.push_back(std::make_shared<K4CompressedFrame>());
//        }

//        // calibration matrix
//        read_array(cameraData.transform.array.data(), file, 16);

//        std::cout << "calibration : " << cameraData.transform << "\n";
//    }

    // read frames
    for(auto &cameraData : m_framesPerCamera){
        for(auto &frame : cameraData.frames){
            double timeMsNotUsed;
            read(timeMsNotUsed, file); // read time ms
//            std::cout << "timeMsNotUsed " << timeMsNotUsed << "\n";
            frame->init_from_file_stream(file);
        }
    }

    // remove empty cameras



    return true;
}

auto DCVolumetricVideo::write_file(std::ofstream &file) -> void{

    // write video type
    std::int8_t videoType = 2;
    write(videoType, file);                                                 // std::int8_t

    // write nb of cameras
    write(static_cast<std::int8_t>(m_framesPerCamera.size()), file);        // std::int8_t

    // write infos per camera
    for(auto &cameraData : m_framesPerCamera){
        // nb frames
        write(static_cast<std::int32_t>(cameraData.frames.size()), file);   // std::int32_t * cameras count
        // calibration matrix
        write_array(cameraData.transform.array.data(), file, 16);           // double * 16
    }

    // writes frames
    for(const auto &cameraData : m_framesPerCamera){
        for(const auto &frame : cameraData.frames){
            double timeMsNotUsed = 0.0;
            write(timeMsNotUsed, file);
            frame->write_to_file_stream(file);
        }
    }
}

auto DCVolumetricVideo::read_legacy_cloud_video_file(std::ifstream &file) -> void{

    // read nb of cameras
    std::int8_t nbCameras;
    read(nbCameras, file);
    initialize(nbCameras);

    // read infos per camera
    std::int32_t nbFrames;
    for(auto &cameraData : m_framesPerCamera){

        // read nb frames
        read(nbFrames, file);

        // create frames
        cameraData.frames.reserve(nbFrames);
        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
            cameraData.frames.push_back(std::make_shared<K4CompressedFrame>());
        }

        // calibration matrix
        read_array(cameraData.transform.array.data(), file, 16);
    }

    // read frames
    for(auto &cameraData : m_framesPerCamera){
        for(auto &frame : cameraData.frames){
            // read frame
            auto k4Frame = dynamic_cast<K4CompressedFrame*>(frame.get());
            if(k4Frame){
                k4Frame->init_legacy_cloud_frame_file_stream(file);
            }
        }
    }
}

auto DCVolumetricVideo::read_legacy_full_video_file(std::ifstream &file) -> void{

    // read nb of cameras
    std::int8_t nbCameras;
    read(nbCameras, file);
    initialize(nbCameras);

    // read infos per camera
    std::int32_t nbFrames;
    for(auto &cameraData : m_framesPerCamera){

        // read nb frames
        read(nbFrames, file);

        // create frames
        cameraData.frames.reserve(nbFrames);
        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
            cameraData.frames.push_back(std::make_shared<K4CompressedFrame>());
        }

        // calibration matrix
        read_array(cameraData.transform.array.data(), file, 16);
    }

    // read frames
    for(auto &cameraData : m_framesPerCamera){
        for(auto &frame : cameraData.frames){

            std::int32_t idFrame;
            std::int64_t timestamp;
            // # read frame info
            read(idFrame, file);
            read(timestamp, file);
            frame->afterCaptureTS = timestamp;

            // read frame            
            auto k4Frame = dynamic_cast<K4CompressedFrame*>(frame.get());
            if(k4Frame){
                k4Frame->init_legacy_full_frame_file_stream(file);
            }
        }
    }
}
