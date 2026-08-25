

// Qt
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QLabel>
#include <QQmlComponent>
#include <QThread>

// base
#include "utility/logger.hpp"
#include "utility/paths.hpp"
#include "depth-camera/settings/dc_settings_paths.hpp"

// qt-utility
#include "qt_logger.hpp"


// local
#include "tqd_controller.hpp"

using namespace tool;
using namespace cam;
using namespace Qt::Literals::StringLiterals;


int main(int argc, char **argv){

    const QString numVersion = "0.1";

    // // init paths
    // DCSettingsPaths::get()->initialize(argv, cam::DCApplicationType::DCManager, 0);

    QSurfaceFormat format;
    format.setVersion(4, 6); // OpenGL 4.6
    format.setProfile(QSurfaceFormat::CoreProfile); // Core Profile (for modern OpenGL)
    QSurfaceFormat::setDefaultFormat(format);

    // Enable OpenGL context
    QGuiApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    std::unique_ptr<TqdController> controller = nullptr;

    int res = 0;
    {
        QApplication app(argc, argv);

        MyEventFilter* filter = new MyEventFilter(&app);
        app.installEventFilter(filter); // Install it on the application

        QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

        // init loggers
        auto logger = BaseLogger::generate_logger_instance<QtLoggerM>();
        logger->init(QApplication::applicationDirPath() + u"/logs"_s, u"ts-qml-demos.html"_s, false);
        logger->set_type_color(QtLoggerM::MessageType::normal,  QColor(189,189,189));
        logger->set_type_color(QtLoggerM::MessageType::warning, QColor(243, 158, 3));
        logger->set_type_color(QtLoggerM::MessageType::error,   QColor(244,4,4));
        logger->set_type_color(QtLoggerM::MessageType::unknow,  Qt::white);
        logger->set_html_file_background_color(u"black"_s);
        logger->log_title(u"Logs %1-v%2"_s.arg(u"ts-qml-demos"_s, numVersion),1);
        logger->log_title(u"[CONTROLLER]"_s,2);

        // rename main thread
        QThread::currentThread()->setObjectName("ts-qml-demos-main");

        // create engine
        QQmlApplicationEngine engine;
        engine.addImportPath(":/qml");
        engine.addImportPath(":/qml/settings");
        engine.addImportPath(":/qml/utility");
        engine.addImportPath(":/qml/samples");

        // set controller
        controller   = std::make_unique<TqdController>();
        QObject::connect(logger, &QtLoggerM::raw_message_signal,   controller.get(), &TqdController::insert_to_log);
        QObject::connect(logger, &QtLoggerM::raw_error_signal,     controller.get(), &TqdController::insert_to_log);
        QObject::connect(logger, &QtLoggerM::raw_warning_signal,   controller.get(), &TqdController::insert_to_log);

        // set data
        controller->set_qml_properties(&engine);

        // load
        engine.load(QUrl("qrc:/qml/AppWindow.qml"));

        // connections
        QObject::connect(
            &engine,
            &QQmlApplicationEngine::objectCreationFailed,
            &app,
            []() {
                qWarning() << "failure";
                QCoreApplication::exit(-1); }
            ,
            Qt::QueuedConnection
        );

        // start application
        res = app.exec();

    }

    qInfo() << "Return " << res;

    qInfo() << "Clean";
    controller = nullptr;

    return res;
}
