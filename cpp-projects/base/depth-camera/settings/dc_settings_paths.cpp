
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

auto DCSettingsPaths::initialize(char *argv[], DCApplicationType type, std::optional<size_t> id) -> void{

    auto paths = Paths::get();
    if(!paths->is_initialized()){
        paths->initialize(argv);
    }

    // common
    // # legacy
    defaultJsonNetwork  = paths->networkDir / "network_default.json"sv;
    jsonNetwork         = paths->networkDir / std::format("network_{}.json"sv,    paths->hostName);
    configNetwork       = paths->networkDir / std::format("network_{}.config"sv,  paths->hostName);
    // ## default
    // ### json
    defaultJson.model                   = paths->calibrationDir / "model_default.json"sv;
    defaultJson.filters                 = paths->settingsDir    / "filters_default.json"sv;
    defaultJson.calibrationFilters      = paths->settingsDir    / "calibration_filters_default.json"sv;
    defaultJson.device                  = paths->settingsDir    / "device_default.json"sv;
    defaultJson.color                   = paths->settingsDir    / "color_default.json"sv;
    // ### config
    defaultConfig.model                 = paths->calibrationDir / "model_default.config"sv;
    defaultConfig.filters               = paths->settingsDir    / "filters_default.config"sv;
    defaultConfig.calibrationFilters    = paths->settingsDir    / "calibration_filters_default.config"sv;
    defaultConfig.device                = paths->settingsDir    / "device_default.config"sv;
    defaultConfig.color                 = paths->settingsDir    / "color_default.config"sv;

    // per type
    switch (type) {
    case DCApplicationType::DCGrabber:{

        if(id.has_value()){
            logName = std::format("dcm_grabber_{}_{}.html"sv,  paths->hostName, id.value());
        }else{
            logName = std::format("dcm_grabber_{}.html"sv,  paths->hostName);
        }

        defaultServer       = paths->configDir / "server_default.json"sv;

        size_t idG = 0;
        if(id.has_value()){
            server          = paths->configDir / std::format("server_{}_{}.json"sv, paths->hostName, id.value());
            idG = id.value();
        }else{
            server          = paths->configDir / std::format("server_{}.json"sv, paths->hostName);
        }

        serverJson.model                   = paths->settingsDir / std::format("model_{}_G{}.json"sv,                   paths->hostName, idG);
        serverJson.filters                 = paths->settingsDir / std::format("filters_{}_G{}.json"sv,                 paths->hostName, idG);
        serverJson.calibrationFilters      = paths->settingsDir / std::format("calibration_filters_{}_G{}.json"sv,     paths->hostName, idG);
        serverJson.device                  = paths->settingsDir / std::format("device_{}_G{}.json"sv,                  paths->hostName, idG);
        serverJson.color                   = paths->settingsDir / std::format("color_{}_G{}.json"sv,                   paths->hostName, idG);
        serverConfig.model                 = paths->settingsDir / std::format("model_{}_G{}.config"sv,                 paths->hostName, idG);
        serverConfig.filters               = paths->settingsDir / std::format("filters_{}_G{}.config"sv,               paths->hostName, idG);
        serverConfig.calibrationFilters    = paths->settingsDir / std::format("calibration_filters_{}_G{}.config"sv,   paths->hostName, idG);
        serverConfig.device                = paths->settingsDir / std::format("device_{}_G{}.config"sv,                paths->hostName, idG);
        serverConfig.color                 = paths->settingsDir / std::format("color_{}_G{}.config"sv,                 paths->hostName, idG);

        break;
    }case DCApplicationType::DCManager:

        if(id.has_value()){
            logName = std::format("dcm_manager_{}_{}.html"sv,  paths->hostName, id.value());
        }else{
            logName = std::format("dcm_manager_{}.html"sv,  paths->hostName);
        }

        defaultClient       = paths->configDir / "client_default.json"sv;
        if(id.has_value()){
             client          = paths->configDir / std::format("client_{}_{}.json"sv, paths->hostName, id.value());
        }else{
            client          = paths->configDir / std::format("client_{}.json"sv, paths->hostName);
        }

        // all files
        allJsonClients.model               = paths->settingsDir / std::format("model_{}_all.json"sv,                 paths->hostName);
        allJsonClients.filters             = paths->settingsDir / std::format("filters_{}_all.json"sv,               paths->hostName);
        allJsonClients.calibrationFilters  = paths->settingsDir / std::format("calibration_filters_{}_all.json"sv,   paths->hostName);
        allJsonClients.device              = paths->settingsDir / std::format("device_{}_all.json"sv,                paths->hostName);
        allJsonClients.color               = paths->settingsDir / std::format("color_{}_all.json"sv,                 paths->hostName);

        // # grabber
        // ## json
        clientsJson.resize(20);
        for(size_t idG = 0; idG < clientsJson.size(); ++idG){
            auto &gPaths = clientsJson[idG];
            gPaths.model                = paths->settingsDir / std::format("model_{}_G{}.json"sv,                 paths->hostName, idG);
            gPaths.filters              = paths->settingsDir / std::format("filters_{}_G{}.json"sv,               paths->hostName, idG);
            gPaths.calibrationFilters   = paths->settingsDir / std::format("calibration_filters_{}_G{}.json"sv,   paths->hostName, idG);
            gPaths.device               = paths->settingsDir / std::format("device_{}_G{}.json"sv,                paths->hostName, idG);
            gPaths.color                = paths->settingsDir / std::format("color_{}_G{}.json"sv,                 paths->hostName, idG);
        }
        // ## config
        allConfigClients.model               = paths->settingsDir / std::format("model_{}_all.config"sv,                 paths->hostName);
        allConfigClients.filters             = paths->settingsDir / std::format("filters_{}_all.config"sv,               paths->hostName);
        allConfigClients.calibrationFilters  = paths->settingsDir / std::format("calibration_filters_{}_all.config"sv,   paths->hostName);
        allConfigClients.device              = paths->settingsDir / std::format("device_{}_all.config"sv,                paths->hostName);
        allConfigClients.color               = paths->settingsDir / std::format("color_{}_all.config"sv,                 paths->hostName);
        clientsConfig.resize(20);
        for(size_t idG = 0; idG < clientsConfig.size(); ++idG){
            auto &gPaths = clientsConfig[idG];
            gPaths.model                = paths->settingsDir / std::format("model_{}_G{}.config"sv,               paths->hostName, idG);
            gPaths.filters              = paths->settingsDir / std::format("filters_{}_G{}.config"sv,             paths->hostName, idG);
            gPaths.calibrationFilters   = paths->settingsDir / std::format("calibration_filters_{}_G{}.config"sv, paths->hostName, idG);
            gPaths.device               = paths->settingsDir / std::format("device_{}_G{}.config"sv,              paths->hostName, idG);
            gPaths.color                = paths->settingsDir / std::format("color_{}_G{}.config"sv,               paths->hostName, idG);
        }

        break;
    default:
        break;
    }

}

