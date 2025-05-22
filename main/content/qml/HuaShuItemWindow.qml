import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// 新增话术/编辑话术的窗口
WindowBase {
    id: huaShuItemWindow
    flags: Qt.Window|Qt.FramelessWindowHint
    width: 557
    height: 380
    minimumWidth: 557
    minimumHeight: 380
    maximumWidth: 557
    maximumHeight: 380
    title: "话术"
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

    // 话术ID
    property string huaShuId: ""

    // 话术标题
    property string huaShuTitle: ""

    // 话术内容
    property string huaShuContent: ""

    // 话术分组ID
    property string huaShuGroupId: ""

    signal save()

    Item {
        parent: contentArea
        width: parent.width-40
        height: parent.height-40
        anchors.centerIn: parent

        Column {
            spacing: 10

            // 话术内容文本框
            Text {
                width: parent.width
                height: 20
                text: "话术内容"
                color: "#969696"
                font.pixelSize: 13
                font.weight: Font.Medium
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }

            // 话术内容输入框
            Rectangle {
                width: parent.width
                height: 200
                color: "white"
                border.width: 1
                border.color: "#e7e7e7"
                clip: true

                // 输入窗口
                TextEdit {
                    id: huaShuContentEdit
                    width: parent.width-20
                    height: parent.height-20
                    anchors.centerIn: parent
                    font.pixelSize: 13
                    color: "black"
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.Wrap
                    text: huaShuItemWindow.huaShuContent

                    selectByMouse: true          // 启用鼠标选择文本
                    selectedTextColor: "white"          // 选中文本的字体颜色
                    selectionColor: "#3399FF"           // 选中文本的背景色(蓝色)
                }
            }

            // 话术标题和话术分组
            Row {
                width: parent.width
                height: 40
                spacing: 10

                // 话术标题文本框
                Text {
                    width: 56
                    height: parent.height
                    text: "话术标题"
                    color: "#969696"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                // 话术标题输入框
                Rectangle {
                    width: 140
                    height: parent.height
                    color: "white"
                    border.width: 1
                    border.color: "#e7e7e7"
                    clip: true

                    // 输入窗口
                    TextEdit {
                        id: titleEdit
                        width: parent.width-20
                        height: parent.height-20
                        anchors.centerIn: parent
                        font.pixelSize: 13
                        color: "black"
                        verticalAlignment: Text.Top
                        text: huaShuItemWindow.huaShuTitle

                        selectByMouse: true          // 启用鼠标选择文本
                        selectedTextColor: "white"          // 选中文本的字体颜色
                        selectionColor: "#3399FF"           // 选中文本的背景色(蓝色)
                    }
                }

                // 所属分组文本框
                Text {
                    width: 56
                    height: parent.height
                    text: "所属分组"
                    color: "#969696"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                // 所属分组下拉框
                GroupComboBox {
                    id: groupComboBox
                    width: 140
                    height: parent.height

                    Component.onCompleted: {
                        var huaShuJsonString = cppMainController.getHuaShuGroupList()
                        var huaShuGroupJson = JSON.parse(huaShuJsonString)
                        setGroupListData(huaShuGroupJson["myGroup"], huaShuItemWindow.huaShuGroupId)
                    }
                }
            }

            // 保存按钮
            Item {
                width: parent.width
                height: 35

                ButtonBase {
                    id: saveButton
                    width: 100
                    height: parent.height
                    anchors.right: parent.right
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
                        if (huaShuContentEdit.text === "") {
                            messageBoxComponent.createObject(huaShuItemWindow, {"messageContent": "话术内容不能为空"})
                            return
                        }

                        if (titleEdit.text === "") {
                            messageBoxComponent.createObject(huaShuItemWindow, {"messageContent": "话术标题不能为空"})
                            return
                        }

                        if (groupComboBox.selGroupId === "") {
                            messageBoxComponent.createObject(huaShuItemWindow, {"messageContent": "所属分组不能为空"})
                            return
                        }

                        huaShuItemWindow.huaShuGroupId = groupComboBox.selGroupId
                        huaShuItemWindow.huaShuTitle = titleEdit.text
                        huaShuItemWindow.huaShuContent = huaShuContentEdit.text
                        huaShuItemWindow.save()

                        huaShuItemWindow.close()
                    }
                }
            }
        }
    }

    Component {
        id: messageBoxComponent
        MessageBox {}
    }
}
