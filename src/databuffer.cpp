#include "databuffer.h"
#include <iostream>

DataBuffer::DataBuffer(QObject *parent) : QBuffer(parent)
{
    m_lastReadLine = 0;
    m_lastAddedLine = -1;
}

DataBuffer::~DataBuffer()
{}


bool DataBuffer::append(QByteArray bytes, qint64 len){
    if(write(bytes, len) == -1)
        return false;
    return true;
}

QByteArray DataBuffer::getLastAddedLine(qint64 dataSize){
    // Lock read/write lock
//    lock.lockForRead();
    mutex.tryLock(1000);

    // Move pos to the beginning of last element
    this->seek(this->pos() - dataSize);

    // Read data without moving pos
    QByteArray retByteArray = this->peek(dataSize);

    // Restore pos
    this->seek(this->pos() + dataSize);

    // Unlock read/write lock
    mutex.unlock();

    return retByteArray;
}

QByteArray DataBuffer::getLastReadLine(qint64 dataSize){
    // Backup previous pos
    prev_pos = this->pos();

    // Lock read/write lock
    mutex.tryLock(1000);

    std::cout << m_lastReadLine << std::endl;

    // Move the pos to the last read position, read the data and increase the last read position by dataSize
    this->seek(m_lastReadLine);
    QByteArray retByteArray = this->peek(dataSize);
    if (m_lastReadLine + dataSize < this->size() || m_lastReadLine == 0){
//        std::cout << "---------------- " << m_lastReadLine << " + " << dataSize << " <= " << this->size() << std::endl;
        m_lastReadLine += dataSize;
    }

    // Restore previous pos
    this->seek(prev_pos);

    // Unlock read/write lock
    mutex.unlock();

    return retByteArray;
}

void DataBuffer::setLastAddedLineIndex(qint16 lastAddedLine, int frameSize){
    if (m_lastAddedLine == -1)
        m_lastAddedLine = 0;
    else
        m_lastAddedLine = lastAddedLine - frameSize;

//    std::cout << "################# m LAST ADDED INDEX: " << m_lastAddedLine << std::endl;
}
qint64 DataBuffer::getLastAddedLineIndex(){
    return m_lastAddedLine;
}

qint64 DataBuffer::getLastReadLineIndex(){
    return m_lastReadLine;
}
