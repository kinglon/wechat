#ifndef WECHATCONTROLLER_H
#define WECHATCONTROLLER_H

#include <QObject>
#include <Windows.h>
#include <QThread>
#include <QSet>
#include <QRect>
#include <QImage>
#include <uiautomation.h>

class WeChat
{
public:
    ~WeChat()
    {
        if (m_chatBtn)
        {
            m_chatBtn->Release();
        }
    }

public:
    // ID
    QString m_id;

    // 主窗口句柄
    HWND m_mainWnd = NULL;

    // 昵称
    QString m_nickName;

    // avatar头像
    QImage m_avatarImg;

    // 标识是否有新消息
    bool m_hasNewMsg = false;

    // 聊天按钮
    IUIAutomationElement* m_chatBtn = nullptr;
};

class WeChatThread: public QThread
{
    Q_OBJECT

public:
    WeChatThread();

protected:
    virtual void run() override;

public:
    void enableMerge(bool merge) { m_enableMerge.store(merge); }

    void setMainWindowHandle(HWND mainWindow) { m_mainWnd = mainWindow;}

signals:
    void hasNewWeChat(WeChat* wechat);

private:
    std::atomic<bool> m_enableMerge;

    // 主窗口句柄
    HWND m_mainWnd = NULL;
};

class WeChatController : public QObject
{
    Q_OBJECT

public:
    explicit WeChatController(QObject *parent = nullptr);

    void run();

    void stop();

    const QVector<WeChat>& getWeChats() { return m_wechats; }

    QString getCurrentWeChatId() { return m_currentWeChatId; }

    void setCurrentWeChatId(QString wechatId) { m_currentWeChatId = wechatId; }

    void setWechatRect(QRect wechatRect);

    QImage getAvatarImg(QString id);

    void setMainWindowHandle(HWND mainWindow) { m_mainWnd = mainWindow; }

    void mergeWeChat(bool merge);

    bool startWeChat();

signals:
    void wechatListChange();

    void wechatStatusChange();

private slots:
    void onMainTimer();

    void onHasNewWeChat(WeChat* wechat);

private:
    void showWeChatWindow(HWND hWnd, bool visible);

private:
    bool m_isRunning = false;

    QVector<WeChat> m_wechats;

    // 标识当前窗口是合并还是拆离
    bool m_merge = true;

    // 当前微信窗口
    QString m_currentWeChatId;

    WeChatThread* m_wechatThread = nullptr;

    // 微信窗口区域
    QRect m_wechatScreenRect;
    QRect m_wechatClientRect;

    // 主窗口句柄
    HWND m_mainWnd = NULL;
};

#endif // WECHATCONTROLLER_H
