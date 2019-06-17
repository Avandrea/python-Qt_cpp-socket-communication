#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "devicediscovery.h"
#include "datareceiver.h"
#include "streamableframe.h"
#include "cameracontrol.h"
#include "imageitem.h"

/*! \mainpage BMCR Spectral classification project
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Main components
 *
 * \subsection step1 1. Discovery
 *
 * The discovery class listen to a specific port (54546) for broadcast communications.
 * When it receives something it checks wheter the received element comes from a Specim camera.
 */

/**
    BMCR_SpectralClassification
    @file main.cpp
    @brief this is the entry point, it registers the required types and launches the app
    @author Andrea Avigni
    @version 1.0 05/06/2019
*/

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qmlRegisterType<DeviceDiscovery>("MyDeviceDiscovery", 1, 0, "DeviceDiscovery");
    qmlRegisterType<DataReceiver>("MyDataReceiver", 1, 0, "DataReceiver");
    qmlRegisterType<CameraControl>("MyCameraControl", 1, 0, "CameraControl");
    qmlRegisterType<ImageItem>("MyImageItem", 1, 0, "ImageItem");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
