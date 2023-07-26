
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

#include "ex_spin_box_w.hpp"

// qt-utility
#include "qt_str.hpp"

using namespace tool;
using namespace tool::ex;


ExSpinBoxW *ExSpinBoxW::init_widget(MinV<int> min, V<int> value, MaxV<int> max, StepV<int> singleStep, bool enabled){
    ui::W::init(w.get(), min,value,max,singleStep,enabled);
    w->setMinimumWidth(30);
    connect(w.get(), QOverload<int>::of(&QSpinBox::valueChanged), this, [=]{trigger_ui_change();});
    return this;
}

auto ExSpinBoxW::increment() -> void{
    w->setValue(std::clamp(w->value()+w->singleStep(), w->minimum(), w->maximum()));
}

auto ExSpinBoxW::decrement() -> void{
    w->setValue(std::clamp(w->value()-w->singleStep(), w->minimum(), w->maximum()));
}


void ExSpinBoxW::update_from_arg(const Arg &arg){

    ExItemW::update_from_arg(arg);

    w->blockSignals(true);

    if(arg.generator.has_value()){

        if(const auto &min = arg.generator->min,
                      &max = arg.generator->max,
                      &step = arg.generator->step;
            min.has_value() && max.has_value() && step.has_value()){

            init_widget(
                MinV<int>{min.value().toInt()},
                V<int>{arg.to_int_value()},
                MaxV<int>{max.value().toInt()},
                StepV<int>{step.value().toInt()}
            );
        }else{
            qWarning() << "ExSpinBoxW Invalid generator";
        }
    }else{
        w->setValue(arg.to_int_value());
    }

    w->blockSignals(false);
}

Arg ExSpinBoxW::convert_to_arg() const{

    Arg arg = ExBaseW::convert_to_arg();
    arg.init_from(w->value());

    // generator
    if(hasGenerator){
        arg.generator->min  = QString::number(w->minimum());
        arg.generator->max  = QString::number(w->maximum());
        arg.generator->step = QString::number(w->singleStep());
    }
    return arg;
}

