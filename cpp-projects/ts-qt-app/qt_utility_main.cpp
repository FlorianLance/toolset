
/*******************************************************************************
** Toolset-ts-qt-app                                                          **
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


#include <QApplication>
#include <QTabWidget>

// qwt
#include <qwt_thermo.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>


// #include "gui/widgets/tree_view_w.hpp"
// #include "gui/widgets/base_qt_sfml_gl_widget.hpp"
#include "qt_logger.hpp"

#include "__JKQtPlotterTest.hpp"


using namespace tool;
// using namespace tool::ui;
using namespace Qt::Literals::StringLiterals;

// auto tree_widget() -> void{
//     auto treeWidget = new tool::ui::TestTreeView(nullptr);
//     //    QTreeWidget *treeWidget = new QTreeWidget();
//     treeWidget->populate();
//     treeWidget->show();

//     //    // Main torrent list
//     //    QStringList headers;
//     //    headers << "Torrent" << "Peers/Seeds" << "Progress"
//     //            << "Down rate"<< "Up rate" << "Status";

//     //    testView->setItemDelegate(new TestViewDelegate(nullptr));
//     //    testView->setHeaderLabels(headers);
//     //    testView->setSelectionBehavior(QAbstractItemView::SelectRows);
//     //    testView->setAlternatingRowColors(true);
//     //    testView->setRootIsDecorated(false);
//     //    testView->show();

//     //    // Set header resize modes and initial section sizes
//     //    QFontMetrics fm = QApplication::fontMetrics();
//     //    QHeaderView *header = testView->header();
//     //    header->resizeSection(0, fm.horizontalAdvance("typical-name-for-a-torrent.torrent"));
//     //    header->resizeSection(1, fm.horizontalAdvance(headers.at(1) + "  "));
//     //    header->resizeSection(2, fm.horizontalAdvance(headers.at(2) + "  "));
//     //    header->resizeSection(3, qMax(fm.horizontalAdvance(headers.at(3) + "  "), fm.horizontalAdvance(" 1234.0 KB/s ")));
//     //    header->resizeSection(4, qMax(fm.horizontalAdvance(headers.at(4) + "  "), fm.horizontalAdvance(" 1234.0 KB/s ")));
//     //    header->resizeSection(5, qMax(fm.horizontalAdvance(headers.at(5) + "  "), fm.horizontalAdvance("Downloading  ")));

//     //    QTreeWidgetItem *item = new QTreeWidgetItem(testView);
//     //    item->setText(0, "test");
//     //    item->setToolTip(0, "tooltip");
//     //    item->setText(1, ("0/0"));
//     //    item->setText(2, "0");
//     //    item->setText(3, "0.0 KB/s");
//     //    item->setText(4, "0.0 KB/s");
//     //    item->setText(5, ("Idle"));
//     //    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
//     //    item->setTextAlignment(1, Qt::AlignHCenter);
// }




auto main(int argc, char *argv[]) -> int{

    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::c());

    QtLoggerM::set_logger_instance(std::make_unique<QtLoggerM>());
    auto logger = QtLoggerM::get_instance();
    logger->init(u""_s,u""_s);
    QObject::connect(logger, &QtLoggerM::message_signal, [&](QString message){
        qDebug() << "M " << message;
    });

    QtLog::message(QSL("Qt-utility"));

    // QWidget *w = new QWidget();
    // w->resize(1920,1080);
    // w->show();

    QTabWidget *tw = new QTabWidget();

    // JKQtPlotter
    // Bouh booh;


    {
        // 1. create a plotter window and get a pointer to the internal datastore (for convenience)





        // tw->addTab(&booh.plot, "JKQT Plot");
    }

    // DataPlotW dpw;
    // {
    //     std::vector<double> x,y;
    //     x.resize(100);
    //     y.resize(100);
    //     for(size_t id = 0; id < 100; ++id){
    //         x[id] = id;
    //         y[id] = 1.0*id + (rand() % 6) - 3;
    //     }

    //     dpw.set_nb_curves(2);
    //     dpw.set_curve_points(0, x, y);

    //     for(size_t id = 0; id < 100; ++id){
    //         x[id] = id;
    //         y[id] = 1.0*id + (rand() % 6) - 3;
    //     }
    //     dpw.set_curve_points(1, x, y);

    //     tw->addTab(&dpw, "JKQT Plot2");
    // }

    // Qwt
    {
        auto thermo = new QwtThermo;
        thermo->setFillBrush( QBrush(Qt::red) );
        thermo->setScale(0, 10);
        tw->addTab(thermo, "Thermo");

        constexpr int plotDataSize = 100;
        double xData[plotDataSize];
        double yData[plotDataSize];
        // set up the initial plot data
        int v = 0;
        for( int index=0; index<plotDataSize; ++index )
        {
            xData[index] = index;
            yData[index] = v;
            v += rand()%6-3;
        }

        auto curve = new QwtPlotCurve;
        auto plot = new QwtPlot;
        // make a plot curve from the data and attach it to the plot
        curve->setSamples(xData, yData, plotDataSize);
        curve->attach(plot);

        plot->setAxisScale(QwtPlot::yLeft,-10,10);
        plot->replot();
        tw->addTab(plot, "Plot");


        // BaseQtSfmlGlWidget *sampleQtGlW = new BaseQtSfmlGlWidget();
        // sampleQtGlW->resize(1920,1080);
        // sampleQtGlW->show();
    }

    tw->show();

    return a.exec();
}

