import QtQuick 2.0
import QtQuick.Layouts 1.15

Item {
    ColumnLayout {
        width: parent.width
        height: parent.height
        spacing: 8

        // 搜索区域
        Rectangle {
            id: searchPanel
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "#E2E2E2"
            radius: 5

            // 搜索框
            TextEdit {
                id: searchEdit
                width: parent.width-30
                height: parent.height-8
                anchors.centerIn: parent
                font.pixelSize: 13

                // 实际文本内容
                property string realText: ""

                // 占位文本
                property string placeholderText: "快速回复"

                // 文本颜色
                color: searchEdit.realText === "" && !searchEdit.activeFocus ? "darkgray" : "black"

                selectByMouse: true          // 启用鼠标选择文本
                selectedTextColor: "white"          // 选中文本的字体颜色
                selectionColor: "#3399FF"           // 选中文本的背景色(蓝色)

                // 绑定文本显示
                text: searchEdit.realText === "" && !searchEdit.activeFocus ? placeholderText : realText

                // 保存实际文本
                onTextChanged: {
                    if (activeFocus || text !== placeholderText) {
                        realText = text
                    }
                }

                // 处理焦点变化
                onActiveFocusChanged: {
                    if (activeFocus && realText === "") {
                        text = ""
                    } else if (!activeFocus && realText === "") {
                        text = placeholderText
                    }
                }
            }
        }

        // 话术区域
        Item {
            id: huaShuPanel
            Layout.fillWidth: true
            Layout.fillHeight: true

            // 分类
            GridView {
                id: categoryGridView
                width: parent.width
                height: rowCount*cellHeight
                anchors.fill: parent
                cellWidth: width/columnCount
                cellHeight: 34
                clip: true

                // 行数列数
                property int columnCount: 4
                property int rowCount: count/columnCount+1

                // 间距
                property int spacing: 4

                delegate: Item {
                    ButtonBase {
                        width: categoryGridView.cellWidth-2*categoryGridView.spacing
                        height: categoryGridView.cellHeight-2*categoryGridView.spacing
                        x: categoryGridView.spacing
                        y: categoryGridView.spacing
                        text: "常用"
                        font.pixelSize: 14
                        textNormalColor: "black"
                        textHoverColor: "white"
                        borderRadius: 5
                        bgNormalColor: "#e9e9e9"
                        bgHoverColor: "#07c160"
                    }
                }
            }
        }

        // 简短话术
        Rectangle {
            id: shortHuaShuPanel
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#e9e9e9"
            radius: 5

            property int padding: 5

            // 第一行
            Row {
                width: parent.width-2*shortHuaShuPanel.padding
                height: (parent.height-3*shortHuaShuPanel.padding)/2
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: shortHuaShuPanel.padding

                ButtonBase {
                    width: 26
                    height: parent.height
                    text: "亲"
                    font.pixelSize: 12
                    textNormalColor: "black"
                    textHoverColor: "white"
                    borderRadius: 5
                    bgNormalColor: "white"
                    bgHoverColor: "#08e371"

                    onClicked: cppMainController.sendMessage(text)
                }
            }
        }

        // 编辑按钮
        ButtonBase {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            text: "编辑"
            font.pixelSize: 14
            textNormalColor: "black"
            borderRadius: 5
            bgNormalColor: "#e9e9e9"
            bgHoverColor: "#D1EDDE"
            bgClickColor: bgHoverColor
        }
    }
}
