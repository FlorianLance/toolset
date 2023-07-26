
/*******************************************************************************
** Toolset-k4-scaner-manager                                                  **
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


#include "k4sm_model.hpp"

// local
#include "k4sm_signals.hpp"

using namespace tool;

auto K4SMModel::initialize() -> bool{

    if(!settings.initialize()){
        return false;
    }
    if(!network.initialize(settings.globalSet.network.clientsInfo)){
        return false;
    }

    size_t nbConnections = network.connections_nb();
    sData.initialize(nbConnections);
    recorder.initialize(nbConnections);
    calibration.initialize(nbConnections);

    return true;
}


auto K4SMModel::clean() -> void{
    network.clean();
}

auto K4SMModel::trigger_settings() -> void{
    settings.trigger_all_models();
    settings.trigger_all_cloud_display();
}

auto K4SMModel::add_feedback(size_t id, network::K4Feedback feedback) -> void{
    readMessagesL.lock();
    messages.emplace_back(id, feedback);
    readMessagesL.unlock();
}

auto K4SMModel::update_synchro(size_t id, int64_t averageDiffNs) -> void{
    settings.grabbersSet[id].synchroAverageDiff = averageDiffNs;
}

auto K4SMModel::ask_calibration() -> void{
    std::vector<camera::K4Model> models;
    for(const auto &grabberS : settings.grabbersSet){
        models.push_back(grabberS.model);
    }
    K4SMSignals::get()->calibrate_signal(std::move(models));
}

auto K4SMModel::update_filtering_mode(bool useNormalMode) -> void{
    if(settings.globalSet.useNormalFilteringSettings != useNormalMode){
        settings.globalSet.useNormalFilteringSettings = useNormalMode;
        for(const auto &grabber : settings.grabbersSet){
            if(useNormalMode){
                network.send_filters(grabber.id, grabber.filters);
            }else{
                network.send_filters(grabber.id, grabber.calibrationFilters);
            }
        }
    }
}

auto K4SMModel::update_filters(size_t id, const camera::K4Filters &filters) -> void {
    settings.grabbersSet[id].filters = filters;
    if(settings.globalSet.useNormalFilteringSettings){
        network.send_filters(id, settings.grabbersSet[id].filters);
    }
}

auto K4SMModel::update_calibration_filters(size_t id, const camera::K4Filters &filters) -> void{
    settings.grabbersSet[id].calibrationFilters = filters;
    if(!settings.globalSet.useNormalFilteringSettings){
        network.send_filters(id, settings.grabbersSet[id].calibrationFilters);
    }
}

auto K4SMModel::read_feedbacks() -> void{

    if(readMessagesL.try_lock()){
        auto size = messages.size();
        if(size != 0){
            for(size_t ii = 0; ii < size; ++ii){
                messagesR.push_back(messages.front());
                messages.pop_front();
            }
        }
        readMessagesL.unlock();
    }

    for(const auto &message : messagesR){
        settings.grabbersSet[message.first].network.connected = network.get_connection(message.first)->grabber_connected_to_server();
        K4SMSignals::get()->feedback_received_signal(message.first, std::format("Valid [{}] received\n", to_string(message.second.receivedMessageType)));
    }
    messagesR.clear();
}

auto K4SMModel::update() -> void{

    // update time
    player.update_time();
    calibration.update_time();

    // read messages from network
    read_feedbacks();

    // read compressed frames
    for(size_t ii = 0; ii < sData.nb_grabbers(); ++ii){

        // check if new compressed frame received
        if(auto lastCompressedFrame =  sData.get_compressed_frame(ii); lastCompressedFrame != nullptr){

            // send it
            K4SMSignals::get()->new_compressed_frame_signal(ii, lastCompressedFrame);

            // invalidate it
            sData.invalid_last_compressed_frame(ii);
        }
    }

    // read frames
    for(size_t ii = 0; ii < sData.nb_grabbers(); ++ii){

        // check if new frame uncompressed
        if(auto lastFrame = sData.get_frame(ii); lastFrame != nullptr){

            // update last frame id
            settings.grabbersSet[ii].network.lastFrameIdReceived = lastFrame->idCapture;

            // send it
            K4SMSignals::get()->new_frame_signal(ii, lastFrame);

            // invalidate it
            sData.invalid_last_frame(ii);
        }
    }

    // update
    player.update_frames();
    recorder.update_frames();
}
