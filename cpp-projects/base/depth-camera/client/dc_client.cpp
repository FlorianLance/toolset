
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

    Buffer<std::unique_ptr<DCClientDevice>> devices;
    DCClientProcessing processing;

    std::mutex readMessagesL;
    std::deque<std::pair<size_t, net::Feedback>> messages;
    std::vector<std::pair<size_t, net::Feedback>> messagesR;
};

DCClient::DCClient(): i(std::make_unique<Impl>()){
}

DCClient::~DCClient(){
    clean();
}

auto DCClient::initialize(const std::string &clientSettingsPath) -> bool{

    if(devices_nb() != 0){
        clean();
    }
    
    if(!settings.load_from_file(clientSettingsPath)){
        return false;
    }
    settings.filePath = clientSettingsPath;

    // init other settings
    i->processing.initialize(settings.devicesS.size(), true);

    generate_clients();


    return true;
}

auto DCClient::load_device_settings_file(size_t idC, const std::string &settingsFilePath) -> bool{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::load_device_settings_file] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return false;
    }
    
    if(settings.devicesS[idC].deviceS.load_from_file(settingsFilePath)){
        settings.devicesS[idC].deviceFilePath = settingsFilePath;
    }else{
        Logger::error(std::format("[DCClient::load_device_settings_file No device settings file found for device n°[{}]], default parameters used instead.\n", idC));
        return false;
    }

    return true;
}

auto DCClient::load_filters_settings_file(size_t idC, const std::string &settingsFilePath) -> bool{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::load_filters_settings_file] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return false;
    }
    
    if(settings.devicesS[idC].filtersS.load_from_file(settingsFilePath)){
        settings.devicesS[idC].filtersFilePath = settingsFilePath;
    }else{
        Logger::error(std::format("[DCClient::load_filters_settings_file] No device settings file found for device n°[{}]], default parameters used instead.\n", idC));
        return false;
    }

    return true;
}

auto DCClient::load_calibration_filters_settings_file(size_t idC, const std::string &settingsFilePath) -> bool{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::load_calibration_filters_settings_file] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return false;
    }
    
    if(settings.devicesS[idC].calibrationFiltersS.load_from_file(settingsFilePath)){
        settings.devicesS[idC].calibrationFiltersFilePath = settingsFilePath;
    }else{
        Logger::error(std::format("[DCClient::load_calibration_filters_settings_file] No device settings file found for device n°[{}]], default parameters used instead.\n", idC));
        return false;
    }

    return true;
}

auto DCClient::load_color_filters_settings_file(size_t idC, const std::string &settingsFilePath) -> bool{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::load_color_filters_settings_file] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return false;
    }
    
    if(settings.devicesS[idC].colorS.load_from_file(settingsFilePath)){
        settings.devicesS[idC].colorFilePath = settingsFilePath;
    }else{
        Logger::error(std::format("[DCClient::load_color_filters_settings_file] No device settings file found for device n°[{}]], default parameters used instead.\n", idC));
        return false;
    }

    return true;
}

auto DCClient::load_device_multi_settings_file(const std::string &multiSettingsFilePath) -> bool{

    std::vector<io::Settings*> settingsFiles;
    for(auto &deviceS : settings.devicesS){
        settingsFiles.push_back(&deviceS.deviceS);
    }

    if(!io::Settings::load_multi_from_file(settingsFiles, multiSettingsFilePath)){
        Logger::error(std::format("[DCClient::load_device_multi_settings_file] Error while reading multi settings file with path [{}].", multiSettingsFilePath));
        return false;
    }

    return true;
}

auto DCClient::load_filters_multi_settings_file(const std::string &multiSettingsFilePath) -> bool{

    std::vector<io::Settings*> settingsFiles;
    for(auto &deviceS : settings.devicesS){
        settingsFiles.push_back(&deviceS.filtersS);
    }

    if(!io::Settings::load_multi_from_file(settingsFiles, multiSettingsFilePath)){
        Logger::error(std::format("[DCClient::load_filters_multi_settings_file] Error while reading multi settings file with path [{}].", multiSettingsFilePath));
        return false;
    }

    return true;
}

