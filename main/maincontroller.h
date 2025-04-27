#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <qqml.h>

class MainController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MainController(QObject *parent = nullptr);

public:
    void run();

public: // QML调用接口
    Q_INVOKABLE void quitApp();

signals:
    // 显示窗口
    void showWindow(QString name);

    // 显示消息
    void showMessage(QString message);
};

#endif // MAINCONTROLLER_H
