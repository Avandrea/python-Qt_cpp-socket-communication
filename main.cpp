#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "devicediscovery.h"
#include "datareceiver.h"
#include "streamableframe.h"
#include "imageitem.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qmlRegisterType<DeviceDiscovery>("MyDeviceDiscovery", 1, 0, "DeviceDiscovery");
    qmlRegisterType<DataReceiver>("MyDataReceiver", 1, 0, "DataReceiver");
    qmlRegisterType<ImageItem>("MyImageItem", 1, 0, "ImageItem");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
