
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


#include "qt_fast_multi_curves_plot_w.hpp"

// Qt
#include <QPen>
#include <QToolTip>
#include <QMouseEvent>
#include <QMenu>

// Qwt
#include "qwt_plot_grid.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_textlabel.h"
#include "qwt_text.h"
#include "qwt_plot_canvas.h"
#include "qwt_scale_div.h"

// base
#include "utility/buffer.hpp"

using namespace Qt::Literals::StringLiterals;
using namespace tool::ui;

struct QtFastMultiCurvesPlotW::Impl{

    QwtPlotCanvas *canvas = nullptr;
    QwtPlotGrid *grid = nullptr;
    QwtPlotTextLabel *leftTitle = nullptr;

    std::vector<std::unique_ptr<QwtPlotCurve>> curves;
    std::vector<QPen> pCurves;

    std::vector<std::unique_ptr<QwtPlotMarker>> vMarkers;
    // std::vector<QPen> pVMarkers;
    std::vector<std::unique_ptr<QwtPlotMarker>> hMarkers;
    // std::vector<QPen> pHMarkers;


    Buffer<QString> curvesNames ={
        "Default name"
    };
    Buffer<QColor> curvesColors = {
        Qt::blue,
    };
    Buffer<double> curvesWidth ={
        1.5
    };


    // y scale
    bool automaticYScale = true;
    double startYScale = -100.;
    double endYScale   = +100.;
    bool addTempMeanOffset = false;
    double yTickStep = 0.0;
};

QtFastMultiCurvesPlotW::QtFastMultiCurvesPlotW() : i(std::make_unique<Impl>()){

    // set background
    i->canvas = new QwtPlotCanvas();
    // i->canvas->setPalette( QColor( "NavajoWhite" ) );
    // i->canvas->setPalette( QColor( "khaki" ) );

    // this->setContextMenuPolicy(Qt::CustomContextMenu);
    // this->customContextMenuRequested();
    // self.ui.tableWidget.setContextMenuPolicy(Qt.CustomContextMenu)
        // self.ui.tableWidget.customContextMenuRequested.connect(self.openMenu)

    i->canvas->setPalette( QColor( "NavajoWhite" ) );
    i->canvas->setPalette( Qt::white);

    i->canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    i->canvas->setLineWidth(1);


    setCanvas(i->canvas);

    // set grid
    i->grid = new QwtPlotGrid();
    i->grid->attach(this);
    // set_grid_lines_display(true, true, true, true);
    // set label
    i->leftTitle = new QwtPlotTextLabel();
    i->leftTitle->attach(this);
    set_left_title("default");


    // set plot settings
    setAutoReplot(false);
    setMouseTracking(true);

    // m_contextMenu = new QMenu();
    // m_contextMenu->addAction("test");
}

QtFastMultiCurvesPlotW::~QtFastMultiCurvesPlotW(){
    for(auto &curve : i->curves){
        curve->detach();
    }
    for(auto &marker : i->vMarkers){
        marker->detach();
    }
}

