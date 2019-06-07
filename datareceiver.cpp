#include "datareceiver.h"
#include "streamableframe.h"
#include <iostream>

DataReceiver::DataReceiver(QObject *parent) : QUdpSocket(parent)
{
    m_socket = new QUdpSocket(this);
//    m_frameData = new QByteArray();
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

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

        std::cout << "Receiving data from " << qPrintable(sender.toString()) << " at port " << qPrintable(QString::number(senderPort)) << std::endl;
        std::cout << "Printing datagram's data size " << qPrintable(QString::number(datagram.size())) << std::endl;

        QString output;
        output += QString(datagram);
        onReadyRead(datagram);
   }
}

bool DataReceiver::bind(const QHostAddress& address, quint16 port /*= 0*/, StreamingMode mode /*= StreamingMode::Unicast*/)
{
    std::cout << "Connecting to ip " << address.toString().toUtf8().constData() << " at port " << QString::number(port).toUtf8().constData() << "." << std::endl;

	if (m_socket->state() != QAbstractSocket::BoundState)
	{
        if (mode == StreamingMode::Unicast)
        {
            if (m_socket->bind(address, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint) == false)
            {
                qDebug() << "error 1 is " << m_socket->errorString();
                return false;
            }
        }
        else
        {
            if (m_socket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress | QAbstractSocket::ReuseAddressHint) == false)
            {
                qDebug() << "error 2 is " << m_socket->errorString();
                return false;
            }

            if (m_socket->joinMulticastGroup(address) == false)
            {
                qDebug() << "error 3 is " << m_socket->errorString();
                return false;
            }
        }
	}
	else
	{
        qDebug() << "Socket already bound";
        return false;
	}

    m_address = address;
    m_port = port;
    m_streamingMode = mode;
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    return true;
}

bool DataReceiver::bind(const QString& maddress, int port /*= 0*/){
    return bind(QHostAddress(maddress), static_cast<quint16>(port) /*= 0*/, StreamingMode::Unicast);
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
    m_socket->close();
}


void DataReceiver::onReadyRead(QByteArray chunk)
{

    if (StreamableFrame::isFrame(chunk))
    {
        m_expectingFrame = true;
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
                /*
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
					emit nextFrame(frame.data(), frame.size(), frame.frameNumber(), frame.dataType());
				}
				else
				{
					qWarning() << "received unknown data";
				}
                */
            }

            m_expectingFrame = false;

            std::cout << "Incoming dimensions: width = " << frame.size().width() << " height = " << frame.size().height() << std::endl;

            // Lock the read/write lock
            m_buffer->mutex.tryLock(1000);
            m_buffer->append(frame.data(), frame.size().width() * frame.size().height());
            m_buffer->mutex.unlock();

            std::cout << "Emit signal newDataReceived with data size of " << m_buffer->size() << std::endl;
            emit newDataReceived(frame.data(), m_buffer->size());
            m_buffer->setLastAddedLineIndex(static_cast<qint16>(m_buffer->size()), frame.size().width() * frame.size().height());
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
