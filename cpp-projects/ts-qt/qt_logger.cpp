
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

using namespace tool;
using namespace Qt::Literals::StringLiterals;

struct QtLoggerM::Impl{

    QString logDirPath;
    QString logName;
    QString absoluteFilePath;
    std::mutex locker;
    std::unique_ptr<QFile> file = nullptr;
    std::unique_ptr<QTextStream> out = nullptr;
    std::chrono::nanoseconds epochStart;

    struct Config{
        bool displayToConsole;
        bool triggerSignals;
        bool formatToHtml;
        bool saveToFile;
        bool addTimestampToHtml;
        QString color;
    };
    std::unordered_map<MessageType, Config> mTypeConfig = {
        {MessageType::normal,   Config{true,  true, true, true,  false, u"DarkBlue"_s}},
        {MessageType::error,    Config{true,  true, true, true,  true,  u"Orange"_s}},
        {MessageType::warning,  Config{true,  true, true, true,  true,  u"DarkRed"_s}},
        {MessageType::log,      Config{false, false,true, true,  true,  u"LightGrey"_s}},
        {MessageType::unknow,   Config{false, false,false,false, false, u"Black"_s}},
    };

    Impl(){}
};

QtLoggerM::QtLoggerM() : i(std::make_unique<Impl>()){
}

QtLoggerM::~QtLoggerM(){
    QtLoggerM::clean();
}

auto QtLoggerM::clean() -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->file){
        i->file->close();
        i->file = nullptr;
        i->out  = nullptr;
    }
}

auto QtLoggerM::init(QStringView logDirectoryPath, QStringView logFileName, bool copyPreviousLog) -> void{

    std::unique_lock<std::mutex> lock(i->locker);

    // connect base log to qt log
    if(logDirectoryPath.length() == 0 || logFileName.length() == 0){
        i->logDirPath = QApplication::applicationDirPath() % u"/logs"_s;
        i->logName    = u"default_"_s % QHostInfo::localHostName() % u".html"_s;
    }else{
        i->logDirPath = logDirectoryPath.toString();
        i->logName    = logFileName.toString();
    }
    i->absoluteFilePath = i->logDirPath % "/" % i->logName;

    if(!QFile::exists(i->logDirPath)){
        QDir logDir(i->logDirPath);
        if(!logDir.mkdir(i->logDirPath)){
            qWarning() << "[QtLog::initialize] Cannot create logging directory path.\n";
            return;
        }
    }

    QFileInfo info(i->absoluteFilePath);
    if(info.exists() && copyPreviousLog){

        int id = i->absoluteFilePath.lastIndexOf('.');
        if(id != -1){
            QString leftPart  = i->absoluteFilePath.left(id);
            QString extension = i->absoluteFilePath.mid(id);
            QString previousFileName  = leftPart % u"_previous_"_s % info.lastModified().toString(u"yyyy-MM-dd_hh-mm-ss"_s) % extension;

            bool canCopy = true;
            if(QFile::exists(previousFileName)){
                if(!QFile::remove(previousFileName)){
                    qWarning() << "[QtLogger::init] Cannot remove previous log file: " << previousFileName;
                    canCopy = false;
                }
            }

            if(canCopy){
                if(!QFile::copy(i->absoluteFilePath, previousFileName)){
                    qWarning() << "[QtLogger::init] Cannot save last log file " << i->absoluteFilePath << " to " << previousFileName;
                }
            }
        }else{
            qWarning() << "[QtLogger::init] Invalid log file path, no extension: " << i->absoluteFilePath;
            return;
        }
    }

    // init log file
    i->epochStart = tool::Time::nanoseconds_since_epoch();
    i->file       = std::make_unique<QFile>(i->absoluteFilePath);

    if(!i->file->open(QFile::WriteOnly | QFile::Text)){
        qWarning() << "[QtLogger::init] Cannot write to log file: " << i->absoluteFilePath;
        return;
    }

    i->out = std::make_unique<QTextStream>(i->file.get());
}

auto QtLoggerM::message(std::string_view message) -> void{
    QtLoggerM::message(QString::fromStdString(std::string(message)));
}

auto QtLoggerM::warning(std::string_view warning) -> void{
    QtLoggerM::warning(QString::fromStdString(std::string(warning)));
}

