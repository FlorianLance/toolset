
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

#pragma once

// std
#include <memory>


// local
#include "thirdparty/sigslot/signal.hpp"

namespace tool {

struct Log{

    enum class MessageType : int{
        normal, warning, error, log, unknow,
    };

    Log();
    ~Log();

    auto init(std::string_view logDirectoryPath = "", std::string_view logFileName ="default_log.html") -> bool;
    auto nofile_init() -> void;
    auto clean() -> void;

    auto message(std::string_view message) -> void;
    auto warning(std::string_view warning) -> void;
    auto error(std::string_view error) -> void;
    auto log(std::string_view log) -> void;
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

struct SLog{

    static auto set_current_static_log(std::weak_ptr<Log> log) -> void{
        if(auto l = log.lock()){
            m_currentStaticLog = std::move(l);
        }else{
            m_currentStaticLog = nullptr;
        }
    }

    static auto message(std::string_view message) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->message(message);
        }
    }

    static auto warning(std::string_view warning) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->warning(warning);
        }
    }

    static auto error(std::string_view error) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->error(error);
        }
    }

    static auto log(std::string_view log) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->log(log);
        }
    }

private:

    static inline std::shared_ptr<Log> m_currentStaticLog = nullptr;
};

struct SMessageGuard{

    SMessageGuard(std::string_view message) : m_message(message){
        using namespace std::string_view_literals;
        SLog::message(std::format("[Start: {}]\n", m_message));
    }
    ~SMessageGuard(){
        SLog::message(std::format("[End: {}]\n", m_message));
    }
private:
    std::string_view m_message;
};

struct SLogGuard{
    SLogGuard(std::string_view log) : m_log(log){
        using namespace std::string_view_literals;
        SLog::log(std::format("[Start: {}]\n", m_log));
    }
    ~SLogGuard(){
        SLog::log(std::format("[End: {}]\n", m_log));
    }
private:
    std::string_view m_log;
};

}
