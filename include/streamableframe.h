/**
 *  BMCR_SpectralClassification
 *  @file streamableframe.h
 *  @brief this class manages the checks over the incoming frame and extract the elements from the frame
 *  @author Andrea Avigni
 *  @version 1.0 05/06/2019
*/

#pragma once

#include <QObject>
#include <QSize>
#include <QJsonObject>
#include <QVector>

#include <QIODevice>

#include <QTextStream>
#include <QFile>
#include <QDataStream>

class StreamableFrame : public QObject
{
    Q_OBJECT

public:
    StreamableFrame(QObject* parent = Q_NULLPTR);
    ~StreamableFrame();

public:
    static quint32 startSequence();
    static bool isFrame(const QByteArray& buffer);
    static bool isComplete(const QByteArray& buffer);    
    bool extract(const QByteArray& streamedBuffer);

    QByteArray data() const;
    QSize size();
    quint32 frameNumber();
    int dataType() const;
    QJsonObject metadata();
    quint8 timestampCount();
    QVector<quint32> timestamps();
  

private:
    QByteArray m_data;
    QSize   m_size;
    quint32 m_frameNumber = 0;
    int m_dataType = 0;
    QByteArray m_metadata;
    quint8 m_timestampCount = 0;
    QVector<quint32> m_timestamps;

    static const quint32 m_startSequence = 0x68dd3ef4; // The hex converted to int32 is = 1759330036
    static const int m_headerSize = 35;

    uint16_t fletcher16( uint8_t const *data, size_t bytes );

};
    
