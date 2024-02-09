
/*******************************************************************************
** Toolset-dc-manager                                                         **
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


#include "dcm_model.hpp"

// local
#include "dcm_signals.hpp"

using namespace tool;

DCMModel::DCMModel(){
    sNetwork = std::make_unique<net::DCServerNetwork>();
}

DCMModel::~DCMModel(){
    clean();
}

auto DCMModel::clean() -> void {

    // clean network first
    sNetwork->clean();
    sNetwork = nullptr;

    sData.clean();

}

auto DCMModel::initialize() -> bool{

    if(!settings.initialize()){
        return false;
    }
    reset_network();

    size_t nbConnections = sNetwork->devices_nb();
    sData.initialize(nbConnections);
    recorder.initialize(nbConnections);
    calibration.initialize(nbConnections);
    return true;
}


auto DCMModel::trigger_settings() -> void{
    settings.trigger_all_models();
    settings.trigger_all_cloud_display();
}

auto DCMModel::reset_network() -> void{
    sNetwork->initialize(settings.networkS.clientsInfo);
}

auto DCMModel::add_feedback(size_t id, net::Feedback feedback) -> void{
    readMessagesL.lock();
    messages.emplace_back(id, feedback);
    readMessagesL.unlock();
}

auto DCMModel::update_synchro(size_t id, int64_t averageDiffNs) -> void{
    settings.grabbersS[id].synchroAverageDiff = averageDiffNs;
}

auto DCMModel::add_default_device() -> void{
    settings.add_default_device();
}

auto DCMModel::ask_calibration() -> void{
    std::vector<cam::DCModelSettings> models;
    for(const auto &grabberS : settings.grabbersS){
        models.push_back(grabberS.model);
    }
    DCMSignals::get()->calibrate_signal(std::move(models));
}

auto DCMModel::update_filtering_mode(bool useNormalMode) -> void{
    if(settings.useNormalFilteringSettings != useNormalMode){
        settings.useNormalFilteringSettings = useNormalMode;
        for(const auto &grabber : settings.grabbersS){
            if(useNormalMode){
                sNetwork->update_filters_settings(grabber.id, grabber.filters);
            }else{
                sNetwork->update_filters_settings(grabber.id, grabber.calibrationFilters);
            }
        }
    }
}

auto DCMModel::update_filters(size_t id, const cam::DCFiltersSettings &filters) -> void {
    settings.grabbersS[id].filters = filters;
    if(settings.useNormalFilteringSettings){
        sNetwork->update_filters_settings(id, settings.grabbersS[id].filters);
    }
}

auto DCMModel::update_calibration_filters(size_t id, const cam::DCFiltersSettings &filters) -> void{
    settings.grabbersS[id].calibrationFilters = filters;
    if(!settings.useNormalFilteringSettings){
        sNetwork->update_filters_settings(id, settings.grabbersS[id].calibrationFilters);
    }
}

auto DCMModel::update_device_settings(size_t idG, const cam::DCDeviceSettings &deviceS) -> void {
    sNetwork->update_device_settings(idG, deviceS);
    sData.update_device_settings(idG, deviceS);
}

auto DCMModel::update_color_settings(size_t idG, const cam::DCColorSettings &colorS) -> void{
    sNetwork->update_color_settings(idG, colorS);
}

auto DCMModel::update_delay_settings(size_t idG, const cam::DCDelaySettings &delayS) -> void{
    sNetwork->update_delay_settings(idG, delayS);
}

auto DCMModel::read_feedbacks() -> void{

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
        settings.grabbersS[message.first].network.connected = sNetwork->device_connected(message.first);
        DCMSignals::get()->feedback_received_signal(message.first, std::format("Valid [{}] received\n", to_string(message.second.receivedMessageType)));
    }
    messagesR.clear();
}

auto DCMModel::update() -> void{

    // read messages from network
    read_feedbacks();

    // read compressed frames
    for(size_t ii = 0; ii < sData.nb_grabbers(); ++ii){

        // check if new compressed frame received
        if(auto lastCompressedFrame =  sData.get_compressed_frame(ii); lastCompressedFrame != nullptr){

            // send it
            DCMSignals::get()->new_compressed_frame_signal(ii, lastCompressedFrame);

            // invalidate it
            sData.invalid_last_compressed_frame(ii);
        }
    }

    // read frames
    for(size_t ii = 0; ii < sData.nb_grabbers(); ++ii){

        // check if new frame uncompressed
        if(auto lastFrame = sData.get_frame(ii); lastFrame != nullptr){

            // update last frame id
            settings.grabbersS[ii].network.lastFrameIdReceived = lastFrame->idCapture;

            // send it
            DCMSignals::get()->new_frame_signal(ii, lastFrame);

            // invalidate it
            sData.invalid_last_frame(ii);
        }
    }

    // update
    calibration.update();
    player.update();
    recorder.update();
}
