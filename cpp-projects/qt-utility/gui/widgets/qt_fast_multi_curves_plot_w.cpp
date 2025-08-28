
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

// Qwt
#include "qwt_plot_grid.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_textlabel.h"
#include "qwt_text.h"
#include "qwt_plot_canvas.h"

using namespace Qt::Literals::StringLiterals;
using namespace tool::ui;

struct QtFastMultiCurvesPlotW::Impl{

    QwtPlotCanvas *canvas = nullptr;
    QwtPlotGrid *grid = nullptr;
    QwtPlotTextLabel *leftTitle = nullptr;
    std::vector<std::unique_ptr<QwtPlotCurve>> curves;
    std::vector<std::unique_ptr<QwtPlotMarker>> vMarkers;

    std::vector<std::unique_ptr<QwtPlotMarker>> hMarkers;

    bool automaticYScale = true;
    double startYScale = -100.;
    double endYScale   = +100.;
    bool addTempMeanOffset = false;
};

QtFastMultiCurvesPlotW::QtFastMultiCurvesPlotW() : i(std::make_unique<Impl>()){

    // set background
    i->canvas = new QwtPlotCanvas();
    i->canvas->setPalette( QColor( "NavajoWhite" ) );
    i->canvas->setPalette( QColor( "khaki" ) );
    i->canvas->setFrameStyle( QFrame::Box | QFrame::Plain );
    i->canvas->setLineWidth(1);
    setCanvas(i->canvas);

    // set grid
    i->grid = new QwtPlotGrid();
    i->grid->attach(this);


    // set label
    i->leftTitle = new QwtPlotTextLabel();
    i->leftTitle->attach(this);
    set_left_title("default");

    // set plot settings
    setAutoReplot(false);
}

QtFastMultiCurvesPlotW::~QtFastMultiCurvesPlotW(){
    for(auto &curve : i->curves){
        curve->detach();
    }
    for(auto &marker : i->vMarkers){
        marker->detach();
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
    setAxisScale(xBottom, first, last);

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

auto QtFastMultiCurvesPlotW::set_nb_curves(size_t nbCurves) -> void{

    for(auto &curve : i->curves){
        curve->detach();
    }

    i->curves.resize(nbCurves);
    for(size_t ii = 0; ii < nbCurves; ++ii){
        auto curve = std::make_unique<QwtPlotCurve>("");
        curve->setTitle(u"Some Points %1"_s.arg(ii));
        curve->setPen(colors[ii], 3 ),
        curve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
        // curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true);
        // curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, true);
        // curve->setPaintAttribute(QwtPlotCurve::FilterPointsAggressive, true);
        curve->attach(this);
        i->curves[ii] = std::move(curve);
    }
}

auto QtFastMultiCurvesPlotW::add_vertical_marker(double x, const QString &label) -> void{

    auto marker = std::make_unique<QwtPlotMarker>();
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setXValue(x);

    QPen pen;
    pen.setWidthF(2.0);
    pen.setColor(Qt::darkGreen);
    marker->setLinePen(pen);
    marker->setLabel(QwtText(label));
    marker->attach(this);
    i->vMarkers.push_back(std::move(marker));
}


auto QtFastMultiCurvesPlotW::add_horizontal_marker(double y, const QString &label) -> void{

    auto marker = std::make_unique<QwtPlotMarker>();
    marker->setLineStyle(QwtPlotMarker::HLine);
    marker->setXValue(y);

    QPen pen;
    pen.setWidthF(2.0);
    pen.setColor(Qt::darkGreen);
    marker->setLinePen(pen);
    marker->setLabel(QwtText(label));
    marker->setLabelAlignment(Qt::AlignLeft);
    marker->attach(this);
    i->hMarkers.push_back(std::move(marker));
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

auto QtFastMultiCurvesPlotW::set_curve_visibility(size_t idCurve, bool state) -> void{

    if(idCurve >= i->curves.size()){
        return;
    }

    i->curves[idCurve]->setVisible(state);
}

auto QtFastMultiCurvesPlotW::set_scale_mode(bool automatic, double startScaleY, double endScaleY, bool addTempMeanOffset) -> void{

    i->automaticYScale  = automatic;
    i->startYScale      = startScaleY;
    i->endYScale        = endScaleY;
    i->addTempMeanOffset = addTempMeanOffset;

    if(i->automaticYScale){
        setAxisAutoScale(QwtAxis::YLeft, true);
    }else{
        setAxisAutoScale(QwtAxis::YLeft, false);
        setAxisScale(QwtAxis::YLeft, i->startYScale, i->endYScale);
    }

    replot();
}
