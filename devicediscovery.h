#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QObject>
#include <QUdpSocket>
#include <QList>
#include <QString>
#include <QStringList>

const quint16 discoveryPort = 54546;

class DeviceDiscovery : public QUdpSocket {
    Q_OBJECT
//    Q_PROPERTY(QStringList addressList READ m_addressList)

public:
    DeviceDiscovery(QObject* parent = Q_NULLPTR);
    ~DeviceDiscovery();

    Q_INVOKABLE void discovery(quint16 port = discoveryPort);

    QStringList m_addressList;

protected Q_SLOTS:
    void readPendingDatagrams();

protected:
    QUdpSocket* m_socket;
};


#endif // DEVICEDISCOVERY_H
