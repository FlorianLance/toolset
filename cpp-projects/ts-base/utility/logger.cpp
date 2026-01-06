
/*******************************************************************************
** Toolset-ts-base                                                            **
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

#include "logger.hpp"

// std
#include <mutex>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <format>
#include <unordered_map>

// local
#include "utility/time.hpp"

using namespace tool;
using namespace std::string_view_literals;

namespace fs = std::filesystem;

struct Logger::Impl{

    fs::path logDirPath;
    fs::path logName;
    fs::path absoluteFilePath;

    std::mutex locker;
    std::unique_ptr<std::ofstream> outStream = nullptr;
    std::chrono::nanoseconds epochStart;

    std::unordered_map<MessageType, Config> mTypeConfig = {
        {MessageType::normal,   Config{true,  true, false, false, true,  false, "DarkBlue"}},
        {MessageType::error,    Config{true,  true, true,  false, true,  true,  "Orange"}},
        {MessageType::warning,  Config{true,  true, true,  false, true,  true,  "DarkRed"}},
        {MessageType::log,      Config{false, false,true,  false, true,  true,  "LightGrey"}},
        {MessageType::unknow,   Config{false, false,false, false, false, false, "Black"}},
    };
};

auto Logger::config(MessageType mType) const -> Logger::Config{
    return i->mTypeConfig[mType];
}

auto Logger::config(MessageType mType) -> Logger::Config&{
    return i->mTypeConfig[mType];
}

Logger::Logger() : i(std::make_unique<tool::Logger::Impl>()){
}

Logger::~Logger(){
    Logger::clean();
}

auto Logger::init(std::string_view logDirectoryPath, std::string_view logFileName) -> bool{
    if(logDirectoryPath.length() == 0 || logFileName.length() == 0){
        std::cerr << "[Log::init] Empty path or filename." << std::endl;
        return false;
    }

    i->logDirPath       = logDirectoryPath;
    i->logName          = logFileName;
    i->absoluteFilePath = i->logDirPath / i->logName;

    if(!fs::exists(i->logDirPath)){
        if(!fs::create_directory(i->logDirPath)){
            std::cerr << "[Log::init] Cannot create logging directory path." << std::endl;
            return false;
        }
    }

    std::unique_lock<std::mutex> lock(i->locker);
    if(fs::exists(i->absoluteFilePath)){

        auto extension   = i->absoluteFilePath.extension();
        auto newFileName = i->absoluteFilePath.filename().replace_extension("");
        newFileName.concat("_previous").concat(extension.string());

        auto previousPath = i->absoluteFilePath;
        previousPath.replace_filename(newFileName);

        bool copyPrevious = true;
        if(fs::exists(previousPath)){
            if(!fs::remove(previousPath)){
                std::cerr << "[Log::init] Cannot remove previous log file." << std::endl;
                copyPrevious = false;
            }
        }

        if(copyPrevious){
            fs::copy(i->absoluteFilePath, previousPath);
        }
    }

    // init log file
    i->epochStart   = Time::nanoseconds_since_epoch();
    i->outStream    = std::make_unique<std::ofstream>();

    i->outStream->open(i->absoluteFilePath);
    if(!i->outStream->is_open()){
        std::cerr << "[Log::init] Cannot write to log file: " << i->absoluteFilePath  << std::endl;
        i->outStream = nullptr;
        return false;
    }

    return true;
}

auto Logger::nofile_init() -> void{
    i->outStream   = nullptr;
    i->epochStart  = Time::nanoseconds_since_epoch();
}

auto Logger::clean() -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->outStream){
        (*i->outStream) << std::flush;
        i->outStream->close();
        i->outStream = nullptr;
    }
}

auto Logger::message(std::string_view message) -> void{

    const auto &conf = i->mTypeConfig[MessageType::normal];
    if(conf.displayToConsole){
        std::cout << message;
    }

    if(conf.saveToFile && !conf.formatHTMLForFile){
        insert_to_log_file(message, false);
    }

    if((conf.saveToFile && conf.formatHTMLForFile) || (conf.triggerSignals && conf.formatHTMLForSignals)){
        auto htmlMessage = to_html_paragraph(conf.htmlColor, message, conf.addTimestampToHtml);
        if(conf.saveToFile && conf.formatHTMLForFile){
            insert_to_log_file(htmlMessage, false);
        }
        if(conf.triggerSignals && conf.formatHTMLForSignals){
            message_signal(std::move(htmlMessage));
        }
    }

    if(conf.triggerSignals && !conf.formatHTMLForSignals){
        message_signal(std::string(message));
    }
}

auto Logger::message_unf(std::string_view message) -> void{
    const auto &conf = i->mTypeConfig[MessageType::normal];
    if(conf.displayToConsole){
        std::cout << message;
    }

    if(conf.saveToFile){
        insert_to_log_file(message, false);
    }

    if(conf.triggerSignals){
        message_signal(std::string(message));
    }
}

auto Logger::warning(std::string_view warning) -> void{

    const auto &conf = i->mTypeConfig[MessageType::warning];
    if(conf.displayToConsole){
        std::cerr << warning;
    }

    if(conf.saveToFile && !conf.formatHTMLForFile){
        insert_to_log_file(warning, true);
    }

    if((conf.saveToFile && conf.formatHTMLForFile) || (conf.triggerSignals && conf.formatHTMLForSignals)){
        auto htmlWarning = to_html_paragraph(conf.htmlColor, warning, conf.addTimestampToHtml);
        if(conf.saveToFile && conf.formatHTMLForFile){
            insert_to_log_file(htmlWarning, true);
        }
        if(conf.triggerSignals && conf.formatHTMLForSignals){
            warning_signal(std::move(htmlWarning));
        }
    }

    if(conf.triggerSignals && !conf.formatHTMLForSignals){
        warning_signal(std::string(warning));
    }
}

auto Logger::error(std::string_view error) -> void{

    const auto &conf = i->mTypeConfig[MessageType::error];
    if(conf.displayToConsole){
        std::cerr << error;
    }

    if(conf.saveToFile && !conf.formatHTMLForFile){
        insert_to_log_file(error, true);
    }

    if((conf.saveToFile && conf.formatHTMLForFile) || (conf.triggerSignals && conf.formatHTMLForSignals)){
        auto htmlError = to_html_paragraph(conf.htmlColor, error, conf.addTimestampToHtml);
        if(conf.saveToFile && conf.formatHTMLForFile){
            insert_to_log_file(htmlError, true);
        }
        if(conf.triggerSignals && conf.formatHTMLForSignals){
            error_signal(std::move(htmlError));
        }
    }

    if(conf.triggerSignals && !conf.formatHTMLForSignals){
        error_signal(std::string(error));
    }
}

auto Logger::log(std::string_view log) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];
    if(conf.displayToConsole){
        std::cout << log;
    }

    if(conf.saveToFile && !conf.formatHTMLForFile){
        insert_to_log_file(log, true);
    }

    if((conf.saveToFile && conf.formatHTMLForFile) || (conf.triggerSignals && conf.formatHTMLForSignals)){
        auto htmlLog = to_html_paragraph(conf.htmlColor, log, conf.addTimestampToHtml);
        if(conf.saveToFile && conf.formatHTMLForFile){
            insert_to_log_file(htmlLog, true);
        }
        if(conf.triggerSignals && conf.formatHTMLForSignals){
            log_signal(std::move(htmlLog));
        }
    }

    if(conf.triggerSignals && !conf.formatHTMLForSignals){
        log_signal(std::string(log));
    }
}

auto Logger::log_unf(std::string_view log) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];
    if(conf.displayToConsole){
        std::cout << log;
    }
    if(conf.saveToFile){
        insert_to_log_file(log, true);
    }

    if(conf.triggerSignals){
        log_signal(std::string(log));
    }
}

auto Logger::log_title(std::string_view log, int level) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];

    if((conf.saveToFile && conf.formatHTMLForFile) || (conf.triggerSignals && conf.formatHTMLForSignals)){
        auto htmlLog = to_html_paragraph(conf.htmlColor, log, conf.addTimestampToHtml);
        if(conf.saveToFile && conf.formatHTMLForFile){
            insert_to_log_file(htmlLog, false);
        }
        if(conf.triggerSignals && conf.formatHTMLForSignals){
            log_signal(std::move(htmlLog));
        }
    }

}

auto Logger::to_html_paragraph(std::string_view colorCode, std::string_view text, bool addTimestamp) -> std::string{

    if(!addTimestamp){
        return std::format("<p><font color=\"{}\">{}</font></p>\n"sv, colorCode, text);
    }else{
        auto diff = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - i->epochStart).count());
        return std::format("<p> [{}] <font color=\"{}\">{}</font></p>\n"sv, diff, colorCode, text);
    }
}

auto Logger::insert_to_log_file(std::string_view message, bool flush) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->outStream){
        if(flush){
            (*i->outStream) << message << std::flush;
        }else{
            (*i->outStream) << message;
        }
    }
}



