
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

#include "logger_w.hpp"

// local
#include "qt_style.hpp"

using namespace tool::ui;

LoggerW::LoggerW(){

    zoomIn(2);
    setStyleSheet(Styles::logger(Styles::Theme::Dark));
    setAcceptDrops(false);
    setOpenLinks(false);
    setUndoRedoEnabled(false);
    setReadOnly(true);
    setContextMenuPolicy(Qt::NoContextMenu);
    document()->setMaximumBlockCount(400);

    QTextBlockFormat format;
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(format);

    // connection
    connect(&m_logTimer, &QTimer::timeout, this, [&]{

        if(m_logs.empty()){
            return;
        }

        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor.insertHtml(m_logs.join(""));
        m_logs.clear();
        setTextCursor(cursor);

    });
    m_logTimer.start(1000/15);
}

auto LoggerW::insert_log(QString log) -> void{
    m_logs.append(std::move(log));
}
