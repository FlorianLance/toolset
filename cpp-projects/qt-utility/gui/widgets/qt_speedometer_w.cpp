

/*******************************************************************************
** Toolset-qt-utility                                                         **
** MIT License                                                                **
** Copyright (c) [2021] [Florian Lance]                                       **
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

#include "qt_speedometer_w.hpp"

// qwt
#include "qwt_dial_needle.h"
#include "qwt_round_scale_draw.h"

using namespace tool::ui;

QtSpeedoMeterW::QtSpeedoMeterW(QWidget *parent) : QwtDial( parent ), m_label( "" ){

    QwtRoundScaleDraw* scaleDraw = new QwtRoundScaleDraw();
    scaleDraw->setSpacing( 8 );
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    scaleDraw->setTickLength( QwtScaleDiv::MinorTick, 0 );
    scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 4 );
    scaleDraw->setTickLength( QwtScaleDiv::MajorTick, 8 );
    //    scaleDraw->setAngleRange(0,30);
    setScaleDraw( scaleDraw );

    setWrapping( false );
    setReadOnly( true );

    //    setOrigin( 90.0 );
    //    setScaleArc( 90.0, 270.0 );

    QwtDialSimpleNeedle* needle = new QwtDialSimpleNeedle(
        QwtDialSimpleNeedle::Arrow, true, Qt::black,
        QColor( Qt::gray ).lighter( 130 ) );
    setNeedle( needle );

    m_pen.setColor(Qt::black);

}

auto QtSpeedoMeterW::set_label(const QString &label) -> void{
    m_label = label;
}

auto QtSpeedoMeterW::label() const -> QString{
    return m_label;
}

auto QtSpeedoMeterW::drawScaleContents(QPainter *painter, const QPointF &center, double radius) const -> void{
    QRectF rect( 0.0, 0.0, 2.0 * radius, 2.0 * radius - 10.0 );
    rect.moveCenter( center );

    QFont font = painter->font();
    font.setPointSize(font.pointSize() * radius*0.015);

    painter->setPen(m_pen);
    painter->setFont(font);

    const int flags = Qt::AlignBottom | Qt::AlignHCenter;
    painter->drawText( rect, flags, m_label );
}
