
// /*******************************************************************************
// ** Toolbox-qt-utility                                                         **
// ** MIT License                                                                **
// ** Copyright (c) [2018] [Florian Lance]                                       **
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

// #include "qt_plot_multi_curves_2d_w.hpp"

// // Qt
// #include <QCoreApplication>

// // Qwt
// #include <qwt_plot_grid.h>
// #include <qwt_symbol.h>
// #include <qwt_legend.h>
// #include <qwt_curve_fitter.h>
// #include <qwt_spline_curve_fitter.h>

// // base
// //#include "../utility/math.hpp"
// //#include "../geometry/point2.hpp"

// using namespace tool;
// using namespace tool::ui;

// PlotCurve2D::PlotCurve2D(){

//     setTitle("");

//     QPen pen;
//     pen.setColor(Qt::black);
//     pen.setWidth(4.);
//     setPen(pen);

//     setRenderHint( QwtPlotItem::RenderAntialiased, true );
//     setCurveAttribute(QwtPlotCurve::Fitted, fitted);
//     setSymbol( new QwtSymbol(
//         QwtSymbol::Ellipse,
//         QBrush(Qt::yellow),
//         QPen(Qt::red, 2),
//         QSize(4, 4))
//     );

//     QwtSplineCurveFitter *fitter = new QwtSplineCurveFitter();
// //    fitter->setSpline()
//     setCurveFitter(fitter);

//     update_plot_samples();

// }


// auto PlotCurve2D::set_fitted_state(bool state) -> void {
//     setCurveAttribute(QwtPlotCurve::Fitted, fitted = state);
// }

// auto PlotCurve2D::update_plot_samples() -> void{
//     setRawSamples(data.xCoords.data(), data.yCoords.data(), static_cast<int>(data.curentSize));
// }

// auto PlotCurve2D::add_points(const PlotData2D &nData, double minX, double maxX) -> void{

//     // resize max buffer
//     size_t newSize = data.curentSize + nData.curentSize;
//     if(data.xCoords.size() < newSize){
//         data.xCoords.resize(newSize);
//         data.yCoords.resize(newSize);
//     }

//     // copy new values at the end
//     std::copy(nData.xCoords.begin(), nData.xCoords.end(), data.xCoords.begin() + data.curentSize);
//     std::copy(nData.yCoords.begin(), nData.yCoords.end(), data.yCoords.begin() + data.curentSize);
//     data.curentSize = newSize;

//     // trunc values after x range
//     int end = -1;
//     for(size_t ii = 0; ii < data.curentSize; ++ii){
//         if(data.xCoords[ii] > maxX){
//             end = static_cast<int>(ii);
//             break;
//         }
//     }
//     if(end != -1 && end < data.curentSize-1){
//         data.curentSize = end+1;
//     }

//     // trunc values before x range
//     int start = -1;
//     for(size_t ii = 0; ii < data.curentSize; ++ii){
//         if(data.xCoords[ii] < minX){
//             start = static_cast<int>(ii);
//             continue;
//         }
//         break;
//     }
//     if(start > 0){
//         data.curentSize -= start-1;
//         std::rotate(data.xCoords.begin(), data.xCoords.begin() + start-1, data.xCoords.end());
//         std::rotate(data.yCoords.begin(), data.yCoords.begin() + start-1, data.yCoords.end());
//     }

//     update_plot_samples();
// }

// PlotMultiCurves2dW::PlotMultiCurves2dW(){

//     // background
//     setCanvasBackground( Qt::lightGray );

//     // axis
//     QFont axisFont("Calibri", 8);
//     setAxisFont(QwtPlot::xBottom,   axisFont);
//     setAxisFont(QwtPlot::yLeft,     axisFont);
//     setAxisScale( QwtPlot::xBottom, minScaleX, maxScaleX);
//     setAxisScale( QwtPlot::yLeft,   minScaleY, maxScaleY);

//     curves.push_back(new PlotCurve2D());
//     insertItem(curves.back());

//     plotTimer.start(13);
//     connect(&plotTimer, &QTimer::timeout, this,[&]{
//         if(replot){
//             QwtPlot::replot();
//         }
//         if(redraw){
//             QwtPlot::repaint();
//         }
//     });

