
#pragma once

// Qt
#include <QGraphicsItem>
#include <QPainter>

// utility
#include "utility/buffer.hpp"

namespace tool::ui {

class PlotGraphicsItem : public QGraphicsItem{

public:

    PlotGraphicsItem(QGraphicsItem *parent = nullptr);
    auto set_raw_data(const tool::Buffer<double> &times, const tool::Buffer<double> &values) -> void;
    auto boundingRect() const -> QRectF override;

protected:

    auto paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) -> void override;

public:

    QPolygonF rawPlot;
    QPolygonF processedPlot;

    double maxRangeX = 10.0;
    double minRangeY = -150.0;
    double maxRangeY =  150.0;

    double width = 200.0;
    double height = 100.0;

    QString name = "DEF";

    QPen middleLinePen;
    QPen rawPlotPen;
    QPen processedPlotPen;
    QPen labelPen;

    static inline QColor backgroundColor = Qt::white;
};

}
