
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

#include "logger.hpp"

// std
#include <mutex>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <format>

// local
#include "utility/time.hpp"

using namespace tool;
using namespace std::string_view_literals;

namespace fs = std::filesystem;

struct Global{
    static inline std::unique_ptr<Logger,LoggerCleaner> instance = nullptr;
    static inline Logger *instancePtr = nullptr;
};

class Logger::Impl{
public:    

    std::unique_ptr<std::ofstream> outStream = nullptr;
    std::mutex locker;
    std::chrono::nanoseconds epochStart;
    bool doFormat = false;

    static inline constexpr std::string_view braces4                   = "{}{}{}{}"sv;
    static inline constexpr std::string_view braces6                   = "{}{}{}{}{}{}"sv;
    static inline constexpr std::string_view startHtmlBalise           = "<p>"sv;
    static inline constexpr std::string_view startTimestampHtmlBalise  = "<p> ["sv;
    static inline constexpr std::string_view midTimestampHtmlBalise    = "] "sv;
    static inline constexpr std::string_view endHtmlBalise             = "</font></p>\n"sv;
    static inline constexpr std::string_view htmlDarkBlueColorBalise   = "<font color=\"DarkBlue\">"sv;
    static inline constexpr std::string_view htmlDarkOrangeColorBalise = "<font color=\"Orange\">"sv;
    static inline constexpr std::string_view htmlDarkRedColorBalise    = "<font color=\"DarkRed\">"sv;
    static inline constexpr std::string_view htmlDarkGreenColorBalise  = "<font color=\"DarkGreen\">"sv;
    static inline constexpr std::string_view htmlLightGreyColorBalise  = "<font color=\"LightGrey\">"sv;
    static inline constexpr std::string_view htmlDarkBlackColorBalise  = "<font color=\"Black\">"sv;
};

Logger::Logger() : i(std::make_unique<tool::Logger::Impl>()){}

Logger::~Logger(){}

auto Logger::get() -> Logger*{
    if(!Global::instance){
        no_file_init();
    }
    return Global::instancePtr;
}

auto Logger::no_file_init(bool doFormat) -> void{

    if(Global::instance == nullptr){
        Global::instance    = std::unique_ptr<Logger,LoggerCleaner>(new Logger(), LoggerCleaner());
        Global::instancePtr = Global::instance.get();
    }

    Global::instancePtr->i->outStream   = nullptr;
    Global::instancePtr->i->doFormat    = doFormat;
    Global::instancePtr->i->epochStart  = Time::nanoseconds_since_epoch();
}

auto Logger::init(std::string_view logDirectoryPath, std::string_view logFileName, bool doFormat) -> bool{

    if(logDirectoryPath.length() == 0 || logFileName.length() == 0){
        std::cerr << "[LOGGER-ERROR] Empty path or filename." << std::endl;
        return false;
    }

    fs::path logDirPath       = logDirectoryPath;
    fs::path logName          = logFileName;
    fs::path absoluteFilePath = logDirPath / logName;

    if(!fs::exists(logDirPath)){
        if(!fs::create_directory(logDirPath)){
            std::cerr << "[LOGGER-ERROR] Cannot create logging directory path." << std::endl;
            return false;
        }
    }

    if(Global::instance == nullptr){

        Global::instance = std::unique_ptr<Logger,LoggerCleaner>(new Logger(), LoggerCleaner());
        Global::instance->i->doFormat = doFormat;

        std::unique_lock<std::mutex> lock(Global::instance->i->locker);
        if(fs::exists(absoluteFilePath)){

            auto extension = absoluteFilePath.extension();
            auto newFileName = absoluteFilePath.filename().replace_extension("");
            newFileName.concat("_previous").concat(extension.string());

            auto previousPath = absoluteFilePath;
            previousPath.replace_filename(newFileName);

            bool copyPrevious = true;
            if(fs::exists(previousPath)){
                if(!fs::remove(previousPath)){
                    std::cerr << "[LOGGER-ERROR] Cannot remove previous log file." << std::endl;
                    copyPrevious = false;
                }
            }

            if(copyPrevious){
                fs::copy(absoluteFilePath, previousPath);
            }
        }

        // init log file
        Global::instancePtr = Global::instance.get();
        Global::instancePtr->i->epochStart   = Time::nanoseconds_since_epoch();
        Global::instancePtr->i->outStream    = std::make_unique<std::ofstream>();

        Global::instancePtr->i->outStream->open(absoluteFilePath);
        if(!Global::instancePtr->i->outStream->is_open()){
            std::cerr << "[LOGGER-ERROR] Cannot write to log file: " << absoluteFilePath  << std::endl;
            Global::instancePtr->i->outStream    = nullptr;
            return false;
        }
    }

    return true;
}

auto Logger::take_ownership() -> std::unique_ptr<Logger, LoggerCleaner>{
    std::unique_ptr<Logger,LoggerCleaner> rLogger = nullptr;
    std::swap(rLogger, Global::instance);
    return rLogger;
}

auto Logger::set_logger_ptr(Logger *logger) -> void {
    Global::instancePtr = logger;
}

auto Logger::message(std::string_view message, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        std::cout << message;
        return;
    }

    if(triggersSignal){
        trigger_message(message, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::normal, message);
    }
}

