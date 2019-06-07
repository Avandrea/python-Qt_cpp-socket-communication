/**
    BMCR_SpectralClassification
    @file databuffer.h
    @brief this buffer manages the data in a circular buffer. It is used by datareceiver to handle the data
    @author Andrea Avigni
    @version 1.0 05/06/19
*/

#ifndef DATABUFFER_H
#define DATABUFFER_H

#include <QBuffer>
//#include <QReadWriteLock>
#include <QMutex>

class DataBuffer: public QBuffer
{
    Q_OBJECT

public:
    DataBuffer(QObject *parent = Q_NULLPTR);
    ~DataBuffer();

    bool append(QByteArray bytes, qint64 len);
    QByteArray getLastAddedLine(qint64 dataSize);
    QByteArray getLastReadLine(qint64 dataSize);

    void setLastAddedLineIndex(qint16 lastAddedLine, int frameSize);
    qint64 getLastAddedLineIndex();
    qint64 getLastReadLineIndex();

    qint64 prev_pos;

//    QReadWriteLock lock;
    QMutex mutex;

//Q_SIGNALS:
//    QByteArray readyRead(QByteArray newdata);

private:
     qint64 m_lastAddedLine;
     qint64 m_lastReadLine;
};

#endif // DATABUFFER_H
