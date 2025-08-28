
#pragma once

// Qt
#include <QGraphicsItem>
#include <QPainter>

// utility
#include "utility/numeric_buffer.hpp"

namespace tool::ui {

class PlotGraphicsItem : public QGraphicsItem{

public:

    PlotGraphicsItem(QGraphicsItem *parent = nullptr);
    auto reset_data() -> void;
    auto set_raw_data(const tool::Buffer<double> &times, const tool::Buffer<double> &values) -> void;
    auto set_raw_data2(const tool::NumericBuffer<double> &times, const tool::NumericBuffer<double> &values) -> void;
    auto boundingRect() const -> QRectF override;

    auto set_size(QSize size) -> void{
        width  = size.width();
        height = size.height();
    }

    auto set_range_y(double min, double max) -> void{
        minRangeY = min;
        maxRangeY = max;
    }

    auto set_automatic_range_y_state(bool state) -> void{
        automaticRangeY = state;
    }

    auto set_scale_mode(bool automatic, double heightRange, bool addTempMeanOffset) -> void{
        automaticRangeY = automatic;
        minRangeY = -heightRange*0.001;
        maxRangeY = +heightRange*0.001;
        addTempMOffset = addTempMeanOffset;
    }

protected:

    auto paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) -> void override;

public:

    QPolygonF rawPlot;
    QPolygonF processedPlot;

    // double maxRangeX = 10.0;
    bool addTempMOffset = false;
    bool automaticRangeY = false;
    double minRangeY = -300.0;
    double maxRangeY =  300.0;

    double width = 200.0;
    double height = 100.0;

    QString name = "DEF";

    QPen middleLinePen;
    QPen rawPlotPen;
    QPen processedPlotPen;
    QPen labelPen;

    tool::NumericBuffer<double> yTemp;

    static inline QColor backgroundColor = Qt::white;
};

}
