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
