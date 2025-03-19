
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

#include "dc_settings_paths.hpp"

// local
#include "utility/logger.hpp"
#include "utility/paths.hpp"

using namespace tool;
using namespace tool::cam;
using namespace std::string_view_literals;

struct Global{
    static inline std::unique_ptr<DCSettingsPaths> instance = nullptr;
};

auto DCSettingsPaths::get() -> DCSettingsPaths*{

    if(Global::instance == nullptr){
        Global::instance = std::make_unique<DCSettingsPaths>();
    }
    return Global::instance.get();
}

auto DCSettingsPaths::initialize(char *argv[], DCApplicationType type, size_t id) -> void{

    auto paths = Paths::get();
    if(!paths->is_initialized()){
        paths->initialize(argv);
    }

    std::string cId = std::format("{}_{}", paths->hostName, id);

    // per type
    switch (type) {
    case DCApplicationType::DCGrabber:{        
        logName                              = std::format("dcm_grabber_{}.html"sv, cId);
        defaultServer                        = paths->configDir   / "server_default.json"sv;
        server                               = paths->configDir   / std::format("server_{}.json"sv, cId);
        // legacy
        serverJson.model                     = paths->settingsDir / std::format("model_{}.json"sv, cId);
        serverJson.filters                   = paths->settingsDir / std::format("filters_{}.json"sv, cId);
        serverJson.calibrationFilters        = paths->settingsDir / std::format("calibration_filters_{}.json"sv, cId);
        serverJson.device                    = paths->settingsDir / std::format("device_{}.json"sv, cId);
        serverJson.color                     = paths->settingsDir / std::format("color_{}.json"sv, cId);
        serverConfig.model                   = paths->settingsDir / std::format("model_{}.config"sv, cId);
        serverConfig.filters                 = paths->settingsDir / std::format("filters_{}.config"sv, cId);
        serverConfig.calibrationFilters      = paths->settingsDir / std::format("calibration_filters_{}.config"sv, cId);
        serverConfig.device                  = paths->settingsDir / std::format("device_{}.config"sv, cId);
        serverConfig.color                   = paths->settingsDir / std::format("color_{}.config"sv, cId);
        break;
    }case DCApplicationType::DCManager:{
        logName                              = std::format("dcm_manager_{}.html"sv,  cId);
        defaultClient                        = paths->configDir / "client_default.json"sv;
        client                               = paths->configDir / std::format("client_{}.json"sv, cId);
        // legacy
        // # all files
        // ## json
        allJsonClients.model                 = paths->settingsDir / std::format("model_{}_all.json"sv, paths->hostName);
        allJsonClients.filters               = paths->settingsDir / std::format("filters_{}_all.json"sv, paths->hostName);
        allJsonClients.calibrationFilters    = paths->settingsDir / std::format("calibration_filters_{}_all.json"sv, paths->hostName);
        allJsonClients.device                = paths->settingsDir / std::format("device_{}_all.json"sv, paths->hostName);
        allJsonClients.color                 = paths->settingsDir / std::format("color_{}_all.json"sv, paths->hostName);
        // ## config
        allConfigClients.model               = paths->settingsDir / std::format("model_{}_all.config"sv, paths->hostName);
        allConfigClients.filters             = paths->settingsDir / std::format("filters_{}_all.config"sv, paths->hostName);
        allConfigClients.calibrationFilters  = paths->settingsDir / std::format("calibration_filters_{}_all.config"sv, paths->hostName);
        allConfigClients.device              = paths->settingsDir / std::format("device_{}_all.config"sv, paths->hostName);
        allConfigClients.color               = paths->settingsDir / std::format("color_{}_all.config"sv, paths->hostName);
        // # client
        // ## json
        clientsJson.resize(20);
        for(size_t idG = 0; idG < clientsJson.size(); ++idG){
            auto &gPaths                     = clientsJson[idG];
            gPaths.model                     = paths->settingsDir / std::format("model_{}.json"sv, cId);
            gPaths.filters                   = paths->settingsDir / std::format("filters_{}.json"sv, cId);
            gPaths.calibrationFilters        = paths->settingsDir / std::format("calibration_filters_{}.json"sv, cId);
            gPaths.device                    = paths->settingsDir / std::format("device_{}.json"sv, cId);
            gPaths.color                     = paths->settingsDir / std::format("color_{}.json"sv, cId);
        }
        // ## config
        clientsConfig.resize(20);
        for(size_t idG = 0; idG < clientsConfig.size(); ++idG){
            auto &gPaths = clientsConfig[idG];
            gPaths.model                = paths->settingsDir / std::format("model_{}.config"sv, cId);
            gPaths.filters              = paths->settingsDir / std::format("filters_{}.config"sv, cId);
            gPaths.calibrationFilters   = paths->settingsDir / std::format("calibration_filters_{}.config"sv, cId);
            gPaths.device               = paths->settingsDir / std::format("device_{}.config"sv, cId);
            gPaths.color                = paths->settingsDir / std::format("color_{}.config"sv, cId);
        }

        break;
    }default:
        break;
    }

    // common
    // # legacy
    defaultJsonNetwork                       = paths->networkDir  / "network_default.json"sv;
    jsonNetwork                              = paths->networkDir  / std::format("network_{}.json"sv, cId);
    configNetwork                            = paths->networkDir  / std::format("network_{}.config"sv, cId);
    // ## default
    // ### json
    defaultJson.model                        = paths->calibrationDir / "model_default.json"sv;
    defaultJson.filters                      = paths->settingsDir    / "filters_default.json"sv;
    defaultJson.calibrationFilters           = paths->settingsDir    / "calibration_filters_default.json"sv;
    defaultJson.device                       = paths->settingsDir    / "device_default.json"sv;
    defaultJson.color                        = paths->settingsDir    / "color_default.json"sv;
    // ### config
    defaultConfig.model                      = paths->calibrationDir / "model_default.config"sv;
    defaultConfig.filters                    = paths->settingsDir    / "filters_default.config"sv;
    defaultConfig.calibrationFilters         = paths->settingsDir    / "calibration_filters_default.config"sv;
    defaultConfig.device                     = paths->settingsDir    / "device_default.config"sv;
    defaultConfig.color                      = paths->settingsDir    / "color_default.config"sv;
}

