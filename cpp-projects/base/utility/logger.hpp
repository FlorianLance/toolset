

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
#include "thirdparty/sigslot/signal.hpp"

namespace tool {

class Logger : public BaseLogger{

public:

    struct Config{
        // dest
        bool displayToConsole;
        bool triggerSignals;
        bool saveToFile;
        // format
        bool formatHTMLForSignals;
        bool formatHTMLForFile;
        bool addTimestampToHtml;
        std::string htmlColor;
    };

    auto config(MessageType mType) const -> Config;
    auto config(MessageType mType) -> Config&;

    static auto get_instance() noexcept -> Logger*{
        if(auto instance = BaseLogger::get_instance()){
            return dynamic_cast<Logger*>(instance);
        }
        return nullptr;
    }

    Logger();
    virtual ~Logger();

    auto init(std::string_view logDirectoryPath = "", std::string_view logFileName ="default_log.html") -> bool;
    auto nofile_init() -> void override;
    auto clean() -> void override;

    auto message(std::string_view message) -> void override;
    auto message_unf(std::string_view message) -> void override;
    auto warning(std::string_view warning) -> void override;
    auto error(std::string_view error) -> void override;
    auto log(std::string_view log) -> void override;
    auto log_unf(std::string_view log) -> void override;
    auto log_title(std::string_view log, int level) -> void;

    // signals
    sigslot::signal<std::string> message_signal;
    sigslot::signal<std::string> warning_signal;
    sigslot::signal<std::string> error_signal;
    sigslot::signal<std::string> log_signal;

protected:

    auto to_html_paragraph(std::string_view colorCode, std::string_view text, bool addTimestamp = false) -> std::string;
    auto insert_to_log_file(std::string_view message, bool flush = true) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};

class Log{
public:

    static auto get() -> BaseLogger*{
        return Logger::get_instance();
    }

    static auto message(std::string_view message) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->message(message);
        }
    }

    static auto message_unf(std::string_view message) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->message_unf(message);
        }
    }

    template<typename... Args>
    static auto fmessage(std::string_view message, Args&&... args) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->message(std::vformat(message, std::make_format_args(args...)));
        }
    }

    template<typename... Args>
    static auto fmessage_unf(std::string_view message, Args&&... args) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->message_unf(std::vformat(message, std::make_format_args(args...)));
        }
    }

    static auto warning(std::string_view warning) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->warning(warning);
        }
    }

    template<typename... Args>
    static auto fwarning(std::string_view warning, Args&&... args) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->warning(std::vformat(warning, std::make_format_args(args...)));
        }
    }

    static auto error(std::string_view error) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->error(error);
        }
    }

    template<typename... Args>
    static auto ferror(std::string_view error, Args&&... args) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->error(std::vformat(error, std::make_format_args(args...)));
        }
    }

    static auto log(std::string_view log) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->log(log);
        }
    }

    static auto log_unf(std::string_view log) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->log_unf(log);
        }
    }


    template<typename... Args>
    static auto flog(std::string_view log, Args&&... args) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->log(std::vformat(log, std::make_format_args(args...)));
        }
    }

    template<typename... Args>
    static auto flog_unf(std::string_view log, Args&&... args) -> void{
        if(auto instance = BaseLogger::get_instance()){
            instance->log_unf(std::vformat(log, std::make_format_args(args...)));
        }
    }
};

struct MessageG{

    MessageG(std::string_view message) : m_message(message){
        using namespace std::string_view_literals;
        Log::fmessage("[Start: {}]\n"sv, m_message);
    }
    ~MessageG(){
        using namespace std::string_view_literals;
        Log::fmessage("[End: {}]\n"sv, m_message);
    }

private:
    std::string_view m_message;
};

struct LogG{

    LogG(std::string_view log) : m_log(log){
        using namespace std::string_view_literals;
        Log::flog("[Start: {}]\n"sv, m_log);
    }
    ~LogG(){
        using namespace std::string_view_literals;
        Log::flog("[End: {}]\n"sv, m_log);
    }

private:
    std::string_view m_log;
};


}

