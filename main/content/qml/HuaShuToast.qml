import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

// 话术吐司，用于悬停时展示完整话术的内容
Window {
    id: huaShuToast
    flags: Qt.Popup | Qt.FramelessWindowHint
    color: "transparent"
    width: 322
    height: huaShuContentText.implicitHeight+huaShuTitleText.height+tipText.height+2*padding
    visible: false

    // 话术标题
    property string huaShuTitle: ""

    // 话术内容
    property string huaShuContent: ""

    property int padding: 15

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"
        clip: true
        border.width: 1
        border.color: "#dddddd"

        Item {
            width: parent.width-2*padding
            height: parent.height-2*padding
            anchors.centerIn: parent

            // 话术标题
            Text {
                id: huaShuTitleText
                width: parent.width
                height: 30
                anchors.top: parent.top
                text: huaShuToast.huaShuTitle
                color: "black"
                font.pixelSize: 15
                font.weight: Font.Medium
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
            }

            // 话术内容
            Text {
                id: huaShuContentText
                width: parent.width
                anchors.top: huaShuTitleText.bottom
                text: huaShuContent
                color: "black"
                font.pixelSize: 13
                font.weight: Font.Medium
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.Wrap  // 自动换行
            }

            // 提醒双击发送
            Text {
                id: tipText
                width: parent.width
                height: 30
                anchors.bottom: parent.bottom
                text: "单击直接发送给当前好友"
                color: "#0ec365"
                font.pixelSize: 15
                font.weight: Font.Medium
                verticalAlignment: Text.AlignBottom
                horizontalAlignment: Text.AlignLeft
            }
        }
    }

    onActiveChanged: {
        if (!active) {
            huaShuToast.close()
        }
    }
}
