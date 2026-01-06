
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

// std
#include <vector>

// Qt
#include <QMouseEvent>

// QWt
#include <qwt_plot_curve.h>
#include <qwt_plot.h>

namespace tool::ui{

struct QtCurve{

    enum class Type : int {
        Custom = 0, YEqualsX,
        EaseInSine, EaseInCubic, EaseInQuint, EaseInCirc, EaseInElastic,
        EaseInQuad, EaseInQuart, EaseInExpo, EaseInBack, //EaseInBounce,
        EaseOutSine, EaseOutCubic, EaseOutQuint, EaseOutCirc,  EaseOutElastic,
        EaseOutQuad, EaseOutQuart, EaseOutExpo, EaseOutBack, //EaseOutBounce,
        EaseInOutSine, EaseInOutCubic, EaseOutInQuint, EaseInOutCirc, EaseInOutElastic,
        EaseInOutQuad, EaseInOutQuart, EaseInOutExpo, EaseInOutBack, //EaseInOutBounce,
        SizeEnum
    };

    QtCurve();

    auto insert_point(double rPosX, double rPosY) -> void;
    auto remove_point(QPointF pt, double minScaleX, double minScaleY, double diffScaleX, double diffScaleY) -> void;
    auto set_fitted_state(bool state) -> void;
    auto set_pen(const QPen &pen) -> void;
    auto set_type(Type type, double minX, double maxX, double minY, double maxY) -> void;
    auto update_plot_samples() -> void;

    std::vector<double> xCoords = {0,1};
    std::vector<double> yCoords = {0,1};
    QwtPlotCurve plotCurve;
    bool fitted = true;

    QtCurve::Type type = QtCurve::Type::Custom;

private :
    static const inline double n1 = 7.5625;
    static const inline double d1 = 2.75;
};


class QtMultiCurvesPlotW : public QwtPlot{

    Q_OBJECT
public:

    QtMultiCurvesPlotW();

public slots:

    auto reset(size_t nbCurves = 1) -> void;

    // data
    auto add_point_to_end(double xOffset, double y, size_t idCurve) -> void;
    auto add_point(double x, double y, size_t idCurve) -> void;
    auto set_points(std::vector<double> x, std::vector<double> y, size_t idCurve) -> void;
    auto set_first_y(double value, size_t idCurve) -> void;
    auto set_last_y(double value, size_t idCurve) -> void;
    auto set_type(QtCurve::Type type, size_t idCurve) -> void;

    // interpolation
    auto set_fitted_state(bool state) -> void;

    // display
    auto remove_symbol(size_t idCurve) -> void;

    auto set_title(QString title) -> void;
    auto set_x_title(QString title) -> void;
    auto set_y_title(QString title) -> void;
    auto set_x_range(qreal min, qreal max) -> void;
    auto set_y_range(qreal min, qreal max) -> void;

    auto set_pen(const QPen &pen, size_t idCurve) -> void;

protected:

    auto mousePressEvent(QMouseEvent *event) -> void override;

signals:

    auto data_updated_signal() -> void;
    auto settings_updated_signal() -> void;

public:

    std::vector<std::unique_ptr<QtCurve>> curves;

private:

    double minScaleX = 0.;
    double maxScaleX = 1.;

    double minScaleY = 0.;
    double maxScaleY = 1.;
};
}
