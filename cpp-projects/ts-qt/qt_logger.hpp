
/*******************************************************************************
** Toolset-ts-qt                                                              **
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

// Qt
#include <QColor>
#include <QObject>
#include <QStringBuilder>

// base
#include "utility/base_logger.hpp"

// local
#include "qt_str.hpp"

namespace tool {

class QtLoggerM : public QObject, public BaseLogger{
Q_OBJECT
public:

    static auto get_instance() noexcept -> QtLoggerM*{
        if(auto instance = BaseLogger::get_instance()){
            return dynamic_cast<QtLoggerM*>(instance);
        }
        return nullptr;
    }

    QtLoggerM();
    virtual ~QtLoggerM();

    auto init(QStringView logDirectoryPath = QStringLiteral(""), QStringView logFileName = QStringLiteral("default_log.html"), bool copyPreviousLog = true)  -> void;
    auto nofile_init() -> void override{}
    auto clean()  -> void override;

    auto message(std::string_view message) -> void override;
    auto warning(std::string_view warning) -> void override;
    auto error(std::string_view error) -> void override;
    auto log(std::string_view log) -> void override;
    auto log_title(std::string_view log, int level) -> void;

    auto message(QStringView message) -> void;
    auto warning(QStringView warning) -> void;
    auto error(QStringView error) -> void;
    auto log(QStringView log) -> void;
    auto log_title(QStringView log, int level) -> void;

    auto set_type_color(MessageType type, const QColor &color) -> void;
    auto set_html_file_background_color(const QString &color) -> void;

signals:

    auto message_signal(QString message) -> void;
    auto warning_signal(QString warning) -> void;
    auto error_signal(QString error) -> void;
    auto log_signal(QString log) -> void;

private:

    auto to_html_paragraph(QStringView colorCode, QStringView text, bool addTimestamp) -> QString;
    auto insert_to_log_file(QStringView message, bool flush = true) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};

class QtLog{
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
    static auto log_title(std::string_view log, int level) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->log_title(log, level);
        }
    }

    static auto message(QStringView message) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->message(message);
        }
    }
    static auto warning(QStringView warning) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->warning(warning);
        }
    }
    static auto error(QStringView error) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->error(error);
        }
    }
    static auto log(QStringView log) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->log(log);
        }
    }
    static auto log_title(QStringView &log, int level) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->log_title(log, level);
        }
    }

    static auto message(const QString &message) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->message(message);
        }
    }
    static auto warning(const QString &warning) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->warning(warning);
        }
    }
    static auto error(const QString &error) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->error(error);
        }
    }
    static auto log(const QString &log) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->log(log);
        }
    }
    static auto log_title(const QString &log, int level) -> void{
        if(auto instance = QtLoggerM::get_instance()){
            instance->log_title(log, level);
        }
    }
};

struct QtMessageGuard{

    QtMessageGuard(QStringView message) : m_message(message){
        using namespace std::string_view_literals;
        QtLog::message(QSL("[Start: %1]\n").arg(m_message));
    }
    ~QtMessageGuard(){
        QtLog::message(QSL("[End: %1]\n").arg(m_message));
    }
private:
    QStringView m_message;
};

struct QtLogGuard{

    QtLogGuard(QStringView log) : m_log(log){
        using namespace std::string_view_literals;
        QtLog::log(QSL("[Start: %1]\n").arg(m_log));
    }
    ~QtLogGuard(){
        QtLog::log(QSL("[End: %1]\n").arg(m_log));
    }
private:
    QStringView m_log;
};

}