auto Logger::warning(std::string_view warning, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        std::cerr << warning << std::flush;
        return;
    }

    if(triggersSignal){
        trigger_warning(warning, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::warning, warning);
    }
}

auto Logger::log(std::string_view log, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        return;
    }

    if(triggersSignal){
        trigger_log(log, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::log, log);
    }
}

auto Logger::error(std::string_view error, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        std::cerr << error << std::flush;
        return;
    }

    if(triggersSignal){
        trigger_error(error, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::error, error);
    }
}

auto Logger::message_id(std::string_view message, SenderT sType, int sKey, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        std::cout << message;
        return;
    }

    if(triggersSignal){
        trigger_message_id(message, sType, sKey, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::normal, message);
    }
}


auto Logger::warning_id(std::string_view warning, SenderT sType, int sKey, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        std::cerr << warning << std::flush;
        return;
    }

    if(triggersSignal){
        trigger_warning_id(warning, sType, sKey, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::warning, warning);
    }
}


auto Logger::error_id(std::string_view error, SenderT sType, int sKey, bool htmlFormat, bool triggersSignal, bool saveToFile) -> void {

    if(Global::instancePtr == nullptr){
        std::cerr << error << std::flush;
        return;
    }

    if(triggersSignal){
        trigger_error_id(error, sType, sKey, htmlFormat && Global::instancePtr->i->doFormat);
    }

    if(saveToFile && (Global::instancePtr->i->outStream != nullptr)){
        insert_line_to_log_file(MessageT::error, error);
    }
}


auto Logger::trigger_message(std::string_view message, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->message_signal(to_html_line(MessageT::normal,message));
    }else{
        Global::instancePtr->message_signal(std::string(message));
    }
}

auto Logger::trigger_warning(std::string_view warning, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->warning_signal(to_html_line(MessageT::warning, warning));
    }else{
        Global::instancePtr->warning_signal(std::string(warning));
    }
}

auto Logger::trigger_log(std::string_view log, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->log_signal(to_html_line(MessageT::log, log));
    }else{
        Global::instancePtr->log_signal(std::string(log));
    }
}

auto Logger::trigger_error(std::string_view error, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->error_signal(to_html_line(MessageT::error, error));
    }else{
        Global::instancePtr->error_signal(std::string(error));
    }
}

auto Logger::trigger_message_id(std::string_view message, SenderT sType, int sKey, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->message_id_signal(to_html_line(MessageT::normal,message), sType, sKey);
    }else{
        Global::instancePtr->message_id_signal(std::string(message), sType, sKey);
    }
}

auto Logger::trigger_warning_id(std::string_view warning, SenderT sType, int sKey, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->warning_id_signal(to_html_line(MessageT::warning, warning), sType, sKey);
    }else{
        Global::instancePtr->warning_id_signal(std::string(warning), sType, sKey);
    }
}

auto Logger::trigger_error_id(std::string_view error, SenderT sType, int sKey, bool htmlFormat) -> void {
    if(htmlFormat){
        Global::instancePtr->error_id_signal(to_html_line(MessageT::error, error), sType, sKey);
    }else{
        Global::instancePtr->error_id_signal(std::string(error), sType, sKey);
    }
}

auto Logger::status(std::string_view status, int ms) -> void {
    Global::instancePtr->status_signal(std::string(status), ms);
}

auto Logger::progress(int state) -> void {
    Global::instancePtr->progress_signal(state);
}

auto Logger::insert_line_to_log_file(MessageT type, std::string_view message) -> void {

    std::unique_lock<std::mutex> lock(Global::instancePtr->i->locker);
    if(Global::instancePtr->i->outStream){
        (*Global::instancePtr->i->outStream) << to_html_line(type, message, true) << std::flush;
    }
}

auto Logger::to_html_line(MessageT type, std::string_view text, bool addTimestamp) -> std::string {

    std::string_view colorCode;
    switch (type) {
    case MessageT::normal:
        colorCode = Logger::Impl::htmlDarkBlueColorBalise;
        break;
    case MessageT::warning:
        colorCode = Logger::Impl::htmlDarkOrangeColorBalise;
        break;
    case MessageT::error:
        colorCode = Logger::Impl::htmlDarkRedColorBalise;
        break;
    case MessageT::log:
        colorCode = Logger::Impl::htmlDarkGreenColorBalise;
        break;
    case MessageT::unknow:
        colorCode = Logger::Impl::htmlDarkBlackColorBalise;
        break;
    }    

    if(!addTimestamp){
        return std::format(Logger::Impl::braces4, Logger::Impl::startHtmlBalise, colorCode, text, Logger::Impl::endHtmlBalise);
    }else{
        auto diff = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - Global::instancePtr->i->epochStart).count());
        return std::format(Logger::Impl::braces6, Logger::Impl::startTimestampHtmlBalise, diff, Logger::Impl::midTimestampHtmlBalise, colorCode, text, Logger::Impl::endHtmlBalise);
    }
}

auto Logger::clean() -> void {

    std::unique_lock<std::mutex> lock(Global::instancePtr->i->locker);
    if(Global::instancePtr->i->outStream){
        Global::instancePtr->i->outStream->close();
        Global::instancePtr->i->outStream = nullptr;
    }
    Global::instancePtr = nullptr;
}

auto LoggerCleaner::operator()(Logger *logger) -> void {
    logger->clean();
    delete logger;
}
