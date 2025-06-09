
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

#include "qt_ui.hpp"

// local
#include "qt_logger.hpp"

using namespace tool;
using namespace tool::ui;
using namespace Qt::Literals::StringLiterals;

auto W::init_frame_on_off(QFrame *f, bool on, bool enabled) -> QFrame*{
    const QSignalBlocker blocker(f);
    f->setStyleSheet(on ? "background-color:green;" : "background-color:red;");
    f->setEnabled(enabled);
    return f;
}

auto W::init(QComboBox *cb, QStringList items, QString currentItem, bool enabled) -> QComboBox*{
    const QSignalBlocker blocker(cb);
    cb->addItems(items);
    cb->setEnabled(enabled);
    cb->setCurrentText(currentItem);
    return cb;
}

auto W::init(QComboBox *cb, QStringList items, int index, bool enabled) -> QComboBox*{
    const QSignalBlocker blocker(cb);
    cb->addItems(items);
    cb->setEnabled(enabled);
    if(index < cb->count()){
        cb->setCurrentIndex(index);
    }
    return cb;
}

auto W::init(QCheckBox *cb, const QString &txt, bool checked, bool enabled) -> QCheckBox*{
    const QSignalBlocker blocker(cb);
    cb->setText(txt);
    cb->setChecked(checked);
    cb->setEnabled(enabled);
    return cb;
}

auto W::init(QRadioButton *rb, const QString &txt, bool checked, bool enabled) -> QRadioButton*{
    const QSignalBlocker blocker(rb);
    rb->setText(txt);
    rb->setEnabled(enabled);
    rb->setChecked(checked);
    return rb;
}

auto W::init(QSlider *sl, MinV<int> min, V<int> value, MaxV<int> max, StepV<int> singleStep, bool enabled) -> QSlider*{
    const QSignalBlocker blocker(sl);
    sl->setMinimum(min.v);
    sl->setMaximum(max.v);
    sl->setValue(value.v);
    sl->setSingleStep(singleStep.v);
    sl->setEnabled(enabled);
    return sl;
}

auto W::init(QPushButton *pb, const QString &txt, bool enabled) -> QPushButton*{
    const QSignalBlocker blocker(pb);
    pb->setText(txt);
    pb->setEnabled(enabled);
    return pb;
}

auto W::init(QTextEdit *te, const QString &txt, Qt::TextFormat tf, bool enabled) -> QTextEdit*{

    const QSignalBlocker blocker(te);
    if(tf == Qt::TextFormat::PlainText){
        te->setPlainText(txt);
    }else if(tf == Qt::TextFormat::RichText){
        te->setHtml(txt);
    }else{
        te->setMarkdown(txt);
    }
    te->setEnabled(enabled);
    return te;
}

auto W::init(QLineEdit *le, const QString &txt, bool enabled) -> QLineEdit*{
    const QSignalBlocker blocker(le);
    le->setText(txt);
    le->setEnabled(enabled);
    return le;
}

auto W::init(QLabel *la, const QString &txt, bool enabled) -> QLabel*{
    const QSignalBlocker blocker(la);
    la->setText(txt);
    la->setEnabled(enabled);
    return la;
}

auto W::init(QSpinBox *sb, MinV<int> min, V<int> value, MaxV<int> max, StepV<int> singleStep, bool enabled) -> QSpinBox*{
    const QSignalBlocker blocker(sb);
    sb->setMinimum(min.v);
    sb->setMaximum(max.v);
    sb->setValue(value.v);
    sb->setSingleStep(singleStep.v);
    sb->setEnabled(enabled);
    return sb;
}

auto W::init(QDoubleSpinBox *dsb, MinV<qreal> min, V<qreal> value, MaxV<qreal> max, StepV<qreal> singleStep, int decimals, bool enabled) -> QDoubleSpinBox*{
    const QSignalBlocker blocker(dsb);
    dsb->blockSignals(true);
    dsb->setDecimals(decimals);
    dsb->setMinimum(min.v);
    dsb->setMaximum(max.v);
    dsb->setValue(value.v);
    dsb->setSingleStep(singleStep.v);
    dsb->setEnabled(enabled);
    dsb->blockSignals(false);
    return dsb;
}

auto W::set(QLabel *la, const QString &txt) -> void{
    const QSignalBlocker blocker(la);
    la->setText(txt);
}

auto W::set(QSpinBox *sb, int value) -> void{
    const QSignalBlocker blocker(sb);
    sb->setValue(value);
}

auto W::set(QDoubleSpinBox *dsb, double value) -> void{
    const QSignalBlocker blocker(dsb);
    dsb->setValue(value);
}

auto W::set(QComboBox *cb, int index) -> void{
    const QSignalBlocker blocker(cb);
    cb->setCurrentIndex(index);
}

auto W::set(QComboBox *cb, const QString &txt) -> void{
    const QSignalBlocker blocker(cb);
    cb->setCurrentText(txt);
}

