
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
#include <QPlainTextEdit>
#include <QCompleter>

namespace tool::ui{

class QtCodeEditorW : public QPlainTextEdit{
    Q_OBJECT

public:
    QtCodeEditorW(QWidget *parent = nullptr);

    auto line_number_area_paint_event(QPaintEvent *event) -> void;
    auto line_number_area_width() -> int;

    // auto setCompleter(QCompleter *completer) -> void;
    // auto completer() const -> QCompleter*{return c;};

protected:
    auto resizeEvent(QResizeEvent *event) -> void override;
    // auto keyPressEvent(QKeyEvent *e) -> void override;
    // auto focusInEvent(QFocusEvent *e) -> void override;

private slots:

    auto highlight_current_line() -> void;
    auto update_line_number_area(const QRect &rect, int dy) -> void;
    // auto update_line_number_area_width(int newBlockCount) -> void;
    // auto insertCompletion(const QString &completion) -> void;

public:

    QColor highlightedLineColor = QColor(80,80,80);
    int offsetLineCounter = 0;

private:
    QWidget *lineNumberArea = nullptr;
    // QString textUnderCursor() const;
    // QCompleter *c = nullptr;
    // QStringList words;
};

class QtLineNumberAreaW : public QWidget{

public:

    QtLineNumberAreaW(QtCodeEditorW *editor) : QWidget(editor), codeEditor(editor){}

    auto sizeHint() const -> QSize override{
        return QSize(codeEditor->line_number_area_width(), 0);
    }

protected:
    auto paintEvent(QPaintEvent *event) -> void override{
        codeEditor->line_number_area_paint_event(event);
    }

private:
    QtCodeEditorW *codeEditor = nullptr;
};

}
