
// /*******************************************************************************
// ** Toolbox-qt-utility                                                         **
// ** MIT License                                                                **
// ** Copyright (c) [2024] [Florian Lance]                                       **
// **                                                                            **
// ** Permission is hereby granted, free of charge, to any person obtaining a    **
// ** copy of this software and associated documentation files (the "Software"), **
// ** to deal in the Software without restriction, including without limitation  **
// ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
// ** and/or sell copies of the Software, and to permit persons to whom the      **
// ** Software is furnished to do so, subject to the following conditions:       **
// **                                                                            **
// ** The above copyright notice and this permission notice shall be included in **
// ** all copies or substantial portions of the Software.                        **
// **                                                                            **
// ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
// ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
// ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
// ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
// ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
// ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
// ** DEALINGS IN THE SOFTWARE.                                                  **
// **                                                                            **
// ********************************************************************************/

// #pragma once

// // std
// #include <vector>

// // Qt
// #include <QMouseEvent>
// #include <QTimer>

// // QWt
// #include <qwt_plot.h>
// #include <qwt_plot_curve.h>
// #include <qwt_curve_fitter.h>
// #include <qwt_text.h>
// #include <qwt_plot_marker.h>
// #include <qwt_symbol.h>

// struct PlotData2D{
//     std::vector<double> xCoords;
//     std::vector<double> yCoords;
//     size_t curentSize = 0;
// };

// namespace tool::ui{
// struct PlotCurve2D : public QwtPlotCurve{

//     PlotCurve2D();

//     auto set_fitted_state(bool state) -> void;
//     auto set_pen(const QPen &pen) -> void;

// //    auto insert_point(double rPosX, double rPosY) -> void;
// //    auto remove_point(QPointF pt, double minScaleX, double minScaleY, double diffScaleX, double diffScaleY) -> void;
// //    auto update_x_range(double minX, double maxX) -> void;
//     auto add_points(const PlotData2D &data, double minX, double maxX) -> void;

//     auto update_plot_samples() -> void;

//     PlotData2D data;
//     bool fitted = true;
// };

// class PlotMultiCurves2dW : public QwtPlot{

//     Q_OBJECT
// public:

//     PlotMultiCurves2dW();
//     ~PlotMultiCurves2dW();
//     void mousePressEvent(QMouseEvent *event) override;

// signals:

//     void data_updated_signal();
//     void settings_updated_signal();

// private:

//     double minScaleX = 0.;
//     double maxScaleX = 1.;

//     double minScaleY = 0.;
//     double maxScaleY = 1.;

//     bool replot = false;
//     bool redraw = false;

// public slots:

//     // display
//     auto remove_symbol(size_t idCurve) -> void;
//     auto set_pen(const QPen &pen, size_t idCurve) -> void;
//     auto set_title(QString title) -> void;
//     auto set_x_title(QString title) -> void;
//     auto set_y_title(QString title) -> void;

//     auto add_trigger_line(double x) -> void;

//     // interpolation
//     auto set_fitted_state(size_t idCurve, bool state) -> void;

//     // data
//     auto reset(size_t nbCurves = 1) -> void;
//     auto add_points(const PlotData2D &data, size_t idCurve, double minX, double maxX) -> void;

// public:

//     std::vector<PlotCurve2D*> curves;

// protected:

//     QTimer plotTimer;
// };
// }
