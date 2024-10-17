

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

#include "log.hpp"

// std
#include <fstream>
#include <filesystem>
#include <iostream>
#include <unordered_map>

// local
#include "utility/time.hpp"

using namespace tool;
using namespace std::string_view_literals;

namespace fs = std::filesystem;

struct Log::Impl{

    fs::path logDirPath;
    fs::path logName;
    fs::path absoluteFilePath;

    std::mutex locker;
    std::unique_ptr<std::ofstream> outStream = nullptr;
    std::chrono::nanoseconds epochStart;

    struct Config{
        bool displayToConsole;
        bool triggerSignals;
        bool formatToHtml;
        bool saveToFile;
        bool addTimestampToHtml;
        std::string color;
    };

    std::unordered_map<MessageType, Config> mTypeConfig = {
        {MessageType::normal,   Config{true,  true, true, false, false, "DarkBlue"}},
        {MessageType::error,    Config{true,  true, true, true,  true,  "Orange"}},
        {MessageType::warning,  Config{true,  true, true, true,  true,  "DarkRed"}},
        {MessageType::log,      Config{false, false,true, true,  true,  "LightGrey"}},
        {MessageType::unknow,   Config{false, false,false,false, false, "Black"}},
    };
};

Log::Log() : i(std::make_unique<tool::Log::Impl>()){}

Log::~Log(){
    clean();
}

auto Log::init(std::string_view logDirectoryPath, std::string_view logFileName) -> bool{

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

auto Log::nofile_init() -> void {
    i->outStream   = nullptr;
    i->epochStart  = Time::nanoseconds_since_epoch();
}

auto Log::clean() -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->outStream){
        (*i->outStream) << std::flush;
        i->outStream->close();
        i->outStream = nullptr;
    }
}

auto Log::message(std::string_view message) -> void{

    const auto &conf = i->mTypeConfig[MessageType::normal];
    if(conf.displayToConsole){
        std::cout << message;
    }

    if(conf.saveToFile || conf.triggerSignals){

        if(conf.formatToHtml){
            auto htmlMessage = to_html_paragraph(conf.color, message, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlMessage, false);
            }
            if(conf.triggerSignals){
                message_signal(std::move(htmlMessage));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(message, false);
            }
            if(conf.triggerSignals){
                message_signal(std::string(message));
            }
        }
    }
}

auto Log::warning(std::string_view warning) -> void{

    const auto &conf = i->mTypeConfig[MessageType::warning];
    if(conf.displayToConsole){
        std::cerr << warning;
    }

    if(conf.saveToFile || conf.triggerSignals){

        if(conf.formatToHtml){
            auto htmlWarning = to_html_paragraph(conf.color, warning, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlWarning, false);
            }
            if(conf.triggerSignals){
                warning_signal(std::move(htmlWarning));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(warning, false);
            }
            if(conf.triggerSignals){
                warning_signal(std::string(warning));
            }
        }
    }
}

auto Log::error(std::string_view error) -> void{

    const auto &conf = i->mTypeConfig[MessageType::warning];
    if(conf.displayToConsole){
        std::cerr << error;
    }

    if(conf.saveToFile || conf.triggerSignals){

        if(conf.formatToHtml){
            auto htmlError = to_html_paragraph(conf.color, error, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlError, false);
            }
            if(conf.triggerSignals){
                error_signal(std::move(htmlError));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(error, false);
            }
            if(conf.triggerSignals){
                error_signal(std::string(error));
            }
        }
    }
}

auto Log::log(std::string_view log) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];

    if(conf.saveToFile || conf.triggerSignals){
        if(conf.formatToHtml){
            auto htmlLog = to_html_paragraph(conf.color, log, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlLog);
            }
            if(conf.triggerSignals){
                log_signal(std::move(htmlLog));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(log);
            }
            if(conf.triggerSignals){
                log_signal(std::string(log));
            }
        }
    }
}

auto Log::log_title(std::string_view log, int level) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];
    if(conf.saveToFile || conf.triggerSignals){

        auto htmlLog = std::format("<h{}>{}</h{}>"sv, level, to_html_paragraph(conf.color, log, false), level);
        if(conf.saveToFile){
            insert_to_log_file(htmlLog);
        }
        if(conf.triggerSignals){
            log_signal(std::move(htmlLog));
        }
    }
}

auto Log::to_html_paragraph(std::string_view colorCode, std::string_view text, bool addTimestamp) -> std::string{

    if(!addTimestamp){
        return std::format("<p><font color=\"{}\">{}</font></p>\n"sv, colorCode, text);
    }else{
        auto diff = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - i->epochStart).count());
        return std::format("<p> [{}] {}{}</font></p>\n"sv, diff, colorCode, text);
    }
}

auto Log::insert_to_log_file(std::string_view message, bool flush) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->outStream){
        if(flush){
            (*i->outStream) << message << std::flush;
        }else{
            (*i->outStream) << message;
        }
    }
}
