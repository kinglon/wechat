import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

WindowBase {
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
        Item {
            id: leftPannel
            width: 88
            height: parent.height
            anchors.left: parent.left
        }

        // 竖线
        Rectangle {
           width: 1
           height: parent.height
           anchors.left: leftPannel.right
           color: "#E5E5E5"
        }

        // 横线
        Rectangle {
           width: parent.width - leftPannel.width
           height: 1
           anchors.top: parent.top
           anchors.right: parent.right
           color: "#E5E5E5"
        }

        // 微信区域
        Item {
            id: wechatPannel
            width: parent.width-leftPannel.width-1
            height: parent.height-1
        }
    }

    Component.onCompleted: {
        cppMainController.showWindow.connect(function(name) {
            //
        })

        cppMainController.showMessage.connect(function(message) {
            //
        })
    }

    //可能是qmltype信息不全，有M16警告，这里屏蔽下
    //@disable-check M16
    onClosing: function(closeEvent) {
        cppMainController.quitApp()
    }
}
