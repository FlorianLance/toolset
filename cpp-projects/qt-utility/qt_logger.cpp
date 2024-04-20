
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

#include "qt_logger.hpp"

// std
#include <mutex>

// Qt
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QDateTime>
#include <QHostInfo>
#include <QString>
#include <QColor>

// base
#include "utility/time.hpp"

// qt-utility
#include "qt_str.hpp"

using namespace tool;
using namespace Qt::Literals::StringLiterals;

struct QtLogger::Impl{

    static inline bool displayConsole = false;
    static inline std::mutex locker;
    static inline std::unique_ptr<QtLogger> logger = nullptr;
    static inline std::unique_ptr<QFile> file = nullptr;
    static inline std::unique_ptr<QTextStream> out = nullptr;

    static inline std::chrono::nanoseconds epochStart;

    static inline QString htmlNormalBaliseColor     = u"DarkBlue"_s;
    static inline QString htmlWarningBaliseColor    = u"Orange"_s;
    static inline QString htmlErrorBaliseColor      = u"DarkRed"_s;
    static inline QString htmlLogBaliseColor        = u"LightGrey"_s;
    static inline QString htmlUnknowBaliseColor     = u"Black"_s;

    Impl(){}
};

QtLogger::QtLogger() : m_p(std::make_unique<Impl>()){
}

QtLogger::~QtLogger(){
}

auto QtLogger::get() -> QtLogger *{
    if(QtLogger::Impl::logger != nullptr){
        return QtLogger::Impl::logger.get();
    }
    return nullptr;
}

auto QtLogger::init(QString logDirectoryPath, QString logFileName, bool displayConsole, bool copyPreviousLog) -> void{

    QtLogger::Impl::displayConsole = displayConsole;

    if(logDirectoryPath.length() == 0 || logFileName.length() == 0){
        logDirectoryPath = QApplication::applicationDirPath() % u"/logs"_s;
        logFileName      = u"default_"_s % QHostInfo::localHostName() % u".html"_s;
    }

    QString logDirPath       = logDirectoryPath;
    QString logName          = logFileName;
    QString absoluteFilePath = logDirPath % "/" % logName;

    if(!QFile::exists(logDirPath)){
        QDir logDir(logDirPath);
        if(!logDir.mkdir(logDirPath)){
            qWarning() << "[QtLogger::init] Cannot create logging directory path.\n";
            return;
        }
    }

    std::unique_lock<std::mutex> lock(QtLogger::Impl::locker);
    if(QtLogger::Impl::logger == nullptr){

        QFileInfo info(absoluteFilePath);

        if(info.exists() && copyPreviousLog){

            int id = absoluteFilePath.lastIndexOf('.');
            if(id != -1){
                QString leftPart  = absoluteFilePath.left(id);
                QString extension = absoluteFilePath.mid(id);
                QString previousFileName  = leftPart % u"_previous_"_s % info.lastModified().toString(u"yyyy-MM-dd_hh-mm-ss"_s) % extension;

                bool canCopy = true;
                if(QFile::exists(previousFileName)){
                    if(!QFile::remove(previousFileName)){
                        qWarning() << "[QtLogger::init] Cannot remove previous log file: " << previousFileName;
                        canCopy = false;
                    }
                }

                if(canCopy){
                    if(!QFile::copy(absoluteFilePath, previousFileName)){
                        qWarning() << "[QtLogger::init] Cannot save last log file " << absoluteFilePath << " to " << previousFileName;
                    }
                }
            }else{
                qWarning() << "[QtLogger::init] Invalid log file path, no extension: " << absoluteFilePath;
                return;
            }
        }

        // init log file
        QtLogger::Impl::epochStart = Time::nanoseconds_since_epoch();
        QtLogger::Impl::logger     = std::make_unique<QtLogger>();
        QtLogger::Impl::file       = std::make_unique<QFile>(absoluteFilePath);

        if(!QtLogger::Impl::file->open(QFile::WriteOnly | QFile::Text)){
            qWarning() << "[QtLogger::init] Cannot write to log file: " << absoluteFilePath;
            return;
        }

        QtLogger::Impl::out = std::make_unique<QTextStream>(QtLogger::Impl::file.get());
    }
}

auto QtLogger::clean() -> void{

    std::unique_lock<std::mutex> lock(QtLogger::Impl::locker);
    if(QtLogger::Impl::file){
        QtLogger::Impl::file->close();
        QtLogger::Impl::file = nullptr;
        QtLogger::Impl::out  = nullptr;
    }
    QtLogger::Impl::logger = nullptr;
}

