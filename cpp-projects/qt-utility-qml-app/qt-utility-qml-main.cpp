
// Qt
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
// #include <QIdentityProxyModel>
#include <QQuickStyle>

// local
// #include "ah_remote_controller.hpp"

int main(int argc, char *argv[]){


    // std::unique_ptr<AHRemoteController> controller = nullptr;

    int res = 0;
    {
        // init ugi
        QGuiApplication app(argc, argv);

        // create engine
        QQmlApplicationEngine engine;

        // set controller
        // controller   = std::make_unique<AHRemoteController>();

        // define style
        // QQuickStyle::setStyle("Material");

        // set data
        // controller->set_qml_properties(&engine);

        // connections
        QObject::connect(
            &engine,
            &QQmlApplicationEngine::objectCreationFailed,
            &app,
            []() { QCoreApplication::exit(-1); },
            Qt::QueuedConnection
            );

        // load module
        engine.addImportPath("qrc:/qml");
        engine.load(QUrl(QStringLiteral("qrc:/qml/QtUtilityMain.qml")));

        if (engine.rootObjects().isEmpty()){
            return -1;
        }


        // start application
        res = app.exec();

        qInfo() << "Clean";
        // controller = nullptr;
    }

    qInfo() << "Return " << res;
    return res;
}
