
/*******************************************************************************
** Toolset-qt-utility                                                         **
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

// base
#include "utility/log.hpp"

// Qt
#include <QObject>

namespace tool {

struct QtLog : public QObject{
    Q_OBJECT
public:

    enum class MessageType : int{
        normal, warning, error, log, unknow,
    };

    QtLog();
    ~QtLog();

    auto initialize(QStringView logDirectoryPath = QStringLiteral(""), QStringView logFileName = QStringLiteral("default_log.html"), bool copyPreviousLog = true)  -> void;
    auto clean()  -> void;

    auto message(QStringView message) -> void;
    auto warning(QStringView warning) -> void;
    auto error(QStringView error) -> void;
    auto log(QStringView log) -> void;
    auto log_title(QStringView log, int level) -> void;

    auto set_type_color(MessageType type, const QColor &color) -> void;
    auto set_html_file_background_color(const QString &color) -> void;

signals:

    auto message_signal(QString message) -> void;
    auto error_signal(QString error) -> void;
    auto warning_signal(QString warning) -> void;
    auto log_signal(QString log) -> void;

private:    

    struct Impl;
    std::unique_ptr<Impl> i;

    auto to_html_paragraph(QStringView colorCode, QStringView text, bool addTimestamp) -> QString;
    auto insert_to_log_file(QStringView message, bool flush = true) -> void;
};

struct SQtLog{

    static auto set_current_static_log(std::weak_ptr<QtLog> log) -> void{
        if(auto l = log.lock()){
            m_currentStaticLog = std::move(l);
        }else{
            m_currentStaticLog = nullptr;
        }
    }

    static auto message(QStringView message) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->message(message);
        }
    }

    static auto warning(QStringView warning) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->warning(warning);
        }
    }

    static auto error(QStringView error) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->error(error);
        }
    }

    static auto log(QStringView log) -> void{
        if(m_currentStaticLog){
            m_currentStaticLog->log(log);
        }
    }

private:

    static inline std::shared_ptr<QtLog> m_currentStaticLog = nullptr;
};

struct SQtMessageGuard{

    SQtMessageGuard(QStringView message) : m_message(message){
        using namespace std::string_view_literals;
        SQtLog::message(QStringLiteral("[Start: %1]\n").arg(m_message));
    }
    ~SQtMessageGuard(){
        SQtLog::message(QStringLiteral("[End: %1]\n").arg(m_message));
    }
private:
    QStringView m_message;
};

struct SQtLogGuard{

    SQtLogGuard(QStringView log) : m_log(log){
        using namespace std::string_view_literals;
        SQtLog::log(QStringLiteral("[Start: %1]\n").arg(m_log));
    }
    ~SQtLogGuard(){
        SQtLog::log(QStringLiteral("[End: %1]\n").arg(m_log));
    }
private:
    QStringView m_log;
};

}
