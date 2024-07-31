
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


#include "dc_client.hpp"

// local
#include "utility/logger.hpp"
#include "dc_client_local_device.hpp"
#include "dc_client_remote_device.hpp"

using namespace tool::cam;

DCClient::DCClient(){
}

DCClient::~DCClient(){
    clean();
}

auto DCClient::initialize(const std::string &connectionSettingsPath) -> bool{

    if(devices_nb() != 0){
        clean();
    }

    if(!m_connectionSettings.load_from_file(connectionSettingsPath)){
        m_connectionSettings.default_initialize();
    }

    // init other settings
    m_processing.initialize(devices_nb(), true);
    devicesS.resize(devices_nb());

    size_t idDevice = 0;
    for(const auto &connectionS : m_connectionSettings.connectionsS){

        std::unique_ptr<DCClientDevice> device = nullptr;
        if(connectionS->isLocal){
            Logger::message("[DCClient::initialize] Create local device.\n"sv);
            auto lDevice = std::make_unique<DCClientLocalDevice>();
            lDevice->local_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCFrame> frame){
                m_processing.new_frame(idDevice, std::move(frame));
            });
            device = std::move(lDevice);
        }else{
            Logger::message("[DCClient::initialize] Create remote device.\n"sv);
            auto rDevice = std::make_unique<DCClientRemoteDevice>();

            rDevice->remote_feedback_signal.connect([this,idDevice](net::Feedback feedback){
                m_readMessagesL.lock();
                m_messages.emplace_back(idDevice, feedback);
                m_readMessagesL.unlock();
            });
            rDevice->remote_synchro_signal.connect([this,idDevice](std::int64_t averageTimestampDiffNs){
                devicesS[idDevice].synchroAverageDiff = averageTimestampDiffNs;
            });
            rDevice->remote_network_status_signal.connect([this,idDevice](net::UdpNetworkStatus status){
                devicesS[idDevice].receivedNetworkStatus = status;
            });
            rDevice->remote_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCCompressedFrame> cFrame){
                m_processing.new_compressed_frame(idDevice, std::move(cFrame));
            });
            device = std::move(rDevice);
        }

        device->data_status_signal.connect([this,idDevice](net::UdpDataStatus status){
            devicesS[idDevice].receivedDataStatus = status;
        });

        Logger::message("[DCClient::initialize] Initialize device.\n"sv);
        if(!device->initialize(connectionS.get())){
            Logger::error("[DCClient::initialize] Cannot initialize device.\n"sv);
            continue;
        }

        Logger::message("[DCClient::initialize] Add device to list.\n"sv);
        devices.push_back(std::move(device));
        ++idDevice;
    }
    return true;
}

