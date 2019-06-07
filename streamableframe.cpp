#include "streamableframe.h"
#include <QtCore/qdebug.h>
#include <QJsonDocument>

#include <iostream>

// Header size 33 bytes + 2 byte of checksum
//[-----------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------]
//[                                                             Header                                                                                        | metadata | data |   timestamps    ]
//[-----------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------]
//[ startcode | frame number | width | height | data type | metadata size | data size | timestamp count | metadata checksum | data checksum | header checksum | metadata | data |   timestamps    ]
//[-----------------------------------------------------------------------------------------------------------------------------------------+----------+------+-----------------------------------]
//[     4     |      4       |   4   |   4    |     4     |       4       |     4     |       1         |         2         |       2       |        2        |     ~    |  ~   |      ~          ]
//[-----------------------------------------------------------------------------------------------------------------------------------------+----------+------+-----------------------------------]


StreamableFrame::StreamableFrame(QObject* parent)
    : QObject(parent)
{

}

StreamableFrame::~StreamableFrame(){}

quint32 StreamableFrame::startSequence()
{
    return m_startSequence;
}


bool StreamableFrame::isFrame(const QByteArray& buffer)
{
    QByteArray frame = buffer;
    QDataStream stream(&frame, QIODevice::ReadOnly);
    quint32 start = 0;
    stream >> start;

    if (start != startSequence())
        std::cout << "StreamableFrame::isFrame failed: start (" << start << ") != startSequence(" << startSequence() << ")" << std::endl;

    return start == startSequence();
}


bool StreamableFrame::isComplete(const QByteArray& streamedBuffer)
{
	if (streamedBuffer.size() < m_headerSize)
	{
        std::cout << "StreamableFrame::isComplete received too small buffer: can't contain a valid header." << std::endl;
//		qDebug() << "StreamableFrame::isComplete received too small buffer: can't contain a valid header.";
		return false;
	}

    if (!isFrame(streamedBuffer))
    {
        // Not a frames
        return false;
    }

    QByteArray buffer = streamedBuffer;
    QDataStream stream(&buffer, QIODevice::ReadOnly);

    quint32 start = 0;
    quint32 frameNumber = 0;
    QSize size;
    int dataType = 0;
    int metadataSize = 0;
    int dataSize = 0;
    quint8 timestampCount = 0;

    // Header start 4 bytes
    stream >> start;

    // Frame number 4 bytes
    stream >> frameNumber;

    // Width 4 bytes - heght 4 bytes
    stream >> size;

    // Data type 4 byte
    stream >> dataType;

    // Metadata size 4 bytes
    stream >> metadataSize;

    // Data size 4 bytes
    stream >> dataSize;

    // Timestamp count 1 byte
    stream >> timestampCount;

    int timestampSize = timestampCount * sizeof(quint32);

    int expectedSize = m_headerSize + metadataSize + dataSize + timestampSize;
    int sizeNow = streamedBuffer.size();

    if (sizeNow != expectedSize)
        std::cout << "StreamableFrame::isComplete: size " << sizeNow << " != " << expectedSize << " (= m_headerSize + metadataSize + dataSize + timestampSize)" << std::endl;

    return sizeNow == expectedSize;
}

uint16_t StreamableFrame::fletcher16( uint8_t const *data, size_t bytes )
{
        uint16_t sum1 = 0xff, sum2 = 0xff;

        while (bytes) {
                size_t tlen = bytes > 20 ? 20 : bytes;
                bytes -= tlen;
                do {
                        sum2 += sum1 += *data++;
                } while (--tlen);
                sum1 = (sum1 & 0xff) + (sum1 >> 8);
                sum2 = (sum2 & 0xff) + (sum2 >> 8);
        }
        /* Second reduction step to reduce sums to 8 bits */
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);
        return sum2 << 8 | sum1;
}


bool StreamableFrame::extract(const QByteArray& streamedBuffer)
{
    if (!isComplete(streamedBuffer))
    {
        return false;
    }

    Q_ASSERT(streamedBuffer.size() >= m_headerSize);

    QByteArray buffer = streamedBuffer;
    QDataStream stream(&buffer, QIODevice::ReadOnly);

    quint32 start = 0;
    quint32 frameNumber = 0;
    QSize size;
    int dataType = 0;
    int metadataSize = 0;
    int dataSize = 0;
    quint8 timestampCount = 0;
    quint16 headerChecksum = 0;
    quint16 metadataChecksum = 0;
    quint16 dataChecksum = 0;
    QByteArray metadata;
    QByteArray data;

    // Header start 4 bytes
    stream >> start;

    // Frame number 4 bytes
    stream >> frameNumber;

    // Width 4 bytes - height 4 bytes
    stream >> size;

    // Data type 4 byte
    stream >> dataType;

    // Metadata size 4 bytes
    stream >> metadataSize;

    // Data size 4 bytes
    stream >> dataSize;

    // Timestamp count
    stream >> timestampCount;

    // Metadata checksum 2 bytes
    stream >> metadataChecksum;

    // Data checksum 2 bytes
    stream >> dataChecksum;

    // header checksum 2 bytes
    stream >> headerChecksum;

    // Metadata - variable length
    metadata.resize(metadataSize);
    stream.readRawData(metadata.data(), metadataSize);

    // Data - variable length
    data.resize(dataSize);
    stream.readRawData(data.data(), dataSize);

    for (int n = 0; n < timestampCount; n++)
    {
        quint32 ts = 0;
        stream >> ts;
        m_timestamps << ts;
    }

    bool headerValid = 0 == headerChecksum;
//    bool validMetadata = Checksum::fletcher16Checksum(const_cast<quint8*>(reinterpret_cast<const quint8*>(metadata.constData())), metadata.size()) == metadataChecksum;
    bool validMetadata = 0 == metadataChecksum; // = fletcher16(const_cast<quint8*>(reinterpret_cast<const quint8*>(metadata.constData())), static_cast<unsigned long>(metadata.size())) == metadataChecksum;
    bool validData = 0 == dataChecksum;

    if (headerValid && validMetadata && validData)
    {
        m_data = data;
        m_size = size;
        m_frameNumber = frameNumber;
        m_dataType = dataType;
        m_metadata = metadata;
        m_timestampCount = timestampCount;
        return true;
    }
    else
    {
        return false;
    }
}


QByteArray StreamableFrame::data() const
{
    return m_data;
}


QSize StreamableFrame::size()
{
    return m_size;
}


quint32 StreamableFrame::frameNumber()
{
    return m_frameNumber;
}


int StreamableFrame::dataType() const
{
    return m_dataType;
}


QJsonObject StreamableFrame::metadata()
{
    QJsonDocument doc = QJsonDocument::fromJson(m_metadata);
    return doc.object();

}


quint8 StreamableFrame::timestampCount()
{
    return m_timestampCount;
}


QVector<quint32> StreamableFrame::timestamps()
{
    return m_timestamps;
}







