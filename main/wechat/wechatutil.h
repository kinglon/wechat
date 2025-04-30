#pragma once

#include <QString>
#include <Windows.h>
#include <QImage>
#include <uiautomation.h>

class WeChatUtil
{
public:
    WeChatUtil(HWND hWnd) { m_hWnd = hWnd; }

public:
    static bool initializeUIA();

    // 获取昵称
    QString getNickName();

    // 获取头像
    QImage getAvatar();

    // 检查是否有新消息
    bool hasNewMessage(bool& has);

private:
    // 获取导航工具栏对象
    IUIAutomationElement* getNavigationToolbar();

    // 截取指定元素区域的图片
    QImage captureElement(IUIAutomationElement* element);

    // 截取指定窗口上指定区域的图片，regionRect是相对于客户区域
    static QImage captureWindowRegion(HWND hWnd, const RECT& regionRect);

private:
    static IUIAutomation* m_uiAutomation;

    HWND m_hWnd = NULL;
};
