#include "datareceiver.h"
#include "streamableframe.h"
#include <iostream>

DataReceiver::DataReceiver(QObject *parent) : QUdpSocket(parent)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
//    connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));

    m_buffer = new DataBuffer();
    m_buffer->open(QIODevice::ReadWrite);
}

DataReceiver::~DataReceiver()
{
    close();
	m_socket->deleteLater();
}

void DataReceiver::readPendingDatagrams()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_socket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

//        qWarning() << "[DATA RECEIVER] Receiving data from " << qPrintable(sender.toString()) << " at port " << qPrintable(QString::number(senderPort));
//        qWarning() << "[DATA RECEIVER] Printing datagram's data size " << qPrintable(QString::number(datagram.size()));

        onReadyRead(datagram);
   }
}

bool DataReceiver::bind(const QHostAddress& address, quint16 port /*= 0*/, StreamingMode mode /*= StreamingMode::Unicast*/)
{
    qWarning() << "[DATA RECEIVER] Binding to ip " << address.toString().toUtf8().constData() << " at port " << QString::number(port).toUtf8().constData() << ".";

	if (m_socket->state() != QAbstractSocket::BoundState)
	{
        if (mode == StreamingMode::Unicast)
        {
            if (m_socket->bind(address, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint) == false)
            {
                qDebug() << "[DATA RECEIVER] error 1 is " << m_socket->errorString();
                return false;
            }
        }
        else
        {
            if (m_socket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress | QAbstractSocket::ReuseAddressHint) == false)
            {
                qDebug() << "[DATA RECEIVER] error 2 is " << m_socket->errorString();
                return false;
            }

            if (m_socket->joinMulticastGroup(address) == false)
            {
                qDebug() << "[DATA RECEIVER] error 3 is " << m_socket->errorString();
                return false;
            }
        }
	}
	else
	{
        qDebug() << "[DATA RECEIVER] Socket already bound";
        return false;
	}

//    m_address = address;
//    m_portFTP = port;
    m_streamingMode = mode;
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    return true;
}

bool DataReceiver::bind(const QString& deviceAddress, int portFTP, int portRPC, int portMulticast, const QString& localAddress){
    m_address = deviceAddress;
    m_portMulticast = portMulticast;
    m_portFTP = portFTP;
    m_portRPC = portRPC;

    return bind(QHostAddress(deviceAddress), static_cast<quint16>(m_portMulticast) /*= 0*/, StreamingMode::Multicast);
}

bool DataReceiver::connectDevice(const QString& deviceIp, const QString& port, const QString& localAddress){

    qWarning() << "[DATA RECEIVER] Connecting to ip " << deviceIp << " at port " << port << ".";
    m_address = deviceIp;
    m_portFTP = port.toUShort();
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

//    qWarning() << m_socket->bind(QHostAddress(localAddress), 8000, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
//    qWarning() << m_socket->errorString();

    if (m_socket->state() != QAbstractSocket::BoundState)
    {
        m_socket->connectToHost(QHostAddress(deviceIp), port.toUShort());
    }
    else
    {
        qDebug() << "[DATA RECEIVER] Socket already connected";
        return false;
    }

    return true;
}

void DataReceiver::onConnected(){
    qWarning() << "[DATA RECEIVER] Connection to " << m_address << ":" << QString::number(m_portFTP).toUtf8().constData() << " established";
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

bool DataReceiver::send(const QString& address, int port, const QString& msg){
    QByteArray data;
    data += msg;
    m_socket->writeDatagram(data, QHostAddress(address), static_cast<quint16>(port));
    return true;
}

QImage DataReceiver::getLastAddedLine(int width, int height){
    QByteArray array = m_buffer->getLastAddedLine(width*height);
    return QImage(reinterpret_cast<unsigned char*>(array.data()), width, height, width, QImage::Format_Grayscale8);
}

QImage DataReceiver::getLastReadLine(int width, int height){
    QByteArray array = m_buffer->getLastReadLine(width*height);

//    unsigned char* test = reinterpret_cast<unsigned char*>(array.data());

//    for (int i = 0; i < array.size(); ++i){
//        std::cout << +test[i] << " ";
//        if ((i + 1) % 26 == 0)
//            std::cout << std::endl;
//    }

    return QImage(reinterpret_cast<unsigned char*>(array.data()), width, height, width, QImage::Format_Grayscale8);
}

void DataReceiver::close()
{
    qWarning() << "[DATA RECEIVER] Disconnecting from " << m_address << ":" << QString::number(m_portFTP).toUtf8().constData() << ".";
    m_socket->disconnectFromHost();
    m_socket->close();
}


void DataReceiver::onReadyRead(QByteArray chunk)
{
    if (StreamableFrame::isFrame(chunk))
    {
        m_expectingFrame = true;

        // Check whether an header is received before the end of the previous line
        if (StreamableFrame::isComplete(m_frameData))
            m_frameData.clear();
    }

    if (m_expectingFrame)
    {
        m_frameData.append(chunk);

        if (StreamableFrame::isComplete(m_frameData))
        {
            StreamableFrame frame;

            // Discard frame if not valid
            if (frame.extract(m_frameData))
            {
                // Check Metadata: if it exists and contains a field bufferType do some stuff
                QJsonObject json = frame.metadata();
                QString bufferType = json["bufferType"].toString("not_defined");

                if (bufferType == "line")
                {
                    emit nextLine(frame.data(), frame.size().width(), frame.frameNumber());
                }
                else if (bufferType == "lineBatch")
                {
                    QJsonArray lineNumbersArray = json["lineNumbers"].toArray();
                    QVector<quint32> lineNumbers;

                    for (QJsonValue number : lineNumbersArray)
                    {
                        lineNumbers << number.toInt();
                    }

                    emit nextLineBatch(frame.data(), frame.size().height(), frame.size().width(), lineNumbers);
                }
                else if (bufferType == "frame")
                {
                    qDebug() << "[DATA RECEIVER] Emit signal nextFrame signal with data size of " << frame.size();
                    emit nextFrame(frame.data(), frame.size(), frame.frameNumber(), frame.dataType());
                }
                else
                {
                    qWarning() << "received unknown data";
                }

                // Lock the read/write lock and append to the buffer
//                m_buffer->mutex.tryLock(1000);
//                m_buffer->append(frame.data(), frame.size().width() * frame.size().height());
//                m_buffer->mutex.unlock();


                // Set last added line
//                m_buffer->setLastAddedLineIndex(static_cast<qint16>(m_buffer->size()), frame.size().width() * frame.size().height());
            }

            m_expectingFrame = false;
            m_frameData.clear();
        }
        // This part is for frames that have a valid start sequence but never complete. At some point we need to discard it.
        // Let put an arbitrary value of 4k UHD (3840x2160)
        else
        {
            static int MAX_FRAME_SIZE = 3840 * 2160 * 2;
            if(m_frameData.size() > MAX_FRAME_SIZE)
            {
                m_frameData.clear();
            }
        }
    }


}