// }

// PlotMultiCurves2dW::~PlotMultiCurves2dW(){
//     plotTimer.stop();
//     for(auto item : itemList()){
//         removeItem(item);
//         delete item;
//     }
// }

// auto PlotMultiCurves2dW::set_title(QString title) -> void{
//     QwtText t(title);
//     QFont font("Calibri", 10);
//     font.setBold(true);
//     t.setFont(font);
//     setTitle(t);
// }

// auto PlotMultiCurves2dW::set_x_title(QString title) -> void{
//     QwtText t(title);
//     QFont font("Calibri", 9);
//     font.setBold(false);
//     t.setFont(font);
//     setAxisTitle(QwtPlot::xBottom, t);
// }

// auto PlotMultiCurves2dW::set_y_title(QString title) -> void{
//     QwtText t(title);
//     QFont font("Calibri", 9);
//     font.setBold(false);
//     t.setFont(font);
//     setAxisTitle(QwtPlot::yLeft, t);
// }

// auto PlotMultiCurves2dW::add_trigger_line(double x) -> void{

//     // remove plot markers outside range
//     for(auto item : itemList()){
//         if(auto mark = dynamic_cast<QwtPlotMarker*>(item)){
//             if(mark->value().x() < minScaleX){
//                 removeItem(mark);
//                 delete mark;
//             }
//         }
//     }


//     // add new marker
//     QwtPlotMarker *mark=new QwtPlotMarker;
//     mark->setSymbol(new QwtSymbol(QwtSymbol::VLine,QBrush(Qt::red),QPen(Qt::red),QSize(5,1000)));
//     mark->setValue(x,0.0);
//     insertItem(mark);
// }

// auto PlotMultiCurves2dW::set_pen(const QPen &pen, size_t idCurve) -> void{

//     if(idCurve >= curves.size()){
//         return;
//     }

//     curves[idCurve]->setPen(pen);

//     replot = true;
//     redraw = true;
//     emit settings_updated_signal();
// }

// auto PlotMultiCurves2dW::reset(size_t nbCurves) -> void{

//     for(auto curve : curves){
//         removeItem(curve);
//         delete curve;
//     }
//     curves.clear();

//     for(size_t ii = 0; ii < nbCurves; ++ii){
//         auto curve  = new PlotCurve2D();
//         curve->update_plot_samples();
//         curves.push_back(curve);
//         insertItem(curves.back());
//     }

//     replot = true;
//     redraw = true;
//     emit data_updated_signal();
// }

// auto PlotMultiCurves2dW::add_points(const PlotData2D &data, size_t idCurve, double minX, double maxX) -> void{

//     if(idCurve >= curves.size()){
//         return;
//     }

//     curves[idCurve]->add_points(data,minX,maxX);

//     setAxisScale( QwtPlot::xBottom, minScaleX = minX, maxScaleX = maxX);
// //    auto bounds = std::minmax(curves[idCurve]->data.yCoords.begin(), curves[idCurve]->data.yCoords.begin() + curves[idCurve]->data.curentSize);
// //    setAxisScale( QwtPlot::yLeft, minScaleY = *bounds.first, maxScaleY = *bounds.second);

//     setAxisScale( QwtPlot::yLeft, minScaleY = -1.0, maxScaleY = 1.0);



//     replot = true;
//     redraw = true;

// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// }

// auto PlotMultiCurves2dW::set_fitted_state(size_t idCurve, bool state) -> void{

//     if(idCurve >= curves.size()){
//         return;
//     }
//     curves[idCurve]->set_fitted_state(state);

//     replot = true;
//     redraw = true;
//     emit settings_updated_signal();
// }

// auto PlotMultiCurves2dW::remove_symbol(size_t idCurve) -> void{

//     if(idCurve >= curves.size()){
//         return;
//     }

//     curves[idCurve]->setSymbol(nullptr);

//     replot = true;
//     redraw = true;
//     emit settings_updated_signal();
// }


// auto PlotMultiCurves2dW::mousePressEvent(QMouseEvent *event) -> void{
//     static_cast<void>(event);
// //    if(!canvas()->geometry().contains(event->pos())){
// //        return;
// //    }