auto W::set(QLineEdit *le, const QString &txt) -> void{
    const QSignalBlocker blocker(le);
    le->setText(txt);
}

auto W::set(QCheckBox *cb, bool checked) -> void{
    const QSignalBlocker blocker(cb);
    cb->setChecked(checked);
}

auto W::set(QRadioButton *rb, bool checked) -> void{
    const QSignalBlocker blocker(rb);
    rb->setChecked(checked);
}

auto W::set(QDateTimeEdit *dte, QDateTime dateTime) -> void{
    const QSignalBlocker blocker(dte);
    dte->setDateTime(dateTime);
}

auto W::set(QDateTimeEdit *dte, QTime time) -> void{
    const QSignalBlocker blocker(dte);
    dte->setTime(time);
}


auto W::txt(const QString &txt, Qt::Alignment alignment) -> QLabel*{
    auto t = new QLabel(txt);
    t->setAlignment(alignment);
    return t;
}

auto W::icon(QIcon icon, QSize size) -> QLabel*{
    auto l = new QLabel();
    l->setPixmap(icon.pixmap(size));
    return l;
}

auto W::icon(QColor color, QSize size) -> QLabel*{
    auto l = new QLabel();
    auto p = QPixmap(size);
    p.fill(color);
    l->setPixmap(std::move(p));
    return l;
}

auto L::stretch(QLayout *layout, int stretchF) -> void{
    auto hbl = qobject_cast<QHBoxLayout*>(layout);
    if(hbl){
        hbl->addStretch(stretchF);
        return;
    }
    auto vbl = qobject_cast<QVBoxLayout*>(layout);
    if(vbl){
        vbl->addStretch(stretchF);
        return;
    }
}