auto DCSettingsPaths::client_settings_file() -> std::string{
    if(std::filesystem::exists(client)){
        return client.string();
    }else if(std::filesystem::exists(defaultClient)){
        Logger::warning(std::format("Cannot find current client settings file [{}], use default instead.\n", defaultClient.string()));
        return defaultClient.string();
    }else{
        Logger::warning(std::format("Cannot find default client settings file [{}].\n", defaultClient.string()));
    }
    return {};
}

auto DCSettingsPaths::server_settings_file() -> std::string{
    if(std::filesystem::exists(server)){
        return server.string();
    }else if(std::filesystem::exists(defaultServer)){
        Logger::warning(std::format("Cannot find current server settings file [{}], use default instead.\n", defaultServer.string()));
        return defaultServer.string();
    }else{
        Logger::warning(std::format("Cannot find default server settings file [{}].\n", defaultServer.string()));
    }
    return {};
}

auto DCSettingsPaths::network_settings_file() -> std::string{

    if(std::filesystem::exists(jsonNetwork)){
        return jsonNetwork.string();
    }else if(std::filesystem::exists(configNetwork)){
        return configNetwork.string();
    }else if(std::filesystem::exists(defaultJsonNetwork)){
        Logger::warning(std::format("Cannot find current host network settings file [{}], use default instead.\n", defaultJsonNetwork.string()));
        return defaultJsonNetwork.string();
    }else{
        Logger::warning(std::format("Cannot find current any network settings file and cannot find default file [{}].\n", defaultJsonNetwork.string()));
    }
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
        Logger::warning(std::format("Invalid grabber id [{}] for filters settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.filters)){
        Logger::warning(std::format("Cannot find filters settings file for grabber [{}], use default instead.\n"sv, idG));
        return defaultJson.filters.string();
    }else{
        Logger::warning(std::format("Cannot find default filters settings file [{}].\n"sv, defaultJson.filters.string()));
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
        Logger::warning(std::format("Invalid grabber id [{}] for calibration filters settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.calibrationFilters)){
        Logger::warning(std::format("Cannot find calibration filters settings file for grabber [{}], use default instead.\n"sv, idG));
        return defaultJson.calibrationFilters.string();
    }else{
        Logger::warning(std::format("Cannot find default calibration filters settings file [{}].\n"sv, defaultJson.calibrationFilters.string()));
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
        Logger::warning(std::format("Invalid grabber id [{}] for device settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.device)){
        Logger::warning(std::format("Cannot find device settings file for grabber [{}], use default instead.\n"sv, idG));
        return defaultJson.device.string();
    }else{
        Logger::warning(std::format("Cannot find default device settings file [{}].\n"sv, defaultJson.device.string()));
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
        Logger::warning(std::format("Invalid grabber id [{}] for color settings file.\n"sv, idG));
    }

    if(std::filesystem::exists(defaultJson.color)){
        Logger::warning(std::format("Cannot find color settings file for grabber [{}], use default instead.\n"sv, idG));
        return defaultJson.color.string();
    }else{
        Logger::warning(std::format("Cannot find default color settings file [{}].\n"sv, defaultJson.color.string()));
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
        Logger::warning(std::format("Invalid grabber id [{}] for model settings file.\n", idG));
    }


    if(std::filesystem::exists(defaultJson.model)){
        Logger::warning(std::format("Cannot find model settings file for grabber [{}], use default instead.\n"sv, idG));
        return defaultJson.model.string();
    }else{
        Logger::warning(std::format("Cannot find default modelsettings file [{}].\n"sv, defaultJson.model.string()));
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

