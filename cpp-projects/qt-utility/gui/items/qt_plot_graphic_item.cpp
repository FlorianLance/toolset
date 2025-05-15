


#include "qt_plot_graphic_item.hpp"

using namespace tool::ui;

PlotGraphicsItem::PlotGraphicsItem(QGraphicsItem *parent) : QGraphicsItem(parent){

    middleLinePen.setColor(QColor(0,0,0,255));
    rawPlotPen.setColor(QColor(255,0,0,255));
    processedPlotPen.setColor(QColor(0,0,255,255));
    labelPen.setColor(QColor(0,0,0,255));
}

auto PlotGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) -> void{

    static_cast<void>(option);
    static_cast<void>(widget);

    // draw background
    painter->fillRect(boundingRect(), backgroundColor);

    // middle line
    painter->setPen(middleLinePen);
    painter->drawLine(QPointF(0, height/2), QPointF(width, height/2));

    // draw raw plot
    painter->setPen(rawPlotPen);
    painter->drawPolyline(rawPlot);

    // draw processed plot
    painter->setPen(processedPlotPen);
    painter->drawPolyline(processedPlot);

    // label
    painter->setPen(labelPen);
    painter->drawText(QRectF(boundingRect().topLeft(), QSize(25,25)), name);

    // test
    //  painter->fillRect(width/2-5,0,width/2+5, 2*height/3.0, Qt::blue);
    //        painter->fillRect(0,  2*height/3.0, width/4, 1*height/3.0, Qt::blue);
    //0,0,10,2*height/3.0);
}

auto PlotGraphicsItem::set_raw_data(const tool::Buffer<double> &times, const tool::Buffer<double> &values) -> void{

    if(times.size() < 2 || (times.size() != values.size())){
        return;
    }

    auto firstX = times.front();
    auto lastX  = times.back();
    if(lastX < maxRangeX){
        lastX = maxRangeX;
    }
    auto diffX = lastX - firstX;
    auto diffY = maxRangeY - minRangeY;

    rawPlot.resize(times.size());
    for(size_t ii = 0; ii < times.size(); ++ii){
        rawPlot[ii].rx() = width*(times[ii] - firstX)/diffX;

        auto y = -values[ii];
        if(y < minRangeY){
            y = minRangeY;
        }else if (y > maxRangeY){
            y = maxRangeY;
        }
        rawPlot[ii].ry() = height*(y - minRangeY)/diffY;
    }
}

auto PlotGraphicsItem::boundingRect() const -> QRectF{
    return QRectF(0,0,width,height);
}
