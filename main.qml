import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtGraphicalEffects 1.0
import MyDeviceDiscovery 1.0
import MyDataReceiver 1.0
import MyImageItem 1.0

Window {
    id: mainWindow
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Hello World")
    minimumWidth: 1700
    minimumHeight: 720

    property int smallMargins: 5
    property int componentsHeight: height * 0.08
    property int listViewComponentsHeight: 30
    property bool isConnected: false

    property color primary: "#42a5f5"
    property color primaryLight: "#80d6ff"
    property color secondary: "#26a69a"
    property color secondaryLight: "#64d8cb"


    DataReceiver{
        id: dataReceiver
        onNewDataReceived: {
            // receiving QByteArray array, qint64 bufferSize
            label_bufferSize.text = bufferSize + ""
        }
    }
    DeviceDiscovery{
        id: deviceDiscovery
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
            width: parent.width
            height: componentsHeight
            text: "Discovery"
            background: Rectangle {
                    anchors.fill: parent
                    color: primaryLight
            }
            onClicked: {
                deviceDiscovery.discovery() //54546
            }
        }
        Rectangle {
            id: rectangle_discovery
            width: parent.width
            height: listview_discovery.height
            color: secondaryLight
            border.color: secondary
            MouseArea{
                anchors.fill: parent
                onClicked: console.log((listview_discovery.model.length * listview_delegate.height))

            }
            ListView {
                id: listview_discovery
                width: parent.width
                height: listViewComponentsHeight + listview_discovery.model.length * listViewComponentsHeight
                anchors.top: rectangle_discovery.top
                anchors.topMargin: 0
                spacing: 0
                model: [
                    {"ip": "192.168.0.22", "port": "42222"},
                    {"ip": "192.168.0.155", "port": "42564"},
                    {"ip": "192.168.0.555", "port": "38"}
                ]
                header: Text{
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
                delegate: Rectangle {
                    id: listview_delegate
                    width: parent.width
                    height: listViewComponentsHeight
                    color: "transparent"
                    border.color: secondary
                    Text {
                        id: text_listView
                        width: parent.width
                        height: parent.height
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: listview_discovery.model[index]["ip"] + ":" + listview_discovery.model[index]["port"]
                        font.pointSize: 12
                        color: "black"
                    }
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
                if (!btn_connect.mChecked){
                    isConnected = dataReceiver.bind("127.0.0.1", 8001)
                    dataReceiver.send("127.0.0.1", 8000, "send me the image")
                    btn_connect.mChecked = true
                    btn_connect.text = "Disconnect"
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
