#include "wechatcontroller.h"
#include <QTimer>
#include <QUuid>
#include <QCursor>
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
            WeChatUtil wechatUtil(wechatMainWnd);
            QString nickName = wechatUtil.getNickName();
            if (nickName.isEmpty())
            {
                qInfo("failed to get the nick name");
                continue;
            }

            QImage avatarImg = wechatUtil.getAvatar();
            if (!isQImageOpaque(avatarImg))
            {
                qInfo("found a wechat window, but avatar is transparent");
                continue;
            }

            qInfo("found a new wechat main window: %s", nickName.toStdString().c_str());

            WeChat* wechat = new WeChat();
            wechat->m_id = QUuid::createUuid().toString().remove('{').remove('}');
            wechat->m_mainWnd = wechatMainWnd;
            wechat->m_nickName = nickName;
            wechat->m_avatarImg = avatarImg;
            emit hasNewWeChat(wechat);

            ::SetParent(wechatMainWnd, m_mainWnd);

            // 去除单实例标识
            PatchWeChat();            
        }
    }
}

bool WeChatThread::isQImageOpaque(const QImage& image)
{
    if (image.isNull() || image.format() != QImage::Format_ARGB32)
    {
        return false;
    }

    // image.save(R"(C:\Users\zengxiangbin\Downloads\111\bbb.png)");

    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) != 255)
            {
                return false; // 发现透明像素
            }
        }
    }
    return true;
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

void WeChatController::exitAccount(QString wechatId)
{
    for (const auto& item : m_wechats)
    {
        if (item.m_id == wechatId)
        {
            DWORD dwProcessId = 0;
            GetWindowThreadProcessId(item.m_mainWnd, &dwProcessId);
            if (dwProcessId == 0)
            {
                qCritical("failed to call GetWindowThreadProcessId, error: %d", GetLastError());
                break;
            }

            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
            if (hProcess == NULL)
            {
                qCritical("failed to call OpenProcess, pid:%d, error: %d", dwProcessId, GetLastError());
                break;
            }

            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);

            break;
        }
    }
}

void WeChatController::moveAccount(bool up, QString wechatId)
{
    bool change = false;
    for (int i=0; i<m_wechats.size(); i++)
    {
        if (m_wechats[i].m_id == wechatId)
        {
            auto item = m_wechats[i];
            if (up && i > 0)
            {
                m_wechats[i] = m_wechats[i-1];
                m_wechats[i-1] = item;
                change = true;
            }
            else if (!up && i < m_wechats.size()-1)
            {
                m_wechats[i] = m_wechats[i+1];
                m_wechats[i+1] = item;
                change = true;
            }

            break;
        }
    }

    if (change)
    {
        emit wechatListChange();
    }
}

void WeChatController::onMainTimer()
{
    if (!m_isRunning)
    {
        return;
    }

    // 检测微信是否已经退出
    for (auto it=m_wechats.begin(); it!=m_wechats.end(); it++)
    {
        if (!::IsWindow(it->m_mainWnd))
        {
            bool current = it->m_id == m_currentWeChatId;
            it->release();
            m_wechats.erase(it);
            if (current)
            {
                if (m_wechats.size() > 0)
                {
                    m_currentWeChatId = m_wechats[0].m_id;
                }
                else
                {
                    m_currentWeChatId = "";
                }
            }
            emit wechatListChange();
            return;
        }
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

WeChat* WeChatController::getCurrentWeChat()
{
    for (auto& item : m_wechats)
    {
        if (item.m_id == m_currentWeChatId)
        {
            return &item;
        }
    }

    return nullptr;
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
            item.release();
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

void WeChatController::sendMessage(const QString& message)
{
    if (message.isEmpty())
    {
        qCritical("message is empty");
        return;
    }

    WeChat* currentWeChat = getCurrentWeChat();
    if (currentWeChat == nullptr)
    {
        qInfo("current wechat is null");
        return;
    }

    // 获取当前光标位置
    QPoint originalPos = QCursor::pos();

    // 设置窗口激活
    ::SetFocus(currentWeChat->m_mainWnd);

    // 获取发送按钮和消息输入框
    WeChatUtil wechatUtil(currentWeChat->m_mainWnd);
    if (currentWeChat->m_sendBtn == nullptr)
    {
        currentWeChat->m_sendBtn = wechatUtil.getSendBtn();
    }

    if (currentWeChat->m_messageEdit == nullptr)
    {
        currentWeChat->m_messageEdit = wechatUtil.getMessageEdit();
    }

    if (currentWeChat->m_sendBtn == nullptr || currentWeChat->m_messageEdit == nullptr)
    {
        qCritical("failed to find the send button or message edit");
        return;
    }

    // 消息输入框设置消息内容
    if (!WeChatUtil::inputText(currentWeChat->m_messageEdit, message))
    {
        // 重新获取再尝试一次
        currentWeChat->m_messageEdit->Release();
        currentWeChat->m_messageEdit = wechatUtil.getMessageEdit();
        if (currentWeChat->m_messageEdit == nullptr)
        {
            qCritical("failed to find the message edit");
            return;
        }

        if (!WeChatUtil::inputText(currentWeChat->m_messageEdit, message))
        {
            return;
        }
    }

    // 点击发送按钮
    if (!WeChatUtil::clickButton(currentWeChat->m_sendBtn))
    {
        // 重新获取再尝试一次
        currentWeChat->m_sendBtn->Release();
        currentWeChat->m_sendBtn = wechatUtil.getSendBtn();
        if (currentWeChat->m_sendBtn == nullptr)
        {
            qCritical("failed to find the send button");
            return;
        }

        if (!WeChatUtil::clickButton(currentWeChat->m_sendBtn))
        {
            return;
        }
    }

    // 鼠标移回原处
    QCursor::setPos(originalPos);
}