// //    auto pos = event->pos()-canvas()->pos();
// //    QPointF pt(
// //        1.*pos.x()/canvas()->width(),
// //        1. - 1.*pos.y()/canvas()->height()
// //    );

// //    const auto diffScaleX = maxScaleX - minScaleX;
// //    const auto diffScaleY = maxScaleY - minScaleY;
// //    const auto rPosX      = minScaleX + pt.x()*diffScaleX;
// //    const auto rPosY      = minScaleY + pt.y()*diffScaleY;

// //    if(event->button() == Qt::MouseButton::LeftButton){

// //        if(curves.size() > 0){
// //            curves[0]->insert_point(rPosX, rPosY);
// //            curves[0]->update_plot_samples();
// //        }

// //    }else if(event->button() == Qt::MouseButton::RightButton){
// //        if(curves.size() > 0){
// //            curves[0]->remove_point(pt, minScaleX, minScaleY, diffScaleX, diffScaleY);
// //            curves[0]->update_plot_samples();
// //        }
// //    }

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// }




// //void CurveW::add_point_to_end(double xOffset, double y, size_t idCurve){

// //    if(idCurve >= curves.size()){
// //        return;
// //    }

// //    if(curves[idCurve]->xCoords.size() > 0){
// //        curves[idCurve]->xCoords.push_back(curves[idCurve]->xCoords[curves[idCurve]->xCoords.size()-1]+xOffset);
// //    }else{
// //        curves[idCurve]->xCoords.push_back(xOffset);
// //    }
// //    curves[idCurve]->yCoords.push_back(y);
// //    curves[idCurve]->update_plot_samples();

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();

// //}

// //void CurveW::add_point(double x, double y, size_t idCurve){

// //    if(idCurve >= curves.size()){
// //        return;
// //    }

// //    size_t id = 0;
// //    for(size_t ii = 0; ii < curves[idCurve]->xCoords.size(); ++ii){
// //        if(x < curves[idCurve]->xCoords[ii]){
// //            id = ii;
// //            break;
// //        }
// //    }
// //    curves[idCurve]->xCoords.insert(std::begin(curves[idCurve]->xCoords) + id, x);
// //    curves[idCurve]->yCoords.insert(std::begin(curves[idCurve]->yCoords) + id, y);
// //    curves[idCurve]->update_plot_samples();

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// //}

// //void CurveW::set_points(std::vector<double> x, std::vector<double> y, size_t idCurve){

// //    if(idCurve >= curves.size()){
// //        return;
// //    }

// //    curves[idCurve]->xCoords = std::move(x);
// //    curves[idCurve]->yCoords = std::move(y);
// //    curves[idCurve]->update_plot_samples();

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// //}

// //void CurveW::set_first_y(double value, size_t idCurve){

// //    if(idCurve >= curves.size()){
// //        return;
// //    }

// //    curves[idCurve]->yCoords[0] = value;
// //    curves[idCurve]->update_plot_samples();

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// //}

// //void CurveW::set_last_y(double value, size_t idCurve){

// //    if(idCurve >= curves.size()){
// //        return;
// //    }

// //    curves[idCurve]->data.yCoords[data.c] [curves[idCurve]->yCoords.size()-1] = value;
// //    curves[idCurve]->update_plot_samples();

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// //}

// //void CurveW::set_type(Curve::Type type, size_t idCurve){

// //    if(idCurve >= curves.size()){
// //        return;
// //    }

// //    curves[idCurve]->set_type(type, minScaleX, maxScaleX, minScaleX, maxScaleY);
// //    curves[idCurve]->update_plot_samples();

// //    QwtPlot::replot();
// //    QwtPlot::repaint();
// //    emit data_updated_signal();
// //}



// //auto Curve::update_x_range(double minX, double maxX) -> void{

// //    int count = 0;
// //    for(const auto &x : xCoords){
// //        if(x < minX){
// //            ++count;
// //        }
// //    }

// //    if(count > 0){
// //       xCoords.erase(std::begin(xCoords), std::begin(xCoords) + count);
// //       yCoords.erase(std::begin(yCoords), std::begin(yCoords) + count);
// //    }

