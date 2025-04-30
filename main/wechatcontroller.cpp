#include "wechatcontroller.h"
#include <QTimer>
#include <QUuid>
#include "wechat/openwechat.h"
#include "wechat/wechatutil.h"

#pragma comment(lib, "user32.lib")

// 离屏位置
#define OFFSCREEN_X  -5000

WeChatThread::WeChatThread()
{
    m_enableMerge.store(true);
}

void WeChatThread::run()
{
    while(true)
    {
        QThread::msleep(300);
        if (!m_enableMerge.load())
        {
            continue;
        }

        // 抓取微信主窗口
        HWND wechatMainWnd = FindWindow(L"WeChatMainWndForPC", NULL);
        if (wechatMainWnd)
        {
            WeChat* wechat = new WeChat();
            wechat->m_id = QUuid::createUuid().toString();
            wechat->m_mainWnd = wechatMainWnd;

            WeChatUtil wechatUtil(wechatMainWnd);
            wechat->m_nickName = wechatUtil.getNickName();
            wechat->m_avatarImg = wechatUtil.getAvatar();
            if (wechat->m_nickName.isEmpty())
            {
                qDebug("failed to get the nick name");
                delete wechat;
                continue;
            }

            ::SetParent(wechatMainWnd, m_mainWnd);
            emit hasNewWeChat(wechat);

            // 去除单实例标识
            PatchWeChat();

            qInfo("found a new wechat main window: %s", wechat->m_nickName.toStdString().c_str());
        }
    }
}

WeChatController::WeChatController(QObject *parent)
    : QObject{parent}
{

}

void WeChatController::run()
{
    // 初始化uiautomation
    if (!WeChatUtil::initializeUIA())
    {
        return;
    }

    // 开启定时器监控微信窗口
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WeChatController::onMainTimer);
    timer->start(200);
}

void WeChatController::onMainTimer()
{
    for (const auto& item : m_wechats)
    {
        if (m_wechatRect.isEmpty())
        {
            continue;
        }

        RECT rect;
        if (!::GetWindowRect(item.m_mainWnd, &rect))
        {
            qDebug("failed to get the wechat window rect, error: %d", GetLastError());
            continue;
        }

        bool isInScreen = rect.left != OFFSCREEN_X;
        if (item.m_id == m_currentWeChatId && !isInScreen)
        {
            // 当前窗口不在屏幕内，移到屏幕内
            ::MoveWindow(item.m_mainWnd,
                         m_wechatRect.x(), m_wechatRect.y(),
                         m_wechatRect.width(), m_wechatRect.height(), TRUE);
        }
        else if (item.m_id != m_currentWeChatId && isInScreen)
        {
            // 非当前窗口在屏幕内，移出屏幕
            ::MoveWindow(item.m_mainWnd,
                         OFFSCREEN_X, m_wechatRect.y(),
                         m_wechatRect.width(), m_wechatRect.height(), FALSE);
        }
        else if (item.m_id == m_currentWeChatId && isInScreen)
        {
            // 当前窗口在屏幕内，如果没有在正确的位置，调整下大小和位置
            if (rect.left != m_wechatRect.x() || rect.top != m_wechatRect.y())
            {
                ::MoveWindow(item.m_mainWnd,
                             m_wechatRect.x(), m_wechatRect.y(),
                             m_wechatRect.width(), m_wechatRect.height(), TRUE);
            }
        }
    }
}

void WeChatController::onHasNewWeChat(WeChat* wechat)
{
    bool found = false;
    for (auto& item : m_wechats)
    {
        if (item.m_mainWnd == wechat->m_mainWnd)
        {
            qCritical("it has the same main window");
            found = true;
            break;
        }
    }

    if (!found)
    {
        m_wechats.append(*wechat);
        emit wechatListChange();
    }

    delete wechat;
}

QImage WeChatController::getAvatarImg(QString id)
{
    // todo by yejinlong 加载默认头像
    static QImage defaultImg;

    for (const auto& item : m_wechats)
    {
        if (item.m_id == id)
        {
            if (!item.m_avatarImg.isNull())
            {
                return item.m_avatarImg;
            }

            break;
        }
    }

    return defaultImg;
}

void WeChatController::setMainWindowHandle(HWND mainWindow)
{
    if (mainWindow == NULL || m_mainWnd != NULL)
    {
        return;
    }

    m_mainWnd = mainWindow;

    // 开启线程抓取微信窗口
    m_wechatThread = new WeChatThread();
    m_wechatThread->setMainWindowHandle(m_mainWnd);
    connect(m_wechatThread, &WeChatThread::hasNewWeChat, this, &WeChatController::onHasNewWeChat);
    m_wechatThread->start();
}

void WeChatController::mergeWeChat(bool merge)
{
    m_merge = merge;

    if (m_wechatThread)
    {
        m_wechatThread->enableMerge(merge);
    }

    if (!merge)
    {
        // 微信窗口的父窗口恢复为桌面，并显示
        for (const auto& item : m_wechats)
        {
            ::SetParent(item.m_mainWnd, NULL);
            ::ShowWindow(item.m_mainWnd, SW_SHOW);
        }

        // 清空微信列表
        m_wechats.clear();
        emit wechatListChange();
    }
}

bool WeChatController::startWeChat()
{
    return ::startWeChat();
}
