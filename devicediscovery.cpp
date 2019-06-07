#include "devicediscovery.h"
#include <iostream>

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

        std::cout << "Receiving data from " << qPrintable(sender.toString()) << " at port " << qPrintable(QString::number(senderPort)) << std::endl;
        std::cout << "Printing datagram's data size " << qPrintable(QString::number(datagram.size())) << std::endl;


//        QString output;
//        output += QString(datagram);
//        onReadyRead(datagram);
   }


    qWarning() << "Message from: " << sender.toString();
    qWarning() << "Message port: " << senderPort;
    qWarning() << "Message: " << datagram;

    qWarning() << " RX ";


    if (!ipIsInList(sender.toString())){
        parseJSON()
    }
}