auto DCSettingsPaths::client_settings_file() -> std::string{

    if(std::filesystem::exists(client)){
        return client.string();
    }
    Log::warning(std::format("Cannot find client file [{}].\n", client.string()));

    if(std::filesystem::exists(defaultClient)){
        return defaultClient.string();
    }
    Log::warning(std::format("Cannot find default client settings file [{}].\n", defaultClient.string()));
    
    Log::warning("[DCSettingsPaths::client_settings_file] Cannot find any client file.\n");
    return {};
}

auto DCSettingsPaths::server_settings_file() -> std::string{

    if(std::filesystem::exists(server)){
        return server.string();
    }
    Log::warning(std::format("Cannot find server file [{}].\n", server.string()));

    if(std::filesystem::exists(defaultServer)){
        return defaultServer.string();
    }
    Log::warning(std::format("Cannot find default server settings file [{}].\n", defaultServer.string()));
    
    Log::warning("[DCSettingsPaths::server_settings_file] Cannot find any server file.\n");
    return {};
}

auto DCSettingsPaths::network_settings_file() -> std::string{

    if(std::filesystem::exists(jsonNetwork)){
        return jsonNetwork.string();
    }
    Log::warning(std::format("Cannot find JSON network file [{}]\n", jsonNetwork.string()));

    if(std::filesystem::exists(configNetwork)){
        return configNetwork.string();
    }
    Log::warning(std::format("Cannot find config network file [{}]\n", configNetwork.string()));

    if(std::filesystem::exists(defaultJsonNetwork)){
        return defaultJsonNetwork.string();
    }
    Log::warning(std::format("Cannot find default JSON network file [{}]\n", defaultJsonNetwork.string()));
    
    Log::error("[DCSettingsPaths::network_settings_file] Cannot find any network file.\n");
    return {};
}

