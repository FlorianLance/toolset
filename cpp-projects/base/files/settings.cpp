

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

using namespace tool::files;

auto BaseSettings::init_from_binary_file(const std::string &filePath) -> bool{
    Logger::message(std::format("Open [{}] binary file with path [{}]\n", file_description(),  filePath));
    if(auto content = File::read_content(filePath); content.has_value()){        

        if(content->size() != total_data_size()){
            Logger::warning(std::format("Input [{}] file with path [{}] of size [{}] different from expected [{}]\n",
            file_description(), filePath, content->size(), total_data_size()));
        }
        m_inputFileSize = content.value().size();
        init_from_data(reinterpret_cast<std::int8_t*>(content.value().data()));
    }else{
        Logger::error(std::format("Cannot open [{}] file with path: [{}]\n", file_description(), filePath));
        return false;
    }

    return true;
}

auto BaseSettings::init_from_text_file(const std::string &filePath) -> bool{
    Logger::message(std::format("Open [{}] text file with path [{}]\n", file_description(),  filePath));
    if(auto content = File::read_content(filePath); content.has_value()){
        init_from_text(content.value());
    }else{
        Logger::error(std::format("Cannot open [{}] file with path: [{}]\n", file_description(), filePath));
        return false;
    }

    return true;
}

auto BaseSettings::save_to_text_file(const std::string &filePath) const -> bool{
    Logger::message(std::format("Save [{}] to text file with path [{}]\n", file_description(),  filePath));
    if(!File::write_text_content(filePath, convert_to_text())){
        Logger::error(std::format("Cannot save [{}] file with path: [{}]\n", file_description(), filePath));
        return false;
    }
    return true;
}

auto BaseSettings::save_to_binary_file(const std::string &filePath) const -> bool{
    Logger::message(std::format("Save [{}] to binary file with path [{}]\n", file_description(),  filePath));
    std::vector<std::int8_t> content;
    content.resize(total_data_size());
    convert_to_data(content.data());
    if(!File::write_binary_content(filePath, content)){
        Logger::error(std::format("Cannot save [{}] file with path: [{}]\n", file_description(), filePath));
        return false;
    }
    return true;
}

auto BaseSettings::save_to_text_file(const std::vector<BaseSettings*> &settingsA, const std::string &filePath) -> bool{

    if(settingsA.size() == 0){
        Logger::error("Input multi settings array  is empty.\n");
        return false;
    }
    std::string content;
    std::vector<std::string> contents;
    for(const auto settings : settingsA){
        contents.push_back(settings->convert_to_text());
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

    auto contents = String::split(content.value(), "%%%");
    if(contents.size() <= 1){
        Logger::error("Invalid content for multi settings file.\n");
        return false;
    }

    size_t nbParts = std::stoi(contents[0]);
    if(settingsA.size() != nbParts){
        Logger::error(std::format("Invalid number of parts ([{}] instead of [{}]) for multi settings file.\n", nbParts, settingsA.size()));
        return false;
    }

    for(size_t ii = 0; ii < nbParts; ++ii){
        settingsA[ii]->init_from_text(contents[ii+1]);
    }

    return true;
}

#include <iostream>

auto BaseSettings::save_to_binary_file(const std::vector<BaseSettings *> &settingsA, const std::string &filePath) -> bool{

    if(settingsA.size() == 0){
        Logger::error("Input multi settings array is empty.\n");
        return false;
    }

    std::vector<std::int8_t> content;
    size_t totalDataSize = 0;
    for(const auto settings : settingsA){
        totalDataSize += settings->total_data_size();
    }

    content.resize(totalDataSize + sizeof(std::int16_t) + settingsA.size() * sizeof(size_t));
    auto data = content.data();

    size_t offset = 0;
    std::int16_t nbParts = static_cast<std::int16_t>(settingsA.size());
    write(nbParts, data, offset);

    for(const auto settings : settingsA){
        settings->convert_to_data(content.data() + offset);
        offset += settings->total_data_size();
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
    read(nbParts, data, offset);

    if(nbParts == 0 || (settingsA.size() != nbParts)){
        Logger::error(std::format("Invalid number of parts ([{}] instead of [{}]) for multi settings file.\n", nbParts, settingsA.size()));
        return false;
    }

    // read data per element
    for(auto settings : settingsA){
        settings->init_from_data(data + offset);
        offset += settings->total_data_size();
    }

    return true;
}

