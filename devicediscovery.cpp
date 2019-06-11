#include "devicediscovery.h"
#include <iostream>
#include <QJsonDocument>

DeviceDiscovery::DeviceDiscovery(QObject *parent) : QUdpSocket(parent)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

DeviceDiscovery::~DeviceDiscovery(){
    m_socket->close();
    m_socket->deleteLater();
}

void DeviceDiscovery::discovery(quint16 port)
{
    QHostAddress sender;
    quint16 senderPort;

    m_socket->bind(QHostAddress::Broadcast, port);
}

void DeviceDiscovery::readPendingDatagrams(){
    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;

    while (m_socket->hasPendingDatagrams()) {
        datagram.resize(m_socket->pendingDatagramSize());

        m_socket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        qWarning() << "[DEVICE DISCOVERY] Receiving data from " << qPrintable(sender.toString()) << " at port " << qPrintable(QString::number(senderPort));
        qWarning() << "[DEVICE DISCOVERY] Printing datagram's data size " << qPrintable(QString::number(datagram.size()));
   }


//    qWarning() << "Message from: " << sender.toString();
//    qWarning() << "Message port: " << senderPort;
//    qWarning() << "Message: " << datagram;
//    qWarning() << " RX ";


    // Check if the ip is in the list: if not parse the json and store the data
    QString senderString = sender.toString();
    if (!ipIsInList(senderString)){

        // Parse the incoming json
        QJsonParseError error;
        jsonDiscovery = QJsonDocument::fromJson(datagram.data(), &error).object();

        // If the json comes from a specim camera
        if (jsonDiscovery["publisher"] == "Specim" && jsonDiscovery.contains("services")){

            // Parse the elements inside services
            QJsonObject jsonServices(jsonDiscovery["services"].toObject());
            QJsonObject jsonFtp = jsonServices["ftp"].toObject();
            QJsonObject jsonRpc = jsonServices["rpc"].toObject();
            QString portFTP = QString::number(jsonFtp["port"].toInt());
            QString portRPC = QString::number(jsonRpc["port"].toInt());

            // Create a temporary map and insert ip, portFTP and portRPC
            QVariantMap tempMap;
//            tempMap.insert("ip", senderString);
            tempMap.insert("portFTP", portFTP);
            tempMap.insert("portRPC", portRPC);
//            m_addressList.insert(senderString, tempMap);

            QVariantMap map;
            map.insert(senderString, tempMap);

            // Add new ips to the string list that will be shown in the GUI
            qWarning() << "[DEVICE DISCOVERY] adding new ip to devices list.";
            addressStringList.append(senderString);
//            emit addressStringListChanged(addressStringList);
            emit addressStringListChanged(map);
        }
    }
}

bool DeviceDiscovery::ipIsInList(QString ip){
    if (m_addressList.contains(ip))
        return true;
    else {
        return false;
    }
//    QListIterator<QMap<QString, QString>> i(m_addressList);
//    while (i.hasNext()){
//        if (ip == i.next()["ip"])
//            return true;
//    }
//    return false;
}

void DeviceDiscovery::close()
{
    m_socket->close();
    m_addressList.clear();
    addressStringList.clear();
}
