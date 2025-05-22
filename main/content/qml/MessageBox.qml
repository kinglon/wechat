import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

WindowBase {
    objectName: "messageBox"
    id: messageBox
    flags: Qt.Window|Qt.FramelessWindowHint
    width: 352
    height: 177
    minimumWidth: 352
    minimumHeight: 177
    maximumWidth: 352
    maximumHeight: 177
    title: "提醒"
    color: "transparent"
    backgroundColor: "#F5F5F5"
    titleBarColor: "#07c160"
    titleBarHeight: 37
    hasLogo: false
    titleTextColor: "white"
    titleTextFontSize: 14
    hasMaxButton: false
    hasMinButton: false
    closeBtnImage: "qrc:/content/res/icon_close2.png"
    borderRadius: 1
    modality: Qt.WindowModal

    // 消息内容
    property string messageContent: ""

    // 标志是否有取消按钮
    property bool hasCancelButton: false

    signal accepted()

    Item {
        parent: contentArea
        width: parent.width-40
        height: parent.height-40
        anchors.centerIn: parent

        // 消息内容
        Text {
            width: parent.width
            height: 50
            anchors.top: parent.top
            text: messageBox.messageContent
            color: "black"
            font.pixelSize: 13
            font.weight: Font.Medium
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
            clip: true
        }

        // 确定按钮
        ButtonBase {
            id: okButton
            width: 100
            height: 35
            anchors.right: cancelButton.visible?cancelButton.left:parent.right
            anchors.rightMargin: cancelButton.visible?10:0
            anchors.bottom: parent.bottom
            text: "确定"
            textNormalColor: "white"
            font.pixelSize: 13
            bgNormalColor: "#07c160"
            bgClickColor: "#08e371"
            bgHoverColor: bgClickColor
            borderWidth: 0
            normalBorderWidth: 0
            borderRadius: height/2

            onClicked: {
                messageBox.accepted()
                messageBox.close()
            }
        }

        // 取消按钮
        ButtonBase {
            id: cancelButton
            visible: messageBox.hasCancelButton
            width: 100
            height: 35
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: "取消"
            textNormalColor: "white"
            font.pixelSize: 13
            bgNormalColor: "#07c160"
            bgClickColor: "#08e371"
            bgHoverColor: bgClickColor
            borderWidth: 0
            normalBorderWidth: 0
            borderRadius: height/2

            onClicked: {
                messageBox.close()
            }
        }
    }
}
