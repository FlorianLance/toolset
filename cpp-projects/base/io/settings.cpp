

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

#include "settings.hpp"


// local
#include "utility/io_file.hpp"
#include "utility/io_data.hpp"
#include "utility/string.hpp"
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace std::literals::string_view_literals;
using namespace tool::io;
using namespace tool::data;
using json = nlohmann::json;

auto Settings::save_to_json_binary_file(const std::string &filePath) -> bool{

    Logger::log(std::format("[BaseSettings::save_to_json_binary_file] Save [{}] settings to binary file with path [{}]\n", type_description(), filePath));
    if(!File::write_binary_content(filePath, convert_to_json_binary())){
        Logger::error(std::format("[BaseSettings::save_to_json_binary_file] Cannot save [{}] settings to binary file, impossible to open file with path: [{}]\n", type_description(), filePath));
        return false;
    }
    return true;
}

auto Settings::save_to_json_str_file(const std::string &filePath) -> bool{

    Logger::log(std::format("[BaseSettings::save_to_json_str_file] Save [{}] settings to text file with path [{}]\n", type_description(), filePath));
    if(!File::write_text_content(filePath, convert_to_json_str())){
        Logger::error(std::format("[BaseSettings::save_to_json_str_file] Cannot save [{}] settings to text file, impossible to open file with path: [{}]\n", type_description(), filePath));
        return false;
    }
    return true;
}

auto Settings::load_from_json_binary_file(const std::string &filePath) -> bool{

    Logger::log(std::format("[BaseSettings::load_from_json_binary_file] Load from [{}] settings binary file with path [{}]\n", type_description(),  filePath));
    if(auto content = File::read_content(filePath); content.has_value()){
        auto data = std::span<const uint8_t>(reinterpret_cast<const std::uint8_t*>(content.value().data()), content.value().size());
        if(init_from_json_binary(data)){
            return true;
        }
        Logger::error(std::format("[BaseSettings::load_from_json_binary_file] Cannot load [{}] settings from binary file, impossible to read file with path: [{}]\n", type_description(), filePath));
        return false;
    }

    Logger::error(std::format("[BaseSettings::load_from_json_binary_file] Cannot load [{}] settings from binary file, impossible to open file with path: [{}]\n", type_description(), filePath));
    return false;
}

auto Settings::load_from_json_str_file(const std::string &filePath) -> bool{

    Logger::log(std::format("[BaseSettings::load_from_json_str_file] Init from [{}] settings text file with path [{}]\n", type_description(),  filePath));
    if(auto content = File::read_content(filePath); content.has_value()){
        if(init_from_json_str(content.value())){
            return true;
        }
        Logger::error(std::format("[BaseSettings::load_from_json_str_file] Cannot load [{}] settings from text file, impossible to parse file with path: [{}]\n", type_description(), filePath));
        return false;
    }

    Logger::error(std::format("[BaseSettings::load_from_json_str_file] Cannot load [{}] settings from text file, impossible to open file with path: [{}]\n", type_description(), filePath));
    return false;
}

auto Settings::convert_to_json_str() const -> std::string{
    return convert_to_json().dump(4);
}

auto Settings::convert_to_json_binary() const -> std::vector<std::uint8_t>{
    return json::to_bson(convert_to_json());
}

auto Settings::init_from_json_binary(std::span<const std::uint8_t> jsonData) -> bool{
    try {
        init_from_json(json::from_bson(jsonData));
        return true;
    }catch (const json::parse_error& e){
        Logger::error(std::format("BaseSettings::init_from_json_str] Error while reading JSON binary [{}] for settings [{}]\n", e.what(), type_description()));
    }
    return false;
}

auto Settings::init_from_json_str(std::string_view jsonStr) -> bool{
    try {
        init_from_json(json::parse(jsonStr));
        return true;
    }catch (const json::parse_error& e){
        Logger::error(std::format("BaseSettings::init_from_json_str] Error while reading JSON string [{}] for settings [{}]\n", e.what(), type_description()));
    }
    return false;
}