auto QtLogger::set_html_ui_type_message_color(QtLogger::MessageType type, const QColor &col) -> void{

    switch (type) {
    case MessageType::normal:
        QtLogger::Impl::htmlNormalBaliseColor = col.name();
        break;
    case MessageType::warning:
        QtLogger::Impl::htmlWarningBaliseColor = col.name();
        break;
    case MessageType::error:
        QtLogger::Impl::htmlErrorBaliseColor = col.name();
        break;
    case MessageType::log:
        QtLogger::Impl::htmlLogBaliseColor = col.name();
        break;
    case MessageType::unknow:
        QtLogger::Impl::htmlUnknowBaliseColor = col.name();
        break;
    }
}

auto QtLogger::to_html_paragraph(QtLogger::MessageType type, QStringView text, bool addTimestamp) -> QString{

    QStringView colorCode;
    switch (type) {
    case QtLogger::MessageType::normal:
        colorCode = QtLogger::Impl::htmlNormalBaliseColor;
        break;
    case QtLogger::MessageType::warning:
        colorCode = QtLogger::Impl::htmlWarningBaliseColor;
        break;
    case QtLogger::MessageType::error:
        colorCode = QtLogger::Impl::htmlErrorBaliseColor;
        break;
    case QtLogger::MessageType::log:
        colorCode = QtLogger::Impl::htmlLogBaliseColor;
        break;
    case QtLogger::MessageType::unknow:
        colorCode = QtLogger::Impl::htmlUnknowBaliseColor;
        break;
    }

    if(!addTimestamp){
        return u"<p> <font color=\"%1\"> %2 </font></p>"_s.arg(colorCode, text);
    }else{
        return u"<p> [%1] <font color=\"%2\"> %3 </font></p>"_s.arg(
            QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - QtLogger::Impl::epochStart).count()),
            colorCode,
            text
        );
    }
}

auto QtLogger::message(const QString &message, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->message_signal(
                to_html_paragraph(MessageType::normal, message, false)
            );
            if(QtLogger::Impl::displayConsole){
                qDebug() << message;
            }
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::normal, message, true));
        }
    }
}

auto QtLogger::warning(const QString &warning, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->warning_signal(
                to_html_paragraph(MessageType::warning, warning, false)
            );
            if(QtLogger::Impl::displayConsole){
                qWarning() << warning;
            }
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::warning, warning, true));
        }
    }
}

auto QtLogger::error(const QString &error, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->error_signal(
                to_html_paragraph(MessageType::error, error, false)
            );
            if(QtLogger::Impl::displayConsole){
                qDebug() << error;
            }
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::error, error, true));
        }
    }
}

auto QtLogger::log(const QString &log, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->log_signal(to_html_paragraph(MessageType::log, log, false));
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::log, log, true));
        }
    }
}

auto QtLogger::insert_to_log_file(QStringView message, bool flush) -> void{

    std::unique_lock<std::mutex> lock(QtLogger::Impl::locker);
    if(QtLogger::Impl::out){
        if(flush){
            (*QtLogger::Impl::out) << message << Qt::flush;
        }else{
            (*QtLogger::Impl::out) << message;
        }
    }
}


auto QtLogger::status(const QString &status, int ms) -> void{
    if(auto logger = QtLogger::get(); logger != nullptr){
        emit QtLogger::Impl::logger->status_signal(status, ms);
    }
}

auto QtLogger::progress(int state) -> void {
    if(auto logger = QtLogger::get(); logger != nullptr){
        emit QtLogger::Impl::logger->progress_signal(state);
    }
}

auto QtLogger::unity_message(QStringView message, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            int idRichText = message.indexOf(u"|R|"_s);
            if(idRichText != -1){
                emit QtLogger::Impl::logger->unity_message_signal(message.chopped(idRichText));
            }else{
                emit QtLogger::Impl::logger->unity_message_signal(message);
            }
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::normal, message, true));
        }
    }
}

auto QtLogger::unity_error(QStringView error, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->unity_error_signal(error);
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::error, error, true));
        }
    }
}

auto QtLogger::unity_warning(QStringView warning, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->unity_warning_signal(warning);
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::warning, warning, true));
        }
    }
}

auto QtLogger::unity_unknow(QStringView unknow, bool triggersSignal, bool saveToFile) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        if(triggersSignal){
            emit QtLogger::Impl::logger->unity_unknow_signal(unknow);
        }

        if(saveToFile){
            insert_to_log_file(to_html_paragraph(MessageType::unknow, unknow, true));
        }
    }
}
