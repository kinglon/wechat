import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQml.Models 2.15

// 主界面右侧话术面板
Item {
    id: huaShuRootPanel

    // 所在的窗口
    property var ownerWindow: null

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
                verticalAlignment: Text.AlignVCenter

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
                        huaShuPanel.searchHuaShu()
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

            // 当前分类
            property string currentGroupId: ""

            // 分类
            GridView {
                id: categoryGridView
                width: parent.width
                height: rowCount*cellHeight
                anchors.top: parent.top
                cellWidth: width/columnCount
                cellHeight: 34
                clip: true
                visible: count>0

                // 行数列数
                property int columnCount: 4
                property int rowCount: count==0?0:(count-1)/columnCount+1

                // 间距
                property int spacing: 6

                delegate: Item {
                    ButtonBase {
                        width: categoryGridView.cellWidth-2*categoryGridView.spacing
                        height: categoryGridView.cellHeight-2*categoryGridView.spacing
                        x: categoryGridView.spacing
                        y: categoryGridView.spacing
                        text: groupName
                        font.pixelSize: 13
                        textNormalColor: "black"
                        textHoverColor: "white"
                        borderRadius: 3
                        bgNormalColor: "#e9e9e9"
                        bgHoverColor: "#07c160"
                        isSelected: current

                        onClicked: {
                            huaShuPanel.currentGroupId = groupId
                            huaShuPanel.searchHuaShu()
                        }
                    }
                }

                model: ListModel {
                    id: categoryListModel
                }
            }

            // 话术
            ListView {
                id: huaShuListView
                width: parent.width
                height: huaShuPanel.height-categoryGridView.height
                anchors.bottom: parent.bottom
                spacing: 0
                clip: true
                visible: categoryGridView.visible

                // 当前悬停的话术ID
                property string hoverHuaShuId: ""

                // 吐司窗口及显示位置
                property var toastWindow: null
                property int toastWindowY: 0

                Component {
                    id: toastWindowComponent
                    HuaShuToast {}
                }

                onHoverHuaShuIdChanged: {
                    if (hoverHuaShuId === "") {
                        if (toastWindow) {
                            toastWindow.close()
                        }
                    } else {
                        if (toastWindow == null) {
                            toastWindow = toastWindowComponent.createObject(null)
                        }

                        for (var i=0; i<huaShuListModel.count; i++) {
                            if (huaShuListModel.get(i)["huaShuId"] === hoverHuaShuId) {
                                toastWindow.huaShuTitle = huaShuListModel.get(i)["huaShuTitle"]
                                toastWindow.huaShuContent = huaShuListModel.get(i)["huaShuContent"]
                                break
                            }
                        }

                        var huaShuListViewGlobalPos = huaShuListView.mapToGlobal(0, 0)
                        toastWindow.x = huaShuListViewGlobalPos.x-toastWindow.width
                        toastWindow.y = huaShuListView.toastWindowY
                        toastWindow.visible = true;
                        toastWindow.requestActivate();
                    }
                }

                model: ListModel {
                    id: huaShuListModel
                }

                delegate: Item {
                    id: delegateItem
                    width: huaShuListView.width
                    height: 37

                    function getHuaShuContent() {
                        return huaShuIndex+"  "+"<font color='#0ec365'>"+huaShuTitle+"</font>"+" "+huaShuContent
                    }

                    Text {
                        anchors.fill: parent
                        text: getHuaShuContent()
                        color: "black"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        textFormat: Text.RichText // 启用富文本解析
                    }

                    // 分隔线
                    Rectangle {
                        width: parent.width
                        height: 1
                        anchors.bottom: parent.bottom
                        color: "#dddddd"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        onDoubleClicked: {
                            cppMainController.sendMessage(huaShuContent)
                        }

                        onEntered: {
                            var mouseGlobalPos = delegateItem.mapToGlobal(mouseX, mouseY)
                            huaShuListView.toastWindowY = mouseGlobalPos.y
                            huaShuListView.hoverHuaShuId = huaShuId
                        }

                        onExited: {
                            huaShuListView.hoverHuaShuId = ""
                        }
                    }
                }
            }

            function searchHuaShu() {
                categoryListModel.clear()
                huaShuListModel.clear()

                var keyWord = searchEdit.realText
                var huaShuJsonString = cppMainController.searchHuaShu(keyWord, currentGroupId)
                if (huaShuJsonString === "") {
                    return
                }

                var huaShuJson = JSON.parse(huaShuJsonString)
                for (var i=0; i<huaShuJson["group"].length; i++) {
                    categoryListModel.append(huaShuJson["group"][i])
                }
                for (i=0; i<huaShuJson["huashu"].length; i++) {
                    huaShuListModel.append(huaShuJson["huashu"][i])
                    if (huaShuJson["huashu"][i]["current"]) {
                        huaShuPanel.currentGroupId = huaShuJson["huashu"][i]["groupId"]
                    }
                }
            }

            Component.onCompleted: {
                searchHuaShu()
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
                id: shortHuaShuRow1
                width: parent.width-2*shortHuaShuPanel.padding
                height: (parent.height-3*shortHuaShuPanel.padding)/2
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: shortHuaShuPanel.padding
                spacing: 5

                JianDuanHuaShuButton {
                    width: 26
                    text: "亲"
                }

                JianDuanHuaShuButton {
                    width: 40
                    text: "您好"
                }

                JianDuanHuaShuButton {
                    width: 72
                    text: "欢迎光临"
                }

                JianDuanHuaShuButton {
                    width: 40
                    text: "在的"
                }

                JianDuanHuaShuButton {
                    width: 40
                    text: "谢谢"
                }
            }

            // 第二行
            Row {
                width: shortHuaShuRow1.width
                height: shortHuaShuRow1.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: shortHuaShuPanel.padding
                spacing: 5

                JianDuanHuaShuButton {
                    width: 56
                    text: "请稍等"
                }

                JianDuanHuaShuButton {
                    width: 40
                    text: "在吗"
                }

                JianDuanHuaShuButton {
                    width: 40
                    text: "好的"
                }

                JianDuanHuaShuButton {
                    width: 72
                    text: "非常抱歉"
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

            onClicked: {
                var window = huaShuWindowComponent.createObject(huaShuRootPanel.ownerWindow)
                window.closing.connect(function(close) {
                    huaShuPanel.searchHuaShu()
                })
            }
        }
    }

    Component {
        id: huaShuWindowComponent
        HuaShuWindow {}
    }
}
