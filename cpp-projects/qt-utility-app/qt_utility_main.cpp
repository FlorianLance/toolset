
/*******************************************************************************
** Toolset-qt-utility                                                         **
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

// // std
// #include <iostream>
// #include <any>

// Qt
// #include <QTextStream>
#include <QApplication>
// #include <QFile>
// #include <QDir>
// #include <QString>
// #include <QDebug>
// #include <QHostInfo>
// #include <QFontMetrics>
// #include <QHeaderView>

// qt-utility
#include "qt_logger.hpp"
#include "gui/widgets/tree_view_w.hpp"
#include "gui/widgets/base_qt_sfml_gl_widget.hpp"

using namespace tool;
using namespace tool::ui;


auto tree_widget() -> void{
    auto treeWidget = new tool::ui::TestTreeView(nullptr);
    //    QTreeWidget *treeWidget = new QTreeWidget();
    treeWidget->populate();
    treeWidget->show();

    //    // Main torrent list
    //    QStringList headers;
    //    headers << "Torrent" << "Peers/Seeds" << "Progress"
    //            << "Down rate"<< "Up rate" << "Status";

    //    testView->setItemDelegate(new TestViewDelegate(nullptr));
    //    testView->setHeaderLabels(headers);
    //    testView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //    testView->setAlternatingRowColors(true);
    //    testView->setRootIsDecorated(false);
    //    testView->show();

    //    // Set header resize modes and initial section sizes
    //    QFontMetrics fm = QApplication::fontMetrics();
    //    QHeaderView *header = testView->header();
    //    header->resizeSection(0, fm.horizontalAdvance("typical-name-for-a-torrent.torrent"));
    //    header->resizeSection(1, fm.horizontalAdvance(headers.at(1) + "  "));
    //    header->resizeSection(2, fm.horizontalAdvance(headers.at(2) + "  "));
    //    header->resizeSection(3, qMax(fm.horizontalAdvance(headers.at(3) + "  "), fm.horizontalAdvance(" 1234.0 KB/s ")));
    //    header->resizeSection(4, qMax(fm.horizontalAdvance(headers.at(4) + "  "), fm.horizontalAdvance(" 1234.0 KB/s ")));
    //    header->resizeSection(5, qMax(fm.horizontalAdvance(headers.at(5) + "  "), fm.horizontalAdvance("Downloading  ")));

    //    QTreeWidgetItem *item = new QTreeWidgetItem(testView);
    //    item->setText(0, "test");
    //    item->setToolTip(0, "tooltip");
    //    item->setText(1, ("0/0"));
    //    item->setText(2, "0");
    //    item->setText(3, "0.0 KB/s");
    //    item->setText(4, "0.0 KB/s");
    //    item->setText(5, ("Idle"));
    //    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    //    item->setTextAlignment(1, Qt::AlignHCenter);
}


auto main(int argc, char *argv[]) -> int{

    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::c());    

    tool::QtLogger::init("","");
    QApplication::connect(QtLogger::get(), &QtLogger::message_signal,  [&](QString message){
        qDebug() << "M " << message;
    });
    QtLogger::message(QSL("Qt-utility"));


    BaseQtSfmlGlWidget *sampleQtGlW = new BaseQtSfmlGlWidget();
    sampleQtGlW->show();


    return a.exec();
}