auto QtLoggerM::error(std::string_view error) -> void{
    QtLoggerM::error(QString::fromStdString(std::string(error)));
}

auto QtLoggerM::log(std::string_view log) -> void{
    QtLoggerM::log(QString::fromStdString(std::string(log)));
}

auto QtLoggerM::message(QStringView message) -> void{

    const auto &conf = i->mTypeConfig[MessageType::normal];
    if(conf.displayToConsole){
        qInfo() << message;
    }

    if(conf.saveToFile || conf.triggerSignals){

        if(conf.formatToHtml){
            QString htmlMessage = to_html_paragraph(conf.color, message, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlMessage);
            }
            if(conf.triggerSignals){
                emit message_signal(std::move(htmlMessage));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(message);
            }
            if(conf.triggerSignals){
                emit message_signal(message.toString());
            }
        }
    }
}

auto QtLoggerM::warning(QStringView warning) -> void{

    const auto &conf = i->mTypeConfig[MessageType::warning];
    if(conf.displayToConsole){
        qWarning() << warning;
    }

    if(conf.saveToFile || conf.triggerSignals){
        if(conf.formatToHtml){
            QString htmlWarning = to_html_paragraph(conf.color, warning, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlWarning);
            }
            if(conf.triggerSignals){
                emit warning_signal(std::move(htmlWarning));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(warning);
            }
            if(conf.triggerSignals){
                emit warning_signal(warning.toString());
            }
        }
    }
}

auto QtLoggerM::error(QStringView error) -> void{

    const auto &conf = i->mTypeConfig[MessageType::error];
    if(conf.displayToConsole){
        qCritical() << error;
    }

    if(conf.saveToFile || conf.triggerSignals){
        if(conf.formatToHtml){
            QString htmlError = to_html_paragraph(conf.color, error, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlError);
            }
            if(conf.triggerSignals){
                emit error_signal(std::move(htmlError));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(error);
            }
            if(conf.triggerSignals){
                emit error_signal(error.toString());
            }
        }
    }
}

auto QtLoggerM::log(QStringView log) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];

    if(conf.saveToFile || conf.triggerSignals){
        if(conf.formatToHtml){
            QString htmlLog = to_html_paragraph(conf.color, log, conf.addTimestampToHtml);
            if(conf.saveToFile){
                insert_to_log_file(htmlLog);
            }
            if(conf.triggerSignals){
                emit log_signal(std::move(htmlLog));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(log);
            }
            if(conf.triggerSignals){
                emit log_signal(log.toString());
            }
        }
    }
}

auto QtLoggerM::log_title(QStringView log, int level) -> void{

    const auto &conf = i->mTypeConfig[MessageType::log];
    if(conf.saveToFile || conf.triggerSignals){

        auto htmlLog = u"<h%1>%2</h%1>"_s.arg(QString::number(level), to_html_paragraph(conf.color, log, false));
        if(conf.saveToFile){
            insert_to_log_file(htmlLog);
        }
        if(conf.triggerSignals){
            emit log_signal(log.toString());
        }
    }
}

auto QtLoggerM::set_type_color(MessageType type, const QColor &color) -> void{
    auto &conf = i->mTypeConfig[type];
    conf.color = color.name();
}

auto QtLoggerM::set_html_file_background_color(const QString &color) -> void{
    insert_to_log_file(u"<body style=\"background-color:%1;\">"_s.arg(color));
}

auto QtLoggerM::to_html_paragraph(QStringView colorCode, QStringView text, bool addTimestamp) -> QString{

    using namespace Qt::Literals::StringLiterals;
    if(!addTimestamp){
        return u"<p><font color=\"%1\"> %2 </font></p>"_s.arg(colorCode, text);
    }else{
        return u"<p><font color=\"%1\">[%2] %3 </font></p>"_s.arg(
            colorCode,
            QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(tool::Time::nanoseconds_since_epoch() - i->epochStart).count()),
            text
        );
    }
}

auto QtLoggerM::insert_to_log_file(QStringView message, bool flush) -> void{

    std::unique_lock<std::mutex> lock(i->locker);
    if(i->out){
        if(flush){
            (*i->out) << message << Qt::flush;
        }else{
            (*i->out) << message;
        }
    }
}

