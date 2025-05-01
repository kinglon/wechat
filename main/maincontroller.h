#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <qqml.h>
#include <wechatcontroller.h>
#include <QQmlApplicationEngine>

class MainController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MainController(QObject *parent = nullptr);

public:
    void setQmlEngine(QQmlApplicationEngine* qmlEngine) { m_qmlEngine = qmlEngine; }

    QImage getAvatarImg(QString id) { return m_wechatController.getAvatarImg(id); }

public: // QML调用接口
    Q_INVOKABLE void quitApp();

    Q_INVOKABLE bool addAccount();

    Q_INVOKABLE void mergeWeChat(bool merge);

    Q_INVOKABLE void setCurrentWeChat(QString wechatId);

    Q_INVOKABLE void updateWeChatRect(int x, int y, int width, int height);

    // 主窗口已创建
    Q_INVOKABLE void mainWndReady();

signals:
    // 显示窗口
    void showWindow(QString name);

    // 显示消息
    void showMessage(QString message);

    // 微信列表改变
    void wechatListChange(QString wechatJson);

    // 微信状态改变
    void wechatStatusChange(QString wechatJson);

private slots:
    void onWeChatListChange();

    void onWeChatStatusChange();

private:
    void run();

private:
    QQmlApplicationEngine* m_qmlEngine = nullptr;

    WeChatController m_wechatController;
};

#endif // MAINCONTROLLER_H
