
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

#include "ex_combo_box_text_w.hpp"

using namespace tool;
using namespace tool::ex;

ExComboBoxTextW::ExComboBoxTextW(QString name) : ExItemW<QComboBox>(UiType::Combo_box_text, name){
    connect(w.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{trigger_ui_change();});
}

ExComboBoxTextW *ExComboBoxTextW::init_widget(QStringList items, Index index, bool enabled){
    ui::W::init(w.get(), items, enabled);
    if(index.v < w->count()){
        w->setCurrentIndex(index.v);
    }
    return this;
}

ExComboBoxTextW *ExComboBoxTextW::init_widget(QStringList items, QString currentText, bool enabled){
    ui::W::init(w.get(), items, enabled);
    if(currentText.length() > 0){
        w->setCurrentText(currentText);
    }
    return this;
}

void ExComboBoxTextW::update_from_arg(const Arg &arg){

    ExItemW::update_from_arg(arg);

    w->blockSignals(true);

    if(arg.generator.has_value()){
        if(const auto &info = arg.generator->info; info.has_value()){
            init_widget(arg.generator->info.value().split("|"), arg.to_string_value());
        }else{
            qWarning() << "ExComboBoxTextW Invalid generator.";
        }
    }else{
        const auto text  = arg.to_string_value();
        if(text.length() > 0){
            w->setCurrentText(text);
        }
    }

    w->blockSignals(false);
}

Arg ExComboBoxTextW::convert_to_arg() const{

    Arg arg = ExBaseW::convert_to_arg();
    arg.init_from(w->currentText());

    // generator
    if(hasGenerator){
        arg.generator->info = "";
        for(int ii = 0; ii < w->count(); ++ii){
            arg.generator->info.value() += w->itemText(ii);
            if(ii < w->count()-1){
                arg.generator->info.value() += "|";
            }
        }
    }
    return arg;
}
