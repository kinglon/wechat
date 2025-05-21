import QtQuick 2.0

// 简短话术按钮，如：亲，您好，欢迎光临
ButtonBase {
    width: 26
    height: parent.height
    text: ""
    font.pixelSize: 12
    textNormalColor: "black"
    textHoverColor: "white"
    borderRadius: 3
    bgNormalColor: "white"
    bgHoverColor: "#08e371"

    onClicked: cppMainController.sendMessage(text)
}
