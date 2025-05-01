import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

WindowBase {
    objectName: "MainWindow"
    id: mainWindow
    flags: Qt.Window|Qt.FramelessWindowHint|Qt.WindowMaximizeButtonHint|Qt.WindowMinimizeButtonHint
    width: 930
    height: 650
    minimumWidth: 820
    minimumHeight: 620
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

                delegate: ButtonBase {
                    width: parent.width
                    height: 80
                    anchors.horizontalCenter: parent.horizontalCenter
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
                        for (var j=1; j<wechatModel.count; j++)
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
           width: parent.width - leftPannel.width - verticalLine.width
           height: 1
           anchors.top: parent.top
           anchors.right: parent.right
           color: "#E5E5E5"
        }

        // 微信区域
        Item {
            id: wechatPannel
            width: parent.width-leftPannel.width-verticalLine.width
            height: parent.height-horizonLine.height
            anchors.left: verticalLine.right
            anchors.top: horizonLine.bottom

            // 覆盖微信窗口关闭按钮的区域
            Item {
                id: wechatCoverPannel
                width: 149
                height: 42
                anchors.right: parent.right
                anchors.top: parent.top

                // 覆盖微信的窗口
                property val coverWindow: null

                function updateCoverWindowPos() {
                    if (coverWindow == null) {
                        coverWindow = wechatCoverWindowComponent.createObject(mainWindow, {"width": wechatCoverPannel.width, "height": wechatCoverPannel.height})
                    }

                    const topLeft = wechatCoverPannel.mapToGlobal(0, 0)
                    coverWindow.x = coverPannelTopLeft.x
                    coverWindow.y = coverPannelTopLeft.y
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
    }

    Component.onCompleted: {
        cppMainController.showWindow.connect(function(name) {
            //
        })

        cppMainController.showMessage.connect(function(message) {
            //
        })

        cppMainController.wechatListChange.connect(onWeChatListChange)

        setTimeout(function() {
            cppMainController.mainWndReady()
            wechatCoverPannel.updateCoverWindowPos()
        }, 100);
    }

    function onWeChatListChange(wechatJson) {
        var wechats = JSON.parse(wechatJson)
        wechatModel.clear()
        for (var i=0; i<wechats.length; i++) {
            wechatModel.append(wechats[i])
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
}
