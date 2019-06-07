/**
    BMCR_SpectralClassification
    @file datareceiver.h
    @brief this is the main class, it manages the streamablefram class and the databuffer class
    @author
    @version
*/

#include <QObject>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QJsonArray>
#include <QImage>
#include "databuffer.h"

enum class StreamingMode
{
    Unicast = 0,
    Multicast
};

class DataReceiver: public QUdpSocket
{
    Q_OBJECT
//    Q_PROPERTY(QList<QString> addressList READ m_addressList CONSTANT)
//    Q_PROPERTY(QImage myImage READ m_image)

public:
    DataReceiver(QObject* parent = Q_NULLPTR);
    ~DataReceiver();
    Q_INVOKABLE bool bind(const QString& address, int port);
    bool bind(const QHostAddress& address, quint16 port = 0, StreamingMode mode = StreamingMode::Unicast);
    Q_INVOKABLE bool send(const QString& address, int port, const QString& msg);
    Q_INVOKABLE QImage getLastAddedLine(int width, int height);
    Q_INVOKABLE QImage getLastReadLine(int width, int height);
    Q_INVOKABLE void close();

Q_SIGNALS:
    void nextFrame(const QByteArray& buffer, const QSize& size, quint32 frameNumber, int type);
    void nextLine(const QByteArray& buffer, int width, quint32 frameNumber);
    void nextLineBatch(const QByteArray& batch, int batchSize, int width, const QVector<quint32>& frameNumbers);
    void newDataReceived(QByteArray array, qint64 bufferSize);

protected Q_SLOTS:
    void onReadyRead(QByteArray chunk);
    void readPendingDatagrams();

protected:
    QList<QString> m_addressList;
    QHostAddress m_address;
    int m_port;
    StreamingMode m_streamingMode;
    bool m_expectingFrame;
    QUdpSocket *m_socket;
    QByteArray m_frameData;

    DataBuffer *m_buffer;
    QImage m_image;

};

