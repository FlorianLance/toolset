
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
#include "frame/dc_frame_compressor.hpp"
#include "frame/dc_frame_uncompressor.hpp"

using namespace std::literals::string_view_literals;
using namespace tool::cam;
using namespace std::chrono;

DCVideo &DCVideo::operator=(const DCVideo &other){
    
    m_devicesCompressedFrames = other.m_devicesCompressedFrames;
    for(size_t id = 0; id < other.m_devicesFrameUncompressors.size(); ++id){
        m_devicesFrameUncompressors.push_back(std::make_unique<DCFrameUncompressor>());
    }
    m_devicesTransforms = other.m_devicesTransforms;

    return *this;
}

auto DCVideo::initialize(size_t nbDevices) -> void{

    clean();
    
    m_devicesCompressedFrames.resize(nbDevices);
    m_devicesFrameUncompressors.resize(nbDevices);
    for(auto &uncompressor : m_devicesFrameUncompressors){
        uncompressor = std::make_unique<DCFrameUncompressor>();
    }

    m_devicesTransforms.resize(nbDevices);
    m_devicesTransforms.fill(geo::Mat4d::identity());
}

auto DCVideo::clean() -> void{
    if(nb_devices() > 0){
        m_devicesCompressedFrames.clear();
        m_devicesFrameUncompressors.clear();
        m_devicesTransforms.clear();
    }
}

auto DCVideo::add_device() -> void{
    m_devicesCompressedFrames.push_back({});
    m_devicesFrameUncompressors.push_back(std::make_unique<DCFrameUncompressor>());
    m_devicesTransforms.push_back(geo::Mat4d::identity());
}

auto DCVideo::remove_last_device() -> void{
    if(nb_devices() > 0){
        m_devicesCompressedFrames.remove_last();
        m_devicesFrameUncompressors.remove_last();
        m_devicesTransforms.remove_last();
    }
}

auto DCVideo::nb_devices() const noexcept -> size_t{
    return m_devicesCompressedFrames.size();
}

auto DCVideo::count_frames_from_all_devices() const noexcept -> size_t{
    size_t count = 0;
    for(const auto &cFrames : m_devicesCompressedFrames){
        count += cFrames.nb_frames();
    }
    return count;
}

auto DCVideo::uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idDevice, size_t idFrame, DCFrame &frame) -> bool{
    if(const auto camD = get_compressed_frames_ptr(idDevice)){
        if(auto cFrame = camD->get_frame_ptr(idFrame)){
            return uncompress_frame(gSettings, idDevice, cFrame, frame);
        }
    }
    return false;
}

auto DCVideo::uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idDevice, DCCompressedFrame *cFrame, DCFrame &frame) -> bool{
    if(auto unc = uncompressor(idDevice); unc != nullptr){
        return unc->uncompress(gSettings, cFrame, frame);
    }
    return false;
}

