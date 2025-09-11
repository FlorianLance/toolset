
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
#include "qwt_plot_multi_barchart.h"

namespace tool::ui {

class QtBarPlotW : public QwtPlot{
    Q_OBJECT

public:

    QtBarPlotW(QWidget* = nullptr);
    auto set_titles(QStringList titles) -> void;
    auto set_values(const QVector<QVector<double>> &values) -> void;
    auto set_y_title(const QString &title) -> void;

    auto set_y_axis_scale(double min, double max) -> void;

    auto set_colors(std::vector<QColor> nColors) -> void{
        colors = nColors;
    }

private:

    std::vector<QColor> colors = {
        Qt::darkBlue, Qt::blue, Qt::cyan, Qt::darkCyan, Qt::green,
        Qt::darkGreen, Qt::red, Qt::darkRed, Qt::yellow, Qt::darkYellow
    };

    QwtPlotMultiBarChart* m_barChartItem = nullptr;
};
}