// //    // erase values > maxScaleX
// //    int start = -1;
// //    for(size_t ii = 0; ii < xCoords.size(); ++ii){
// //        if(xCoords[ii] > maxX){
// //            start = static_cast<int>(ii);
// //            break;
// //        }
// //    }

// //    if(start > -1){
// //        xCoords.erase(std::begin(xCoords) + start, std::end(xCoords));
// //        yCoords.erase(std::begin(yCoords) + start, std::end(yCoords));
// //    }

// //    // set first and last x values
// //    if(xCoords.size() >= 2){
// //        curve->xCoords[0] = minScaleX;
// //        curve->xCoords[curve->xCoords.size()-1] = maxScaleX;
// //    }else if(curve->xCoords.size() == 1){

// //        if(std::abs(minScaleX-curve->xCoords[0]) < std::abs(maxScaleX-curve->xCoords[0])){
// //            curve->yCoords = {curve->yCoords[0], maxScaleY};
// //        }else{
// //            curve->yCoords = {minScaleY, curve->yCoords[0]};
// //        }
// //        curve->xCoords = {minScaleX, maxScaleX};

// //    }else{
// //        curve->xCoords = {minScaleX, maxScaleX};
// //        curve->yCoords = {minScaleY, maxScaleY};
// //    }


// //}



// //double Curve::ease_out_bounce(double x){

// //    return (t = +t) < b1 ?
// //        b0 * t * t : t < b3 ?
// //            b0 * (t -= b2) * t + b4 :
// //        t < b6 ? b0 * (t -= b5) * t + b7 : b0 * (t -= b8) * t + b9;



// //    if (x < (1. / d1)) {
// //        return n1 * std::pow(2, x);
// //    } else if (x < (2 / d1)) {
// //        const auto xp = x - 1.5;
// //        return  n1 * (xp  / d1) * xp + 0.75;
// //    } else if (x < (2.5 / d1)) {
// //        const auto xp = x - 2.25;
// //        return n1 * (xp  / d1) * xp + 0.9375;
// //    } else {
// //        const auto xp = x - 2.625;
// //        return n1 * (xp  / d1) * xp + 0.984375;
// //    }

// //    return 0.;
// //}

// //void Curve::insert_point(double rPosX, double rPosY){

// //    type = Type::Custom;

// //    for(size_t ii = 0; ii < xCoords.size(); ++ii){
// //        if(xCoords[ii] > rPosX){
// //            xCoords.insert(std::begin(xCoords) + static_cast<int>(ii), rPosX);
// //            yCoords.insert(std::begin(yCoords) + static_cast<int>(ii), rPosY);
// //            break;
// //        }
// //    }
// //}

// //void Curve::remove_point(QPointF pt, double minScaleX, double minScaleY, double diffScaleX, double diffScaleY){

// //    type = Type::Custom;

// //    double min = 0.1;
// //    int index = -1;

// //    for(size_t ii = 0; ii < xCoords.size(); ++ii){

// //        const auto &px = (xCoords[ii]-minScaleX)/diffScaleX;
// //        const auto &py = (yCoords[ii]-minScaleY)/diffScaleY;
// //        double dist = geo::norm(geo::Pt2<double>{pt.x()-px, pt.y()-py});
// //        if(dist < min){
// //            min = dist;
// //            index = static_cast<int>(ii);
// //        }
// //    }

// //    if(index > -1 && index != 0 && (index < static_cast<int>(xCoords.size())-1)){
// //        xCoords.erase(std::begin(xCoords) + index);
// //        yCoords.erase(std::begin(yCoords) + index);
// //    }
// //}








// //    void set_x_range(qreal min, qreal max);
// //    void set_y_range(qreal min, qreal max);
//     // data
// //    void add_point_to_end(double xOffset, double y, size_t idCurve);
// //    void add_point(double x, double y, size_t idCurve);
// //    void set_points(std::vector<double> x, std::vector<double> y, size_t idCurve);
// //    void set_first_y(double value, size_t idCurve);
// //    void set_last_y(double value, size_t idCurve);
// //    void set_type(Curve::Type type, size_t idCurve);