auto DCClient::load_device_settings_file(size_t idC, const std::string &settingsFilePath) -> bool{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::load_device_settings_file] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return false;
    }

    if(devicesS[idC].deviceS.load_from_file(settingsFilePath)){
        devicesS[idC].deviceFilePath = settingsFilePath;
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

    if(devicesS[idC].filtersS.load_from_file(settingsFilePath)){
        devicesS[idC].filtersFilePath = settingsFilePath;
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

    if(devicesS[idC].calibrationFiltersS.load_from_file(settingsFilePath)){
        devicesS[idC].calibrationFiltersFilePath = settingsFilePath;
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

    if(devicesS[idC].colorS.load_from_file(settingsFilePath)){
        devicesS[idC].colorFilePath = settingsFilePath;
    }else{
        Logger::error(std::format("[DCClient::load_color_filters_settings_file] No device settings file found for device n°[{}]], default parameters used instead.\n", idC));
        return false;
    }

    return true;
}

auto DCClient::load_device_multi_settings_file(const std::string &multiSettingsFilePath) -> bool{

    std::vector<io::Settings*> settingsFiles;
    for(auto &deviceS : devicesS){
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
    for(auto &deviceS : devicesS){
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
    for(auto &deviceS : devicesS){
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
    for(auto &deviceS : devicesS){
        settingsFiles.push_back(&deviceS.colorS);
    }

    if(!io::Settings::load_multi_from_file(settingsFiles, multiSettingsFilePath)){
        Logger::error(std::format("[DCClient::load_color_filters_multi_settings_file] Error while reading multi settings file with path [{}].", multiSettingsFilePath));
        return false;
    }

    return true;
}

auto DCClient::clean() -> void{

    for(auto &device : devices){
        device->clean();
    }
    m_processing.clean();

    devices.clear();
    devicesS.clear();
}

auto DCClient::update() -> void{

    // read messages from network
    read_feedbacks();

    // read compressed frames
    std::vector<std::shared_ptr<cam::DCCompressedFrame>> cFrames(m_processing.nb_frame_processors(), nullptr);
    for(size_t idC = 0; idC < m_processing.nb_frame_processors(); ++idC){
        // check if new frame uncompressed
        if(auto cFrame = m_processing.get_compressed_frame(idC); cFrame != nullptr){
            cFrames[idC] = cFrame;
        }
    }
    // read frames
    std::vector<std::shared_ptr<cam::DCFrame>> frames(m_processing.nb_frame_processors(), nullptr);
    for(size_t idC = 0; idC < m_processing.nb_frame_processors(); ++idC){
        // check if new frame
        if(auto frame = m_processing.get_frame(idC); frame != nullptr){
            frames[idC] = frame;
        }
    }

    // send frames
    for(size_t idC = 0; idC < m_processing.nb_frame_processors(); ++idC){

        if(cFrames[idC]){

            if(devicesS[idC].lastCompressedFrameIdReceived != cFrames[idC]->idCapture){

                // update last compresesd frame id
                devicesS[idC].lastCompressedFrameIdReceived = cFrames[idC]->idCapture;

                // send it
                new_compressed_frame_signal(idC, std::move(cFrames[idC]));

                // invalidate it
                // sData.invalid_last_compressed_frame(ii);
            }
        }

        if(frames[idC]){

            if(devicesS[idC].lastFrameIdReceived != frames[idC]->idCapture){

                // update last frame id
                devicesS[idC].lastFrameIdReceived = frames[idC]->idCapture;

                // send it
                new_frame_signal(idC, std::move(frames[idC]));

                // invalidate it
                // sData.invalid_last_frame(ii);
            }
        }
    }
}

auto DCClient::devices_nb() const noexcept -> size_t{
    return devices.size();
}

auto DCClient::device_connected(size_t idC) const noexcept -> bool{
    if(idC < devices_nb()){
        return devices[idC]->device_connected();
    }
    return false;
}

auto DCClient::device_type(size_t idC) const noexcept -> DCClientType{
    if(idC < devices_nb()){
        return devices[idC]->type();
    }
    return DCClientType::undefined;
}

auto DCClient::init_connection_with_remote_device(size_t idC) -> void{
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::init_remote_connection] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    if(devices[idC]->type() == DCClientType::remote){
        dynamic_cast<DCClientRemoteDevice*>(devices[idC].get())->init_remote_connection();
    }
}

auto DCClient::read_network_data_from_remote_device(size_t idC) -> size_t{

    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::read_network_data_from_remote_device] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return 0;
    }

    if(devices[idC]->type() == DCClientType::remote){
        return dynamic_cast<DCClientRemoteDevice*>(devices[idC].get())->read_data_from_network();
    }

    return 0;
}

auto DCClient::apply_command(size_t idC, net::Command command) -> void{
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::apply_command] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    devices[idC]->apply_command(command);
}

auto DCClient::use_normal_filters() -> void{
    m_useNormalFilteringSettings = true;
}

auto DCClient::use_calibration_filters() -> void{
    m_useNormalFilteringSettings = false;
}

auto DCClient::update_filters(size_t idC, const DCFiltersSettings &filtersS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    devicesS[idC].filtersS = filtersS;
    if(m_useNormalFilteringSettings){
        devices[idC]->update_filters_settings(devicesS[idC].filtersS);
    }
}

auto DCClient::update_calibration_filters(size_t idC, const DCFiltersSettings &filtersS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_calibration_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    devicesS[idC].calibrationFiltersS = filtersS;
    if(!m_useNormalFilteringSettings){
        devices[idC]->update_filters_settings(devicesS[idC].calibrationFiltersS);
    }
}

auto DCClient::update_device_settings(size_t idC, const DCDeviceSettings &deviceS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_device_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    devicesS[idC].deviceS = deviceS;
    devices[idC]->update_device_settings(devicesS[idC].deviceS);
}

auto DCClient::update_color_settings(size_t idC, const DCColorSettings &colorS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_color_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    devicesS[idC].colorS = colorS;
    devices[idC]->update_color_settings(devicesS[idC].colorS);
}

auto DCClient::update_delay_settings(size_t idC, const DCDelaySettings &delayS) -> void{
    
    if(idC > devices_nb()){
        Logger::error(std::format("[DCClient::update_delay_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idC, devices_nb()));
        return;
    }

    devicesS[idC].delayS = delayS;
    devices[idC]->update_delay_settings(devicesS[idC].delayS);
}

auto DCClient::update_model_settings(size_t idC, const DCModelSettings &modelS) -> void{
    devicesS[idC].modelS = modelS;
    update_model_settings_signal(idC, devicesS[idC].modelS);
}

auto DCClient::trigger_all_models_settings() -> void{
    for(size_t idC = 0; idC < devicesS.size(); ++idC){
        update_model_settings_signal(idC, devicesS[idC].modelS);
    }
}

auto DCClient::trigger_all_device_display_settings() -> void{
    for(size_t idC = 0; idC < devicesS.size(); ++idC){
        update_device_display_settings_signal(idC, devicesS[idC].deviceDisplayS);
    }
}

auto DCClient::read_feedbacks() -> void{

    if(m_readMessagesL.try_lock()){
        auto size = m_messages.size();
        if(size != 0){

            for(size_t ii = 0; ii < size; ++ii){
                m_messagesR.push_back(m_messages.front());
                m_messages.pop_front();
            }
        }
        m_readMessagesL.unlock();
    }

    for(const auto &message : m_messagesR){

        if(message.first > devices_nb()){
            // ...
            continue;
        }

        devicesS[message.first].connected = devices[message.first]->device_connected();
        feedback_received_signal(message.first, std::format("Valid [{}] received\n", to_string(static_cast<tool::net::DCMessageType>(message.second.receivedMessageType))));
    }
    m_messagesR.clear();
}



