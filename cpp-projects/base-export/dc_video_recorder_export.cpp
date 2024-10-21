
/*******************************************************************************
** Toolset-base-export                                                        **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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


#include "dc_video_recorder_export.hpp"

#include "utility/logger.hpp"

using namespace tool::cam;

DCVideoRecorderExport::DCVideoRecorderExport(){

}

DCVideoRecorderExport::~DCVideoRecorderExport(){
    recorder.stop_recording();
    recorder.reset_recording();
}

auto DCVideoRecorderExport::dll_log_message(const std::string &message) -> void{
    if(logMessageCBP != nullptr){
        (*logMessageCBP)(message.c_str(),0);
    }else{
        tool::Log::message(message);
    }
}

auto DCVideoRecorderExport::dll_log_warning(const std::string &message) -> void{
    if(logMessageCBP != nullptr){
        (*logMessageCBP)(message.c_str(),1);
    }else{
        tool::Log::warning(message);
    }
}

auto DCVideoRecorderExport::dll_log_error(const std::string &message) -> void{
    if(logMessageCBP != nullptr){
        (*logMessageCBP)(message.c_str(),2);
    }else{
        tool::Log::error(message);
    }
}

auto DCVideoRecorderExport::init_callbacks(LogMessageCB logCB) -> void{
    logMessageCBP  = std::make_unique<LogMessageCB>(logCB);
}

auto DCVideoRecorderExport::associate_with_client(DCClient &client) -> void{
    recorder.initialize(client.devices_nb());
    for(size_t idD = 0; idD < client.devices_nb(); ++idD){
        recorder.update_model(idD, client.settings.devicesS[idD].modelS);
    }
    client.new_frame_signal.connect(&DCVideoRecorderExport::add_frame, this);
}

auto DCVideoRecorderExport::dissociate_from_client(DCClient &client) -> void{
    client.new_frame_signal.disconnect(&DCVideoRecorderExport::add_frame, this);
}

auto DCVideoRecorderExport::add_frame(size_t idD, std::shared_ptr<DCFrame> frame) -> void{
    recorder.add_frame(idD, std::move(frame));
}

tool::cam::DCVideoRecorderExport* create__dc_video_recorder_export(){
    return new tool::cam::DCVideoRecorderExport();
}

void delete__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    delete dcVideoRecorderExport;
}

void init_callbacks__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, LogMessageCB logCB){
    dcVideoRecorderExport->init_callbacks(logCB);
}

void associate_with_client__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, tool::cam::DCClientExport *dcClientExport){
    dcVideoRecorderExport->associate_with_client(dcClientExport->client);
}

void dissociate_from_client__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, tool::cam::DCClientExport *dcClientExport){
    dcVideoRecorderExport->dissociate_from_client(dcClientExport->client);
}

void start_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    dcVideoRecorderExport->recorder.start_recording();
}

void stop_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    dcVideoRecorderExport->recorder.stop_recording();
}

void reset_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    dcVideoRecorderExport->recorder.reset_recording();
}

int is_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    return dcVideoRecorderExport->recorder.is_recording() ? 1 : 0;
}

int nb_devices__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    return static_cast<int>(dcVideoRecorderExport->recorder.video()->nb_devices());
}

int nb_frames__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, int idD){
    return static_cast<int>(dcVideoRecorderExport->recorder.video()->nb_frames(idD));
}

float duration__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport){
    return static_cast<float>(dcVideoRecorderExport->recorder.video()->duration_ms());
}

