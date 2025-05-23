import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// 客户服务窗口，显示二维码
WindowBase {
    id: customerServiceWindow
    flags: Qt.Window|Qt.FramelessWindowHint
    width: 323
    height: 360
    minimumWidth: 323
    minimumHeight: 360
    maximumWidth: 323
    maximumHeight: 360
    title: "客服"
    color: "transparent"
    backgroundColor: "#F5F5F5"
    titleBarColor: "transparent"
    titleBarHeight: 37
    hasLogo: false
    titleTextColor: "#2E2E2E"
    titleTextFontSize: 14
    hasMaxButton: false
    hasMinButton: false
    closeBtnImage: "qrc:/content/res/icon_close.png"
    borderRadius: 1
    modality: Qt.WindowModal

    Item {
        parent: contentArea
        width: parent.width-40
        height: parent.height-40
        anchors.centerIn: parent

        Image {
            width: 282
            height: 281
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "image://memory/qrcode"
        }
    }
}
