import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

WindowBase {
    objectName: "MainWindow"
    id: mainWindow
    flags: Qt.Window|Qt.FramelessWindowHint|Qt.WindowMaximizeButtonHint|Qt.WindowMinimizeButtonHint
    width: 1180
    height: 650
    minimumWidth: 1080
    minimumHeight: 620
    title: "多开神器"
    color: "transparent"
    backgroundColor: "#F5F5F5"
    hasLogo: true
    logoWidth: 25
    logoImage: "qrc:/content/res/logo.ico"
    titleTextColor: "#2E2E2E"
    titleTextFontSize: 14
    hasMaxButton: true
    minBtnImage: "qrc:/content/res/icon_min.png"
    maxBtnImage: "qrc:/content/res/icon_max.png"
    storeBtnImage: "qrc:/content/res/icon_restore.png"
    closeBtnImage: "qrc:/content/res/icon_close.png"

    // 微信头像右击弹出菜单
    property var wechatMenu: null

    Item {
        parent: contentArea
        anchors.fill: parent

        // 左侧栏
        Column {
            id: leftPannel
            width: 78
            height: parent.height
            anchors.left: parent.left
            spacing: 5

            ButtonBase {
                id: addBtn
                width: parent.width
                height: 35
                text: "添加账号"
                textNormalColor: "#2E2E2E"
                font.pixelSize: 13
                bgNormalColor: "#E9E9E9"
                bgClickColor: bgNormalColor
                bgHoverColor: "#D1EDDE"
                borderWidth: 0
                normalBorderWidth: 0
                borderRadius: 3

                onClicked: cppMainController.addAccount()
            }

            ListView {
                id: wechatList
                width: parent.width
                height: parent.height-addBtn.height-mergeBtn.height-2*leftPannel.spacing
                spacing: 10
                clip: true

                model: ListModel {
                    id: wechatModel
                }

                delegate: Item {
                    width: wechatList.width
                    height: 80

                    // 右键菜单
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onReleased: {
                            if (mouse.button === Qt.RightButton) {                                
                                if (wechatMenu == null) {
                                    wechatMenu = wechatListMenuComponent.createObject(null)
                                }
                                wechatMenu.wechatId = wechatId
                                var globalPos = mapToGlobal(mouseX, mouseY)
                                wechatMenu.x = globalPos.x
                                wechatMenu.y = globalPos.y
                                wechatMenu.visible = true;
                                wechatMenu.requestActivate();
                            }
                        }
                    }

                    ButtonBase {
                        anchors.fill: parent
                        text: nickName
                        textNormalColor: current?"#00FF80":"#2E2E2E"
                        font.pixelSize: 13
                        icon.source: avatarImg
                        icon.width: 44
                        icon.height: 44
                        display: AbstractButton.TextUnderIcon
                        spacing: 4
                        bgNormalColor: "transparent"
                        bgClickColor: bgNormalColor
                        bgHoverColor: bgNormalColor

                        onClicked: {
                            cppMainController.setCurrentWeChat(wechatId)
                            for (var j=0; j<wechatModel.count; j++)
                            {
                                if (wechatModel.get(j)["wechatId"] === wechatId)
                                {
                                    wechatModel.setProperty(j, "current", true);
                                }
                                else
                                {
                                    wechatModel.setProperty(j, "current", false);
                                }
                            }
                        }

                        onTextNormalColorChanged: { updateButtonStatus(); }
                    }

                    // 新消息小红点
                    Rectangle {
                        z: 1
                        width: 12
                        height: width
                        anchors.right: parent.right
                        anchors.rightMargin: 23-width/2
                        anchors.top: parent.top
                        anchors.topMargin: 12-height/2
                        radius: width/2
                        color: "red"
                        visible: hasNewMsg
                    }
                }
            }

            ButtonBase {
                id: mergeBtn
                width: parent.width
                height: 30
                text: isMerge?"拆分窗口":"合并窗口"
                textNormalColor: "#2E2E2E"
                font.pixelSize: 12
                bgNormalColor: "#E9E9E9"
                bgClickColor: bgNormalColor
                bgHoverColor: "#D1EDDE"
                borderWidth: 0
                normalBorderWidth: 0
                borderRadius: 3

                // 标识当前微信窗口是否为合并状态
                property bool isMerge: true

                onClicked: {
                    isMerge = !isMerge
                    cppMainController.mergeWeChat(isMerge)
                }
            }
        }

        // 竖线
        Item {
           id: verticalLine
           width: 11
           height: parent.height
           anchors.left: leftPannel.right

           Rectangle {
               width: 1
               height: parent.height
               anchors.right: parent.right
               color: "#E5E5E5"
           }
        }

        // 横线
        Rectangle {
           id: horizonLine
           width: parent.width - leftPannel.width - verticalLine.width - verticalLine2.width - huaShu.width
           height: 1
           anchors.top: parent.top
           anchors.left: verticalLine.right
           color: "#E5E5E5"
        }

        // 微信区域
        Item {
            id: wechatPannel
            width: horizonLine.width
            height: parent.height-horizonLine.height
            anchors.left: verticalLine.right
            anchors.top: horizonLine.bottom

            // 覆盖微信窗口关闭按钮的区域
            Item {
                id: wechatCoverPannel
                width: 149
                height: 28
                anchors.right: parent.right
                anchors.top: parent.top

                // 覆盖微信的窗口
                property var coverWindow: null

                function updateCoverWindowPos() {
                    if (coverWindow == null) {
                        coverWindow = wechatCoverWindowComponent.createObject(mainWindow, {"width": wechatCoverPannel.width, "height": wechatCoverPannel.height})
                    }

                    const topLeft = wechatCoverPannel.mapToGlobal(0, 0)
                    coverWindow.x = topLeft.x
                    coverWindow.y = topLeft.y
                }
            }

            function updateWeChatRect() {
                const topLeft = mapToGlobal(0, 0)
                cppMainController.updateWeChatRect(topLeft.x, topLeft.y, width, height)
                wechatCoverPannel.updateCoverWindowPos()
            }

            onXChanged: updateWeChatRect()
            onYChanged: updateWeChatRect()
            onWidthChanged: updateWeChatRect()
            onHeightChanged: updateWeChatRect()
            Component.onCompleted: updateWeChatRect()
        }

        // 竖线
        Item {
           id: verticalLine2
           width: 11
           height: parent.height
           anchors.right: huaShu.left

           Rectangle {
               width: 1
               height: parent.height
               anchors.left: parent.left
               color: "#E5E5E5"
           }
        }

        // 话术
        HuaShuControl {
            id: huaShu
            width: 275
            height: parent.height
            anchors.right: parent.right
            ownerWindow: mainWindow
        }

        // 启动页面，盖在上面
        Rectangle {
            id: launchPanel
            anchors.fill: parent
            color: mainWindow.backgroundColor
            z: 1

            Item {
                width: parent.width
                height: qrCodeImg.height + launchBtn.height + 50
                anchors.centerIn: parent

                // 二维码
                Image {
                    id: qrCodeImg
                    width: 283
                    height: 283
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "image://memory/qrcode"
                    fillMode: Image.PreserveAspectCrop
                }

                // 启动按钮
                ButtonBase {
                    id: launchBtn
                    width: 240
                    height: 65
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    bgNormalColor: "#07C160"
                    bgClickColor: bgNormalColor
                    bgHoverColor: "#08E371"
                    text: "立即启动"
                    font.pixelSize: 23
                    textNormalColor: "#FFFFFF"

                    onClicked: {
                        launchPanel.visible = false
                        cppMainController.mainWndReady()
                        wechatPannel.updateWeChatRect()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        cppMainController.showWindow.connect(function(name) {})
        cppMainController.showMessage.connect(function(message) {})
        cppMainController.wechatListChange.connect(onWeChatListChange)
        cppMainController.wechatStatusChange.connect(onWechatStatusChange)

        // 居中显示在主屏幕上
        var primaryScreenRect = cppMainController.getPrimaryScreenRect()
        mainWindow.x = primaryScreenRect.x + (primaryScreenRect.width - mainWindow.width) / 2
        mainWindow.y = primaryScreenRect.y + (primaryScreenRect.height - mainWindow.height) / 2
    }

    function onWeChatListChange(wechatJson) {
        var wechats = JSON.parse(wechatJson)
        wechatModel.clear()
        for (var i=0; i<wechats.length; i++) {
            wechatModel.append(wechats[i])
        }
    }

    function onWechatStatusChange(wechatJson) {
        var wechats = JSON.parse(wechatJson)
        for (var i = 0; i < wechatModel.count; i++) {
            for (var j=0; j<wechats.length; j++) {
                if (wechatModel.get(i).wechatId === wechats[j].wechatId) {
                    wechatModel.setProperty(i, "hasNewMsg", wechats[j].hasNewMsg);
                }
            }
        }
    }

    //可能是qmltype信息不全，有M16警告，这里屏蔽下
    //@disable-check M16
    onClosing: function(closeEvent) {
        cppMainController.quitApp()
    }

    onXChanged: wechatPannel.updateWeChatRect()
    onYChanged: wechatPannel.updateWeChatRect()

    Component {
        id: wechatCoverWindowComponent
        WechatCoverWindow {}
    }

    Component {
        id: wechatListMenuComponent
        WechatListMenu {}
    }
}
