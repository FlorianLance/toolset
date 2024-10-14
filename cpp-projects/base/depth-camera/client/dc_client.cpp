
/*******************************************************************************
** Toolset-base                                                               **
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

#include "dc_client.hpp"

// std
#include <deque>

// local
#include "utility/logger.hpp"
#include "dc_client_local_device.hpp"
#include "dc_client_remote_device.hpp"
#include "dc_client_device.hpp"
#include "dc_client_processing.hpp"

using namespace tool::cam;

struct DCClient::Impl{

    Buffer<std::unique_ptr<DCClientDevice>> cDevices;
    DCClientProcessing processing;

    std::mutex readMessagesL;
    std::deque<std::pair<size_t, net::Feedback>> messages;
    std::vector<std::pair<size_t, net::Feedback>> messagesR;

    auto generate_client(DCClientSettings &settings, size_t idDevice) -> std::unique_ptr<DCClientDevice>{

        std::unique_ptr<DCClientDevice> device = nullptr;
        if(settings.devicesS[idDevice].connectionS.connectionType == DCClientType::Local){

            Logger::message("[DCClient::initialize] Create local device.\n"sv);
            auto lDevice = std::make_unique<DCClientLocalDevice>();
            lDevice->local_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCFrame2> frame){
                processing.new_frame(idDevice, std::move(frame));
            });
            device = std::move(lDevice);

        }else if(settings.devicesS[idDevice].connectionS.connectionType == DCClientType::Remote){

            Logger::message("[DCClient::initialize] Create remote device.\n"sv);
            auto rDevice = std::make_unique<DCClientRemoteDevice>();
            rDevice->remote_feedback_signal.connect([this,idDevice](net::Feedback feedback){
                readMessagesL.lock();
                messages.emplace_back(idDevice, feedback);
                readMessagesL.unlock();
            });
            rDevice->remote_synchro_signal.connect([&,idDevice](std::int64_t averageTimestampDiffNs){
                settings.devicesS[idDevice].synchroAverageDiff = averageTimestampDiffNs;
            });
            rDevice->remote_network_status_signal.connect([&,idDevice](net::UdpNetworkStatus status){
                settings.devicesS[idDevice].receivedNetworkStatus = status;
            });
            rDevice->remote_data_frame_signal.connect([&,idDevice](std::shared_ptr<cam::DCDataFrame> cFrame){
                processing.new_data_frame(idDevice, std::move(cFrame));
            });
            device = std::move(rDevice);
        }

        device->data_status_signal.connect([&,idDevice](net::UdpDataStatus status){
            settings.devicesS[idDevice].receivedDataStatus = status;
        });

        Logger::message("[DCClient::initialize] Initialize device.\n"sv);
        if(!device->initialize(settings.devicesS[idDevice].connectionS)){
            Logger::error("[DCClient::initialize] Cannot initialize device.\n"sv);
        }

        return device;
    }
};

DCClient::DCClient(): i(std::make_unique<Impl>()){
}

DCClient::~DCClient(){
    clean();
}

auto DCClient::initialize(const std::string &clientSettingsPath, bool startThreads) -> bool{

    if(devices_nb() != 0){
        clean();
    }
    
    if(!settings.load_from_file(clientSettingsPath)){
        return false;
    }
    settings.filePath = clientSettingsPath;
    for(auto &device : settings.devicesS){
        // device.connectionS.connectionType == DCClientType::Remote
        // device.deviceS;
        device.connectionS.startReadingThread = startThreads;
    }

    // init other settings
    i->processing.initialize(settings.devicesS.size(), startThreads);

    generate_clients();

    return true;
}

auto DCClient::clean() -> void{

    Logger::log("[DCClient::clean] Clean devices.\n"sv);
    for(auto &device : i->cDevices){
        device->clean();
    }
    Logger::log("[DCClient::clean] Clean processing.\n"sv);
    i->processing.clean();
    i->cDevices.clear();

    Logger::log("[DCClient::clean] Clear settings.\n"sv);
    settings.devicesS.clear();

}

auto DCClient::update() -> void{

    // read messages from network
    read_feedbacks();

    // read data frames
    std::vector<std::shared_ptr<cam::DCDataFrame>> dFrames(i->processing.nb_devices(), nullptr);
    for(size_t idC = 0; idC < i->processing.nb_devices(); ++idC){
        // check if new frame uncompressed
        if(auto cFrame = i->processing.get_data_frame(idC); cFrame != nullptr){
            dFrames[idC] = cFrame;
        }
    }

    // read frames
    std::vector<std::shared_ptr<cam::DCFrame2>> frames(i->processing.nb_devices(), nullptr);
    for(size_t idC = 0; idC < i->processing.nb_devices(); ++idC){
        // check if new frame
        if(auto frame = i->processing.get_frame(idC); frame != nullptr){
            frames[idC] = frame;
        }
    }

    // send frames
    for(size_t idC = 0; idC < i->processing.nb_devices(); ++idC){

        if(dFrames[idC]){
            
            if(settings.devicesS[idC].lastDataFrameIdReceived != dFrames[idC]->idCapture){

                // update last compresesd frame id
                settings.devicesS[idC].lastDataFrameIdReceived = dFrames[idC]->idCapture;

                // send it
                new_data_frame_signal(idC, std::move(dFrames[idC]));

                // invalidate it
                // sData.invalid_last_compressed_frame(ii);
            }
        }

        if(frames[idC]){
            
            if(settings.devicesS[idC].lastFrameIdReceived != frames[idC]->idCapture){

                // update last frame id
                settings.devicesS[idC].lastFrameIdReceived = frames[idC]->idCapture;

                // send it
                new_frame_signal(idC, std::move(frames[idC]));

                // invalidate it
                // sData.invalid_last_frame(ii);
            }
        }
    }

    for(auto &device : i->cDevices){
        if(auto rD = dynamic_cast<DCClientRemoteDevice*>(device.get())){
            if(rD->device_connected()){
                rD->ping();
            }
        }
    }

    for(size_t idD = 0; idD < devices_nb(); ++idD){
        settings.devicesS[idD].processindUCUsage = static_cast<int>(100.0*i->processing.get_uc_usage(idD));
    }
}

auto DCClient::devices_nb() const noexcept -> size_t{
    return i->cDevices.size();
}

auto DCClient::device_connected(size_t idC) const noexcept -> bool{
    if(idC < devices_nb()){
        return i->cDevices[idC]->device_connected();
    }
    return false;
}

auto DCClient::device_type(size_t idC) const noexcept -> DCClientType{
    if(idC < devices_nb()){
        return i->cDevices[idC]->type();
    }
    return DCClientType::Undefined;
}

auto DCClient::init_connection_with_remote_device(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::init_remote_connection] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    if(i->cDevices[idC]->type() == DCClientType::Remote){
        dynamic_cast<DCClientRemoteDevice*>(i->cDevices[idC].get())->init_remote_connection(settings.clientId);
    }
}

auto DCClient::read_data_from_external_thread(size_t idC) -> size_t{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::read_data_from_external_thread] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return 0;
    }
    return i->cDevices[idC]->read_data_from_external_thread();
}

auto DCClient::trigger_packets_from_remote_device(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::trigger_packets_from_remote_device] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    if(i->cDevices[idC]->type() == DCClientType::Remote){
        dynamic_cast<DCClientRemoteDevice*>(i->cDevices[idC].get())->trigger_received_packets();
    }
}

auto DCClient::reset_network() -> void{

    for(size_t idD = 0; idD < devices_nb(); ++idD){
        if(auto rD = dynamic_cast<DCClientRemoteDevice*>( i->cDevices[idD].get())){
            rD->clean();
        }
    }

    for(size_t idD = 0; idD < devices_nb(); ++idD){
        if(auto rD = dynamic_cast<DCClientRemoteDevice*>( i->cDevices[idD].get())){
            settings.update_connection_settings(settings.devicesS[idD].connectionS);
            rD->initialize(settings.devicesS[idD].connectionS);
        }
    }

}

auto DCClient::legacy_initialize(const std::string &legacyNetworkSettingsFilePath) -> bool{

    if(devices_nb() != 0){
        clean();
    }

    DCDeprecatedClientConnectionSettings dS;
    if(!dS.initialize(legacyNetworkSettingsFilePath)){
        return false;
    }
    settings.sType          = io::SettingsType::Dc_client_connection;
    settings.version        = io::SettingsVersion::LastVersion;
    settings.filePath       = dS.filePath;

    settings.ipv4Interfaces = dS.ipv4Interfaces;
    settings.ipv6Interfaces = dS.ipv6Interfaces;
    settings.devicesS.resize(dS.connectionsS.size());

    for(size_t idD = 0; idD < settings.devicesS.size(); ++idD){
        auto &coS = settings.devicesS[idD].connectionS;
        if(dS.connectionsS[idD]->isLocal){
            coS.connectionType = DCClientType::Local;
        }else{
            auto *remoteC = dynamic_cast<DCDeprecatedRemoteDeviceConnectionSettings*>(dS.connectionsS[idD].get());
            coS.connectionType      = DCClientType::Remote;
            coS.idReadingInterface  = remoteC->serverS.idReadingInterface;
            coS.readingPort         = remoteC->serverS.readingPort;
            coS.sendingAddress      = remoteC->serverS.sendingAddress;
            coS.processedSendingAddress = coS.sendingAddress;

            coS.sendingPort         = remoteC->serverS.sendingPort;
            coS.protocol            = remoteC->serverS.protocol;
            coS.startReadingThread  = remoteC->serverS.startReadingThread;
        }
        settings.devicesS[idD].set_id(idD);
    }

    // init other settings
    i->processing.initialize(settings.devicesS.size(), true);

    generate_clients();

    return true;
}

auto DCClient::apply_command(size_t idC, net::Command command) -> void{
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_command] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    i->cDevices[idC]->apply_command(command);
}

auto DCClient::add_device(DCClientType connectionType) -> void{
    auto lg = LogGuard("DCClient::add_device"sv);
    settings.add_device(connectionType);
    i->processing.add_device_processor(true);
    i->cDevices.push_back(i->generate_client(settings, settings.devicesS.size()-1));
}

auto DCClient::remove_last_device() -> void{
    if(devices_nb() > 0){
        auto lg = LogGuard("DCClient::remove_last_device"sv);
        settings.devicesS.remove_last();
        i->processing.remove_last_processor();
        i->cDevices.back()->clean();
        i->cDevices.remove_last();
    }
}

auto DCClient::reset_remote_device(size_t idD) -> void{
    if(idD < devices_nb()){
        if(auto rD = dynamic_cast<DCClientRemoteDevice*>( i->cDevices[idD].get())){
            auto lg = LogGuard("DCClient::reset_remote_device"sv);
            settings.update_connection_settings(settings.devicesS[idD].connectionS);
            rD->clean();
            rD->initialize(settings.devicesS[idD].connectionS);
        }
    }
}

auto DCClient::use_normal_filters() -> void{
    settings.useNormalFilteringSettings = true;
}

auto DCClient::use_calibration_filters() -> void{
    settings.useNormalFilteringSettings = false;
}

auto DCClient::update_filters_settings(size_t idC, const DCFiltersSettings &filtersS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].filtersS = filtersS;
    if(settings.useNormalFilteringSettings){
        apply_filters_settings(idC);
    }
}

auto DCClient::update_calibration_filters_settings(size_t idC, const DCFiltersSettings &filtersS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_calibration_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].calibrationFiltersS = filtersS;
    if(!settings.useNormalFilteringSettings){
        apply_calibration_filters_settings(idC);
    }
}

auto DCClient::update_device_settings(size_t idC, const DCDeviceSettings &deviceS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_device_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
        
    settings.devicesS[idC].deviceS = deviceS;
    apply_device_settings(idC);
}

auto DCClient::update_color_settings(size_t idC, const DCColorSettings &colorS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_color_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].colorS = colorS;
    apply_color_settings(idC);
}

auto DCClient::update_misc_settings(size_t idC, const DCMiscSettings &miscS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_misc_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].miscS = miscS;
    apply_misc_settings(idC);
}

auto DCClient::update_model_settings(size_t idC, const DCModelSettings &modelS) -> void{
    settings.devicesS[idC].modelS = modelS;
    update_model_settings_signal(idC, settings.devicesS[idC].modelS);
}

auto DCClient::apply_device_settings(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_device_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    if(i->cDevices[idC]->device_connected()){
        i->cDevices[idC]->update_device_settings(settings.devicesS[idC].deviceS);
        i->processing.update_device_settings(idC, settings.devicesS[idC].deviceS);
    }
}

auto DCClient::apply_filters_settings(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_filters_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    if(i->cDevices[idC]->device_connected()){
        i->cDevices[idC]->update_filters_settings(settings.devicesS[idC].filtersS);
    }
}

auto DCClient::apply_calibration_filters_settings(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_calibration_filters_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    if(i->cDevices[idC]->device_connected()){
        i->cDevices[idC]->update_filters_settings(settings.devicesS[idC].calibrationFiltersS);
    }
}

auto DCClient::apply_color_settings(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_color_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    if(i->cDevices[idC]->device_connected()){
        i->cDevices[idC]->update_color_settings(settings.devicesS[idC].colorS);
    }
}

auto DCClient::apply_misc_settings(size_t idC) -> void{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_delay_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    if(i->cDevices[idC]->device_connected()){
        i->cDevices[idC]->update_misc_settings(settings.devicesS[idC].miscS);
    }
}

auto DCClient::trigger_all_models_settings() -> void{
    for(size_t idC = 0; idC < settings.devicesS.size(); ++idC){
        update_model_settings_signal(idC, settings.devicesS[idC].modelS);
    }
}

auto DCClient::trigger_all_device_display_settings() -> void{
    for(size_t idC = 0; idC < settings.devicesS.size(); ++idC){
        update_device_display_settings_signal(idC, settings.devicesS[idC].displayS);
    }
}

auto DCClient::process_frames_from_external_thread(size_t idD) -> void{
    if(idD > devices_nb()){
        Logger::error(std::format("[DCClient::process_frames_from_external_thread] Invalid id [{}], nb of devices available [{}].\n"sv, idD, devices_nb()));
        return;
    }
    i->processing.process(idD);
}

auto DCClient::generate_clients() -> void{    
    for(size_t idDevice = 0; idDevice < settings.devicesS.size(); ++idDevice){
        i->cDevices.push_back(i->generate_client(settings, idDevice));
    }
}

auto DCClient::messages_count() -> size_t{

    if(i->readMessagesL.try_lock()){
        size_t count = i->messages.size();
        i->readMessagesL.unlock();
        return count;
    }

    return 0;
}

auto DCClient::read_feedbacks() -> void{

    if(i->readMessagesL.try_lock()){
        auto size = i->messages.size();
        if(size != 0){

            for(size_t ii = 0; ii < size; ++ii){
                i->messagesR.push_back(i->messages.front());
                i->messages.pop_front();
            }
        }
        i->readMessagesL.unlock();
    }

    for(const auto &message : i->messagesR){        
        if(message.first < settings.devicesS.size()){
            settings.devicesS[message.first].connected = i->cDevices[message.first]->device_connected();
            feedback_received_signal(message.first, message.second);
        }       
    }
    i->messagesR.clear();
}




