/**
 *  BMCR_SpectralClassification
 *  @file cameracontrol.h
 *  @brief this class exposes some methods for camera parameters control
 *  @author Andrea Avigni
 *  @version 1.0 12/06/19
 *
 *  The list of features is
 *
 * <table>
 * <caption id="multi_row">Features</caption>
 * <tr><th>Name                                 <th>Mode (r,w)      <th>Type      <th>Description                                               <th>Notes
 * <tr><td>camera.binning.spatial               <td>rw              <td>enum      <td>horizontal binning 1,2,4                                  <td>binnings > 1 are only available if preprocessing.enabled = True
 * <tr><td>camera.binning.spectral              <td>rw              <td>enum      <td>vertical binning 1,2,4                                    <td>binnings > 1 are only available if preprocessing.enabled = True
 * <tr><td>camera.data.bitsperpixel             <td>r               <td>int       <td>bits per pixel                                            <td>
 * <tr><td>camera.data.bytesperpixel            <td>r               <td>int       <td>bytes per pixel                                           <td>
 * <tr><td>camera.data.maxnumber                <td>r               <td>int       <td>maximum DN value of raw data                              <td>
 * <tr><td>camera.exposure                      <td>rw              <td>float     <td>exposure time in ms                                       <td>also min and max available
 * <tr><td>camera.framerate                     <td>rw              <td>float     <td>framerate in Hz                                           <td>also min and max available
 * <tr><td>camera.framerate.measured            <td>r               <td>float     <td>current measured FPS                                      <td>
 * <tr><td>camera.image.height                  <td>r               <td>int       <td>image height in pixels                                    <td>it is recommended to use height parameters from img header data
 * <tr><td>camera.image.width                   <td>r               <td>int       <td>image widht in pixels                                     <td>it is recommended to use width parameters from img header data
 * <tr><td>camera.model                         <td>r               <td>string    <td>prints out the camera model                               <td>e.g. FX50
 * <tr><td>camera.shutter.state                 <td>r               <td>enum      <td>current state of shutter                                  <td>open or close
 * <tr><td>camera.shutter.close                 <td>w               <td>command   <td>close shutter                                             <td>
 * <tr><td>camera.shutter.open                  <td>w               <td>command   <td>open shutter                                              <td>
 * <tr><td>camera.temperature.update            <td>w               <td>command   <td>updates temperature features                              <td>
 * <tr><td>camera.temperature.fpa               <td>r               <td>float     <td>detector temp in Kelvin                                   <td>should be ~80 kelvins
 * <tr><td>camera.temperature.fpga              <td>r               <td>float     <td>detector fpga temperature in Celsius                      <td>
 * <tr><td>camera.temperature.board             <td>r               <td>float     <td>telectrics board temperature in Celsius                   <td>
 * <tr><td>scb.temperature.temp1                <td>r               <td>float     <td>Hot plate temperature in Celsius                          <td>
 * <tr><td>scb.temperature.temp2                <td>r               <td>float     <td>Optics temperature in Celsius                             <td>stabilized in 25C°
 * <tr><td>scb.temperature.temp3                <td>r               <td>float     <td>Shutter temperature in Celsius                            <td>
 * <tr><td>camera.trigger.mode                  <td>rw              <td>enum      <td>trigger mode = internal, CC1 exposure, CC1 sync           <td>
 * <tr><td>camera.wavelengthtable               <td>r               <td>enum      <td>current wavelength table                                  <td>
 * <tr><td>networking.multicast.datagramsize    <td>rw              <td>int       <td>size of UDP packet sent                                   <td>max = 8972
 * <tr><td>output.stream.frames.udp             <td>rw              <td>bool      <td>enables streaming of detector frames through UDP          <td>
 * <tr><td>output.stream.frames.web             <td>rw              <td>bool      <td>enables streaming of detector frames through web socket   <td>
 * <tr><td>preprocessing.batch.frames           <td>rw              <td>int       <td>number of frames to be preprocessed at once               <td>default = 10 (should not be changed)
 * <tr><td>preprocessing.enabled                <td>rw              <td>bool      <td>enables preprocessing                                     <td>binnings > 1 are only available if preprocessing.enabled = True
 * <tr><td>processing.batch.frames              <td>rw              <td>int       <td>number of frames to be processed at once (UDP sending)    <td>default = 5 (should not be changed)
 * <tr><td>serialnumber                         <td>r               <td>int       <td>serial number of the device                               <td>
 * </table>
 *
 * The command structure is as follows
 * <table>
 * <tr><th>Command          <th>Structure
 * <tr><td>cortex.set       <td> Send: "{"jsonrpc": "2.0", "method": "cortex.set", "params": ["camera.framerate", "{\"args\": [[6, 10.0]]}"], "id": 1}" <br> Receive: "{"jsonrpc": "2.0", "result": true, "id": 1}"
 * <tr><td>cortex.get       <td> Send: "{"jsonrpc": "2.0", "method": "cortex.get", "params": ["camera.framerate"], "id": 2}" <br> Receive: "{"jsonrpc": "2.0", "result": 10, "id": 2}"
 * <tr><td>cortex.command   <td> Send: "{"jsonrpc": "2.0", "method": "cortex.command", "params": ["camera.shutter.open"], "id": 3}" <br> Receive: "{"jsonrpc": "2.0", "result": true, "id": 2}"
 * </table>
 *
*/

#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class CameraControl: public QTcpSocket
{
    Q_OBJECT
public:
    CameraControl(QObject* parent = Q_NULLPTR);
    ~CameraControl();

    Q_INVOKABLE bool connectDevice(QString host, QString port);
    Q_INVOKABLE void close();
    Q_INVOKABLE bool startStreaming(QString ip, quint16 port);
    Q_INVOKABLE bool stopStreaming(QString ip, quint16 port);

Q_SIGNALS:
    void connectionEstablished();

protected:
    QTcpSocket* m_socket;
    void readPendingDatagrams();

    bool sendCommand(QByteArray data);


    QString m_address;
    int m_portRPC;

private:

    /**
        Builds the json object to be sent to the camera. The output json is in the form
        {"jsonrpc": "2.0", "method": "cortex.<method>", "params": ["<feature_name>", "{"args": [<args>]}"], "id": 1}
        args should be in the form [6, 10.0] and if it is not present the json will not contain that field
        @param methodName
        @param param to be used for the method, it can be either a parameter name for both set and get actions (e.g. camera.framerate) or a method name for a command (e.g. camera.shutter.open)
        @param args the input to be used for the set function
        @return a formatted json object to be sent via tcp
    */
    static QJsonObject jsonBuilder(const QString& methodName, const QString& paramName, const QJsonArray& args);
    

private Q_SLOTS:
    void onConnected();
};

#endif // CAMERACONTROL_H
