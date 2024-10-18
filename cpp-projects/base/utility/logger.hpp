

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

#pragma once

// std
#include <memory>
#include <string>

// local
#include "base_logger.hpp"

namespace tool {

class Logger2 : public BaseLogger{

public:

    Logger2();
    virtual ~Logger2();

    auto init(std::string_view logDirectoryPath = "", std::string_view logFileName ="default_log.html") -> bool;
    auto nofile_init() -> void override;
    auto clean() -> void override;

    auto message(std::string_view message) -> void override;
    auto warning(std::string_view warning) -> void override;
    auto error(std::string_view error) -> void override;
    auto log(std::string_view log) -> void override;
    auto log_title(std::string_view log, int level) -> void;

    // signals
    sigslot::signal<std::string> message_signal;
    sigslot::signal<std::string> warning_signal;
    sigslot::signal<std::string> error_signal;
    sigslot::signal<std::string> log_signal;

private:

    auto to_html_paragraph(std::string_view colorCode, std::string_view text, bool addTimestamp = false) -> std::string;
    auto insert_to_log_file(std::string_view message, bool flush = true) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};

class Log2{
public:

    static auto message(std::string_view message) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->message(message);
        }
    }
    static auto warning(std::string_view warning) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->warning(warning);
        }
    }
    static auto error(std::string_view error) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->error(error);
        }
    }
    static auto log(std::string_view log) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->log(log);
        }
    }
};


class Logger;
struct LoggerCleaner {
    auto operator()(Logger* logger) -> void;
};

class Logger{

public:

    enum class MessageT : int{
        normal, warning, error, log, unknow,
    };

    enum class SenderT : int {
        GUI, Component, Resource
    };

    Logger();
    ~Logger();

    static auto get() -> Logger*;

    static auto init(std::string_view logDirectoryPath = "", std::string_view logFileName ="default_log.html", bool doFormat = false) -> bool;
    static auto no_file_init(bool doFormat = false) -> void;
    static auto take_ownership() -> std::unique_ptr<Logger, LoggerCleaner>;
    static auto set_logger_ptr(Logger *logger) -> void;

    static auto message(std::string_view message, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto error(std::string_view error, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto warning(std::string_view warning, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto log(std::string_view log, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;

    static auto message_id(std::string_view message, SenderT sType, int sKey, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto error_id(std::string_view error, SenderT sType, int sKey, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto warning_id(std::string_view warning, SenderT sType, int sKey, bool htmlFormat = true, bool triggersSignal = true, bool saveToFile = true) -> void;

    static auto status(std::string_view status, int ms = 0) -> void;
    static auto progress(int state) -> void;

    static auto clean() -> void;

// signals
    sigslot::signal<std::string> message_signal;
    sigslot::signal<std::string> warning_signal;
    sigslot::signal<std::string> error_signal;
    sigslot::signal<std::string> log_signal;

    sigslot::signal<std::string, SenderT, int> message_id_signal;
    sigslot::signal<std::string, SenderT, int> warning_id_signal;
    sigslot::signal<std::string, SenderT, int> error_id_signal;

    sigslot::signal<std::string, int> status_signal;
    sigslot::signal<int> progress_signal;

private:

    static auto trigger_message(std::string_view message, bool htmlFormat = true) -> void;
    static auto trigger_error(std::string_view error, bool htmlFormat = true) -> void;
    static auto trigger_warning(std::string_view warning, bool htmlFormat = true) -> void;
    static auto trigger_log(std::string_view log, bool htmlFormat = true) -> void;

    static auto trigger_message_id(std::string_view message, SenderT sType, int sKey, bool htmlFormat = true) -> void;
    static auto trigger_error_id(std::string_view error, SenderT sType, int sKey, bool htmlFormat = true) -> void;
    static auto trigger_warning_id(std::string_view warning, SenderT sType, int sKey, bool htmlFormat = true) -> void;

    static auto to_html_line(MessageT type, std::string_view text, bool addTimestamp = false) -> std::string;
    static auto insert_line_to_log_file(MessageT type, std::string_view message) -> void;

    class Impl;
    std::unique_ptr<Impl> i;
};

struct MessageGuard{

    MessageGuard(std::string_view message) : m_message(message){
        using namespace std::string_view_literals;
        Logger::message(std::format("[Start: {}]\n", m_message));
    }
    ~MessageGuard(){
        Logger::message(std::format("[End: {}]\n", m_message));
    }
private:
    std::string_view m_message;
};

struct LogGuard{

    LogGuard(std::string_view log) : m_log(log){
        using namespace std::string_view_literals;
        Logger::log(std::format("[Start: {}]\n", m_log));
    }
    ~LogGuard(){
        Logger::log(std::format("[End: {}]\n", m_log));
    }
private:
    std::string_view m_log;
};

}

