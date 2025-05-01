import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

Window {
    id: wechatCoverWindow
    flags: Qt.Window|Qt.FramelessWindowHint
    visible: true
    width: 149
    height: 32
    title: "WeChatCover"
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        color: "#F5F5F5"
    }
}
