
#include "tqd_controller.hpp"

// Qt
#include <QKeyEvent>

// local
#include "samples/base_sample.hpp"


bool MyEventFilter::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            qDebug() << "Escape key intercepted by event filter!";
            // Returning true means you've handled the event and it won't be
            // passed on to the target object (e.g., the TextField).
            // return true;
        }
    }
    // For all other events, pass them along.
    return QObject::eventFilter(watched, event);
}


TqdController::TqdController(){

    register_types();

    // load_data();

    // set connections
    // connect(&model, &BscModel::connected_state_updated_signal, this, &BscController::connected_state_updated_signal);

    // connect(&model.clientW, &ClientWorker::new_frame_signal, this, [&](size_t idC, std::shared_ptr<tool::cam::DCFrame> frame){
    //     if(idC < view.colorItems.size()){
    //         if(view.colorItems[idC] != nullptr){
    //             view.colorItems[idC]->set_frame(frame);
    //         }
    //         if(view.depthItems[idC] != nullptr){
    //             view.depthItems[idC]->set_frame(frame);
    //         }
    //         if(view.cloud3dItems[idC] != nullptr){
    //             view.cloud3dItems[idC]->set_frame(frame);
    //         }
    //     }
    // },Qt::QueuedConnection);
}

void TqdController::initialize(int nbCameras){

    qDebug() << "[INVOKED] initialize";

    // auto cSettings = model.settings;
    // cSettings.devicesS.resize(nbCameras);

    // emit model.initialize_client_signal(cSettings);
}

void TqdController::clean() {
    // emit model.clean_signal();
}

auto TqdController::register_types() -> void{

    // items
    // qmlRegisterType<CustomPaintedItem>("com.example", 1, 0, "CustomPaintedItem");
    qmlRegisterType<Sample3dItemViewer>("items", 1, 0, "Sample3dItemViewer");


    qmlRegisterType<tool::gl::BaseGlSample>("samples", 1, 0, "BaseGlSample");
    qmlRegisterType<tool::gl::DiffuseGlSample>("samples", 1, 0, "DiffuseGlSample");

    // Sample3dItemViewer

    // # models
    // qmlRegisterType<BBoxFilteringsModel>(   "BBoxFilterings",   1, 0, "BBoxFilteringsModel");

    // # lists
    // qmlRegisterUncreatableType<ObjectsList>("Objects",          1, 0, "ObjectsList",            QStringLiteral("ObjectsList should not be created in QML"));

    // # types
    // qRegisterMetaType<ConnectionProxyType>("connectionProxyType");

    // # enums
    // qmlRegisterUncreatableType<ConnectionProxyTypeClass>("Enums", 1, 0, "connectionProxyType", "Not creatable as it is an enum type");

    // # others types
    // qmlRegisterType<UuidView>("Uuid", 1, 0, "uuidView");

    // qmlRegisterUncreatableMetaObject(
    //     CPPEnums::staticMetaObject, // meta object created by Q_NAMESPACE macro
    //     "cpp.enums",                // import statement (can be any string)
    //     1, 0,                          // major and minor version of the import
    //     "CPPEnums",                 // name in QML (does not have to match C++ name)
    //     "Error: only enums"            // error in case someone tries to create a MyNamespace object
    // );
}




auto TqdController::set_qml_properties(QQmlApplicationEngine *engine) -> void{
    engine->rootContext()->setContextProperty(QStringLiteral("controller"),  this);
}
