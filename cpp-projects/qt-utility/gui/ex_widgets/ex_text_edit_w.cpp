
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

#include "ex_text_edit_w.hpp"



// qt-utility
#include "gui/widgets/qt_text_widget_highlighter.hpp"

using namespace tool::ex;

ExTextEditW::ExTextEditW(QString name) : ExItemW<QTextEdit>(UiType::Text_edit, name) {
    connect(w.get(), &QTextEdit::textChanged,this, [=]{trigger_ui_change();});
}

ExTextEditW *ExTextEditW::init_widget(QString txt, Qt::TextFormat tf, bool enabled){
    this->tf = tf;
    ui::W::init(w.get(), txt, tf, enabled);
    return this;
}


ExTextEditW *ExTextEditW::init_widget_as_csharp_editor(const QColor &bc, QString txt, bool enabled){

    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    w->setFont(font);

    // tab size
    QFontMetrics metrics(font);
    auto distance = metrics.horizontalAdvance("    ");
    w->setTabStopDistance(distance);

    ui::CSharpHighlighter *cshStartFunction = new ui::CSharpHighlighter(w->document(), &CSharpHighlighting::csharpHighlingRules);
    static_cast<void>(cshStartFunction);
    w->setStyleSheet(QString("background-color: rgb(%1,%2,%3); border: 0px ;").arg(bc.red()).arg(bc.green()).arg(bc.blue()));
    w->zoomIn(2);

    this->tf = Qt::TextFormat::PlainText;
    w->setPlainText(txt);
    w->setEnabled(enabled);
    return this;
}

void ExTextEditW::update_from_arg(const Arg &arg){

    ExItemW::update_from_arg(arg);

    w->blockSignals(true);

    if(arg.generator.has_value()){
        // ...
    }
    if(tf == Qt::TextFormat::PlainText){
        w->setPlainText(arg.to_string_value());
    }else if(tf == Qt::TextFormat::RichText){
        w->setText(arg.to_string_value());
    }else if(tf == Qt::TextFormat::MarkdownText){
        w->setMarkdown(arg.to_string_value());
    }

    w->blockSignals(false);
}

Arg ExTextEditW::convert_to_arg() const{

    Arg arg = ExBaseW::convert_to_arg();
    arg.init_from(get_text());

    // generator
    if(hasGenerator){
        // ...
    }
    return arg;
}

QString ExTextEditW::get_text() const{
    if(tf == Qt::TextFormat::PlainText){
        return w->toPlainText();
    }else if(tf == Qt::TextFormat::RichText){
        return w->toHtml();
    }else if(tf == Qt::TextFormat::MarkdownText){
        return w->toMarkdown();
    }

    return "";
}
