
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

#include "dc_video_recorder.hpp"


// local
#include "utility/time.hpp"
#include "utility/logger.hpp"
#include "utility/stop_watch.hpp"
#include "depth-camera/frame/dc_frame_compressor.hpp"

using namespace tool::cam;
using namespace std::chrono;


struct DCVideoRecorder::Impl{

    nanoseconds recordingStartTimestamp;
    StopWatch recordingStopWatch;
    std::vector<std::shared_ptr<DCCompressedFrame>> currentCompressedFrames;
    std::vector<std::shared_ptr<DCFrame>> currentFrames;

    DCVideo videoResource;

    DCFrameCompressor compressor;
};

DCVideoRecorder::DCVideoRecorder(): i(std::make_unique<Impl>()){
}

DCVideoRecorder::~DCVideoRecorder(){
}


auto DCVideoRecorder::initialize(size_t nbDevices) -> void{

    // video
    i->videoResource.initialize(nbDevices);
    // states
    states = {};
    states.nbFramesRecorded  = std::vector<size_t>(nbDevices, 0);
    states.currentFrames     = std::vector<size_t>(nbDevices, 0);
    // frames
    i->currentFrames.resize(nbDevices);
    std::fill(i->currentFrames.begin(), i->currentFrames.end(), nullptr);
    i->currentCompressedFrames.resize(nbDevices);
    std::fill(i->currentCompressedFrames.begin(), i->currentCompressedFrames.end(), nullptr);
}

auto DCVideoRecorder::uncompress_frame(size_t idCamera, DCFrame &frame) -> bool{

    if(idCamera >= i->currentFrames.size()){
        return false;
    }

    if(i->currentCompressedFrames[idCamera] == nullptr){
        return false;
    }

    if(i->currentCompressedFrames[idCamera]->idCapture == frame.idCapture){
        return false;
    }

    return i->videoResource.uncompress_frame(settings.generation, idCamera, i->currentCompressedFrames[idCamera].get(), frame);
}

auto DCVideoRecorder::add_compressed_frame_to_default_camera(std::shared_ptr<DCCompressedFrame> cFrame) -> void{
    add_compressed_frame(0, std::move(cFrame));
}

auto DCVideoRecorder::add_compressed_frame(size_t idCamera, std::shared_ptr<DCCompressedFrame> cFrame) -> void{

    if(!is_recording() || idCamera >= i->videoResource.nb_cameras()){
        return;
    }

    if(i->recordingStartTimestamp.count() > cFrame->receivedTS){
        Logger::error("[DCRecorder::add_compressed_frame] Invalid frame timestamp.\n");
        return;
    }

    if((i->videoResource.nb_frames(idCamera) < settings.cameraMaxFramesToRecord) && (i->recordingStopWatch.ellapsed_milli_s() < settings.maxDurationS*1000.0)){

        // add frame to video
        i->videoResource.add_compressed_frame(idCamera, std::move(cFrame));

        // update video duration
        states.duration = i->videoResource.duration_ms();

        ++states.nbFramesRecorded[idCamera];
    }
}

auto DCVideoRecorder::add_frame_to_default_camera(std::shared_ptr<DCFrame> frame) -> void{
    add_frame(0, std::move(frame));
}

auto DCVideoRecorder::add_frame(size_t idCamera, std::shared_ptr<DCFrame> frame) -> void{

    if(!is_recording() || idCamera >= i->videoResource.nb_cameras()){
        return;
    }

    if(i->recordingStartTimestamp.count() > frame->receivedTS){
        Logger::error("[DCRecorder::add_frame] Invalid frame timestamp.\n");
        return;
    }

    if((i->videoResource.nb_frames(idCamera) < settings.cameraMaxFramesToRecord) && (i->recordingStopWatch.ellapsed_milli_s() < settings.maxDurationS*1000.0)){

        // add frame to video
        i->videoResource.add_compressed_frame(idCamera, i->compressor.compress(settings.compression, *frame));

        // update video duration
        states.duration = i->videoResource.duration_ms();

        ++states.nbFramesRecorded[idCamera];
    }
}

auto DCVideoRecorder::set_time(double timeMs) -> void{

    if(timeMs > video()->duration_ms()){
        timeMs = video()->duration_ms();
    }
    states.currentTime = timeMs;

    for(size_t idC = 0; idC < video()->nb_cameras(); ++idC){
        if(auto idF = video()->closest_frame_id_from_time(idC, states.currentTime); idF != -1){
            i->currentCompressedFrames[idC] = i->videoResource.get_compressed_frame(idC, idF).lock();
            states.currentFrames[idC] = idF;
        }
    }
}

auto DCVideoRecorder::update() -> void{

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

auto DCVideoRecorder::video() -> DCVideo* {
    return &i->videoResource;
}

auto DCVideoRecorder::is_recording() const noexcept -> bool {
    return states.isRecording;
}

auto DCVideoRecorder::start_recording() -> void {

    if(is_recording()){
        return;
    }

    states.isRecording = true;
    i->recordingStartTimestamp = Time::nanoseconds_since_epoch();
    i->recordingStopWatch.start();
}

auto DCVideoRecorder::stop_recording() -> void {

    if(!is_recording()){
        return;
    }

    states.isRecording = false;
    i->recordingStopWatch.stop();
}

auto DCVideoRecorder::reset_recording() -> void {

    // clean video
    i->videoResource.remove_all_cameras_compressed_frames();

    // reset stop watch
    i->recordingStopWatch.reset();

    // reset frames
    std::fill(std::begin(i->currentCompressedFrames),   std::end(i->currentCompressedFrames), nullptr);
    std::fill(std::begin(i->currentFrames),             std::end(i->currentFrames), nullptr);

    // reset states
    states.currentTime = 0.0;
    states.duration = 0.0;
    std::fill(std::begin(states.nbFramesRecorded),   std::end(states.nbFramesRecorded), 0);
    std::fill(std::begin(states.currentFrames),      std::end(states.currentFrames), 0);
}

auto DCVideoRecorder::update_settings(DCVideoRecorderSettings recordingsS) noexcept -> void{
    settings = recordingsS;
}

auto DCVideoRecorder::update_model(size_t id, const DCModelSettings &model) -> void{
    auto tr = model.compute_full_transformation();
    video()->set_transform(id, tr.conv<double>());
}

auto DCVideoRecorder::save_to_file(std::string_view path) -> bool{
    return i->videoResource.save_to_file(path);
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
