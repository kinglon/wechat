import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// 分组窗口
WindowBase {
    objectName: "groupWindow"
    id: groupWindow
    flags: Qt.Window|Qt.FramelessWindowHint
    width: 352
    height: 177
    minimumWidth: 352
    minimumHeight: 177
    maximumWidth: 352
    maximumHeight: 177
    title: "新增分类"
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

    signal save(string groupName)

    Item {
        parent: contentArea
        width: parent.width-40
        height: parent.height-40
        anchors.centerIn: parent

        Rectangle {
            width: parent.width
            height: 37
            color: "white"

            // 输入窗口
            TextEdit {
                id: groupNameEdit
                width: parent.width-20
                height: parent.height-10
                anchors.centerIn: parent
                font.pixelSize: 13
                color: "black"
                verticalAlignment: Text.AlignVCenter

                selectByMouse: true          // 启用鼠标选择文本
                selectedTextColor: "white"          // 选中文本的字体颜色
                selectionColor: "#3399FF"           // 选中文本的背景色(蓝色)
            }
        }

        // 保存按钮
        ButtonBase {
            id: saveButton
            width: 100
            height: 35
            anchors.right: cancelButton.left
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            text: "保存"
            textNormalColor: "white"
            font.pixelSize: 13
            bgNormalColor: "#07c160"
            bgClickColor: "#08e371"
            bgHoverColor: bgClickColor
            borderWidth: 0
            normalBorderWidth: 0
            borderRadius: height/2

            onClicked: {
                if (groupNameEdit.text.length > 0) {
                    groupWindow.save(groupNameEdit.text)
                }
                groupWindow.close()
            }
        }

        // 取消按钮
        ButtonBase {
            id: cancelButton            
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
                groupWindow.close()
            }
        }
    }
}
