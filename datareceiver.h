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

    /**
        Binds the client to the port obtained from the discovery for receiving data. It should be port 22, but the discovery manages this by itself.
        It also sets the selectedDevice as an incoming device

        @param deviceIp the device ip as a string that will be used for the connection
        @param localAddress the local address to bind the port to
        @return true if the connection succeded and false otherwise
    */
    Q_INVOKABLE bool connectDevice(const QString& deviceIp, const QString& port, const QString& localAddress = "0.0.0.0");

    /**
        Binds the client to the port obtained from the discovery for receiving data. It should be port 22, but the discovery manages this by itself.
        It also sets the selectedDevice as an incoming device

        @param deviceAddress the device ip as a string that will be used for the connection
        @param localAddress the local address to bind the port to
        @return true if the connection succeded and false otherwise
    */
    Q_INVOKABLE bool bind(const QString& deviceAddress, int portFTP, int portRPC, const QString& address = "0.0.0.0");

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
    void onConnected();
    void onReadyRead(QByteArray chunk);
    void readPendingDatagrams();

protected:
    QList<QString> m_addressList;
    QString m_address;
    int m_portFTP;
    int m_portRPC;
    StreamingMode m_streamingMode;
    bool m_expectingFrame;
    QUdpSocket *m_socket;
    QByteArray m_frameData;

    DataBuffer *m_buffer;
    QImage m_image;

};