auto DCSettingsPaths::client_filters_settings_file(size_t idG) -> std::string{

    if(idG < clientsJson.size()){
        if(std::filesystem::exists(clientsJson[idG].filters)){
            return clientsJson[idG].filters.string();
        }
        if(std::filesystem::exists(clientsConfig[idG].filters)){
            return clientsConfig[idG].filters.string();
        }
    }else{
        Log::warning(std::format("Invalid client id [{}] for filters settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.filters)){
        Log::warning(std::format("Cannot find filters settings file for client [{}], use default instead.\n"sv, idG));
        return defaultJson.filters.string();
    }else{
        Log::warning(std::format("Cannot find default filters settings file [{}].\n"sv, defaultJson.filters.string()));
    }
    return {};
}

auto DCSettingsPaths::client_calibration_filters_settings_file(size_t idG) -> std::string{

    if(idG < clientsJson.size()){
        if(std::filesystem::exists(clientsJson[idG].calibrationFilters)){
            return clientsJson[idG].calibrationFilters.string();
        }
        if(std::filesystem::exists(clientsConfig[idG].calibrationFilters)){
            return clientsConfig[idG].calibrationFilters.string();
        }
    }else{
        Log::warning(std::format("Invalid client id [{}] for calibration filters settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.calibrationFilters)){
        Log::warning(std::format("Cannot find calibration filters settings file for client [{}], use default instead.\n"sv, idG));
        return defaultJson.calibrationFilters.string();
    }else{
        Log::warning(std::format("Cannot find default calibration filters settings file [{}].\n"sv, defaultJson.calibrationFilters.string()));
    }
    return {};
}

auto DCSettingsPaths::client_device_settings_file(size_t idG) -> std::string{

    if(idG < clientsJson.size()){
        if(std::filesystem::exists(clientsJson[idG].device)){
            return clientsJson[idG].device.string();
        }
        if(std::filesystem::exists(clientsConfig[idG].device)){
            return clientsConfig[idG].device.string();
        }
    }else{
        Log::warning(std::format("Invalid client id [{}] for device settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.device)){
        Log::warning(std::format("Cannot find device settings file for client [{}], use default instead.\n"sv, idG));
        return defaultJson.device.string();
    }else{
        Log::warning(std::format("Cannot find default device settings file [{}].\n"sv, defaultJson.device.string()));
    }
    return {};
}

auto DCSettingsPaths::client_color_settings_file(size_t idG) -> std::string{

    if(idG < clientsJson.size()){
        if(std::filesystem::exists(clientsJson[idG].color)){
            return clientsJson[idG].color.string();
        }
        if(std::filesystem::exists(clientsConfig[idG].color)){
            return clientsConfig[idG].color.string();
        }
    }else{
        Log::warning(std::format("Invalid client id [{}] for color settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.color)){
        Log::warning(std::format("Cannot find color settings file for client [{}], use default instead.\n"sv, idG));
        return defaultJson.color.string();
    }else{
        Log::warning(std::format("Cannot find default color settings file [{}].\n"sv, defaultJson.color.string()));
    }
    return {};
}

auto DCSettingsPaths::client_model_settings_file(size_t idG) -> std::string{

    if(idG < clientsJson.size()){
        if(std::filesystem::exists(clientsJson[idG].model)){
            return clientsJson[idG].model.string();
        }
        if(std::filesystem::exists(clientsConfig[idG].model)){
            return clientsConfig[idG].model.string();
        }
    }else{
        Log::warning(std::format("Invalid client id [{}] for model settings file.\n", idG));
    }


    if(std::filesystem::exists(defaultJson.model)){
        Log::warning(std::format("Cannot find model settings file for client [{}], use default instead.\n"sv, idG));
        return defaultJson.model.string();
    }else{
        Log::warning(std::format("Cannot find default modelsettings file [{}].\n"sv, defaultJson.model.string()));
    }
    return {};
}

auto DCSettingsPaths::server_filters_settings_file() -> std::string{
    if(std::filesystem::exists(serverJson.filters)){
        return serverJson.filters.string();
    }
    if(std::filesystem::exists(serverConfig.filters)){
        return serverConfig.filters.string();
    }
    if(std::filesystem::exists(defaultJson.filters)){
        return defaultJson.filters.string();
    }
    if(std::filesystem::exists(defaultConfig.filters)){
        return defaultConfig.filters.string();
    }
    return {};
}

auto DCSettingsPaths::server_calibration_filters_settings_file() -> std::string{
    if(std::filesystem::exists(serverJson.calibrationFilters)){
        return serverJson.calibrationFilters.string();
    }
    if(std::filesystem::exists(serverConfig.calibrationFilters)){
        return serverConfig.calibrationFilters.string();
    }
    if(std::filesystem::exists(defaultJson.calibrationFilters)){
        return defaultJson.calibrationFilters.string();
    }
    if(std::filesystem::exists(defaultConfig.calibrationFilters)){
        return defaultConfig.calibrationFilters.string();
    }
    return {};
}

auto DCSettingsPaths::server_device_settings_file() -> std::string{
    if(std::filesystem::exists(serverJson.device)){
        return serverJson.device.string();
    }
    if(std::filesystem::exists(serverConfig.device)){
        return serverConfig.device.string();
    }
    if(std::filesystem::exists(defaultJson.device)){
        return defaultJson.device.string();
    }
    if(std::filesystem::exists(defaultConfig.device)){
        return defaultConfig.device.string();
    }
    return {};
}

auto DCSettingsPaths::server_color_settings_file() -> std::string{
    if(std::filesystem::exists(serverJson.color)){
        return serverJson.color.string();
    }
    if(std::filesystem::exists(serverConfig.color)){
        return serverConfig.color.string();
    }
    if(std::filesystem::exists(defaultJson.color)){
        return defaultJson.color.string();
    }
    if(std::filesystem::exists(defaultConfig.color)){
        return defaultConfig.color.string();
    }
    return {};
}

auto DCSettingsPaths::server_model_settings_file() -> std::string {
    if(std::filesystem::exists(serverJson.model)){
        return serverJson.model.string();
    }
    if(std::filesystem::exists(serverConfig.model)){
        return serverConfig.model.string();
    }
    if(std::filesystem::exists(defaultJson.model)){
        return defaultJson.model.string();
    }
    if(std::filesystem::exists(defaultConfig.model)){
        return defaultConfig.model.string();
    }
    return {};
}

