
/*******************************************************************************
** Toolset-qt-utility                                                         **
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
#include <QObject>

// local
#include "qt_str.hpp"

namespace tool {

class QtLogger : public QObject{

Q_OBJECT
public:

    enum class MessageType : int{
        normal, warning, error, log, unknow,
    };

    QtLogger();
    ~QtLogger();

    static auto get() -> QtLogger*;

    static auto init(QString logDirectoryPath = "", QString logFileName = QSL("default_log.html"), bool displayConsole = false, bool copyPreviousLog = true) -> void;
    static auto clean() -> void;

    static auto message(const QString &message, bool triggersSignal = true, bool saveToFile = true, bool addTimeStamp = true) -> void;
    static auto warning(const QString &warning, bool triggersSignal = true, bool saveToFile = true, bool addTimeStamp = true) -> void;
    static auto error(const QString &error, bool triggersSignal = true, bool saveToFile = true, bool addTimeStamp = true) -> void;
    static auto log(const QString &log, bool triggersSignal = false) -> void;
    static auto log_title(const QString &log, int level, bool triggersSignal = false) -> void;

    static auto set_log_background_color(const QString &color) -> void;

    static auto set_html_ui_type_message_color(MessageType type, const QColor &col) -> void;

    static auto status(const QString &status, int ms = 0) -> void;
    static auto progress(int state) -> void;

    // TODO: rename
    static auto unity_message(QStringView message, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto unity_error(QStringView error, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto unity_warning(QStringView warning, bool triggersSignal = true, bool saveToFile = true) -> void;
    static auto unity_unknow(QStringView unknow, bool triggersSignal = true, bool saveToFile = true) -> void;

private:

    static auto to_html_paragraph(QtLogger::MessageType type, QStringView text, bool addTimestamp) -> QString;
    static auto insert_to_log_file(QStringView message, bool flush = true) -> void;

signals:

    auto message_signal(QString message) -> void;
    auto error_signal(QString error) -> void;
    auto warning_signal(QString warning) -> void;
    auto log_signal(QString log) -> void;
    auto status_signal(QString status, int ms) -> void;

    auto unity_message_signal(QStringView message) -> void;
    auto unity_error_signal(QStringView error) -> void;
    auto unity_warning_signal(QStringView warning) -> void;
    auto unity_unknow_signal(QStringView unknow) -> void;

    auto progress_signal(int state) -> void;

private:

    struct Impl;
    std::unique_ptr<Impl> m_p;
};

struct QtMessageGuard{

    QtMessageGuard(QStringView message) : m_message(message){
        using namespace std::string_view_literals;
        QtLogger::message(QSL("[Start: %1]\n").arg(m_message));
    }
    ~QtMessageGuard(){
        QtLogger::message(QSL("[End: %1]\n").arg(m_message));
    }
private:
    QStringView m_message;
};

struct QtLogGuard{

    QtLogGuard(QStringView log) : m_log(log){
        using namespace std::string_view_literals;
        QtLogger::log(QSL("[Start: %1]\n").arg(m_log));
    }
    ~QtLogGuard(){
        QtLogger::log(QSL("[End: %1]\n").arg(m_log));
    }
private:
    QStringView m_log;
};

}

