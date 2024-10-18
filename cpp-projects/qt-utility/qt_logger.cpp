
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

struct QtLogger2::Impl{

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
        {MessageType::normal,   Config{true,  true, true, false, false, u"DarkBlue"_s}},
        {MessageType::error,    Config{true,  true, true, true,  true,  u"Orange"_s}},
        {MessageType::warning,  Config{true,  true, true, true,  true,  u"DarkRed"_s}},
        {MessageType::log,      Config{false, false,true, true,  true,  u"LightGrey"_s}},
        {MessageType::unknow,   Config{false, false,false,false, false, u"Black"_s}},
        };

    Impl(){}
};

QtLogger2::QtLogger2() : i(std::make_unique<Impl>()){
}

QtLogger2::~QtLogger2(){
    QtLogger2::clean();
}

auto QtLogger2::clean() -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->file){
        i->file->close();
        i->file = nullptr;
        i->out  = nullptr;
    }
}

auto QtLogger2::init(QStringView logDirectoryPath, QStringView logFileName, bool copyPreviousLog) -> void{

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

auto QtLogger2::message(std::string_view message) -> void{
    QtLogger2::message(QString::fromStdString(std::string(message)));
}

auto QtLogger2::warning(std::string_view warning) -> void{
    QtLogger2::warning(QString::fromStdString(std::string(warning)));
}

auto QtLogger2::error(std::string_view error) -> void{
    QtLogger2::error(QString::fromStdString(std::string(error)));
}

auto QtLogger2::log(std::string_view log) -> void{
    QtLogger2::log(QString::fromStdString(std::string(log)));
}

auto QtLogger2::message(QStringView message) -> void{

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
                message_signal(std::move(htmlMessage));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(message);
            }
            if(conf.triggerSignals){
                message_signal(message.toString());
            }
        }
    }
}

auto QtLogger2::warning(QStringView warning) -> void{

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
                warning_signal(std::move(htmlWarning));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(warning);
            }
            if(conf.triggerSignals){
                warning_signal(warning.toString());
            }
        }
    }
}

auto QtLogger2::error(QStringView error) -> void{

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
                error_signal(std::move(htmlError));
            }
        }else{
            if(conf.saveToFile){
                insert_to_log_file(error);
            }
            if(conf.triggerSignals){
                error_signal(error.toString());
            }
        }
    }
}

auto QtLogger2::log(QStringView log) -> void{

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

auto QtLogger2::log_title(QStringView log, int level) -> void{

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

auto QtLogger2::set_type_color(MessageType type, const QColor &color) -> void{
    auto &conf = i->mTypeConfig[type];
    conf.color = color.name();
}

auto QtLogger2::set_html_file_background_color(const QString &color) -> void{
    insert_to_log_file(u"<body style=\"background-color:%1;\">"_s.arg(color));
}

auto QtLogger2::to_html_paragraph(QStringView colorCode, QStringView text, bool addTimestamp) -> QString{

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

auto QtLogger2::insert_to_log_file(QStringView message, bool flush) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    if(i->out){
        if(flush){
            (*i->out) << message << Qt::flush;
        }else{
            (*i->out) << message;
        }
    }
}






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
        return u"<p><font color=\"%1\"> %2 </font></p>"_s.arg(colorCode, text);
    }else{
        return u"<p><font color=\"%1\">[%2] %3 </font></p>"_s.arg(
            colorCode,
            QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - QtLogger::Impl::epochStart).count()),            
            text
        );
    }
}

auto QtLogger::message(const QString &message, bool triggersSignal, bool saveToFile, bool addTimeStamp) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        auto htmlMessage = to_html_paragraph(MessageType::normal, message, addTimeStamp);
        if(saveToFile){
            insert_to_log_file(htmlMessage);
        }
        if(triggersSignal){
            if(QtLogger::Impl::displayConsole){
                qDebug() << message;
            }
            emit QtLogger::Impl::logger->message_signal(std::move(htmlMessage));
        }
    }
}

auto QtLogger::warning(const QString &warning, bool triggersSignal, bool saveToFile, bool addTimeStamp) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        auto htmlWarning = to_html_paragraph(MessageType::warning, warning, addTimeStamp);
        if(saveToFile){
            insert_to_log_file(htmlWarning);
        }
        if(triggersSignal){
            if(QtLogger::Impl::displayConsole){
                qDebug() << warning;
            }
            emit QtLogger::Impl::logger->warning_signal(std::move(htmlWarning));
        }
    }
}

auto QtLogger::error(const QString &error, bool triggersSignal, bool saveToFile, bool addTimeStamp) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        auto htmlError = to_html_paragraph(MessageType::error, error, addTimeStamp);
        if(saveToFile){
            insert_to_log_file(htmlError);
        }
        if(triggersSignal){
            if(QtLogger::Impl::displayConsole){
                qDebug() << error;
            }
            emit QtLogger::Impl::logger->error_signal(std::move(htmlError));
        }
    }
}

auto QtLogger::log(const QString &log, bool triggersSignal) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        auto htmlLog = to_html_paragraph(MessageType::log, log, true);
        insert_to_log_file(htmlLog);

        if(triggersSignal){
            emit QtLogger::Impl::logger->log_signal(std::move(htmlLog));
        }
    }
}

auto QtLogger::log_title(const QString &log, int level, bool triggersSignal) -> void{

    if(auto logger = QtLogger::get(); logger != nullptr){

        auto htmlLog = u"<h%1>%2</h%1>"_s.arg(QString::number(level), to_html_paragraph(QtLogger::MessageType::log, log, false));
        insert_to_log_file(htmlLog);

        if(triggersSignal){
            emit QtLogger::Impl::logger->log_signal(std::move(htmlLog));
        }
    }
}

auto QtLogger::set_log_background_color(const QString &color) -> void{
    if(auto logger = QtLogger::get(); logger != nullptr){
        insert_to_log_file(u"<body style=\"background-color:%1;\">"_s.arg(color));
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
