import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: groupComboBox
    width: 140
    height: parent.height
    color: "transparent"
    border.color: "#e7e7e7"
    border.width: 1

    // 下拉框
    property var popup: null

    // 选择的分组ID
    property string selGroupId: ""

    // 显示当前选中的文本
    Text {
        id: selectedText
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        color: "black"
        font.pixelSize: 13
        font.weight: Font.Medium
    }

    // 下拉箭头
    ButtonBase {
        width: icon.width+4
        height: parent.height-4
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        display: Button.IconOnly
        icon.source: "qrc:/content/res/combo_arrow.png"
        icon.width: 26
        icon.height: 26

        onClicked: {
            if (popup == null) {
                return
            }

            var globalPos = groupComboBox.mapToGlobal(0, 0)
            popup.x = globalPos.x
            popup.y = globalPos.y - popup.height - 6
            popup.visible = true;
            popup.requestActivate();
        }
    }

    function setGroupListData(groupList, selGroupId) {
        if (popup == null) {
            popup = groupPopupComponent.createObject(null)
            popup.width = groupComboBox.width
            popup.userData = groupList
            for (var i=0; i<groupList.length; i++) {
                popup.menuListModel.append({"menuId": groupList[i]["groupId"], "menuText": groupList[i]["groupName"]})
            }

            popup.menuClick.connect(function(menuId) {
                var groups = popup.userData
                for (var i=0; i<groups.length; i++) {
                    if (groups[i]["groupId"] === menuId) {
                        selectedText.text = groups[i]["groupName"]
                        groupComboBox.selGroupId = menuId
                        break
                    }
                }
            })
        }

        if (groupList.length > 0) {
            selectedText.text = groupList[0]["groupName"]
            groupComboBox.selGroupId = groupList[0]["groupId"]
            for (i=0; i<groupList.length; i++) {
                if (groupList[i]["groupId"] === selGroupId)
                {
                    selectedText.text = groupList[i]["groupName"]
                    groupComboBox.selGroupId = groupList[i]["groupId"]
                    break
                }
            }
        }
    }

    Component {
        id: groupPopupComponent
        MenuBase {}
    }
}