auto Settings::init_from_json(const nlohmann::json &json) -> void{
    size_t unreadCount = 0;
    // sType   = static_cast<tool::io::SettingsType>(read_value<int>(json, unreadCount, "type"));
    version = static_cast<tool::io::SettingsVersion>(read_and_return_value(json, unreadCount, "version"sv, static_cast<int>(version)));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[BaseSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto Settings::convert_to_json() const -> json{
    return json{
        // {"type",        static_cast<int>(sType)},
        {"version",     static_cast<int>(version)},
        {"type_string", settings_name(sType)},
    };
}

auto Settings::load_from_file(const std::string &filePath) -> bool{

    if(filePath.empty()){
        Logger::message(std::format("Cannot open [{}] file, empty path\n", type_description()));
        return false;
    }

    Logger::message(std::format("Open [{}] file with path [{}]\n", type_description(),  filePath));

    if(filePath.contains(".json")){
        return load_from_json_str_file(filePath);
    }else if(filePath.contains(".bson")){
        return load_from_json_binary_file(filePath);
    }else if(filePath.contains(".config")){

        Logger::message(std::format("Read legacy file [{}]\n", type_description()));

        size_t offset = 0;
        if(auto content = File::read_content(filePath); content.has_value()){
            switch(sType){
            case SettingsType::Device:
                init_from_data(reinterpret_cast<std::byte*>(content.value().data()), offset, content.value().size());
                return true;
            case SettingsType::Color:
                init_from_data(reinterpret_cast<std::byte*>(content.value().data()), offset, content.value().size());
                return true;
            case SettingsType::Filters:
                init_from_data(reinterpret_cast<std::byte*>(content.value().data()), offset, content.value().size());
                return true;
            case SettingsType::Model:{
                std::string_view contentV = content.value();
                init_from_text(contentV);
                return true;
            }break;
            case SettingsType::Udp_server:{
                std::string_view contentV = content.value();
                init_from_text(contentV);
                return true;
            }break;
            case SettingsType::Dc_client_connection:{
                std::string_view contentV = content.value();
                init_from_text(contentV);
                return true;
            }break;
            default:
                return false;
            }
        }
    }
    Logger::error("[BaseSettings::init_from_file] Cannot read file.\n");

    return false;
}

auto Settings::init_from_text(std::string_view &text) -> void{

    if(text.starts_with("Type: "sv)){
        if(auto values = String::split_view(text, " "sv); values.size() == 2){
            sType = static_cast<SettingsType>(String::to_int(values[1]));
        }
        text = String::advance_view_to_delim(text, "\n"sv);
    }
    if(text.starts_with("Version: "sv)){
        if(auto values = String::split_view(text, " "sv); values.size() == 2){
            version = static_cast<SettingsVersion>(String::to_int(values[1]));
        }
        text = String::advance_view_to_delim(text, "\n"sv);
    }
}

auto Settings::init_from_data(const std::byte * const data, size_t &offset, size_t sizeData) -> void{
    read(sType, data, offset, sizeData);
    read(version, data, offset, sizeData);
}


auto Settings::load_multi_from_json_txt_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool{

    if(settingsA.empty()){
        Logger::error("Input multi settings array is empty.\n");
        return false;
    }

    // read  content
    auto content = File::read_content(filePath);
    if(!content.has_value()){
        Logger::error(std::format("Cannot read file with path [{}].\n", filePath));
        return false;
    }
    if(content.value().length() == 0){
        Logger::error(std::format("File with path [{}] is empty.\n", filePath));
        return false;
    }

    std::string_view contentV = content.value();

    auto jData = json::parse(contentV);
    if(jData.contains("settings"sv)){
        json::array_t arr = jData["settings"sv];
        if(arr.size() == settingsA.size()){
            for(size_t idS = 0; idS < arr.size(); ++idS){
                settingsA[idS]->init_from_json(arr[idS]);
            }
        }else{
            return false;
        }
    }else{
        return false;
    }

    // ...

    return true;
}

auto Settings::save_multi_to_json_str_file(std::span<Settings *> settingsA, const std::string &filePath) -> bool{

    auto firstT = settingsA.front()->type();
    for(const auto &settings : settingsA){
        if(settings->type() != firstT){
            Logger::error("[BaseSettings::save_multi_to_json_str_file] ...");
            return false;
        }
    }

    json json;
    json::array_t arr;
    for(const auto &settings : settingsA){
        arr.push_back(settings->convert_to_json());
    }
    json["settings"sv] = arr;

    Logger::log(std::format("Save {}] multi settings to text file with path [{}]\n", settingsA.front()->type_description(), filePath));
    if(!File::write_text_content(filePath, json.dump(4))){
        Logger::error(std::format("[BaseSettings::save_multi_to_json_str_file] Cannot save [{}] multi settings to json str file, impossible to open file with path: [{}]\n", settingsA.front()->type_description(), filePath));
        return false;
    }
    return true;
}

auto Settings::load_multi_from_file(std::span<Settings *> settingsA, const std::string &filePath) -> bool{

    auto firstT = settingsA.front()->type();
    for(const auto &settings : settingsA){
        if(settings->type() != firstT){
            Logger::error("[BaseSettings::load_multi_from_file] ...");
            return false;
        }
    }

    Logger::message(std::format("Open multi [{}] file with path [{}]\n", settingsA.front()->type_description(),  filePath));

    if(filePath.contains(".json")){
        return load_multi_from_json_txt_file(settingsA, filePath);
    }else if(filePath.contains(".bson")){
        // return load_from_json_binary_file(filePath);
    }else if(filePath.contains(".config")){

        Logger::message(std::format("Read legacy file [{}]\n", settingsA.front()->type_description()));
        switch(firstT){
        case SettingsType::Device:
            return init_from_binary_file(settingsA, filePath);
        case SettingsType::Color:
            return init_from_binary_file(settingsA, filePath);
        case SettingsType::Filters:
            return init_from_binary_file(settingsA, filePath);
        case SettingsType::Model:{
            return init_from_text_file(settingsA, filePath);
        }break;
        case SettingsType::Udp_server:{
            return init_from_text_file(settingsA, filePath);
        }break;
        case SettingsType::Dc_client_connection:{
            return init_from_text_file(settingsA, filePath);
        }break;
        default:
            return false;
        }

    }
    Logger::error("[BaseSettings::load_multi_from_file] Cannot read file.\n");
    return false;
}


auto Settings::init_from_text_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool{

    if(settingsA.empty()){
        Logger::error("Input multi settings arra  is empty.\n");
        return false;
    }

    // read  content
    auto content = File::read_content(filePath);
    if(!content.has_value()){
        Logger::error(std::format("Cannot read file with path [{}].\n", filePath));
        return false;
    }
    if(content.value().length() == 0){
        Logger::error(std::format("File with path [{}] is empty.\n", filePath));
        return false;
    }

    std::string_view contentV = content.value();

    auto contents = String::split_view(contentV, "%%%");
    if(contents.size() <= 1){
        Logger::error("Invalid content for multi settings file.\n");
        return false;
    }

    size_t nbParts = String::to_int(contents[0]);
    if(settingsA.size() != nbParts){
        Logger::error(std::format("Invalid number of parts ([{}] instead of [{}]) for multi settings file.\n", nbParts, settingsA.size()));
        return false;
    }

    for(size_t ii = 0; ii < nbParts; ++ii){
        settingsA[ii]->init_from_text(contents[ii+1]);
    }

    return true;
}

auto Settings::init_from_binary_file(std::span<Settings*> settingsA, const std::string &filePath) -> bool{

    if(settingsA.empty()){
        Logger::error("Input multi settings array is empty.\n");
        return false;
    }

    // read  content
    auto content = File::read_content(filePath);
    if(!content.has_value()){
        Logger::error(std::format("Cannot read file with path [{}].\n", filePath));
        return false;
    }
    if(content.value().length() == 0){
        Logger::error(std::format("File with path [{}] is empty.\n", filePath));
        return false;
    }

    // read nb of elements
    auto data = reinterpret_cast<std::byte*>(content.value().data());
    std::int16_t nbParts = 0;
    size_t offset = 0;
    size_t sizeData = content.value().size();
    read(nbParts, data, offset, sizeData);

    if(nbParts == 0 || (settingsA.size() != nbParts)){
        Logger::error(std::format("Invalid number of parts ([{}] instead of [{}]) for multi settings file.\n", nbParts, settingsA.size()));
        return false;
    }

    // read data per element
    for(auto settings : settingsA){
        settings->init_from_data(data, offset, sizeData);
    }

    return true;
}