auto QtFastMultiCurvesPlotW::set_curves_colors(std::span<QColor> colors, bool update) -> void{
    // OK
    i->curvesColors.resize(colors.size());
    std::copy(colors.begin(), colors.end(), i->curvesColors.begin());
    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_curves_names(std::span<QString> names, bool update) -> void{
    // OK
    i->curvesNames.resize(names.size());
    std::copy(names.begin(), names.end(), i->curvesNames.begin());
    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_curve_name(size_t idCurve, const QString &name, bool update) -> void{
    // OK
    if(idCurve >= i->curvesNames.size()){
        i->curvesNames.resize(idCurve+1);
    }
    i->curvesNames[idCurve] = name;
    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_curves_identical_width(double width, bool update) -> void{
    // OKf
    i->curvesWidth.fill(width);
    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_curves_widths(std::span<double> widths, bool update) -> void{
    // OK
    i->curvesWidth.resize(widths.size());
    std::copy(widths.begin(), widths.end(), i->curvesWidth.begin());
    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_curve_width(size_t idCurve, double width, bool update) -> void{
    // OK
    if(idCurve >= i->curvesWidth.size()){
        i->curvesWidth.resize(idCurve+1);
    }
    i->curvesWidth[idCurve] = width;
    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_curve_color(size_t idCurve, const QColor &color, bool update) -> void{
    // OK
    if(idCurve >= i->curvesColors.size()){
        i->curvesColors.resize(idCurve+1);
    }
    i->curvesColors[idCurve] = color;
    if(update){
        update_curves_info();
    }
}



auto QtFastMultiCurvesPlotW::set_curve_points(size_t idCurve, const std::span<const double> x, const std::span<const double> y) -> void{

//        # update fft and eeg curves
//             with self._gui_lock:
//                 eeg_channel_height = self._eeg_channel_height
//             if eeg_channel_height == 0.0:
//                 eeg_channel_height = self._eeg_data.max() - self._eeg_data.min()

//             with self._eeg_lock:
//                 for c in range(0, len(eeg_lines)):
//                     eeg_lines[c].set_ydata(
//                         self._eeg_data[c] / eeg_channel_height
//                         + float(self._channels - c)
//                     )


    if(idCurve >= i->curves.size()){
        return;
    }

    auto first = x.front();
    auto last  = x.back();

    std::vector<std::unique_ptr<QwtPlotMarker>> nvMarkers;
    for(auto &marker : i->vMarkers){
        if(marker->xValue() < first || marker->xValue() > last){
            marker->detach();
            marker = nullptr;
        }else{
            nvMarkers.push_back(std::move(marker));
        }
    }
    std::swap(i->vMarkers, nvMarkers);

    setAxisAutoScale(xBottom, false);


    auto factor = 30.0 / width();

    setAxisScale(xBottom, first - (last-first)*factor, last);

    if(i->automaticYScale){
        setAxisAutoScale(QwtAxis::YLeft, true);
    }else{
        setAxisAutoScale(QwtAxis::YLeft, false);

        if(!i->addTempMeanOffset){
            setAxisScale(QwtAxis::YLeft, i->startYScale, i->endYScale);
        }else{
            double mean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
            setAxisScale(QwtAxis::YLeft, i->startYScale+mean, i->endYScale+mean);
        }
    }

    if(x.size() == y.size() && !x.empty() && idCurve < i->curves.size()){
        i->curves[idCurve]->setRawSamples(
            x.data(),
            y.data(), static_cast<int>(x.size())
        );
    }

    replot();
}

auto QtFastMultiCurvesPlotW::set_nb_curves(size_t nbCurves, bool update) -> void{

    // OK
    for(auto &curve : i->curves){
        curve->detach();
    }

    i->curves.resize(nbCurves);
    i->pCurves.resize(nbCurves);
    for(size_t ii = 0; ii < nbCurves; ++ii){
        auto curve = std::make_unique<QwtPlotCurve>("");
        curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
        // curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true);
        // curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, true);
        // curve->setPaintAttribute(QwtPlotCurve::FilterPointsAggressive, true);
        curve->attach(this);
        i->curves[ii] = std::move(curve);
    }

    if(update){
        update_curves_info();
    }
}

auto QtFastMultiCurvesPlotW::set_grid_lines_display(bool showX, bool showY, bool showMinX, bool showMinY) -> void{
    i->grid->enableX(showX);
    i->grid->enableY(showY);
    i->grid->enableXMin(showMinX);
    i->grid->enableYMin(showMinY);
}


auto QtFastMultiCurvesPlotW::set_nb_horiontal_markers(size_t nbMarkers) -> void{
    i->hMarkers.clear();
    // int offsetY = height()/nbMarkers;
    // int currentY = 0;
    // auto max = axisInterval(QwtAxis::YLeft).maxValue();
    // auto min = axisInterval(QwtAxis::YLeft).minValue();
    //(1.0*ii/nbMarkers) *(max-min)
    for(size_t ii = 0; ii < nbMarkers; ++ii){
        add_horizontal_marker(0, u"M%1"_s.arg(QString::number(ii)));
        // currentY += offsetY;
    }
}

auto QtFastMultiCurvesPlotW::set_horizontal_marker_info(size_t idM, double y, const QString &label) -> void{
    if(idM < i->hMarkers.size()){
        i->hMarkers[idM]->setLabel(label);
        i->hMarkers[idM]->setYValue(y);
    }
}

auto QtFastMultiCurvesPlotW::add_vertical_marker(double x, const QString &label) -> void{

    auto marker = std::make_unique<QwtPlotMarker>();
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setXValue(x);

    QPen pen;
    pen.setWidthF(2.0);
    pen.setColor(Qt::black);

    QwtText t(label);
    marker->setLabel(t);

    marker->setLinePen(pen);
    marker->attach(this);
    i->vMarkers.push_back(std::move(marker));
}


auto QtFastMultiCurvesPlotW::add_horizontal_marker(double y, const QString &label) -> void{

    auto marker = std::make_unique<QwtPlotMarker>();
    marker->setLineStyle(QwtPlotMarker::HLine);
    marker->setYValue(y);

    QPen pen;
    pen.setWidthF(0.2);
    pen.setColor(Qt::black);
    marker->setLinePen(pen);

    QwtText t(label);
    marker->setLabel(t);
    marker->setLabelAlignment(Qt::AlignLeft);
    marker->attach(this);
    i->hMarkers.push_back(std::move(marker));
}

auto QtFastMultiCurvesPlotW::get_horizontal_marker_name(size_t idM) const -> QString {
    if(idM < i->hMarkers.size()){
        return i->hMarkers[idM]->label().text();
    }
    return "";
}

auto QtFastMultiCurvesPlotW::set_left_title(const QString &title, const QColor &color) -> void{
    auto t = QwtText(title);
    QFont font;
    font.setBold(true);
    t.setFont(font);
    t.setRenderFlags( Qt::AlignLeft | Qt::AlignTop );
    t.setColor(color);
    i->leftTitle->setText(t);
}



auto QtFastMultiCurvesPlotW::update_curves_info() -> void{
    for(size_t idC = 0; idC < i->curves.size(); ++idC){
        auto &pCurve = i->pCurves[idC];

        if(idC < i->curvesColors.size()){
            pCurve.setColor(i->curvesColors[idC]);
        }else{
            pCurve.setColor(Qt::black);
        }

        auto curve = i->curves[idC].get();
        if(idC < i->curvesNames.size()){
            curve->setTitle(i->curvesNames[idC]);
        }else{
            curve->setTitle(u"Default name"_s);
        }

        if(idC < i->curvesWidth.size()){
            pCurve.setWidthF(i->curvesWidth[idC]);
        }else{
            pCurve.setWidthF(1.5);
        }

        curve->setPen(pCurve);
    }
}

// void QtFastMultiCurvesPlotW::contextMenuEvent(QContextMenuEvent *event){
//     QwtPlot::contextMenuEvent(event);
//     qDebug() << "show menu " << event->globalPos();
//     // QMenu menu(this);
//     // menu.addAction(cutAct);
//     // menu.addAction(copyAct);
//     // menu.addAction(pasteAct);
//     // menu.exec(event->globalPos());
// }


auto QtFastMultiCurvesPlotW::set_curve_visibility(size_t idCurve, bool state) -> void{
    if(idCurve >= i->curves.size()){
        return;
    }
    i->curves[idCurve]->setVisible(state);
}


auto QtFastMultiCurvesPlotW::disable_y_ticks() -> void{
    QwtScaleDiv d;
    d.setInterval(0.,1.0);
    d.setTicks(static_cast<int>(QwtScaleDiv::TickType::NoTick),{});
    setAxisScaleDiv( QwtAxis::YLeft, d);
}

// auto QtFastMultiCurvesPlotW::show_tooltip(const QPoint &globalPos, const QString &text) -> void{
//     QToolTip::showText(globalPos,text);
// }

void QtFastMultiCurvesPlotW::mousePressEvent(QMouseEvent *event){

    bool leftB  = (event->button() == Qt::MouseButton::LeftButton);
    bool rightB = (event->button() == Qt::MouseButton::RightButton);

    QString label;
    int closestMarkerId = 0;

    if(!i->hMarkers.empty()){

        auto factor = 1.0-1.0*event->pos().y() / height();
        auto distMin = std::numeric_limits<double>::max();

        for(size_t idM = 0; idM < i->hMarkers.size(); ++idM){
            auto m = i->hMarkers[idM].get();
            auto dist = std::abs(factor - m->yValue());
            if(dist < distMin){
                distMin = dist;
                label = m->label().text();
                closestMarkerId = idM;
            }
        }

        for(const auto &m : i->hMarkers){
            auto dist = std::abs(factor - m->yValue());
            if(dist < distMin){
                distMin = dist;
                label = m->label().text();
            }
        }


        //
    }

    if(leftB){
        if(!m_isLeftPressed){
            m_isLeftPressed = true;
            QToolTip::showText(this->mapToGlobal(event->pos()), u"Electrode: "_s % label);
            emit left_click_pressed_signal(this->mapToGlobal(event->pos()), closestMarkerId, label);
        }
    }

    if(rightB){
        emit right_click_pressed_signal(this->mapToGlobal(event->pos()), closestMarkerId, label);
        // if(!m_isRightPressed){
        //     m_isRightPressed = true;
        // }
    }
}

void QtFastMultiCurvesPlotW::mouseReleaseEvent(QMouseEvent *event){

    if(event->button() == Qt::MouseButton::LeftButton){
        m_isLeftPressed = false;
        QToolTip::hideText();
        emit left_click_released_signal();
    }else if(event->button() == Qt::MouseButton::RightButton){
        // m_isRightPressed = false;
        emit right_click_released_signal();
    }
}


auto QtFastMultiCurvesPlotW::set_x_axis_scaling_infos(bool automatic, double startScaleX, double endScaleX, double step, bool visible) -> void{
    setAxisAutoScale(QwtAxis::XBottom, automatic);
    setAxisScale(QwtAxis::XBottom, startScaleX, endScaleX, step);
    setAxisVisible(QwtAxis::XBottom, visible);
}


auto QtFastMultiCurvesPlotW::set_y_axis_scaling_infos(bool automatic, double startScaleY, double endScaleY, bool addTempMeanOffset, double step, bool visible) -> void{

    i->automaticYScale      = automatic;
    i->startYScale          = startScaleY;
    i->endYScale            = endScaleY;
    i->addTempMeanOffset    = addTempMeanOffset;
    i->yTickStep            = step;

    setAxisAutoScale(QwtAxis::YLeft, i->automaticYScale);
    setAxisScale(QwtAxis::YLeft, i->startYScale, i->endYScale, step);
    setAxisVisible(QwtAxis::YLeft, visible);

    replot();
}