auto F::h_line() -> QFrame*{
    auto line = new QFrame();
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(320, 150, 118, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QFrame *F::old_gen_frame(QLayout *layout, std::vector<std::pair<QWidget *, int> > widgetsStretch, int stretchFactor, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow){


    QFrame *frame = new QFrame();
    frame->setLayout(layout);
    layout->setContentsMargins(margins.left, margins.top, margins.right, margins.bottom);
    layout->setSpacing(margins.inter);

    for(auto widget : widgetsStretch){
        frame->layout()->addWidget(widget.first);
    }

    if(auto hbL = qobject_cast<QHBoxLayout*>(frame->layout()); hbL != nullptr){
        for(size_t ii = 0; ii < widgetsStretch.size(); ++ii){
            hbL->setStretch(static_cast<int>(ii), widgetsStretch[ii].second);
        }
        if(stretchFactor > 0){
            hbL->addStretch(stretchFactor);
        }

    }else if(auto vbL = qobject_cast<QVBoxLayout*>(frame->layout()); hbL != nullptr){
        for(size_t ii = 0; ii < widgetsStretch.size(); ++ii){
            vbL->setStretch(static_cast<int>(ii), widgetsStretch[ii].second);
        }
        if(stretchFactor > 0){
            vbL->addStretch(stretchFactor);
        }
    }

    frame->setFrameShadow(shadow);
    frame->setFrameShape(shape);

    return frame;
}

QFrame *F::old_gen_frame(QLayout *layout, std::vector<QWidget *> widgets, LStretchD strech, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow){

    QFrame *frame = new QFrame();
    frame->setLayout(layout);
    layout->setContentsMargins(margins.left, margins.top, margins.right, margins.bottom);
    layout->setSpacing(margins.inter);

    for(auto widget : widgets){
        frame->layout()->addWidget(widget);
    }

    if(auto hbL = qobject_cast<QHBoxLayout*>(frame->layout()); hbL != nullptr){

        for(size_t ii = 0; ii < strech.elementsStretch.size(); ++ii){
            if(ii < static_cast<size_t>(layout->count())){
                hbL->setStretch(static_cast<int>(ii), strech.elementsStretch[ii]);
            }
        }

        if(strech.spacerStretch > 0){
            hbL->addStretch(strech.spacerStretch);
        }


    }else if(auto vbL = qobject_cast<QVBoxLayout*>(frame->layout()); vbL != nullptr){

        for(size_t ii = 0; ii < strech.elementsStretch.size(); ++ii){
            if(ii < static_cast<size_t>(layout->count())){
                vbL->setStretch(static_cast<int>(ii), strech.elementsStretch[ii]);
            }
        }

        if(strech.spacerStretch > 0){
            vbL->addStretch(strech.spacerStretch);
        }
    }

    frame->setFrameShadow(shadow);
    frame->setFrameShape(shape);

    return frame;
}

QFrame *F::old_gen(QLayout *layout, std::vector<QWidget *> widgets, LStretch stretch, LMargins margins, QFrame::Shape shape, QFrame::Shadow shadow){
    QFrame *frame = new QFrame();
    frame->setLayout(layout);
    if(!margins.v){
        layout->setContentsMargins(0,0,0,0);
    }

    for(auto widget : widgets){
        frame->layout()->addWidget(widget);
    }

    auto *hbL = qobject_cast<QHBoxLayout*>(frame->layout());
    if(hbL && stretch.v){
        hbL->addStretch(20);
    }
    auto *vbL = qobject_cast<QVBoxLayout*>(frame->layout());
    if(vbL && stretch.v){
        vbL->addStretch(20);
    }
    frame->setFrameShadow(shadow);
    frame->setFrameShape(shape);

    return frame;
}

auto F::horiz(std::vector<WL> widgets, int spacerStretch, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow) -> QFrame* {
    return gen(L::HB(), std::move(widgets), spacerStretch, margins, shape, shadow);
}

auto F::vert(std::vector<WL> widgets, int spacerStretch, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow) -> QFrame* {
    return gen(L::VB(), std::move(widgets), spacerStretch, margins, shape, shadow);
}

auto F::grid(std::vector<WA> widgets, std::vector<int> rowsS, std::vector<int> colsS, int hStretch, int vStretch, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow) -> QFrame*{

    QFrame *frame = new QFrame();
    frame->setFrameShadow(shadow);
    frame->setFrameShape(shape);

    auto layout = L::G();
    layout->setContentsMargins(margins.left, margins.top, margins.right, margins.bottom);
    layout->setSpacing(margins.inter);
    frame->setLayout(layout);

    if(widgets.size() > rowsS.size()*colsS.size()){
        QtLog::error(u"F::grid error"_s);
        return frame;
    }

    size_t idW = 0;
    for(size_t ii = 0; ii < rowsS.size(); ++ii){
        layout->setRowStretch(static_cast<int>(ii), rowsS[ii]);
        for(size_t jj = 0; jj < colsS.size(); ++jj){
            if(idW < widgets.size()){
                layout->addWidget(widgets[idW].w, static_cast<int>(ii), static_cast<int>(jj), widgets[idW].a);
            }else{
                break;
            }
            ++idW;
        }
    }
    for(size_t ii = 0; ii < colsS.size(); ++ii){
        layout->setColumnStretch(static_cast<int>(ii), colsS[ii]);
    }

    if(vStretch != 0){
        layout->setRowStretch(layout->rowCount(), 100);
    }
    if(hStretch != 0){
        layout->setColumnStretch(layout->columnCount(), 100);
    }

    return frame;
}

auto F::grid2(std::vector<WAP> widgets, std::vector<int> rowsS, std::vector<int> colsS, int addRowStretch, int addColStretch, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow) -> QFrame*{

    QFrame *frame = new QFrame();
    frame->setFrameShadow(shadow);
    frame->setFrameShape(shape);

    auto layout = L::G();
    layout->setContentsMargins(margins.left, margins.top, margins.right, margins.bottom);
    layout->setSpacing(margins.inter);
    frame->setLayout(layout);

    for(auto &widget : widgets){
        layout->addWidget(widget.w, widget.rowP, widget.colP, widget.rowsS,  widget.colsS, widget.a);
    }
    for(size_t ii = 0; ii < rowsS.size(); ++ii){
        layout->setRowStretch(static_cast<int>(ii), rowsS[ii]);
    }
    for(size_t ii = 0; ii < colsS.size(); ++ii){
        layout->setColumnStretch(static_cast<int>(ii), colsS[ii]);
    }

    if(addRowStretch != 0){
        layout->setRowStretch(layout->rowCount(), addRowStretch);
    }
    if(addColStretch != 0){
        layout->setColumnStretch(layout->columnCount(), addColStretch);
    }

    return frame;
}

auto F::gen(QLayout *layout, std::vector<WL> widgets, int spacerStretch, LMarginsD margins, QFrame::Shape shape, QFrame::Shadow shadow) -> QFrame* {

    QFrame *frame = new QFrame();
    frame->setFrameShadow(shadow);
    frame->setFrameShape(shape);

    layout->setContentsMargins(margins.left, margins.top, margins.right, margins.bottom);
    layout->setSpacing(margins.inter);
    frame->setLayout(layout);

    for(auto widget : widgets){
        frame->layout()->addWidget(widget.w);
    }

    if(auto hbL = qobject_cast<QHBoxLayout*>(frame->layout()); hbL != nullptr){

        for(size_t ii = 0; ii < widgets.size(); ++ii){
            hbL->setStretch(static_cast<int>(ii), widgets[ii].stretchFactor);
        }

        if(spacerStretch > 0){
            hbL->addStretch(spacerStretch);
        }

    }else if(auto vbL = qobject_cast<QVBoxLayout*>(frame->layout()); vbL != nullptr){

        for(size_t ii = 0; ii < widgets.size(); ++ii){
            vbL->setStretch(static_cast<int>(ii), widgets[ii].stretchFactor);
        }

        if(spacerStretch > 0){
            vbL->addStretch(spacerStretch);
        }
    }

    return frame;
}