auto DCClient::load_calibration_filters_multi_settings_file(const std::string &multiSettingsFilePath) -> bool{

    std::vector<io::Settings*> settingsFiles;
    for(auto &deviceS : settings.devicesS){
        settingsFiles.push_back(&deviceS.calibrationFiltersS);
    }

    if(!io::Settings::load_multi_from_file(settingsFiles, multiSettingsFilePath)){
        Logger::error(std::format("[DCClient::load_calibration_filters_multi_settings_file] Error while reading multi settings file with path [{}].", multiSettingsFilePath));
        return false;
    }

    return true;
}

auto DCClient::load_color_filters_multi_settings_file(const std::string &multiSettingsFilePath) -> bool{

    std::vector<io::Settings*> settingsFiles;
    for(auto &deviceS : settings.devicesS){
        settingsFiles.push_back(&deviceS.colorS);
    }

    if(!io::Settings::load_multi_from_file(settingsFiles, multiSettingsFilePath)){
        Logger::error(std::format("[DCClient::load_color_filters_multi_settings_file] Error while reading multi settings file with path [{}].", multiSettingsFilePath));
        return false;
    }

    return true;
}

auto DCClient::clean() -> void{

    for(auto &device : i->devices){
        device->clean();
    }
    i->processing.clean();

    i->devices.clear();
    settings.devicesS.clear();
}

auto DCClient::update() -> void{

    // read messages from network
    read_feedbacks();

    // read compressed frames
    std::vector<std::shared_ptr<cam::DCCompressedFrame>> cFrames(i->processing.nb_frame_processors(), nullptr);
    for(size_t idC = 0; idC < i->processing.nb_frame_processors(); ++idC){
        // check if new frame uncompressed
        if(auto cFrame = i->processing.get_compressed_frame(idC); cFrame != nullptr){
            cFrames[idC] = cFrame;
        }
    }
    // read frames
    std::vector<std::shared_ptr<cam::DCFrame>> frames(i->processing.nb_frame_processors(), nullptr);
    for(size_t idC = 0; idC < i->processing.nb_frame_processors(); ++idC){
        // check if new frame
        if(auto frame = i->processing.get_frame(idC); frame != nullptr){
            frames[idC] = frame;
        }
    }

    // send frames
    for(size_t idC = 0; idC < i->processing.nb_frame_processors(); ++idC){

        if(cFrames[idC]){
            
            if(settings.devicesS[idC].lastCompressedFrameIdReceived != cFrames[idC]->idCapture){

                // update last compresesd frame id
                settings.devicesS[idC].lastCompressedFrameIdReceived = cFrames[idC]->idCapture;

                // send it
                new_compressed_frame_signal(idC, std::move(cFrames[idC]));

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
}

auto DCClient::devices_nb() const noexcept -> size_t{
    return i->devices.size();
}

auto DCClient::device_connected(size_t idC) const noexcept -> bool{
    if(idC < devices_nb()){
        return i->devices[idC]->device_connected();
    }
    return false;
}

auto DCClient::device_type(size_t idC) const noexcept -> DCClientType{
    if(idC < devices_nb()){
        return i->devices[idC]->type();
    }
    return DCClientType::undefined;
}

auto DCClient::init_connection_with_remote_device(size_t idC) -> void{
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::init_remote_connection] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    if(i->devices[idC]->type() == DCClientType::remote){
        dynamic_cast<DCClientRemoteDevice*>(i->devices[idC].get())->init_remote_connection();
    }
}

auto DCClient::read_network_data_from_remote_device(size_t idC) -> size_t{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::read_network_data_from_remote_device] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return 0;
    }

    if(i->devices[idC]->type() == DCClientType::remote){
        return dynamic_cast<DCClientRemoteDevice*>(i->devices[idC].get())->read_data_from_network();
    }

    return 0;
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
            coS.connectionType = DCDeviceConnectionType::Local;
        }else{
            auto *remoteC = dynamic_cast<DCDeprecatedRemoteDeviceConnectionSettings*>(dS.connectionsS[idD].get());
            coS.connectionType      = DCDeviceConnectionType::Remote;
            coS.idReadingInterface  = remoteC->serverS.idReadingInterface;
            coS.readingPort         = remoteC->serverS.readingPort;
            coS.sendingAddress      = remoteC->serverS.sendingAdress;
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

    i->devices[idC]->apply_command(command);
}

auto DCClient::use_normal_filters() -> void{
    settings.useNormalFilteringSettings = true;
}

