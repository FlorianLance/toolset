

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

// std
#include <filesystem>
#include <fstream>

// local
#include "utility/io_file.hpp"
#include "utility/io_data.hpp"
#include "utility/string.hpp"
#include "utility/logger.hpp"

using namespace std::literals::string_view_literals;
using namespace tool::io;

auto BaseSettings::init_from_binary_file(const std::string &filePath) -> bool{
    Logger::message(std::format("Open [{}] binary file with path [{}]\n", type_description(),  filePath));
    if(auto content = File::read_content(filePath); content.has_value()){        

        if(content->size() != total_data_size()){
            Logger::error(std::format("Input [{}] file with path [{}] of size [{}] different from expected [{}]\n",
            type_description(), filePath, content->size(), total_data_size()));
            return false;
        }
        size_t offset = 0;
        init_from_data(reinterpret_cast<std::int8_t*>(content.value().data()), offset, content.value().size());
    }else{
        Logger::error(std::format("Cannot open [{}] file with path: [{}]\n", type_description(), filePath));
        return false;
    }

    return true;
}

auto BaseSettings::init_from_text_file(const std::string &filePath) -> bool{
    Logger::message(std::format("Open [{}] text file with path [{}]\n", type_description(),  filePath));
    if(auto content = File::read_content(filePath); content.has_value()){
        std::string_view contentV = content.value();
        init_from_text(contentV);
    }else{
        Logger::error(std::format("Cannot open [{}] file with path: [{}]\n", type_description(), filePath));
        return false;
    }

    return true;
}

auto BaseSettings::save_to_text_file(const std::string &filePath) const -> bool{
    Logger::message(std::format("Save [{}] to text file with path [{}]\n", type_description(),  filePath));
    if(!File::write_text_content(filePath, write_to_text())){
        Logger::error(std::format("Cannot save [{}] file with path: [{}]\n", type_description(), filePath));
        return false;
    }
    return true;
}

#include <iostream>

auto BaseSettings::save_to_binary_file(const std::string &filePath) const -> bool{
    Logger::message(std::format("Save [{}] to binary file with path [{}]\n", type_description(),  filePath));
    std::vector<std::int8_t> content;
    content.resize(total_data_size());

    std::cout <<"BIN: " << total_data_size() << "\n";
    size_t offset = 0;
    write_to_data(content.data(), offset, content.size());
    if(!File::write_binary_content(filePath, content)){
        Logger::error(std::format("Cannot save [{}] file with path: [{}]\n", type_description(), filePath));
        return false;
    }
    return true;
}

auto BaseSettings::init_from_text(std::string_view &text) -> void{

    if(text.starts_with("Type: "sv)){
        if(auto values = String::split_view(text, " "sv); values.size() == 2){
            sType = static_cast<SettingsType>(String::to_int(values[1]));
        }
        text = String::advance_view_to_delim(text, "\n"sv);
    }
    if(text.starts_with("Version: "sv)){
        if(auto values = String::split_view(text, " "sv); values.size() == 2){
            version = static_cast<Version>(String::to_int(values[1]));
        }
        text = String::advance_view_to_delim(text, "\n"sv);
    }
}

auto BaseSettings::write_to_text() const -> std::string{
    return std::format("Type: {}\nVersion: {}\n", static_cast<int>(sType), static_cast<int>(version));
}

auto BaseSettings::init_from_data(const int8_t * const data, size_t &offset, size_t sizeData) -> void{
    // if(offset + total_data_size() > sizeData){
    //     tool::Logger::error(std::format("BaseSettings::init_from_data: Not enought data space for initializing from data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
    //     return;
    // }
    read(sType, data, offset, sizeData);
    read(version, data, offset, sizeData);
}

auto BaseSettings::write_to_data(int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    // std::cout << "B:" << total_data_size() << " "<< offset << " " << sizeData << "\n";
    // if(offset + total_data_size() > sizeData){
    //     tool::Logger::error(std::format("BaseSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
    //     return;
    // }
    write(sType, data, offset, sizeData);
    write(version, data, offset, sizeData);
}

auto BaseSettings::total_data_size() const noexcept -> size_t{
    return
        sizeof(sType) +
        sizeof(version);
}

auto BaseSettings::save_to_text_file(const std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool{

    if(settingsA.size() == 0){
        Logger::error("Input multi settings array  is empty.\n");
        return false;
    }
    std::string content;
    std::vector<std::string> contents;
    for(const auto settings : settingsA){
        contents.push_back(settings->write_to_text());
    }
    if(!File::write_text_content(filePath, std::format("{}%%%{}", contents.size(), String::join(contents, "%%%")))){
        Logger::error(std::format("Cannot write multi settings array to path [{}].\n", filePath));
        return false;
    }
    return true;
}

auto BaseSettings::init_from_text_file(std::vector<BaseSettings *> &settingsA, const std::string &filePath) -> bool{

    if(settingsA.size() == 0){
        Logger::error("Input multi settings array  is empty.\n");
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

auto BaseSettings::save_to_binary_file(const std::vector<BaseSettings *> &settingsA, const std::string &filePath) -> bool{

    if(settingsA.size() == 0){
        Logger::error("Input multi settings array is empty.\n");
        return false;
    }

    std::vector<std::int8_t> content;
    size_t totalDataSize = sizeof(std::int16_t) + settingsA.size() * sizeof(size_t);
    for(const auto settings : settingsA){
        totalDataSize += settings->total_data_size();
    }
    content.resize(totalDataSize);
    auto data = content.data();

    size_t offset = 0;
    std::int16_t nbParts = static_cast<std::int16_t>(settingsA.size());
    write(nbParts, data, offset, totalDataSize);

    for(const auto settings : settingsA){
        settings->write_to_data(data, offset, totalDataSize);
    }

    if(!File::write_binary_content(filePath, content)){
        Logger::error(std::format("Cannot write multi settings array to path [{}].\n", filePath));
        return false;
    }
    return true;
}

auto BaseSettings::init_from_binary_file(std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool{

    if(settingsA.size() == 0){
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
    auto data = reinterpret_cast<std::int8_t*>(content.value().data());
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

