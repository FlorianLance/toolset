
// /*******************************************************************************
// ** Toolset-ts-qt-app                                                          **
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

// #include "JKQtPlotterTest.hpp"

// Bouh::Bouh(QObject *parent) : QObject(parent){


//     plot.setToolbarEnabled(false);
//     plot.setContextMenuMode(jkqtpcmmNoContextMenu);
//     plot.setMouseTracking(false);
//     plot.setMousePositionShown(false);
//     plot.setMouseActionToolbarActionsActive(false);

//     JKQTPDatastore* ds=plot.getDatastore();

//     // 2. now we create data for a simple plot (a sine curve)

//     const int Ndata=100;
//     for (int i=0; i<Ndata; i++) {
//         const double x=double(i)/double(Ndata)*8.0*JKQTPSTATISTICS_PI;
//         X<<x;
//         Y<<sin(x);
//     }

//     // 3. make data available to JKQTPlotter by adding it to the internal datastore.
//     //    Note: In this step the data is copied (of not specified otherwise), so you can
//     //          reuse X and Y afterwards!
//     //    the variables columnX and columnY will contain the internal column ID of the newly
//     //    created columns with names "x" and "y" and the (copied) data from X and Y.
//     size_t columnX=ds->addColumn(X.data(), X.size(), "x");
//     size_t columnY=ds->addColumn(Y.data(), Y.size(), "y");


//     qDebug() << "columns " << columnX << columnY;

//     // 4. create a graph in the plot, which plots the dataset X/Y:
//     JKQTPXYLineGraph* graph1=new JKQTPXYLineGraph(&plot);
//     graph1->setXColumn(columnX);
//     graph1->setYColumn(columnY);
//     graph1->setTitle(QObject::tr("sine graph"));


//     // 5. add the graph to the plot, so it is actually displayed
//     plot.addGraph(graph1);

//     // 6. autoscale the plot so the graph is contained
//     plot.zoomToFit();
//     // plot.setAbsoluteX(0., 10.);
//     // plot.setAbsoluteY(-1., 1.);
//     // plot.setMagnification(1.0);

//     plot.getPlotter()->setPlotLabel(QObject::tr("Graph Title"));
//     plot.getPlotter()->setUseAntiAliasingForGraphs(false);
//     plot.getPlotter()->setUseAntiAliasingForSystem(false);
//     plot.getPlotter()->setUseAntiAliasingForText(false);

//     connect(&plot, &JKQTPlotter::zoomChangedLocally, this, [&](double newxmin, double newxmax, double newymin, double newymax, JKQTPlotter* sender){
//         plot.blockSignals(true);
//         plot.zoomToFit();
//         plot.blockSignals(false);
//     });

//     connect(&timer, &QTimer::timeout, this, [&, Ndata, columnX,columnY](){
//         qDebug() << "timeout";

//         for (int i=0; i<Ndata; i++) {
//         //     const double x=double(rand()%Ndata)/double(Ndata)*8.0*JKQTPSTATISTICS_PI;
//         //     X[i] = x;
//             Y[i] = sin(rand()%Ndata*i);
//         //     // X<<x;
//         //     // Y<<sin(x);
//         }

//         // ds->setColumnCopiedData(columnX, X.data(), X.size());
//         // ds->setColumnCopiedData(columnY, Y.data(), Y.size());
//         plot.redrawPlot();
//     });

//     timer.start(1000/30.);
// }


