
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

#include "k4_recorder.hpp"

using namespace tool::camera;
using namespace std::chrono;

auto K4Recorder::initialize(size_t nbGrabbers) -> void{

    m_videoResource.initialize(nbGrabbers);
    m_states.nbFramesRecorded.resize(nbGrabbers);
    m_states.currentFrames.resize(nbGrabbers);

    m_currentFrames.resize(nbGrabbers);
    m_currentCompressedFrames.resize(nbGrabbers);

    states_updated_signal(m_states);
}

auto K4Recorder::uncompress_frame(size_t idCamera, K4Frame &frame) -> bool{

    if(idCamera >= m_currentFrames.size()){
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

auto K4Recorder::add_compressed_frame_to_default_camera(std::shared_ptr<K4CompressedFrame> frame) -> void{
    add_compressed_frame(0, std::move(frame));
}

#include <iostream>
#include <chrono>

auto K4Recorder::add_compressed_frame(size_t idCamera, std::shared_ptr<K4CompressedFrame> frame) -> void{

    if(!is_recording() || idCamera >= m_videoResource.nb_cameras()){
        return;
    }

    if(m_videoResource.total_nb_frames() == 0){
        // update start timestamp
//        m_videoResource.set_video_start_timestamp(m_sw.fist_start_timestamp_ns());
    }

    using namespace std::chrono;

    auto ff = video()->first_frame_capture_timestamp();
    if(ff != -1){//.has_value()){
        auto diff = duration_cast<milliseconds>(nanoseconds(frame->afterCaptureTS-ff/**.value()*/));
        std::cout << idCamera << " " << video()->get_camera_data(idCamera)->nb_frames() << " "  << diff << "\n";
    }



    if((m_videoResource.nb_frames(idCamera) < m_settings.cameraMaxFramesToRecord) && (m_sw.ellapsed_milli_s() < m_settings.maxDurationS*1000.0)){

        // add frame to video
        m_videoResource.add_compressed_frame(idCamera, std::move(frame));

        // update end timestamp
//        m_videoResource.set_video_end_timestamp(m_sw.fist_start_timestamp_ns() + m_sw.ellapsed_nano_s());

        // update video duration
        m_states.duration = m_videoResource.duration_ms();
        if(idCamera == 1){
//            auto c = video()->get_camera_data(idCamera);
//            std::cout << frame->afterCaptureTS << " " <<  c->first_frame_capture_timestamp().value() << " " << video()->get_camera_data(idCamera)->first_frame_capture_timestamp().value() << " "<<  idCamera << " " << m_states.duration << "\n";
        }

        ++m_states.nbFramesRecorded[idCamera];
        states_updated_signal(m_states);
    }
} 

auto K4Recorder::set_time(double timeMs) -> void{

    if(timeMs > video()->duration_ms()){
        timeMs = video()->duration_ms();
    }
    m_states.currentTime = timeMs;

    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){
        if(auto idF = video()->closest_frame_id_from_time(idC, m_states.currentTime); idF != -1){//.has_value()){
            m_currentCompressedFrames[idC] = m_videoResource.get_compressed_frame(idC, idF/**.value()*/).lock();
            m_states.currentFrames[idC] = idF;//.value();
        }
    }

    states_updated_signal(m_states);
}

auto K4Recorder::update_frames() -> void{

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

auto K4Recorder::video() -> K4VolumetricVideo* {
    return &m_videoResource;
}

auto K4Recorder::is_recording() const noexcept -> bool {
    return m_states.isRecording;
}

auto K4Recorder::start_recording() -> void {
    m_states.isRecording = true;
    states_updated_signal(m_states);

    m_sw.start();
}

auto K4Recorder::stop_recording() -> void {
    m_states.isRecording = false;
    states_updated_signal(m_states);

    m_sw.stop();
}

auto K4Recorder::reset_recording() -> void {

    // clean video
    m_videoResource.clean_all_cameras_compressed_frames();

    // reset stop watch
    m_sw.reset();

    // reset frames
    std::fill(std::begin(m_currentCompressedFrames), std::end(m_currentCompressedFrames), nullptr);
    std::fill(std::begin(m_currentFrames), std::end(m_currentFrames), nullptr);

    // reset timestamps
//    m_videoResource.set_video_start_timestamp(std::chrono::nanoseconds{0});
//    m_videoResource.set_video_end_timestamp(std::chrono::nanoseconds{0});

    // reset states
    m_states.currentTime = 0.0;
    m_states.duration = 0.0;
    std::fill(std::begin(m_states.nbFramesRecorded), std::end(m_states.nbFramesRecorded), 0);
    std::fill(std::begin(m_states.currentFrames), std::end(m_states.currentFrames), 0);
    states_updated_signal(m_states);
}

auto K4Recorder::update_settings(K4RecorderSettings recordingsS) noexcept -> void{
    m_settings = recordingsS;
}

auto K4Recorder::update_model(size_t id, const K4Model &model) -> void{
    auto tr = model.compute_full_transformation();
    video()->set_transform(id, tr.conv<double>());
}


//    AudioFile<float> af;
//    std::vector<std::vector<float>> buffer;
//    fullVideoManager.audio_samples_all_channels(0, buffer);
//    for(size_t ii = 0; ii < buffer.size(); ++ii){
//        Logger::message(std::format("channel {} with size {}\n", ii, buffer[ii].size()));
//    }
//    auto startT = fullVideoResource.start_time(0);
//    auto endT   = fullVideoResource.end_time(0);
//    double time = (endT-startT)*0.000000001;
//    double samplingRate = buffer[0].size() / time;
//    af.setSampleRate(48000);//static_cast<int>(samplingRate));

//    Logger::message(std::format("time {} {} {}\n",
//                                time, buffer[0].size(), samplingRate));
//    af.setAudioBuffer(buffer);
//    Logger::message(std::format("{} {} {} {}\n",
//                                af.getLengthInSeconds(), af.getBitDepth(), af.getNumChannels(), af.getNumSamplesPerChannel()));

//    std::string audioFilePath = filePathName;
//    str::replace_all2(audioFilePath, ".kvid", ".wav");
//    if(!af.save (audioFilePath, AudioFileFormat::Wave)){
//        Logger::error(std::format("Cannot save audio file {}.\n", audioFilePath));
//    }
