

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

#include "qt_bar_plot_w.hpp"

// qwt
#include "qwt_column_symbol.h"
#include "qwt_plot_layout.h"
#include "qwt_legend.h"
#include "qwt_scale_draw.h"
#include "qwt_text.h"
#include "qwt_plot_renderer.h"
// #include "qwt_math.h"

using namespace tool::ui;

QtBarPlotW::QtBarPlotW( QWidget* parent ) : QwtPlot( parent ){

    // title
    setTitle("Defautl title" );

    // style
    setPalette( Qt::white );
    canvas()->setPalette( Qt::white );


    // bar char item
    m_barChartItem = new QwtPlotMultiBarChart( "Bar Chart " );
    m_barChartItem->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    m_barChartItem->setSpacing( 20 );
    m_barChartItem->setMargin( 3 );
    m_barChartItem->setStyle( QwtPlotMultiBarChart::Grouped );
    m_barChartItem->setOrientation( Qt::Vertical );
    m_barChartItem->attach( this );
    set_titles({"Default"});

    // legend
    insertLegend( new QwtLegend() );

    // axis
    setAxisTitle( QwtAxis::YLeft, "" );
    setAxisTitle( QwtAxis::XBottom, "" );
    setAxisAutoScale( QwtAxis::XBottom);
    setAxisAutoScale( QwtAxis::YLeft );

    // set scale elements
    QwtScaleDraw* scaleDraw1 = axisScaleDraw( QwtAxis::XBottom );
    scaleDraw1->enableComponent( QwtScaleDraw::Backbone, false );
    scaleDraw1->enableComponent( QwtScaleDraw::Ticks, false );


    QwtScaleDraw* scaleDraw2 = axisScaleDraw( QwtAxis::YLeft );
    scaleDraw2->enableComponent( QwtScaleDraw::Backbone, false );
    scaleDraw2->enableComponent( QwtScaleDraw::Ticks, false );
    scaleDraw2->enableComponent( QwtScaleDraw::Labels, false );

    plotLayout()->setAlignCanvasToScale( QwtAxis::XBottom, true );
    plotLayout()->setAlignCanvasToScale( QwtAxis::YLeft, false );
    plotLayout()->setCanvasMargin( 0 );
    updateCanvasMargins();

    // settings
    setAutoFillBackground( true );
    setAutoReplot( false );

    // populate
    set_values({{1.0, 1.0,1.0,1.0}});
    replot();
}

auto QtBarPlotW::set_titles(QStringList titles) -> void{

    QList<QwtText> titlesQWt;
    for(const auto &title : titles){
        titlesQWt += title;
    }
    m_barChartItem->setBarTitles(titlesQWt);
    m_barChartItem->setLegendIconSize( QSize( 10, 14 ) );

    for (int ii = 0; ii < titles.size(); ii++ ){
        QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
        symbol->setLineWidth( 2 );
        symbol->setFrameStyle( QwtColumnSymbol::Raised );
        symbol->setPalette( QPalette(colors[ii%colors.size()] ) );
        m_barChartItem->setSymbol(ii, symbol );
    }

    //    setAxisScale(QwtAxis::XBottom, 0, 1);//titles.size());
    setAxisVisible(QwtAxis::XBottom, false);

}

auto QtBarPlotW::set_values(const QVector<QVector<double>> &values) -> void{
    m_barChartItem->setSamples(values);
    //    replot();
}

auto QtBarPlotW::set_y_title(const QString &title) -> void{
    setAxisTitle( QwtAxis::YLeft, title);
}

auto QtBarPlotW::set_y_axis_scale(double min, double max) -> void{
    setAxisScale(QwtAxis::YLeft, min, max, (max-min)*0.1);
}
