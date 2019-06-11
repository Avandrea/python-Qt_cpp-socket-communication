import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4 as M1
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtGraphicalEffects 1.0
import MyDeviceDiscovery 1.0
import MyDataReceiver 1.0
import MyImageItem 1.0

Window {
    id: mainWindow
    visible: true
    width: 1300
    height: 800
    title: qsTr("Hello World")
    minimumWidth: 1080
    minimumHeight: 720

    property string localhost: "0.0.0.0"

    property int smallMargins: 5
    property int componentsHeight: height * 0.08
    property int listViewComponentsHeight: 30
    property bool isConnected: false

    property color primary: "#42a5f5"
    property color primaryLight: "#80d6ff"
    property color secondary: "#26a69a"
    property color secondaryLight: "#64d8cb"

    property var selectedDevice
    property int selectedRow: -1

    property var modelIp: [
        {"ip": "192.168.0.22"},
        {"ip": "192.168.0.155"},
        {"ip": "192.168.0.555"},
        {"ip": "192.168.0.555"},
        {"ip": "192.168.0.555"}
    ]

    function closeDiscovery(){
        deviceDiscovery.close()
        btn_discovery.mChecked = false
        btn_discovery.text = "Start discovery"
        timer_discovery.stop()
    }


    DataReceiver{
        id: dataReceiver
        onNewDataReceived: {
            // receiving QByteArray array, qint64 bufferSize
            label_bufferSize.text = bufferSize + ""
        }
    }
    DeviceDiscovery{
        id: deviceDiscovery
        onAddressStringListChanged: {
            modelIp = []
            Object.keys(addressList).forEach(function(key){
                modelIp.push({"ip": key, "portFTP": addressList[key]["portFTP"], "portRPC": addressList[key]["portRPC"]})
            })
            modelIpChanged()
        }
    }

    Timer {
        id: timer_discovery
        running: false
        repeat: false
        interval: 5000
        onTriggered: {
            closeDiscovery()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "brown"
        gradient: Gradient
            {
                GradientStop {position: 0.000;color: "#0f2027";}
                GradientStop {position: 0.5;color: "#203a43";}
                GradientStop {position: 1;color: "#2c5364";}
            }
    }


    Button {
        width: 100
        height: 100
        text: "click meee"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onClicked: {
            console.log("*********** ", selectedDevice)
        }
    }

    Column {
        id: column_left
        width: parent.width * 0.1
        anchors.top: parent.top
        anchors.topMargin: smallMargins
        anchors.left: img.right
        anchors.leftMargin: smallMargins
        spacing: 5
        Button{
            id: btn_discovery
            property bool mChecked: false
            width: parent.width
            height: componentsHeight
            text: "Start discovery"
            background: Rectangle {
                    anchors.fill: parent
                    color: primaryLight
            }
            onClicked: {
                if (!btn_discovery.mChecked && isConnected === false){
                    deviceDiscovery.discovery() //54546
                    btn_discovery.mChecked = true
                    btn_discovery.text = "Stop discovery"
                    timer_discovery.start()
                }
                else
                    closeDiscovery()
            }
        }
        Rectangle {
            id: rectangle_discovery
            width: parent.width
            height: listview_discovery.height
            color: secondaryLight
            border.color: secondary
            M1.TableView {
                id: listview_discovery
                width: parent.width
                height: listViewComponentsHeight + listview_discovery.model.length * listViewComponentsHeight
                anchors.top: rectangle_discovery.top
                anchors.topMargin: 0
                model: modelIp
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                backgroundVisible: false
                style: TableViewStyle{
                        textColor: "white"
                        alternateBackgroundColor: "transparent"
                    }
                headerDelegate: Text{
                    id: listview_header
                    width: parent.width
                    height: listViewComponentsHeight
                    horizontalAlignment: Text.AlignHCenter
                    fontSizeMode: Text.HorizontalFit;
                    font.pointSize: 12
                    minimumPointSize: 11
                    font.bold: true
                    text: "Available devices"
                }
                itemDelegate: Rectangle {
                    id: listview_delegate
                    width: parent.width
                    height: listViewComponentsHeight
                    property int row: styleData.row
                    color: "transparent"
                    border.color: secondary
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            selectedDevice = modelIp[row]
                            selectedRow = row
                        }
                    }
                    Text {
                        id: text_listView
                        width: parent.width
                        height: parent.height
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: listview_discovery.model[row] ? listview_discovery.model[row]["ip"] : ""
                        font.pointSize: 12
                        color: "black"
                    }
                }
                rowDelegate: Rectangle{
                    height: listViewComponentsHeight
                    property int row: styleData.row
                    color: row == selectedRow ? primary : "transparent"
                }


                M1.TableViewColumn{
                    role: "ip"
                    width: rectangle_discovery.width
                }
            }
        }

    }

    Column {
        id: column_right
        anchors.top: parent.top
        anchors.topMargin: smallMargins
        anchors.right: parent.right
        anchors.rightMargin: smallMargins
        anchors.left: column_left.right
        anchors.leftMargin: smallMargins
        spacing: 5
        Button{
            id: btn_connect
            property bool mChecked: false
            width: parent.width
            height: componentsHeight
            text: "Connect"
            background: Rectangle {
                    anchors.fill: parent
                    color: mainWindow.isConnected ? "#02ee80" : "#ee0a02"
            }
            onClicked: {
                closeDiscovery()
                console.log(selectedDevice["ip"], selectedDevice["portRPC"], selectedDevice["portFTP"])

                if (!btn_connect.mChecked && typeof selectedDevice !== 'undefined'){
                    isConnected = dataReceiver.bind(selectedDevice["ip"], selectedDevice["portFTP"], selectedDevice["portRPC"])
//                    isConnected = dataReceiver.connectDevice(selectedDevice["ip"], selectedDevice["portFTP"])
                    if (isConnected === true){
                        console.log("Sending message to ", selectedDevice["ip"])
                        dataReceiver.send(selectedDevice["ip"], 54547, "send me the image")
                        btn_connect.mChecked = true
                        btn_connect.text = "Disconnect"
                    }
                    else if (typeof isConnected === 'undefined'){
                        isConnected = false
                    }
                }
                else{
                    dataReceiver.close()
                    isConnected = false
                    btn_connect.mChecked = false
                    btn_connect.text = "Connect"
                }
            }
        }

        Button{
            id: btn_read_last_line
            width: btn_connect.width
            height: btn_connect.height
            text: "Read last \nline"
            background: Rectangle {
                    anchors.fill: parent
                    color: secondary
            }
            onClicked: {
                img.setImage(dataReceiver.getLastAddedLine(26,13))
            }
        }

        Button{
            id: btn_read_last_read
            width: btn_connect.width
            height: btn_connect.height
            text: "Read last \nread line"
            background: Rectangle {
                    anchors.fill: parent
                    color: secondary
            }
            onClicked: {
                img1.setImage(dataReceiver.getLastReadLine(26,13))
            }
        }

        Rectangle {
            id: rectangle_bufferSize
            width: btn_connect.width
            height: btn_connect.height
            color: "transparent"
            border.width: 2
            border.color: "white"
            Text{
                id: text_bufferSize
                width: parent.width
                height: parent.height * 0.5
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.top: rectangle_bufferSize.top
                anchors.topMargin: smallMargins
                text: "Buffer size"
                font.pointSize: 15
                font.italic: true
                color: "white"
            }
            Label {
                id: label_bufferSize
                width: parent.width
                height: parent.height * 0.5
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.bottom: rectangle_bufferSize.bottom
                anchors.bottomMargin: smallMargins
                text: "0"
                font.pointSize: 18
                font.italic: true
                color: "white"
            }
        }
    }

    ImageItem {
      id: img
      width: parent.width * 0.8
      height: parent.height * 0.45
      anchors.top: parent.top
      anchors.topMargin: smallMargins
      anchors.left: parent.left
      anchors.leftMargin: smallMargins
    }

    ImageItem {
      id: img1
      width: parent.width * 0.8
      height: parent.height * 0.45
      anchors.top: img.bottom
      anchors.topMargin: smallMargins
      anchors.left: parent.left
      anchors.leftMargin: smallMargins
    }
}
