


#pragma once

// Qt
#include <QObject>
#include <QDebug>
#include <QTimer>

#include "jkqtplotter/jkqtplotter.h"
#include "jkqtplotter/graphs/jkqtpscatter.h"

class DataPlotW : public JKQTPlotter{
    Q_OBJECT
public:

    DataPlotW() : JKQTPlotter(){

        setToolbarEnabled(false);
        setContextMenuMode(jkqtpcmmNoContextMenu);
        setMouseTracking(false);
        setMousePositionShown(false);
        setMouseActionToolbarActionsActive(false);
        setPlotUpdateEnabled(true);

        m_dataStore = getDatastore();

        set_nb_curves(2);

        connect(&m_timer, &QTimer::timeout, [&](){
            redrawPlot();
        });
        m_timer.start(1000.0/30.0);
    }

    auto set_curve_points(size_t idCurve, const std::span<const double> x, const std::span<const double> y) -> void{

        if(m_graphs.size() <= idCurve){
            return;
        }

        // qDebug() << "set_curve_points " << idCurve << x.size() << y.size();
        m_dataStore->setColumnCopiedData(m_graphs[idCurve]->getXColumn(), x.data(), x.size());
        m_dataStore->setColumnCopiedData(m_graphs[idCurve]->getYColumn(), y.data(), y.size());

        setAbsoluteX(x.front(), x.back());
        setAbsoluteY(y.front(), y.back());

        zoomToFit();

    }

    auto set_nb_curves(size_t nbCurves) -> void{

        // clean data store
        m_dataStore->clear();

        // resize graphs
        clearGraphs(false);
        m_graphs.resize(nbCurves);
        for(size_t idGraph = 0; idGraph < m_graphs.size(); ++idGraph){
            m_graphs[idGraph] = std::make_unique<JKQTPXYLineGraph>(this);
            m_graphs[idGraph]->setXColumn(m_dataStore->addColumn("x"));
            m_graphs[idGraph]->setYColumn(m_dataStore->addColumn("y"));
            m_graphs[idGraph]->setColor(colors[idGraph]);
            m_graphs[idGraph]->setVisible(true);
            addGraph(m_graphs[idGraph].get());
            qDebug() << "columns added " << idGraph << m_graphs[idGraph]->getXColumn() << m_graphs[idGraph]->getYColumn();
        }
    }

    auto add_marker(double x, const QString &label) -> void{
        // ...
    }

    auto set_left_title(const QString &title) -> void{
        // ...

    }

    auto set_curve_visibility(size_t idCurve, bool state) -> void{

        if(m_graphs.size() <= idCurve){
            return;
        }

        m_graphs[idCurve]->setVisible(state);
    }

protected:

    std::vector<QColor> colors = {
        Qt::blue, Qt::red, Qt::yellow, Qt::darkCyan, Qt::green
    };

    JKQTPDatastore *m_dataStore = nullptr;
    std::vector<std::unique_ptr<JKQTPXYLineGraph>> m_graphs;

    QTimer m_timer;
};


class Bouh : QObject{
    Q_OBJECT
public:

    Bouh(QObject *parent = nullptr);


public:
    JKQTPlotter plot;
    QVector<double> X, Y;
    QTimer timer;
};
