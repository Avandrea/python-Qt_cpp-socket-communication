/**
 *  BMCR_SpectralClassification
 *  @file devicediscovery.h
 *  @brief this class manages the discovery of new devices
 *  The discovery waits for any incoming byte array at a specific port (the documentation for the Specim camera define it as 54546).
 *  When an elements comes in, it is parsed as a JSON and the program checks wheter it contains a field "publisher" equal to "Specim"
 *  and a field "services". If it is the case the ip and the relative ports are added to the list of devices.
 *  @author Andrea Avigni
 *  @version 1.0 11/06/2019
*/

#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QObject>
#include <QUdpSocket>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QVariantList>

const quint16 discoveryPort = 54546;

class DeviceDiscovery : public QUdpSocket {
    Q_OBJECT
//    Q_PROPERTY(QVariantList addressStringList READ addressStringList)

public:
    DeviceDiscovery(QObject* parent = Q_NULLPTR);
    ~DeviceDiscovery();

    Q_INVOKABLE void discovery(quint16 port = discoveryPort);
    Q_INVOKABLE void close();

//    QList<QMap<QString, QString>> m_addressList;
    QMap<QString, QMap<QString, QString>> m_addressList;
    QList<QString> addressStringList;

Q_SIGNALS:
//    void addressStringListChanged(const QList<QString> addressList);
    void addressStringListChanged(const QVariantMap addressList);

protected Q_SLOTS:
    void readPendingDatagrams();

protected:
    QUdpSocket* m_socket;
    QJsonObject jsonDiscovery;

    bool ipIsInList(QString ip);
};


#endif // DEVICEDISCOVERY_H
