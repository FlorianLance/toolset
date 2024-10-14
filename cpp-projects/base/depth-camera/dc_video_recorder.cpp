
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
#include "depth-camera/frame/dc_data_frame_generator.hpp"

using namespace tool::cam;
using namespace std::chrono;


struct DCVideoRecorder::Impl{

    nanoseconds recordingStartTimestamp;
    StopWatch recordingStopWatch;
    std::vector<std::shared_ptr<DCDataFrame>> currentDataFrames;
    std::vector<std::shared_ptr<DCFrame2>> currentFrames;

    DCVideo videoResource;

    DCDataFrameGenerator compressor;
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
    i->currentDataFrames.resize(nbDevices);
    std::fill(i->currentDataFrames.begin(), i->currentDataFrames.end(), nullptr);
}

auto DCVideoRecorder::add_device() -> void{
    // video
    i->videoResource.add_device();
    // states
    states.nbFramesRecorded.push_back(0);
    states.currentFrames.push_back(0);
    // frames
    i->currentFrames.push_back(nullptr);
    i->currentDataFrames.push_back(nullptr);
}

auto DCVideoRecorder::remove_last_device() -> void{

    if(i->videoResource.nb_devices() > 0){
        // video
        i->videoResource.remove_last_device();
        // states
        states.nbFramesRecorded.erase(states.nbFramesRecorded.begin() + states.nbFramesRecorded.size() -1);
        states.currentFrames.erase(states.currentFrames.begin() + states.currentFrames.size() -1);
        // frames
        i->currentFrames.erase(i->currentFrames.begin() + i->currentFrames.size() -1);
        i->currentDataFrames.erase(i->currentDataFrames.begin() + i->currentDataFrames.size() -1);
    }
}

auto DCVideoRecorder::uncompress_frame(size_t idDevice, DCFrame2 &frame) -> bool{

    if(idDevice >= i->currentFrames.size()){
        return false;
    }

    if(i->currentDataFrames[idDevice] == nullptr){
        return false;
    }

    if(i->currentDataFrames[idDevice]->idCapture == frame.idCapture){
        return false;
    }

    return i->videoResource.generate_frame(settings.frameGenS, idDevice, i->currentDataFrames[idDevice].get(), frame);
}

auto DCVideoRecorder::add_data_frame_to_default_device(std::shared_ptr<DCDataFrame> dFrame) -> void{
    add_data_frame(0, std::move(dFrame));
}

auto DCVideoRecorder::add_data_frame(size_t idDevice, std::shared_ptr<DCDataFrame> dFrame) -> void{

    if(!is_recording() || idDevice >= i->videoResource.nb_devices()){
        return;
    }

    if(i->recordingStartTimestamp.count() > dFrame->receivedTS){
        Logger::error("[DCVideoRecorder::add_data_frame] Invalid frame timestamp.\n");
        return;
    }

    if((i->videoResource.nb_frames(idDevice) < settings.deviceMaxFramesToRecord) && (i->recordingStopWatch.ellapsed_milli_s() < settings.maxDurationS*1000.0)){

        // add frame to video
        i->videoResource.add_data_frame(idDevice, std::move(dFrame));

        // update video duration
        states.duration = i->videoResource.duration_ms();

        ++states.nbFramesRecorded[idDevice];
    }
}

auto DCVideoRecorder::add_frame_to_default_device(std::shared_ptr<DCFrame2> frame) -> void{
    add_frame(0, std::move(frame));
}

auto DCVideoRecorder::add_frame(size_t idDevice, std::shared_ptr<DCFrame2> frame) -> void{

    if(!is_recording() || idDevice >= i->videoResource.nb_devices()){
        return;
    }

    if(i->recordingStartTimestamp.count() > frame->receivedTS){
        Logger::error("[DCVideoRecorder::add_frame] Invalid frame timestamp.\n");
        return;
    }

    if((i->videoResource.nb_frames(idDevice) < settings.deviceMaxFramesToRecord) && (i->recordingStopWatch.ellapsed_milli_s() < settings.maxDurationS*1000.0)){

        // add frame to video
        i->videoResource.add_data_frame(idDevice, i->compressor.generate(settings.dataFrameGenS, *frame));

        // update video duration
        states.duration = i->videoResource.duration_ms();

        ++states.nbFramesRecorded[idDevice];
    }
}

auto DCVideoRecorder::set_time(double timeMs) -> void{

    if(timeMs > video()->duration_ms()){
        timeMs = video()->duration_ms();
    }
    states.currentTime = timeMs;

    for(size_t idC = 0; idC < video()->nb_devices(); ++idC){
        if(auto idF = video()->closest_frame_id_from_time(idC, states.currentTime); idF != -1){
            i->currentDataFrames[idC] = i->videoResource.get_data_frame(idC, idF).lock();
            states.currentFrames[idC] = idF;
        }
    }
}

auto DCVideoRecorder::update() -> void{

    for(size_t idC = 0; idC < video()->nb_devices(); ++idC){

        if(video()->nb_frames(idC) == 0){
            continue;
        }

        if(i->currentFrames[idC] == nullptr){
            i->currentFrames[idC] = std::make_shared<DCFrame2>();
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
    i->videoResource.remove_all_devices_data_frames();

    // reset stop watch
    i->recordingStopWatch.reset();

    // reset frames
    std::fill(std::begin(i->currentDataFrames),   std::end(i->currentDataFrames), nullptr);
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
