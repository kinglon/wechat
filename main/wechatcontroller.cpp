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
            wechat->m_id = QUuid::createUuid().toString().remove('{').remove('}');
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

            qInfo("found a new wechat main window: %s", wechat->m_nickName.toStdString().c_str());

            ::SetParent(wechatMainWnd, m_mainWnd);
            emit hasNewWeChat(wechat);

            // 去除单实例标识
            PatchWeChat();            
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

    // 开启线程抓取微信窗口
    m_wechatThread = new WeChatThread();
    m_wechatThread->enableMerge(m_merge);
    m_wechatThread->setMainWindowHandle(m_mainWnd);
    connect(m_wechatThread, &WeChatThread::hasNewWeChat, this, &WeChatController::onHasNewWeChat);
    m_wechatThread->start();

    m_isRunning = true;
}

void WeChatController::stop()
{
    m_isRunning = false;
    WeChatUtil::unInitializeUIA();
}

void WeChatController::onMainTimer()
{
    if (!m_isRunning)
    {
        return;
    }

    // 实时更新每个微信窗口的位置
    for (const auto& item : m_wechats)
    {
        if (m_wechatClientRect.isEmpty())
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
            showWeChatWindow(item.m_mainWnd, true);
        }
        else if (item.m_id != m_currentWeChatId && isInScreen)
        {
            // 非当前窗口在屏幕内，移出屏幕
            showWeChatWindow(item.m_mainWnd, false);
        }
        else if (item.m_id == m_currentWeChatId && isInScreen)
        {
            // 当前窗口在屏幕内，如果没有在正确的位置，调整下大小和位置
            if (rect.left != m_wechatScreenRect.x() || rect.top != m_wechatScreenRect.y()
                    || rect.right-rect.left != m_wechatScreenRect.width()
                    || rect.bottom-rect.top != m_wechatScreenRect.height())
            {
                showWeChatWindow(item.m_mainWnd, true);
            }
        }
    }

    // 实时更新是否有新消息的状态
    bool change = false;
    for (auto& item : m_wechats)
    {
        if (item.m_chatBtn == nullptr)
        {
            WeChatUtil wechatUtil(item.m_mainWnd);
            item.m_chatBtn = wechatUtil.getChatBtn();
        }

        if (item.m_chatBtn)
        {
            VARIANT varValue;
            HRESULT hr = item.m_chatBtn->GetCurrentPropertyValue(UIA_LegacyIAccessibleValuePropertyId, &varValue);
            if (SUCCEEDED(hr))
            {
                if (varValue.vt == VT_BSTR)
                {
                    bool hasNewMsg = varValue.bstrVal[0] != L'\0';
                    if (hasNewMsg != item.m_hasNewMsg)
                    {
                        item.m_hasNewMsg = hasNewMsg;
                        change = true;
                    }
                }
                VariantClear(&varValue);
            }
            else
            {
                item.m_chatBtn->Release();
                item.m_chatBtn = nullptr;
            }
        }
    }

    if (change)
    {
        emit wechatStatusChange();
    }
}

void WeChatController::showWeChatWindow(HWND hWnd, bool visible)
{
    if (visible)
    {
        ::ShowWindow(hWnd, SW_MAXIMIZE);
        ::MoveWindow(hWnd, m_wechatClientRect.x(), m_wechatClientRect.y(),
                     m_wechatClientRect.width(), m_wechatClientRect.height(), TRUE);
    }
    else
    {
        ::MoveWindow(hWnd, OFFSCREEN_X, m_wechatClientRect.y(),
                     m_wechatClientRect.width(), m_wechatClientRect.height(), TRUE);
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
        if (m_currentWeChatId.isEmpty())
        {
            m_currentWeChatId = m_wechats[0].m_id;
        }
        emit wechatListChange();
    }

    delete wechat;
}

void WeChatController::setWechatRect(QRect wechatRect)
{
    if (m_mainWnd == NULL)
    {
        return;
    }

    m_wechatScreenRect = wechatRect;

    // 转为客户坐标
    POINT pt;
    pt.x = wechatRect.left();
    pt.y = wechatRect.top();
    ::ScreenToClient(m_mainWnd, &pt);
    m_wechatClientRect.setX(pt.x);
    m_wechatClientRect.setY(pt.y);
    m_wechatClientRect.setWidth(wechatRect.width());
    m_wechatClientRect.setHeight(wechatRect.height());
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
            ::MoveWindow(item.m_mainWnd, m_wechatScreenRect.x(), m_wechatScreenRect.y(),
                         m_wechatScreenRect.width(), m_wechatScreenRect.height(), TRUE);
        }

        // 清空微信列表
        for (auto& item : m_wechats)
        {
            if (item.m_chatBtn)
            {
                item.m_chatBtn->Release();
                item.m_chatBtn = nullptr;
            }
        }
        m_wechats.clear();
        m_currentWeChatId = "";
        emit wechatListChange();
    }
}

bool WeChatController::startWeChat()
{
    return ::startWeChat();
}
