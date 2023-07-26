
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


#include "ex_transformation_w.hpp"

using namespace tool;
using namespace tool::ex;

ExTransformationW::ExTransformationW(QString name) : ExItemW<QFrame>(UiType::Transformation, name){

    w->setFrameShadow(QFrame::Raised);
    w->setFrameShape(QFrame::Shape::Box);

    // T    T       T       T
    // TTr  trX     trY     trZ
    // TRot rotX    rotY    rotZ
    // TSc  scX     scY     scZ
    layout = new QGridLayout();
    w->setLayout(layout);
    layout->addWidget(trName = ui::W::txt("Default"),0,0,1,4);
    layout->addWidget(ui::W::txt("Translation"), 1, 0, 1, 1);
    layout->addWidget(trX(), 1, 1, 1, 1);
    layout->addWidget(trY(), 1, 2, 1, 1);
    layout->addWidget(trZ(), 1, 3, 1, 1);
    layout->addWidget(ui::W::txt("Rotation"), 2, 0, 1, 1);
    layout->addWidget(rotX(), 2, 1, 1, 1);
    layout->addWidget(rotY(), 2, 2, 1, 1);
    layout->addWidget(rotZ(), 2, 3, 1, 1);
    layout->addWidget(ui::W::txt("Scale"), 3, 0, 1, 1);
    layout->addWidget(scX(), 3, 1, 1, 1);
    layout->addWidget(scY(), 3, 2, 1, 1);
    layout->addWidget(scZ(), 3, 3, 1, 1);

    connect(&trX,   &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&trY,   &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&trZ,   &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&rotX,  &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&rotY,  &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&rotZ,  &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&scX,   &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&scY,   &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
    connect(&scZ,   &ExBaseW::ui_change_signal, this, &ExBaseW::ui_change_signal);
}

ExTransformationW *ExTransformationW::init_widget(const QString &title, Vector3dSettings transS, Vector3dSettings rotationS, Vector3dSettings scaleS, bool enabled){

    trName->setText(QSL("<b>") % title % QSL("</b>"));

    trX.init_widget(transS.sX);
    trY.init_widget(transS.sY);
    trZ.init_widget(transS.sZ);
    rotX.init_widget(rotationS.sX);
    rotY.init_widget(rotationS.sY);
    rotZ.init_widget(rotationS.sZ);
    scX.init_widget(scaleS.sX);
    scY.init_widget(scaleS.sY);
    scZ.init_widget(scaleS.sZ);
    w->setEnabled(enabled);
    return this;
}

std::vector<std::any> ExTransformationW::generate_init_any_array(const QString &title, Vector3dSettings transS, Vector3dSettings rotationS, Vector3dSettings scaleS, bool enabled){
    return {
        std::make_any<QString>(title),
        std::make_any<Vector3dSettings>(transS),
        std::make_any<Vector3dSettings>(rotationS),
        std::make_any<Vector3dSettings>(scaleS),
        std::make_any<bool>(enabled)
    };
}

ExBaseW *ExTransformationW::init_widget_from_any_array(std::vector<std::any> &parameters){
    return init_widget(
        std::any_cast<QString>(parameters[0]),
        std::any_cast<Vector3dSettings>(parameters[1]),
        std::any_cast<Vector3dSettings>(parameters[2]),
        std::any_cast<Vector3dSettings>(parameters[3]),
        std::any_cast<bool>(parameters[4])
    );
}

void ExTransformationW::update_from_arg(const Arg &arg){

    ExItemW::update_from_arg(arg);

    auto size = arg.count();
    if(size < 9){
        qWarning() << "Invalid argument value " << arg.name << " for transformation widget.";
        return;
    }

    w->blockSignals(true);

    if(arg.generator.has_value()){

        if(auto dec = arg.generator->decimals; dec.has_value()){
            if(const auto split = dec.value().split(arg.separator()); split.size() > 2){
                set_trans_decimals(split[0].toInt());
                set_rotation_decimals(split[1].toInt());
                set_scale_decimals(split[2].toInt());
            }
        }

        if(auto min = arg.generator->min; min.has_value()){
            if(const auto split = min.value().split(arg.separator()); split.size() > 2){
                const auto tr = split[0].toDouble();
                const auto rt = split[1].toDouble();
                const auto sc = split[2].toDouble();
                set_trans_min({tr,tr,tr});
                set_rotation_min({rt,rt,rt});
                set_scale_min({sc,sc,sc});
            }
        }

        if(auto max = arg.generator->max; max.has_value()){
            if(const auto split = max.value().split(arg.separator()); split.size() > 2){
                const auto tr = split[0].toDouble();
                const auto rt = split[1].toDouble();
                const auto sc = split[2].toDouble();
                set_trans_max({tr,tr,tr});
                set_rotation_max({rt,rt,rt});
                set_scale_max({sc,sc,sc});
            }
        }

        if(auto step = arg.generator->step; step.has_value()){
            if(const auto split = step.value().split(arg.separator()); split.size() > 2){
                const auto tr = split[0].toDouble();
                const auto rt = split[1].toDouble();
                const auto sc = split[2].toDouble();
                set_trans_steps({tr,tr,tr});
                set_rotation_steps({rt,rt,rt});
                set_scale_steps({sc,sc,sc});
            }
        }
    }

    if(auto splitV = arg.split_value(); splitV.size() > 8){
        set_trans_values({splitV[0].toDouble(),splitV[1].toDouble(),splitV[2].toDouble()});
        set_rotation_values({splitV[3].toDouble(),splitV[4].toDouble(),splitV[5].toDouble()});
        set_scale_values({splitV[6].toDouble(),splitV[7].toDouble(),splitV[8].toDouble()});
    }

    w->blockSignals(false);
}

Arg ExTransformationW::convert_to_arg() const{

    Arg arg = ExBaseW::convert_to_arg();
    arg.init_from_args({
        trX.convert_to_arg(),trY.convert_to_arg(),trZ.convert_to_arg(),
        rotX.convert_to_arg(),rotY.convert_to_arg(),rotZ.convert_to_arg(),
        scX.convert_to_arg(),scY.convert_to_arg(),scZ.convert_to_arg()},
        " ", UnityType::System_single
    );

    // generator
    if(hasGenerator){
        arg.generator->min = QString::number(trX.w->minimum())   % arg.separator() %
                            QString::number(rotX.w->minimum())  % arg.separator() %
                            QString::number(scX.w->minimum());

        arg.generator->max = QString::number(trX.w->maximum())   % arg.separator() %
                            QString::number(rotX.w->maximum())  % arg.separator() %
                            QString::number(scX.w->maximum());

        arg.generator->step = QString::number(trX.w->singleStep())   % arg.separator() %
                             QString::number(rotX.w->singleStep())  % arg.separator() %
                             QString::number(scX.w->singleStep());

        arg.generator->decimals = QString::number(trX.w->decimals())   % arg.separator() %
                                 QString::number(rotX.w->decimals())  % arg.separator() %
                                 QString::number(scX.w->decimals());
    }

    return arg;
}

void ExTransformationW::set_from_transform(ExTransformationW *tr){
    trX.w->setValue(tr->trX.w->value());
    trY.w->setValue(tr->trY.w->value());
    trZ.w->setValue(tr->trZ.w->value());
    rotX.w->setValue(tr->rotX.w->value());
    rotY.w->setValue(tr->rotY.w->value());
    rotZ.w->setValue(tr->rotZ.w->value());
    scX.w->setValue(tr->scX.w->value());
    scY.w->setValue(tr->scY.w->value());
    scZ.w->setValue(tr->scZ.w->value());
}

void ExTransformationW::set_enable_state(bool enableTranslation, bool enableRotation, bool enableScale){
    trX.w->setEnabled(enableTranslation);
    trY.w->setEnabled(enableTranslation);
    trZ.w->setEnabled(enableTranslation);
    rotX.w->setEnabled(enableRotation);
    rotY.w->setEnabled(enableRotation);
    rotZ.w->setEnabled(enableRotation);
    scX.w->setEnabled(enableScale);
    scY.w->setEnabled(enableScale);
    scZ.w->setEnabled(enableScale);
}

void ExTransformationW::set_trans_decimals(int d){
    trX.blockSignals(true);
    trY.blockSignals(true);
    trZ.blockSignals(true);
    trX.w->setDecimals(d);
    trY.w->setDecimals(d);
    trZ.w->setDecimals(d);
    trX.blockSignals(false);
    trY.blockSignals(false);
    trZ.blockSignals(false);
}

void ExTransformationW::set_trans_min(geo::Pt3<qreal> m){
    trX.blockSignals(true);
    trY.blockSignals(true);
    trZ.blockSignals(true);
    trX.w->setMinimum(m.x());
    trY.w->setMinimum(m.y());
    trZ.w->setMinimum(m.z());
    trX.blockSignals(false);
    trY.blockSignals(false);
    trZ.blockSignals(false);
}

void ExTransformationW::set_trans_max(geo::Pt3<qreal> m){
    trX.blockSignals(true);
    trY.blockSignals(true);
    trZ.blockSignals(true);
    trX.w->setMaximum(m.x());
    trY.w->setMaximum(m.y());
    trZ.w->setMaximum(m.z());
    trX.blockSignals(false);
    trY.blockSignals(false);
    trZ.blockSignals(false);
}

void ExTransformationW::set_trans_steps(geo::Pt3<qreal> s){
    trX.blockSignals(true);
    trY.blockSignals(true);
    trZ.blockSignals(true);
    trX.w->setSingleStep(s.x());
    trY.w->setSingleStep(s.y());
    trZ.w->setSingleStep(s.z());
    trX.blockSignals(false);
    trY.blockSignals(false);
    trZ.blockSignals(false);
}

void ExTransformationW::set_trans_values(geo::Pt3<qreal> v){
    trX.blockSignals(true);
    trY.blockSignals(true);
    trZ.blockSignals(true);
    trX.w->setValue(v.x());
    trY.w->setValue(v.y());
    trZ.w->setValue(v.z());
    trX.blockSignals(false);
    trY.blockSignals(false);
    trZ.blockSignals(false);
}

void ExTransformationW::set_rotation_decimals(int d){
    rotX.blockSignals(true);
    rotY.blockSignals(true);
    rotZ.blockSignals(true);
    rotX.w->setDecimals(d);
    rotY.w->setDecimals(d);
    rotZ.w->setDecimals(d);
    rotX.blockSignals(false);
    rotY.blockSignals(false);
    rotZ.blockSignals(false);
}

void ExTransformationW::set_rotation_min(geo::Pt3<qreal> m){
    rotX.blockSignals(true);
    rotY.blockSignals(true);
    rotZ.blockSignals(true);
    rotX.w->setMinimum(m.x());
    rotY.w->setMinimum(m.y());
    rotZ.w->setMinimum(m.z());
    rotX.blockSignals(false);
    rotY.blockSignals(false);
    rotZ.blockSignals(false);
}

void ExTransformationW::set_rotation_max(geo::Pt3<qreal> m){
    rotX.blockSignals(true);
    rotY.blockSignals(true);
    rotZ.blockSignals(true);
    rotX.w->setMaximum(m.x());
    rotY.w->setMaximum(m.y());
    rotZ.w->setMaximum(m.z());
    rotX.blockSignals(false);
    rotY.blockSignals(false);
    rotZ.blockSignals(false);
}

void ExTransformationW::set_rotation_steps(geo::Pt3<qreal> s){
    rotX.blockSignals(true);
    rotY.blockSignals(true);
    rotZ.blockSignals(true);
    rotX.w->setSingleStep(s.x());
    rotY.w->setSingleStep(s.y());
    rotZ.w->setSingleStep(s.z());
    rotX.blockSignals(false);
    rotY.blockSignals(false);
    rotZ.blockSignals(false);
}

void ExTransformationW::set_rotation_values(geo::Pt3<qreal> v){
    rotX.blockSignals(true);
    rotY.blockSignals(true);
    rotZ.blockSignals(true);
    rotX.w->setValue(v.x());
    rotY.w->setValue(v.y());
    rotZ.w->setValue(v.z());
    rotX.blockSignals(false);
    rotY.blockSignals(false);
    rotZ.blockSignals(false);
}

void ExTransformationW::set_scale_decimals(int d){
    scX.blockSignals(true);
    scY.blockSignals(true);
    scZ.blockSignals(true);
    scX.w->setDecimals(d);
    scY.w->setDecimals(d);
    scZ.w->setDecimals(d);
    scX.blockSignals(false);
    scY.blockSignals(false);
    scZ.blockSignals(false);
}

void ExTransformationW::set_scale_min(geo::Pt3<qreal> m){
    scX.blockSignals(true);
    scY.blockSignals(true);
    scZ.blockSignals(true);
    scX.w->setMinimum(m.x());
    scY.w->setMinimum(m.y());
    scZ.w->setMinimum(m.z());
    scX.blockSignals(false);
    scY.blockSignals(false);
    scZ.blockSignals(false);
}

void ExTransformationW::set_scale_max(geo::Pt3<qreal> m){
    scX.blockSignals(true);
    scY.blockSignals(true);
    scZ.blockSignals(true);
    scX.w->setMaximum(m.x());
    scY.w->setMaximum(m.y());
    scZ.w->setMaximum(m.z());
    scX.blockSignals(false);
    scY.blockSignals(false);
    scZ.blockSignals(false);
}

void ExTransformationW::set_scale_steps(geo::Pt3<qreal> s){
    scX.blockSignals(true);
    scY.blockSignals(true);
    scZ.blockSignals(true);
    scX.w->setSingleStep(s.x());
    scY.w->setSingleStep(s.y());
    scZ.w->setSingleStep(s.z());
    scX.blockSignals(false);
    scY.blockSignals(false);
    scZ.blockSignals(false);
}

void ExTransformationW::set_scale_values(geo::Pt3<qreal> v){
    scX.blockSignals(true);
    scY.blockSignals(true);
    scZ.blockSignals(true);
    scX.w->setValue(v.x());
    scY.w->setValue(v.y());
    scZ.w->setValue(v.z());
    scX.blockSignals(false);
    scY.blockSignals(false);
    scZ.blockSignals(false);
}

void ExTransformationW::set_as_generator(){
    ExItemW::set_as_generator();
    trX.set_as_generator();
    trY.set_as_generator();
    trZ.set_as_generator();
    rotX.set_as_generator();
    rotY.set_as_generator();
    rotZ.set_as_generator();
    scX.set_as_generator();
    scY.set_as_generator();
    scZ.set_as_generator();
}

