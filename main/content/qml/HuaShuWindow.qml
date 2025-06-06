﻿import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

WindowBase {
    objectName: "HuaShuWindow"
    id: huaShuWindow
    flags: Qt.Window|Qt.FramelessWindowHint
    width: 978
    height: 673
    minimumWidth: 978
    minimumHeight: 673
    maximumWidth: 978
    maximumHeight: 673
    title: "话术管理"
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

    Item {
        parent: contentArea
        anchors.fill: parent

        // 左侧分组区域
        Rectangle {
            width: 198
            height: 594
            anchors.left: parent.left
            anchors.leftMargin: 21
            anchors.top: parent.top
            anchors.topMargin: 25
            color: "white"

            ScrollView  {
                anchors.fill: parent

                Column {
                    id: leftPanel
                    width: parent.width
                    height: myHuaShuHeader.height+myHuaShuListView.height+huaShuTemplateHeader.height+templateHuaShuListView.height
                    spacing: 0

                    // 每行高度
                    property int rowHeight: 40

                    // 当前分组
                    property string currentGroupId: ""

                    // 我的话术头部
                    Item {
                        id: myHuaShuHeader
                        width: parent.width
                        height: leftPanel.rowHeight

                        Text {
                            anchors.fill: parent
                            text: "我的话术"
                            color: "#969696"
                            font.pixelSize: 13
                            font.weight: Font.Medium
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            leftPadding: 23
                        }

                        // 添加分类按钮
                        ButtonBase {
                            width: 72
                            height: parent.height-13
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            text: "添加分类"
                            textNormalColor: "#0ec365"
                            font.pixelSize: 12
                            bgNormalColor: "transparent"
                            bgClickColor: bgNormalColor
                            bgHoverColor: bgNormalColor
                            borderWidth: 1
                            normalBorderWidth: 1
                            borderRadius: 3
                            borderColor: textNormalColor

                            onClicked: {
                                var groupWindow = groupWindowComponent.createObject(huaShuWindow)
                                groupWindow.save.connect(function(groupName) {
                                    cppMainController.addHuaShuGroup(groupName)
                                    leftPanel.loadGroupData()
                                })
                            }
                        }
                    }

                    // 我的话术，分组列表
                    ListView {
                        id: myHuaShuListView
                        width: parent.width
                        height: count*leftPanel.rowHeight
                        spacing: 0
                        clip: true

                        // 右键菜单
                        property var menu: null

                        model: ListModel {
                            id: myHuaShuListModel
                        }

                        delegate: Rectangle {
                            width: myHuaShuListView.width
                            height: leftPanel.rowHeight
                            color: groupId===leftPanel.currentGroupId || myHuaShuItemMouseArea.containsMouse?"#d7f1dd":"transparent"

                            MouseArea {
                                id: myHuaShuItemMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                acceptedButtons: Qt.LeftButton|Qt.RightButton

                                onClicked: leftPanel.currentGroupId=groupId

                                onReleased: {
                                    if (mouse.button === Qt.RightButton) {
                                        myHuaShuListView.menu.userData = groupId
                                        var globalPos = mapToGlobal(mouseX, mouseY)
                                        myHuaShuListView.menu.x = globalPos.x
                                        myHuaShuListView.menu.y = globalPos.y
                                        myHuaShuListView.menu.visible = true;
                                        myHuaShuListView.menu.requestActivate();
                                    }
                                }
                            }

                            Text {
                                anchors.fill: parent
                                text: groupName+" ("+String(huaShuCount)+")"
                                color: "black"
                                font.pixelSize: 13
                                font.weight: Font.Medium
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                leftPadding: 35
                            }
                        }

                        Component.onCompleted: {
                            menu = menuBaseComponent.createObject(null)
                            menu.menuListModel.append({"menuId": "delete", "menuText": "删除"})
                            menu.menuClick.connect(function(menuId) {
                                if (menuId === "delete") {
                                    var currentGroupId = menu.userData
                                    cppMainController.deleteHuaShuGroup(currentGroupId)
                                    leftPanel.loadGroupData()
                                    if (currentGroupId === leftPanel.currentGroupId) {
                                        leftPanel.currentGroupId = ""
                                    }
                                }
                            })
                        }
                    }

                    // 话术模板头部
                    Item {
                        id: huaShuTemplateHeader
                        width: parent.width
                        height: leftPanel.rowHeight

                        Text {
                            anchors.fill: parent
                            text: "话术模板"
                            color: "#969696"
                            font.pixelSize: 13
                            font.weight: Font.Medium
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            leftPadding: 23
                        }
                    }

                    // 话术模板，分组列表
                    ListView {
                        id: templateHuaShuListView
                        width: parent.width
                        height: count*leftPanel.rowHeight
                        spacing: 0
                        clip: true

                        model: ListModel {
                            id: templateHuaShuListModel
                        }

                        delegate: Rectangle {
                            width: templateHuaShuListView.width
                            height: leftPanel.rowHeight
                            color: groupId===leftPanel.currentGroupId || templateHuaShuItemMouseArea.containsMouse?"#d7f1dd":"transparent"

                            MouseArea {
                                id: templateHuaShuItemMouseArea
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: leftPanel.currentGroupId=groupId
                            }

                            Text {
                                anchors.fill: parent
                                text: groupName+" ("+String(huaShuCount)+")"
                                color: "black"
                                font.pixelSize: 13
                                font.weight: Font.Medium
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                leftPadding: 35
                            }
                        }
                    }

                    onCurrentGroupIdChanged: {
                        loadHuaShuData()
                    }

                    function loadGroupData() {
                        myHuaShuListModel.clear()
                        templateHuaShuListModel.clear()

                        var huaShuJsonString = cppMainController.getHuaShuGroupList()
                        var huaShuGroupJson = JSON.parse(huaShuJsonString)
                        for (var i=0; i<huaShuGroupJson["myGroup"].length; i++) {
                            myHuaShuListModel.append(huaShuGroupJson["myGroup"][i])
                        }
                        for (i=0; i<huaShuGroupJson["templateGroup"].length; i++) {
                            templateHuaShuListModel.append(huaShuGroupJson["templateGroup"][i])
                        }
                    }

                    function loadHuaShuData() {
                        huaShuListModel.clear()
                        if (currentGroupId === "") {
                            return
                        }

                        var huaShuJsonString = cppMainController.getHuaShuList(currentGroupId)
                        var huaShuJson = JSON.parse(huaShuJsonString)
                        for (var i=0; i<huaShuJson["huaShuList"].length; i++) {
                            huaShuListModel.append(huaShuJson["huaShuList"][i])
                        }
                    }

                    Component.onCompleted: {
                        loadGroupData()
                    }
                }
            }
        }

        // 右侧话术模板区域
        Rectangle {
            width: 720
            height: 594
            anchors.right: parent.right
            anchors.rightMargin: 21
            anchors.top: parent.top
            anchors.topMargin: 25
            color: "white"

            ColumnLayout {
                id: huaShuColumnLayout
                width: parent.width-40
                height: parent.height
                x: 20
                spacing: 0

                // 话术头部
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    Text {
                        anchors.fill: parent
                        text: "话术模板"
                        color: "black"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                    }

                    // 添加话术按钮
                    ButtonBase {
                        width: 72
                        height: parent.height-13
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        text: "添加话术"
                        textNormalColor: "#0ec365"
                        font.pixelSize: 12
                        bgNormalColor: "transparent"
                        bgClickColor: bgNormalColor
                        bgHoverColor: bgNormalColor
                        borderWidth: 1
                        normalBorderWidth: 1
                        borderRadius: 3
                        borderColor: textNormalColor

                        onClicked: {                            
                            var huaShuItemWindow = huaShuItemWindowComponent.createObject(huaShuWindow)
                            huaShuItemWindow.save.connect(function() {
                                cppMainController.addHuaShu(huaShuItemWindow.huaShuGroupId, huaShuItemWindow.huaShuTitle, huaShuItemWindow.huaShuContent)
                                leftPanel.loadGroupData()
                                if (huaShuItemWindow.huaShuGroupId === leftPanel.currentGroupId) {
                                    leftPanel.loadHuaShuData()
                                }
                            })
                        }
                    }

                    // 横线
                    Rectangle {
                        width: parent.width
                        height: 1
                        anchors.bottom: parent.bottom
                        color: "#dddddd"
                    }
                }

                // 话术列表
                ListView {
                    id: huaShuListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0
                    clip: true

                    property int itemHeight: 78

                    // 右键菜单
                    property var menu: null

                    // 右键菜单处理
                    function onMenuItemClicked(menuId) {
                        var huaShuId = huaShuListView.menu.userData
                        var huaShuItem = null
                        for (var i=0; i<huaShuListModel.count; i++) {
                            if (huaShuListModel.get(i)["huaShuId"] === huaShuId) {
                                huaShuItem = huaShuListModel.get(i)
                                break
                            }
                        }

                        if (huaShuItem == null) {
                            return
                        }

                        if (menuId === "delete") {
                            cppMainController.deleteHuaShu(huaShuId)
                            leftPanel.loadGroupData()
                            leftPanel.loadHuaShuData()
                        } else if (menuId === "edit") {
                            var params = {
                                "huaShuId": huaShuItem["huaShuId"],
                                "huaShuTitle": huaShuItem["huaShuTitle"],
                                "huaShuContent": huaShuItem["huaShuContent"],
                                "huaShuGroupId": huaShuItem["groupId"],
                            }
                            var window = huaShuItemWindowComponent.createObject(null, params)
                            window.save.connect(function() {
                                cppMainController.editHuaShu(window.huaShuId, window.huaShuGroupId, window.huaShuTitle, window.huaShuContent)
                                leftPanel.loadGroupData()
                                leftPanel.loadHuaShuData()
                            })
                        } else if (menuId === "editTo") {
                            params = {
                                "huaShuTitle": huaShuItem["huaShuTitle"],
                                "huaShuContent": huaShuItem["huaShuContent"],
                                "huaShuGroupId": huaShuItem["groupId"],
                            }
                            window = huaShuItemWindowComponent.createObject(null, params)
                            window.save.connect(function() {
                                cppMainController.addHuaShu(window.huaShuGroupId, window.huaShuTitle, window.huaShuContent)
                                leftPanel.loadGroupData()
                            })
                        } else if (menuId === "moveTop") {
                            cppMainController.moveHuaShu(huaShuId, true, true)
                            leftPanel.loadHuaShuData()
                        } else if (menuId === "moveUp") {
                            cppMainController.moveHuaShu(huaShuId, true, false)
                            leftPanel.loadHuaShuData()
                        } else if (menuId === "moveDown") {
                            cppMainController.moveHuaShu(huaShuId, false, false)
                            leftPanel.loadHuaShuData()
                        }
                    }

                    model: ListModel {
                        id: huaShuListModel
                    }

                    delegate: Rectangle {
                        width: huaShuListView.width
                        height: huaShuListView.itemHeight
                        color: huaShuListViewMouseArea.containsMouse?"#d7f1dd":"transparent"
                        clip: true

                        // 标题
                        Text {
                            id: huaShuTitleText
                            width: parent.width
                            height: 35
                            text: huaShuIndex+" "+huaShuTitle
                            color: "black"
                            font.pixelSize: 13
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                        }

                        // 内容
                        Text {
                            width: parent.width
                            height: huaShuListView.itemHeight-huaShuTitleText.height
                            anchors.top: huaShuTitleText.bottom
                            text: huaShuContent
                            color: "black"
                            font.pixelSize: 13
                            font.weight: Font.Medium
                            verticalAlignment: Text.AlignTop
                            horizontalAlignment: Text.AlignLeft
                            wrapMode: Text.Wrap
                            clip: true
                        }

                        // 横线
                        Rectangle {
                            width: parent.width
                            height: 1
                            anchors.bottom: parent.bottom
                            color: "#dddddd"
                        }


                        MouseArea {
                            id: huaShuListViewMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.RightButton

                            onReleased: {
                                if (mouse.button === Qt.RightButton) {
                                    if (huaShuListView.menu) {
                                        huaShuListView.menu.close()
                                        huaShuListView.menu = null
                                    }

                                    huaShuListView.menu = menuBaseComponent.createObject(null)
                                    huaShuListView.menu.menuClick.connect(huaShuListView.onMenuItemClicked)

                                    huaShuListView.menu.menuListModel.clear()
                                    if (isMine) {
                                        huaShuListView.menu.menuListModel.append({"menuId": "moveTop", "menuText": "移至顶部"})
                                        huaShuListView.menu.menuListModel.append({"menuId": "moveUp", "menuText": "上移"})
                                        huaShuListView.menu.menuListModel.append({"menuId": "moveDown", "menuText": "下移"})
                                        huaShuListView.menu.menuListModel.append({"menuId": "edit", "menuText": "编辑"})
                                        huaShuListView.menu.menuListModel.append({"menuId": "delete", "menuText": "删除"})
                                    } else {
                                        huaShuListView.menu.menuListModel.append({"menuId": "editTo", "menuText": "编辑到"})
                                    }

                                    huaShuListView.menu.userData = huaShuId
                                    var globalPos = mapToGlobal(mouseX, mouseY)
                                    huaShuListView.menu.x = globalPos.x
                                    huaShuListView.menu.y = globalPos.y
                                    huaShuListView.menu.show()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        // 居中显示在主屏幕上
        var primaryScreenRect = cppMainController.getPrimaryScreenRect()
        huaShuWindow.x = primaryScreenRect.x + (primaryScreenRect.width - huaShuWindow.width) / 2
        huaShuWindow.y = primaryScreenRect.y + (primaryScreenRect.height - huaShuWindow.height) / 2
    }

    Component {
        id: groupWindowComponent
        GroupWindow {}
    }

    Component {
        id: huaShuItemWindowComponent
        HuaShuItemWindow {}
    }

    Component {
        id: messageBoxComponent
        MessageBox {}
    }

    Component {
        id: menuBaseComponent
        MenuBase {}
    }
}
