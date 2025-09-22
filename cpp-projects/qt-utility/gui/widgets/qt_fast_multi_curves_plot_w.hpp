
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

    // curves
    auto set_nb_curves(size_t nbCurves, bool update = true) -> void;
    // # color
    auto set_curves_colors(std::span<QColor> colors, bool update = true) -> void;
    auto set_curve_color(size_t idCurve, const QColor &color, bool update = true) -> void;
    // # name
    auto set_curves_names(const QStringList &names, bool update = true) -> void;
    auto set_curves_names(std::span<QString> names, bool update = true) -> void;
    auto set_curve_name(size_t idCurve, const QString &name, bool update = true) -> void;
    // # width
    auto set_curves_identical_width(double width, bool update = true) -> void;
    auto set_curves_widths(std::span<double> widths, bool update = true) -> void;
    auto set_curve_width(size_t idCurve, double width, bool update = true) -> void;
    // # visibilty
    auto set_curve_visibility(size_t idCurve, bool state) -> void;
    // # data
    auto set_curve_points(size_t idCurve, const std::span<const double> x, const std::span<const double> y) -> void;

    // canvas
    auto set_left_title(const QString &title, const QColor &color = Qt::black) -> void;

    // grid
    auto set_grid_lines_display(bool showX, bool showY, bool showMinX, bool showMinY) -> void;

    // axis
    auto set_x_axis_scaling_infos(bool automatic, double startScaleX, double endScaleX, double step = 0.0, bool visible = true) -> void;
    auto set_y_axis_scaling_infos(bool automatic, double startScaleY, double endScaleY, bool addTempMeanOffset, double step = 0.0, bool visible = true) -> void;

    // markers
    auto set_nb_horizontal_markers(size_t nbMarkers) -> void;
    auto set_nb_vertical_markers(size_t nbMarkers) -> void;
    auto set_horizontal_marker_info(size_t idM, double y, const QString &label) -> void;
    auto set_vertical_marker_info(size_t idM, double x, const QString &label) -> void;
    auto add_vertical_marker(double x, const QString &label) -> void;
    auto add_horizontal_marker(double y, const QString &label) -> void;
    auto get_horizontal_marker_name(size_t idM) const -> QString;

    auto disable_y_ticks() -> void;

    // auto show_tooltip(const QPoint &globalPos, const QString &text) -> void;

signals:

    auto left_click_pressed_signal(QPoint globalPos, int id, const QString &text) -> void;
    auto right_click_pressed_signal(QPoint globalPos, int id, const QString &text) -> void;
    auto left_click_released_signal() -> void;
    auto right_click_released_signal() -> void;

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool m_isLeftPressed = false;
    bool m_isRightPressed = false;

    // QMenu *m_contextMenu = nullptr;
    // QAction

private:

    auto update_curves_info() -> void;

    struct Impl;
    std::unique_ptr<Impl> i;

    // QWidget interface
// protected:
//     void contextMenuEvent(QContextMenuEvent *event) override;
};
}