auto DCVideo::first_frame_received_timestamp() const noexcept -> std::int64_t{
    auto start = std::numeric_limits<std::int64_t>::max();
    bool found = false;
    for(const auto &deviceCompressedFrames : m_devicesCompressedFrames){
        if(auto devStartTS = deviceCompressedFrames.first_frame_received_timestamp(); devStartTS != -1){
            if(start > devStartTS){
                start = devStartTS;
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
    for(const auto &deviceCompressedFrames : m_devicesCompressedFrames){
        if(auto devEndTS = deviceCompressedFrames.last_frame_received_timestamp(); devEndTS!=-1){
            if(end < devEndTS){
                end = devEndTS;
                found = true;
            }
        }
    }
    if(found){
        return end;
    }
    return -1;
}

auto DCVideo::device_duration_ms(size_t idDevice) const noexcept -> double{
    if(idDevice >= nb_devices()){
        return 0.0;
    }
    return m_devicesCompressedFrames[idDevice].duration_ms();
}

auto DCVideo::device_first_frame_received_timestamp(size_t idDevice) const noexcept -> int64_t{
    if(idDevice >= nb_devices()){
        return -1;
    }
    return m_devicesCompressedFrames[idDevice].first_frame_received_timestamp();
}

auto DCVideo::device_last_frame_received_timestamp(size_t idDevice) const noexcept -> int64_t{
    if(idDevice >= nb_devices()){
        return -1;
    }
    return m_devicesCompressedFrames[idDevice].last_frame_received_timestamp();
}

auto DCVideo::duration_ms() const noexcept -> double {

    if(nb_devices() == 0){
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

auto DCVideo::closest_frame_id_from_time(size_t idDevice, double timeMs) const noexcept -> std::int64_t{

    if(idDevice >= nb_devices()){
        Logger::error("[DCVideo::closest_frame_id_from_time] Camera id invalid.\n"sv);
        return -1;
    }

    if(nb_frames(idDevice) == 0){
        //Logger::error("[DCVideo::closest_frame_id_from_time] No frame available.\n"sv);
        return -1;
    }    

    auto framesPtr = get_compressed_frames_ptr(idDevice);
    if(framesPtr->nb_frames() == 1){
        return 0;
    }

    auto ffTS = nanoseconds(first_frame_received_timestamp());
    // init with first diff
    double prevDiff = std::abs(get_timestamp_diff_time_ms(ffTS.count(), device_first_frame_received_timestamp(idDevice))-timeMs);

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

auto DCVideo::nb_frames(size_t idDevice) const noexcept -> size_t{
    if(idDevice < nb_devices()){
        return get_compressed_frames_ptr(idDevice)->nb_frames();
    }
    return 0;
}

auto DCVideo::min_nb_frames() const noexcept  -> size_t {
    
    if(m_devicesCompressedFrames.empty()){
        return 0;
    }
    
    size_t minFrames = m_devicesCompressedFrames.front().nb_frames();
    for(auto &frames : m_devicesCompressedFrames){
        if(minFrames > frames.nb_frames()){
            minFrames = frames.nb_frames();
        }
    }
    return minFrames;
}

auto DCVideo::get_compressed_frames_ptr(size_t idDevice) const noexcept -> const DCCompressedFrameBuffer*{
    if(idDevice < nb_devices()){
        return &m_devicesCompressedFrames[idDevice];
    }
    Logger::error(std::format("[DCVideo::get_compressed_frames_ptr] Invalid device id: [{}], number of devices available: [{}]\n", idDevice, nb_devices()));
    return nullptr;
}

auto DCVideo::get_compressed_frame(size_t idDevice, size_t idFrame) -> std::weak_ptr<DCCompressedFrame>{
    if(const auto camD = get_compressed_frames_ptr(idDevice)){
        return camD->get_compressed_frame(idFrame);
    }
    return {};
}

auto DCVideo::remove_compressed_frames_until(size_t idDevice, size_t idFrame) -> void{
    if(idDevice < nb_devices()){
        m_devicesCompressedFrames[idDevice].remove_frames_until(idFrame);
    }
}

auto DCVideo::remove_compressed_frames_after(size_t idDevice, size_t idFrame) -> void{
    if(idDevice < nb_devices()){
        m_devicesCompressedFrames[idDevice].remove_frames_after(idFrame);
    }
}

auto DCVideo::keep_only_one_device(size_t idDevice) -> void{
    m_devicesFrameUncompressors.keep_only(idDevice);
    m_devicesCompressedFrames.keep_only(idDevice);
    m_devicesTransforms.keep_only(idDevice);
}

auto DCVideo::keep_only_devices_from_id(const std::vector<size_t> &ids) -> void{
    m_devicesFrameUncompressors.keep_from_ids(ids);
    m_devicesCompressedFrames.keep_from_ids(ids);
    m_devicesTransforms.keep_from_ids(ids);
}

auto DCVideo::remove_all_devices_compressed_frames() noexcept -> void{
    for(auto &frames : m_devicesCompressedFrames){
        frames.clean();
    }
}

auto DCVideo::remove_all_compressed_frames(size_t idDevice) noexcept -> void{
    if(idDevice < nb_devices()){
        m_devicesCompressedFrames[idDevice].clean();
    }
}

auto DCVideo::replace_compressed_frame(size_t idDevice, size_t idFrame, std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(idDevice < nb_devices()){
        if(idFrame < nb_frames(idDevice)){
            m_devicesCompressedFrames[idDevice].frames[idFrame] = std::move(frame);
        }
    }
}

auto DCVideo::get_transform(size_t idDevice) const -> tool::geo::Mat4d{
    if(idDevice < nb_devices()){
        return m_devicesTransforms[idDevice];
    }
    Logger::error(std::format("[DCVideo::get_transform] Invalid device id: [{}], number of devices available: [{}]\n", idDevice, nb_devices()));
    return geo::Mat4d::identity();
}

auto DCVideo::set_transform(size_t idDevice, geo::Mat4d tr) -> void{
    if(idDevice < nb_devices()){
        m_devicesTransforms[idDevice] = tr;
        return;
    }
    Logger::error(std::format("[DCVideo::set_transform] Invalid device id: [{}], number of devices available: [{}]\n", idDevice, nb_devices()));
}

auto DCVideo::add_compressed_frame(size_t idDevice, std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(idDevice >= nb_devices()){
        m_devicesCompressedFrames.resize(idDevice+1);
    }
    
    m_devicesCompressedFrames[idDevice].add_compressed_frame(std::move(frame));
}

auto DCVideo::save_to_file(std::string_view path) -> bool{

    if(path.length() == 0){
        Logger::error("[DCVideo::save_to_file] Empty path.\n");
        return false;
    }

    if(count_frames_from_all_devices() == 0){
        Logger::error("[DCVideo::save_to_file] No available frames to save.\n");
        return false;
    }

    std::ofstream file;
    file.open(path.data(), std::ios_base::binary);
    if(!file.is_open()){
        Logger::error(std::format("[DCVideo::save_to_file] Cannot save compressed frames to {}.\n", path));
        return false;
    }
    file.exceptions(std::ofstream::badbit | std::ofstream::failbit);

    // write file
    bool success = false;
    try{
        write_file(file);
        success = true;
    }catch(const std::exception &e){
        Logger::error(std::format("[DCVideo::save_to_file] Error happend during writing file [{}].\n", e.what()));
    }
    file.close();

    return success;
}

// auto DCVideo::save_to_json_file(std::string_view path) -> bool{

//     // TESTS

//     if(path.length() == 0){
//         Logger::error("[DCVideo::save_to_json_file] Empty path.\n");
//         return false;
//     }

//     if(count_frames_from_all_devices() == 0){
//         Logger::error("[DCVideo::save_to_json_file] No available frames to save.\n");
//         return false;
//     }

//     json json;
//     add_value(json, "cameras_count"sv,  nb_devices());

//     // write infos
//     for(size_t idDevice = 0; idDevice < nb_devices(); ++idDevice){
//         nlohmann::json cInfoJson;
//         add_value(cInfoJson, "nb_frames"sv, m_devicesCompressedFrames[idDevice].frames.size());
//         add_array(cInfoJson, "model"sv, m_devicesTransforms[idDevice].cspan());
//         add_value(json, std::format("infos_camera_{}"sv, idDevice),  cInfoJson);
//     }

//     // write frames
//     for(size_t idDevice = 0; idDevice < nb_devices(); ++idDevice){
//         nlohmann::json cDataJson;
//         for(size_t idFrame = 0; idFrame < m_devicesCompressedFrames[idDevice].nb_frames(); ++idFrame){
//             add_value(cDataJson, std::format("frame_{}"sv, idFrame),  m_devicesCompressedFrames[idDevice].frames[idFrame]->convert_to_json());
//         }
//         add_value(json, std::format("data_camera_{}"sv, idDevice),  cDataJson);
//     }

//     if(!File::write_binary_content(std::string(path), json::to_bson(json))){
//         Logger::error(std::format("[DCVideo::save_to_json_file] Cannot save video to json text file, impossible to open file with path: [{}]\n", path));
//         return false;
//     }

//     return true;
// }

auto DCVideo::load_from_file(std::string_view path) -> bool{

    if(path.length() == 0){
        Logger::error("[DCVideo::load_from_file] Empty path.\n");
        return false;
    }

    // open file
    std::ifstream file(path.data(), std::ios_base::binary);    
    if(!file.is_open()){
        Logger::error(std::format("[DCVideo::load_from_file] Cannot open compressed frames file: [{}].\n", path));
        return false;
    }
    file.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

    // clean data
    remove_all_devices_compressed_frames();

    // read file
    bool success = false;
    try{
        success = read_file(file);
    }catch(const std::exception &e){
        Logger::error(std::format("[DCVideo::load_from_file] Error happend during reading file [{}].\n", e.what()));
        return false;
    }

    return success;
}

auto DCVideo::uncompressor(size_t idDevice) noexcept -> DCFrameUncompressor*{
    if(idDevice < m_devicesFrameUncompressors.size()){
        return m_devicesFrameUncompressors[idDevice].get();
    }
    return nullptr;
}

auto DCVideo::merge_all_devices(const DCFrameGenerationSettings &gSettings, float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void{
    
    if(m_devicesCompressedFrames.empty()){
        return;
    }
    
    if(m_devicesCompressedFrames.front().frames.empty()){
        return;
    }

    for(const auto &cData : m_devicesCompressedFrames){
        if(!cData.check_same_mode_for_every_frame()){
            Logger::error("[DCVideo::merge_all_devices] Mode inconstancie accros devices frames\n");
            return;
        }
    }
    auto mode = m_devicesCompressedFrames.front().first_frame_ptr()->mode;
    for(const auto &cData : m_devicesCompressedFrames){
        if(cData.first_frame_ptr()->mode != mode){
            Logger::error("[DCVideo::merge_all_devices] Mode inconstancie accros devices frames\n");
            return;
        }
    }

    auto c0FirstFrameTS = m_devicesCompressedFrames.front().frames.front()->receivedTS;

    DCFrameCompressor compressor;
    for(size_t idF = 0; idF < nb_frames(0); ++idF){
        
        auto c0Frame = m_devicesCompressedFrames.front().frames[idF].get();
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
        grid.add_cloud(final.cloud, m_devicesTransforms.front().conv<float>());

        for(size_t jj = 1; jj < nb_devices(); ++jj){

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

            grid.add_cloud(current.cloud, m_devicesTransforms[jj].conv<float>());
        }
        grid.compute_grid();
        grid.convert_to_cloud(final.cloud);

        // compress frame
        //m_camerasCompressedFrames.front().frames[idF] = compressor.compress(final, 90);
    }

    m_devicesTransforms.front() = geo::Mat4d::identity();
    
    m_devicesCompressedFrames.resize(1);
    m_devicesFrameUncompressors.resize(1);
}

auto DCVideo::merge_devices_frame_id(const DCFrameGenerationSettings &gSettings, size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, tool::geo::ColoredCloudData &cloud) -> void{

    if(idFrame >= min_nb_frames()){
        // ...
        return ;
    }

    geo::VoxelGrid grid(sizeVoxel, minBound, maxBound);

    DCFrame frame;
    for(size_t ii = 0; ii < nb_devices(); ++ii){
        uncompress_frame(gSettings, ii, idFrame, frame);
        grid.add_cloud(frame.cloud, get_transform(ii).conv<float>());
    }
    grid.compute_grid();
    grid.convert_to_cloud(cloud);
}

auto DCVideo::merge_devices_frame_id(const DCFrameGenerationSettings &gSettings, size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, DCFrame &frame) -> void{

    frame = DCFrame();
    if(idFrame >= min_nb_frames()){
        // ...
        return;
    }

    geo::VoxelGrid grid(sizeVoxel, minBound, maxBound);
    for(size_t ii = 0; ii < nb_devices(); ++ii){
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
    merge_devices_frame_id(gSettings, idFrame, sizeVoxel, minBound, maxBound, frame.cloud);
}


auto DCVideo::total_audio_frames_size(size_t idDevice) const -> size_t{
    if(idDevice < m_devicesCompressedFrames.size()){
        size_t total = 0;
        for(const auto &frame : m_devicesCompressedFrames[idDevice].frames){
            for(const auto &audioChannel : frame->audioFrames){
                total += audioChannel.size();
            }
        }
        return total;
    }
    Logger::error("[DCVideo::total_audio_frames_size] Invalid device id.\n");
    return 0;
}

auto DCVideo::get_audio_samples_all_channels(size_t idDevice, std::vector<std::vector<float> > &audioBuffer) -> void{
    
    auto camData = &m_devicesCompressedFrames[idDevice];

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

auto DCVideo::get_audio_samples_all_channels(size_t idDevice, std::vector<float> &audioBuffer) -> void{
    
    auto camData = &m_devicesCompressedFrames[idDevice];

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
        Logger::error("[DCVideo::read_file] Invalid video type.\n");
        return false;
    }    

    // read nb of devices
    std::int8_t nbDevices;
    read(nbDevices, file);
    initialize(nbDevices);

    // read infos per device
    std::int32_t nbFrames;
    size_t currentC = 0;
    for(size_t idC = 0; idC < m_devicesCompressedFrames.size(); ++idC){

        // read nb frames
        read(nbFrames, file);

        if(nbFrames > 0){
            
            auto &deviceCompressedFrames = m_devicesCompressedFrames[currentC];
            deviceCompressedFrames.frames.reserve(nbFrames);

            for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
                deviceCompressedFrames.frames.push_back(std::make_shared<DCCompressedFrame>());
            }

            // calibration matrix
            read_array(m_devicesTransforms[currentC].array.data(), file, 16);

            ++currentC;
        }else{
            geo::Mat4d transform;
            read_array(transform.array.data(), file, 16);
        }
    }
    m_devicesCompressedFrames.resize(currentC);

    // read frames
    for(auto &deviceData : m_devicesCompressedFrames){
        for(auto &frame : deviceData.frames){
            double timeMsNotUsed;
            read(timeMsNotUsed, file); // read time ms
            frame->init_from_file_stream(file);
        }
    }

    // remove empty devices
    // TODO: ...

    return true;
}

auto DCVideo::write_file(std::ofstream &file) -> void{

    // write video type
    std::int8_t videoType = 2;
    write(videoType, file);                                                                         // std::int8_t

    // write nb of devices
    write(static_cast<std::int8_t>(m_devicesCompressedFrames.size()), file);                        // std::int8_t

    // write infos per device
    for(size_t idDevice = 0; idDevice < nb_devices(); ++idDevice){
        // nb frames
        write(static_cast<std::int32_t>(m_devicesCompressedFrames[idDevice].frames.size()), file);  // std::int32_t * devices count
        // calibration matrix
        write_array(m_devicesTransforms[idDevice].array.data(), file, 16);                          // double * 16
    }

    // writes frames
    for(const auto &deviceCompressedFrames : m_devicesCompressedFrames){
        for(const auto &cFrame : deviceCompressedFrames.frames){
            double timeMsNotUsed = 0.0;
            write(timeMsNotUsed, file);
            cFrame->write_to_file_stream(file);
        }
    }
}

auto DCVideo::read_legacy_cloud_video_file(std::ifstream &file) -> void{

    // read nb of devices
    std::int8_t nbDevices;
    read(nbDevices, file);
    initialize(nbDevices);

    // read infos per device
    std::int32_t nbFrames;
    for(size_t idDevice = 0; idDevice < nb_devices(); ++idDevice){

        // read nb frames
        read(nbFrames, file);

        // create frames
        m_devicesCompressedFrames[idDevice].frames.reserve(nbFrames);
        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
            m_devicesCompressedFrames[idDevice].frames.push_back(std::make_shared<DCCompressedFrame>());
        }

        // calibration matrix
        read_array(m_devicesTransforms[idDevice].array.data(), file, 16);
    }

    // read frames
    for(auto &deviceData : m_devicesCompressedFrames){
        for(auto &frame : deviceData.frames){
            // read frame
            frame->init_legacy_cloud_frame_from_file_stream(file);
        }
    }
}

auto DCVideo::read_legacy_full_video_file(std::ifstream &file) -> void{

    // read nb of devices
    std::int8_t nbDevices;
    read(nbDevices, file);
    initialize(nbDevices);

    // read infos per device
    std::int32_t nbFrames;
    for(size_t idDevice = 0; idDevice < nb_devices(); ++idDevice){

        // read nb frames
        read(nbFrames, file);

        // create frames
        m_devicesCompressedFrames[idDevice].frames.reserve(nbFrames);
        for(size_t ii = 0; ii < static_cast<size_t>(nbFrames); ++ii){
            m_devicesCompressedFrames[idDevice].frames.push_back(std::make_shared<DCCompressedFrame>());
        }

        // calibration matrix
        read_array(m_devicesTransforms[idDevice].array.data(), file, 16);
    }

    // read frames
    for(auto &deviceData : m_devicesCompressedFrames){
        for(auto &frame : deviceData.frames){

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
