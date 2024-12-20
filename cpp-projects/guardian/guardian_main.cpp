

/*******************************************************************************
** Toolset-guardian                                                           **
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

// Qt
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QTime>
#include <QTimer>

// qt-utility
// #include "qt_logger.hpp"

auto wait_process(int ms) -> void {
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime){
        QCoreApplication::processEvents( QEventLoop::AllEvents, 5 > ms ? ms : 5);
    }
}

int main(int argc, char *argv[]){

    QCoreApplication a(argc, argv);

    QStringList args = QCoreApplication::arguments();
    if(args.size() < 2){
        qDebug() << "Not enought arguments.";
        return 0;
    }
    QString exeDirPath = QCoreApplication::applicationDirPath();
    qDebug() << "Executable to guard: " << exeDirPath;

    QString exeProgramPath = QString("%1/%2").arg(exeDirPath, args[1]);
    args.removeFirst();
    qDebug() << "Arguments to use: " << args;

    QProcess process;
    QEventLoop::connect(&process, &QProcess::stateChanged, &a, [&](QProcess::ProcessState newState){
        switch (newState) {
            case QProcess::ProcessState::Running:
                qDebug() << "Running.";
            break;
            case QProcess::ProcessState::Starting:
                qDebug() << "Starting.";
            break;
            case QProcess::ProcessState::NotRunning:
                qDebug() << "Not running.";
            break;
        }
    });

    QEventLoop::connect(&process, &QProcess::errorOccurred, &a, [&](QProcess::ProcessError error){
        switch (error) {
            case QProcess::FailedToStart:
                qDebug() << "process error: failed to start.";
            break;
            case QProcess::Crashed:
                qDebug() << "process error: crashed.";
            break;
            case QProcess::Timedout:
                qDebug() << "process error: timeout.";
            break;
            case QProcess::WriteError:
                qDebug() << "process error: write error.";
            break;
            case QProcess::ReadError:
                qDebug() << "process error: read error.";
            break;
            case QProcess::UnknownError:
                qDebug() << "process error: unknow error.";
            break;
        }
    });

    bool normalExit = false;
    QEventLoop::connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &a, [&](int exitCode, QProcess::ExitStatus exitStatus){
        Q_UNUSED(exitStatus)
        qDebug() << "Exit code: " << exitCode;
        if(exitCode == 1){
            qDebug() << "normal exit";
            normalExit = true;
        }else{
            qDebug() << "crash exit";
        }
    });

    QTimer timer;
    QEventLoop::connect(&timer, &QTimer::timeout,  [&](){

        if(normalExit){
            a.quit();
            return;
        }

        if(process.state() != QProcess::ProcessState::Running && process.state() != QProcess::ProcessState::Starting){
            process.start(exeProgramPath, args);
        }
    });
    timer.start(200);

    return a.exec();
}
