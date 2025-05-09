import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    id: wechatListMenu
    flags: Qt.Popup | Qt.FramelessWindowHint
    color: "transparent"
    width: 130
    height: itemHeight*itemCount
    visible: false

    property int itemHeight: 30
    property int itemCount: 3
    property int itemLeftMargin: 20

    property string wechatId: ""

    Column {
        anchors.fill: parent
        spacing: 0

        // 上移菜单
        Rectangle {
            width: parent.width
            height: wechatListMenu.itemHeight
            color: mouseAreaUp.containsMouse?"#D7F1DD":"#F5F5F5"

            MouseArea {
                id: mouseAreaUp
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    cppMainController.moveAccount(true, wechatId)
                    wechatListMenu.close()
                }
            }

            Text {
                width: parent.width-itemLeftMargin
                height: parent.height
                anchors.right: parent.right

                text: "上移"
                color: "black"
                font.pixelSize: 13
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        // 下移菜单
        Rectangle {
            width: parent.width
            height: wechatListMenu.itemHeight
            color: mouseAreaDown.containsMouse?"#D7F1DD":"#F5F5F5"

            MouseArea {
                id: mouseAreaDown
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    cppMainController.moveAccount(false, wechatId)
                    wechatListMenu.close()
                }
            }

            Text {
                width: parent.width-itemLeftMargin
                height: parent.height
                anchors.right: parent.right

                text: "下移"
                color: "black"
                font.pixelSize: 13
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        // 退出账号菜单
        Rectangle {
            width: parent.width
            height: wechatListMenu.itemHeight
            color: mouseAreaExit.containsMouse?"#D7F1DD":"#F5F5F5"

            MouseArea {
                id: mouseAreaExit
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    cppMainController.exitAccount(wechatId)
                    wechatListMenu.close()
                }
            }

            Text {
                width: parent.width-itemLeftMargin
                height: parent.height
                anchors.right: parent.right

                text: "退出账号"
                color: "black"
                font.pixelSize: 13
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    onActiveChanged: {
        if (!active) {
            wechatListMenu.close()
        }
    }
}
