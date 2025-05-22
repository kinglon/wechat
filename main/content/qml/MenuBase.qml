import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    id: menuBase
    flags: Qt.Popup | Qt.FramelessWindowHint
    color: "transparent"
    width: 130
    height: itemHeight*menuListModelId.count
    visible: false

    property int itemHeight: 30
    property int itemLeftMargin: 20

    // model有2个字段，menuId和menuText
    property alias menuListModel: menuListModelId

    // user data
    property var userData: null

    signal menuClick(string menuId)

    ListView {
        anchors.fill: parent
        spacing: 0
        clip: true

        model: ListModel {
            id: menuListModelId
        }

        delegate: Rectangle {
            width: menuBase.width
            height: menuBase.itemHeight
            color: mouseArea.containsMouse?"#D7F1DD":"#F5F5F5"

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    menuBase.menuClick(menuId)
                    menuBase.close()
                }
            }

            Text {
                width: parent.width-itemLeftMargin
                height: parent.height
                anchors.right: parent.right

                text: menuText
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
            menuBase.close()
        }
    }
}
