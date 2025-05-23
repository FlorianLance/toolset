
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

#pragma once

// qwt
#include "qwt_plot.h"

namespace tool::ui{

class QtFastMultiCurvesPlotW : public QwtPlot{

    Q_OBJECT
public:

    QtFastMultiCurvesPlotW();
    ~QtFastMultiCurvesPlotW();

    auto set_curve_points(size_t idCurve, const std::span<const double> x, const std::span<const double> y) -> void;
    auto set_nb_curves(size_t nbCurves) -> void;
    auto add_marker(double x, const QString &label) -> void;
    auto set_left_title(const QString &title) -> void;
    auto set_curve_visibility(size_t idCurve, bool state) -> void;

private:

    static constexpr std::array<Qt::GlobalColor, 5> colors = {
        Qt::blue, Qt::red, Qt::yellow, Qt::darkCyan, Qt::green
    };

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