auto DCClient::use_calibration_filters() -> void{
    settings.useNormalFilteringSettings = false;
}

auto DCClient::update_filters(size_t idC, const DCFiltersSettings &filtersS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].filtersS = filtersS;
    if(settings.useNormalFilteringSettings){
        i->devices[idC]->update_filters_settings(settings.devicesS[idC].filtersS);
    }
}

auto DCClient::update_calibration_filters(size_t idC, const DCFiltersSettings &filtersS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_calibration_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].calibrationFiltersS = filtersS;
    if(!settings.useNormalFilteringSettings){
        i->devices[idC]->update_filters_settings(settings.devicesS[idC].calibrationFiltersS);
    }
}

auto DCClient::update_device_settings(size_t idC, const DCDeviceSettings &deviceS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_device_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
        
    settings.devicesS[idC].deviceS = deviceS;
    i->devices[idC]->update_device_settings(settings.devicesS[idC].deviceS);
    i->processing.update_device_settings(idC, deviceS);
}

auto DCClient::update_color_settings(size_t idC, const DCColorSettings &colorS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_color_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].colorS = colorS;
    i->devices[idC]->update_color_settings(settings.devicesS[idC].colorS);
}

auto DCClient::update_delay_settings(size_t idC, const DCDelaySettings &delayS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_delay_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }
    
    settings.devicesS[idC].delayS = delayS;
    i->devices[idC]->update_delay_settings(settings.devicesS[idC].delayS);
}

auto DCClient::update_model_settings(size_t idC, const DCModelSettings &modelS) -> void{
    settings.devicesS[idC].modelS = modelS;
    update_model_settings_signal(idC, settings.devicesS[idC].modelS);
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

auto DCClient::generate_clients() -> void{

    size_t idDevice = 0;
    for(auto &clientDeviceS : settings.devicesS){

        std::unique_ptr<DCClientDevice> device = nullptr;
        if(clientDeviceS.connectionS.connectionType == DCDeviceConnectionType::Local){

            Logger::message("[DCClient::initialize] Create local device.\n"sv);
            auto lDevice = std::make_unique<DCClientLocalDevice>();
            lDevice->local_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCFrame> frame){
                i->processing.new_frame(idDevice, std::move(frame));
            });
            device = std::move(lDevice);

        }else if(clientDeviceS.connectionS.connectionType == DCDeviceConnectionType::Remote){

            Logger::message("[DCClient::initialize] Create remote device.\n"sv);
            auto rDevice = std::make_unique<DCClientRemoteDevice>();
            rDevice->remote_feedback_signal.connect([this,idDevice](net::Feedback feedback){
                i->readMessagesL.lock();
                i->messages.emplace_back(idDevice, feedback);
                i->readMessagesL.unlock();
            });
            rDevice->remote_synchro_signal.connect([this,idDevice](std::int64_t averageTimestampDiffNs){
                settings.devicesS[idDevice].synchroAverageDiff = averageTimestampDiffNs;
            });
            rDevice->remote_network_status_signal.connect([this,idDevice](net::UdpNetworkStatus status){
                settings.devicesS[idDevice].receivedNetworkStatus = status;
            });
            rDevice->remote_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCCompressedFrame> cFrame){
                i->processing.new_compressed_frame(idDevice, std::move(cFrame));
            });
            device = std::move(rDevice);
        }

        device->data_status_signal.connect([this,idDevice](net::UdpDataStatus status){
            settings.devicesS[idDevice].receivedDataStatus = status;
        });

        Logger::message("[DCClient::initialize] Initialize device.\n"sv);
        if(!device->initialize(clientDeviceS.connectionS)){
            Logger::error("[DCClient::initialize] Cannot initialize device.\n"sv);
            continue;
        }

        Logger::message("[DCClient::initialize] Add device to list.\n"sv);
        i->devices.push_back(std::move(device));
        ++idDevice;
    }
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

        if(message.first > devices_nb()){
            // ...
            continue;
        }
        
        settings.devicesS[message.first].connected = i->devices[message.first]->device_connected();
        feedback_received_signal(message.first, std::format("Valid [{}] received\n", to_string(static_cast<tool::net::DCMessageType>(message.second.receivedMessageType))));
    }
    i->messagesR.clear();
}



