#include "cameracontrol.h"
#include <QJsonArray>
#include <QJsonDocument>

CameraControl::CameraControl(QObject* parent) : QTcpSocket (parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
}

CameraControl::~CameraControl()
{
    close();
    m_socket->deleteLater();
}

void CameraControl::close()
{
//    qWarning() << "[CAMERA CONTROL] Disconnecting from " << m_address << ":" << QString::number(m_portFTP).toUtf8().constData() << ".";
    m_socket->disconnectFromHost();
    m_socket->close();
}

bool CameraControl::startStreaming(QString ip, quint16 port){
    if (m_socket->state() != QAbstractSocket::ConnectedState)
        m_socket->connectToHost(ip, port); //"192.168.1.50", 46088
    else {
        QJsonArray tempArraySmall = {1, true};
        QJsonArray tempArray;
        tempArray.append(tempArraySmall);
        QJsonObject toBeSentObj = jsonBuilder("cortex.set", "output.stream.frames.udp", tempArray);
        QJsonDocument toBeSentDoc(toBeSentObj);
        QByteArray toBeSentBytes = toBeSentDoc.toJson();
        m_socket->write(toBeSentBytes);
        m_socket->waitForBytesWritten(10);
    }
    qWarning() << m_socket->state();
//    qWarning() << "Connected";
//    QJsonArray tempArraySmall = {1, true};
//    QJsonArray tempArray;
//    tempArray.append(tempArraySmall);
//    QJsonObject toBeSentObj = jsonBuilder("cortex.set", "output.stream.frames.udp", tempArray);
//    QJsonDocument toBeSentDoc(toBeSentObj);
//    QByteArray toBeSentBytes = toBeSentDoc.toJson();
//    m_socket->write(toBeSentBytes);
    return true; // m_socket->waitForBytesWritten(10);
}

void CameraControl::onConnected(){
    QJsonArray tempArraySmall = {1, true};
    QJsonArray tempArray;
    tempArray.append(tempArraySmall);
    QJsonObject toBeSentObj = jsonBuilder("cortex.set", "output.stream.frames.udp", tempArray);
    QJsonDocument toBeSentDoc(toBeSentObj);
    QByteArray toBeSentBytes = toBeSentDoc.toJson();
    m_socket->write(toBeSentBytes);
    m_socket->waitForBytesWritten(10);
}

bool CameraControl::stopStreaming(QString ip, quint16 port){
    if (m_socket->state() != QAbstractSocket::ConnectedState)
        m_socket->connectToHost(ip, port); //"192.168.1.50", 46088
    QJsonArray tempArraySmall = {1, false};
    QJsonArray tempArray;
    tempArray.append(tempArraySmall);
    QJsonObject toBeSentObj = jsonBuilder("cortex.set", "output.stream.frames.udp", tempArray);
    QJsonDocument toBeSentDoc(toBeSentObj);
    QByteArray toBeSentBytes = toBeSentDoc.toJson();
    m_socket->write(toBeSentBytes);
    return m_socket->waitForBytesWritten(10);
}

bool CameraControl::connectDevice(QString host, QString port)
{
    m_socket->connectToHost(host, port.toUShort());
    return m_socket->waitForConnected();
}

bool CameraControl::sendCommand(QByteArray data)
{
    if(m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QJsonArray tempArraySmall = {1, true};
        QJsonArray tempArray;
        tempArray.append(tempArraySmall);
        QJsonObject toBeSentObj = jsonBuilder("cortex.set", "output.stream.frames.udp", tempArray);
        QJsonDocument toBeSentDoc(toBeSentObj);
        QByteArray toBeSentBytes = toBeSentDoc.toJson();

//        m_socket->write(IntToArray(data.size())); //write size of data
        m_socket->write(toBeSentBytes); //write the data itself
        return m_socket->waitForBytesWritten();
    }
    else
        return false;
}

void CameraControl::readPendingDatagrams()
{
    QStringList list;
    while (m_socket->canReadLine()) {
        QString data = QString(m_socket->readLine());
        list.append(data);
        qWarning() << "[CAMERA CONTROL] data is " << data;
   }

    qWarning() << "[CAMERA CONTROL] reading input data";

}

QJsonObject CameraControl::jsonBuilder(const QString& methodName, const QString& paramName, const QJsonArray& args){
    //{"jsonrpc": "2.0", "method": "cortex.set", "params": ["camera.framerate", "{\"args\": [[6, 10.0]]}"], "id": 1}

    // Build the args object if not empty
    QJsonObject argsObj;
    if (!args.empty()){
        argsObj.insert("args", args);
    }
    QJsonDocument argsDoc(argsObj);
    QString argsStr(argsDoc.toJson(QJsonDocument::Compact));

    // Build the params array
    QJsonArray paramsArray;
    paramsArray.append(paramName);
    if (!args.isEmpty()){
        paramsArray.append(argsStr);
    }

    // Fill the object to be returned
    QJsonObject retObj;
    retObj.insert("jsonrpc", "2.0");
    retObj.insert("method", methodName);
    retObj.insert("params", paramsArray);
    retObj.insert("id", 1);

    // Debug print all
    QJsonDocument tempDoc(retObj);
    QString tempString(tempDoc.toJson(QJsonDocument::Compact));
//    qWarning() << "5555555555555555 " << tempString;

    return retObj;
}

//bool CameraControl::send(const QString& address, int port, const QString& msg){
//    QByteArray data;
//    data += msg;
//    m_socket->writeDatagram(data, QHostAddress(address), static_cast<quint16>(port));
//    return true;
//}
