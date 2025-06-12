import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3


Window {
    id: windowBase
    visible: true
    width: 800
    height: 600    

    // 客户区内容（不包含标题栏、边框）
    property alias contentArea: contentArea

    // 背景颜色
    property color backgroundColor: "#222222"

    // 是否有最小化按钮
    property bool hasMinButton: true

    // 是否有最大化按钮
    property bool hasMaxButton: false

    // 最小化、最大化、关闭按钮图片资源路径
    property string minBtnImage: ""
    property string maxBtnImage: ""
    property string storeBtnImage: ""
    property string closeBtnImage: ""
    property int btnImageWidth: 26

    // 是否有LOGO
    property bool hasLogo: true

    // logo图片路径
    property string logoImage: ""

    // logo宽度与高度
    property int logoWidth: 20
    property int logoHeight: logoWidth

    // 标题文本颜色
    property color titleTextColor: "white"

    // 标题文本字体大小
    property int titleTextFontSize: 16

    // 边框的宽度
    property int borderWidth: 1

    // 边框的颜色
    property color borderColor: "#C2C4C6"

    // 边框的圆角
    property int borderRadius: 10

    // 标题栏颜色
    property color titleBarColor: "transparent"

    // 标题栏高度
    property int titleBarHeight: 58

    // 标志是否可以拖动边框改变窗口大小
    property bool resizable: true

    Rectangle {
        id: windowArea
        anchors.fill: parent
        color: windowBase.backgroundColor
        border.width: windowBase.borderWidth
        border.color: windowBase.borderColor
        radius: windowBase.borderRadius

        Column {
            width: parent.width-2*windowArea.radius
            height: parent.height-windowArea.radius
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 0

            // title bar
            Rectangle {
                id: titleBar
                width: parent.width
                height: windowBase.titleBarHeight
                color: windowBase.titleBarColor

                MouseArea {
                    anchors.fill: parent

                    property point clickPos: Qt.point(1,1)

                    onPressed: {
                        clickPos  = Qt.point(mouse.x,mouse.y);
                    }

                    onPositionChanged: {
                        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                        windowBase.x += delta.x;
                        windowBase.y += delta.y;
                    }
                }

                // Logo
                Image {
                    id: logo
                    visible: windowBase.hasLogo
                    width: windowBase.logoWidth
                    height: windowBase.logoHeight
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    fillMode: Image.PreserveAspectFit
                    source: windowBase.logoImage
                }

                // Title text
                Text {
                    id: titleText
                    width: 300
                    height: parent.height
                    anchors.left: windowBase.hasLogo? logo.right : parent.left
                    leftPadding: 3
                    text: windowBase.title
                    color: windowBase.titleTextColor
                    font.pixelSize: windowBase.titleTextFontSize
                    font.weight: Font.Medium
                    verticalAlignment: Text.AlignVCenter
                    anchors.leftMargin: 6
                }

                // 最小化按钮
                ButtonBase {
                    id: minBtn
                    visible: windowBase.hasMinButton
                    width: height+4
                    height: windowBase.btnImageWidth
                    anchors.right: windowBase.hasMaxButton? maxBtn.left : closeBtn.left
                    anchors.rightMargin: 6
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: windowBase.minBtnImage
                    icon.width: width
                    icon.height: height
                    display: AbstractButton.IconOnly

                    onClicked: {
                        windowBase.visibility = Window.Minimized
                    }
                }

                // 最大化按钮
                ButtonBase {
                    id: maxBtn
					visible: windowBase.hasMaxButton
                    width: height+4
                    height: windowBase.btnImageWidth
                    anchors.right: closeBtn.left
                    anchors.rightMargin: 6
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: windowBase.visibility===Window.Maximized?windowBase.storeBtnImage:windowBase.maxBtnImage
                    icon.width: width
                    icon.height: height
                    display: AbstractButton.IconOnly

                    onClicked: {
                        if (windowBase.visibility === Window.Maximized) {
                            windowBase.showNormal()
                        } else {
                            windowBase.visibility = Window.Maximized
                        }
                    }
                }

                // 关闭按钮
                ButtonBase {
                    id: closeBtn
                    width: height+4
                    height: windowBase.btnImageWidth
                    anchors.right: parent.right
                    anchors.rightMargin: 6
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: windowBase.closeBtnImage
                    icon.width: width
                    icon.height: width
                    display: AbstractButton.IconOnly

                    onClicked: {
                        windowBase.close()
                    }
                }
            }

            // Main content area
            Rectangle {
                id: contentArea
                width: parent.width
                height: parent.height - titleBar.height
                color: windowBase.backgroundColor                
            }
        }
    }

    // 上边框可拖动改变大小
    MouseArea {
        width: parent.width
        height: borderWidth*2
        anchors.top: parent.top
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeVerCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newHeight = windowBase.height - delta.y
                if (newHeight > windowBase.minimumHeight) {
                    windowBase.height = newHeight
                    windowBase.y += delta.y
                }
            }
        }
    }

    // 下边框可拖动改变大小
    MouseArea {
        width: parent.width
        height: borderWidth*2
        anchors.bottom: parent.bottom
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeVerCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newHeight = windowBase.height + delta.y
                if (newHeight > windowBase.minimumHeight) {
                    windowBase.height = newHeight
                }
            }
        }
    }

    // 左边框可拖动改变大小
    MouseArea {
        width: borderWidth*2
        height: parent.height
        anchors.left: parent.left
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeHorCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newWidth = windowBase.width - delta.x
                if (newWidth > windowBase.minimumWidth) {
                    windowBase.width = newWidth
                    windowBase.x += delta.x
                }
            }
        }
    }

    // 右边框可拖动改变大小
    MouseArea {
        width: borderWidth*2
        height: parent.height
        anchors.right: parent.right
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeHorCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newWidth = windowBase.width + delta.x
                if (newWidth > windowBase.minimumWidth) {
                    windowBase.width = newWidth
                }
            }
        }
    }

    // 左上角可拖动改变大小
    MouseArea {
        width: borderRadius
        height: borderRadius
        anchors.top: parent.top
        anchors.left: parent.left
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeFDiagCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newWidth = windowBase.width - delta.x
                var newHeight = windowBase.height - delta.y
                if (newWidth > windowBase.minimumWidth) {
                    windowBase.width = newWidth
                    windowBase.x += delta.x
                }
                if (newHeight > windowBase.minimumHeight) {
                    windowBase.height = newHeight
                    windowBase.y += delta.y
                }
            }
        }
    }

    // 右下角可拖动改变大小
    MouseArea {
        width: borderRadius
        height: borderRadius
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeFDiagCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newWidth = windowBase.width + delta.x
                var newHeight = windowBase.height + delta.y
                if (newWidth > windowBase.minimumWidth) {
                    windowBase.width = newWidth
                }
                if (newHeight > windowBase.minimumHeight) {
                    windowBase.height = newHeight
                }
            }
        }
    }

    // 左下角可拖动改变大小
    MouseArea {
        width: borderRadius
        height: borderRadius
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeBDiagCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newWidth = windowBase.width - delta.x
                var newHeight = windowBase.height + delta.y
                if (newWidth > windowBase.minimumWidth) {
                    windowBase.width = newWidth
                    windowBase.x += delta.x
                }
                if (newHeight > windowBase.minimumHeight) {
                    windowBase.height = newHeight
                }
            }
        }
    }

    // 右上角可拖动改变大小
    MouseArea {
        width: borderRadius
        height: borderRadius
        anchors.top: parent.top
        anchors.right: parent.right
        visible: resizable && windowBase.visibility!==Window.Maximized
        z: 2
        cursorShape: Qt.SizeBDiagCursor
        acceptedButtons: Qt.LeftButton
        property point clickPos: Qt.point(1,1)

        onPressed: {
            if (mouse.button == Qt.LeftButton) {
                clickPos  = Qt.point(mouse.x,mouse.y);
            }
        }

        onPositionChanged: {
            if (mouse.buttons == Qt.LeftButton) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y);
                var newWidth = windowBase.width + delta.x
                var newHeight = windowBase.height - delta.y
                if (newWidth > windowBase.minimumWidth) {
                    windowBase.width = newWidth
                }
                if (newHeight > windowBase.minimumHeight) {
                    windowBase.height = newHeight
                    windowBase.y += delta.y
                }
            }
        }
    }

    Component.onCompleted: {
        // 居中显示在主屏幕上
        var primaryScreenRect = cppMainController.getPrimaryScreenRect()
        windowBase.x = primaryScreenRect.x + (primaryScreenRect.width - windowBase.width) / 2
        windowBase.y = primaryScreenRect.y + (primaryScreenRect.height - windowBase.height) / 2
    }
}
