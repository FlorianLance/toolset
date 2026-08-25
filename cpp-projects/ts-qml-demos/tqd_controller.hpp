
#pragma once

// Qt
#include <QObject>
#include <QtQml>
#include <QTimer>
#include <QObject>
#include <QDebug>


// local
#include "items/sample_3d_item_viewer.hpp"
// #include "bsc_model.hpp"
// #include "custom_painted_item.hpp"
// #include "cloud_3d_view_item.hpp"

// MyEventFilter.h

class MyEventFilter : public QObject {
    Q_OBJECT
public:
    MyEventFilter(QObject *parent) : QObject(parent) {
    }


protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};



class TqdModel : public QObject{
    Q_OBJECT
public:

    TqdModel(){}
    ~TqdModel(){}

private:

    auto set_connections() -> void{}
    auto load_settings() -> void{}

signals:

    // ...

public:


private:

};




class TqdView : public QObject{
    Q_OBJECT
public:

    TqdView(){
        // colorItems.resize(6);
        // std::fill(colorItems.begin(), colorItems.end(), nullptr);

        // depthItems.resize(6);
        // std::fill(depthItems.begin(), depthItems.end(), nullptr);

        // cloud3dItems.resize(6);
        // std::fill(cloud3dItems.begin(), cloud3dItems.end(), nullptr);
    }

    // std::vector<CustomPaintedItem*> colorItems;
    // std::vector<CustomPaintedItem*> depthItems;
    // std::vector<Cloud3dViewItem*> cloud3dItems;
    Sample3dItemViewer *sampleViewer = nullptr;
};


class TqdController : public QObject{
    Q_OBJECT
    QML_ELEMENT
public:

    TqdController();


    Q_INVOKABLE void initialize(int nbCameras);
    Q_INVOKABLE void clean();

    // initialization
    // auto set_connections() -> void{}
    // auto load_data() -> void{}
    auto register_types() -> void;
    auto set_qml_properties(QQmlApplicationEngine *engine) -> void;

signals:

    void connected_state_updated_signal(int idClient, bool state);

public slots:

    auto insert_to_log(QString log) -> void{
        qDebug() << "[LOG]" << log;
    }

private:

    TqdView view;
    TqdModel model;
};
