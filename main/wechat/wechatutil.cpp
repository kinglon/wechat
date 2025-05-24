#include "wechatutil.h"
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <QGuiApplication>
#include <QClipboard>
#include <QThread>
#include <QCursor>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")

IUIAutomation* WeChatUtil::m_uiAutomation = nullptr;

bool WeChatUtil::initializeUIA()
{
    if (m_uiAutomation)
    {
        return true;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        qCritical("failed to call CoInitializeEx, error: 0x%x", hr);
        return false;
    }

    IUIAutomation* pUIA = nullptr;
    CoInitialize(NULL);
    hr = CoCreateInstance(CLSID_CUIAutomation, NULL,
                                 CLSCTX_INPROC_SERVER, IID_IUIAutomation,
                                 (void**)&pUIA);
    if (FAILED(hr))
    {
        qCritical("failed to initializeUIA");
        return false;
    }

    m_uiAutomation = pUIA;
    return true;
}

void WeChatUtil::unInitializeUIA()
{
    if (m_uiAutomation)
    {
        m_uiAutomation->Release();
        m_uiAutomation = nullptr;
    }
}

IUIAutomationElement* WeChatUtil::getNavigationToolbar()
{
    if (m_uiAutomation == nullptr)
    {
        return nullptr;
    }

    IUIAutomationElement* pRoot = nullptr;
    m_uiAutomation->ElementFromHandle(m_hWnd, &pRoot);
    if (!pRoot)
    {
        qDebug("failed to get the root element");
        return nullptr;
    }

    // 创建搜索条件：控件类型为工具栏且名称为导航
    VARIANT varProp;
    varProp.vt = VT_I4;
    varProp.lVal = UIA_ToolBarControlTypeId;
    IUIAutomationCondition* pControlTypeCondition = nullptr;
    m_uiAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pControlTypeCondition);

    VARIANT varName;
    varName.vt = VT_BSTR;
    varName.bstrVal = SysAllocString(L"导航");
    IUIAutomationCondition* pNameCondition = nullptr;
    m_uiAutomation->CreatePropertyCondition(UIA_NamePropertyId, varName, &pNameCondition);
    VariantClear(&varName);

    // 组合条件
    IUIAutomationCondition* pCombinedCondition = nullptr;
    m_uiAutomation->CreateAndCondition(pControlTypeCondition, pNameCondition, &pCombinedCondition);

    // 搜索
    IUIAutomationElement* pNavigationToolbar = nullptr;
    pRoot->FindFirst(TreeScope_Descendants, pCombinedCondition, &pNavigationToolbar);

    // 释放资源
    pCombinedCondition->Release();
    pNameCondition->Release();
    pControlTypeCondition->Release();
    pRoot->Release();

    return pNavigationToolbar;
}

QImage WeChatUtil::captureElement(IUIAutomationElement* element)
{
    // 获取元素的边界矩形（相对于屏幕）
    RECT screenRect = {0};
    if (FAILED(element->get_CurrentBoundingRectangle(&screenRect)))
    {
        qCritical("failed to call get_CurrentBoundingRectangle");
        return QImage();
    }

    // 转为客户区矩形
    POINT pt = {screenRect.left, screenRect.top};
    ScreenToClient(m_hWnd, &pt);
    RECT clientRect = {pt.x, pt.y, pt.x+screenRect.right-screenRect.left, pt.y+screenRect.bottom-screenRect.top};

    // 抓取窗口图片，必须从左上角开始抓取
    RECT captrueRect = clientRect;
    captrueRect.left = 0;
    captrueRect.top = 0;
    QImage img = captureWindowRegion(m_hWnd, captrueRect);
    if (img.isNull())
    {
        return img;
    }

    // 裁剪元素区域的图片
    return img.copy(clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
}

QImage WeChatUtil::captureWindowRegion(HWND hwnd, const RECT& rect)
{
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0)
    {
        return QImage();
    }

    // 获取窗口设备上下文
    HDC hdcWindow = GetWindowDC(hwnd);
    if (!hdcWindow)
    {
        return QImage();
    }

    // 创建内存 DC 和位图
    HDC hdcMem = CreateCompatibleDC(hdcWindow);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
    SelectObject(hdcMem, hBitmap);

    // 使用 PrintWindow 捕获窗口内容（支持 GPU 渲染窗口）
    PrintWindow(hwnd, hdcMem, PW_CLIENTONLY);

    // 转换为 32 位 ARGB 格式
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // 从上到下
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixels(width * height * 4);
    GetDIBits(hdcMem, hBitmap, 0, height, pixels.data(), &bmi, DIB_RGB_COLORS);

    // 创建 QImage（ARGB32 格式）
    QImage image(width, height, QImage::Format_ARGB32);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int offset = (y * width + x) * 4;
            BYTE b = pixels[offset];
            BYTE g = pixels[offset + 1];
            BYTE r = pixels[offset + 2];
            BYTE a = pixels[offset + 3];
            image.setPixel(x, y, qRgba(r, g, b, a));
        }
    }

    // 释放资源
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdcWindow);

    return image;
}

QString WeChatUtil::getNickName()
{
    IUIAutomationElement* navigationToolbar = getNavigationToolbar();
    if (navigationToolbar == nullptr)
    {
        return "";
    }

    VARIANT varProp;
    varProp.vt = VT_I4;
    varProp.lVal = UIA_ButtonControlTypeId;
    IUIAutomationCondition* pControlTypeCondition = nullptr;
    m_uiAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pControlTypeCondition);

    QString nickName;
    IUIAutomationElement* button = nullptr;
    navigationToolbar->FindFirst(TreeScope_Children, pControlTypeCondition, &button);
    if (button)
    {
        BSTR name;
        button->get_CurrentName(&name);
        nickName = QString::fromWCharArray(name);
        SysFreeString(name);
        button->Release();
    }

    pControlTypeCondition->Release();
    navigationToolbar->Release();

    return nickName;
}

QImage WeChatUtil::getAvatar()
{
    QImage avatarImg;
    IUIAutomationElement* navigationToolbar = getNavigationToolbar();
    if (navigationToolbar == nullptr)
    {
        return avatarImg;
    }

    VARIANT varProp;
    varProp.vt = VT_I4;
    varProp.lVal = UIA_ButtonControlTypeId;
    IUIAutomationCondition* pControlTypeCondition = nullptr;
    m_uiAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pControlTypeCondition);

    IUIAutomationElement* button = nullptr;
    navigationToolbar->FindFirst(TreeScope_Children, pControlTypeCondition, &button);
    if (button)
    {
        avatarImg = captureElement(button);
        button->Release();
    }

    pControlTypeCondition->Release();
    navigationToolbar->Release();

    return avatarImg;
}

IUIAutomationElement* WeChatUtil::getChatBtn()
{
    IUIAutomationElement* navigationToolbar = getNavigationToolbar();
    if (navigationToolbar == nullptr)
    {
        return nullptr;
    }

    IUIAutomationCondition* pNameCondition = NULL;
    m_uiAutomation->CreatePropertyCondition(UIA_NamePropertyId, CComVariant(L"聊天"), &pNameCondition);

    IUIAutomationCondition* pControlTypeCondition = NULL;
    m_uiAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, CComVariant(UIA_ButtonControlTypeId), &pControlTypeCondition);

    IUIAutomationCondition* pCombinedCondition = NULL;
    m_uiAutomation->CreateAndCondition(pNameCondition, pControlTypeCondition, &pCombinedCondition);

    IUIAutomationElement* button = nullptr;
    navigationToolbar->FindFirst(TreeScope_Children, pCombinedCondition, &button);

    pNameCondition->Release();
    pControlTypeCondition->Release();
    pCombinedCondition->Release();
    navigationToolbar->Release();

    return button;
}

IUIAutomationElement* WeChatUtil::getMessageEdit()
{
    // 先找发送按钮
    IUIAutomationElement* sendButton = getSendBtn();
    if (sendButton == nullptr)
    {
        return nullptr;
    }

    IUIAutomationTreeWalker* walker = nullptr;
    HRESULT hr = m_uiAutomation->get_RawViewWalker(&walker);
    if (FAILED(hr))
    {
        qCritical("failed to get walker, error: 0x%x", hr);
        sendButton->Release();
        return nullptr;
    }

    // 3次父元素
    IUIAutomationElement* pCurrent = sendButton;
    for (int i = 0; i < 3; ++i)
    {
        IUIAutomationElement* pParent = nullptr;
        walker->GetParentElement(pCurrent, &pParent);
        pCurrent->Release(); // 释放当前元素
        pCurrent = pParent;

        if (!pCurrent) break; // 没有更多父元素
    }
    // sendButton不需要释放，查询父元素的时候已经释放了
    walker->Release();

    if (pCurrent == nullptr)
    {
        qCritical("failed to get parent");
        return nullptr;
    }

    // 查找编辑框控件
    IUIAutomationCondition* pControlTypeCondition = NULL;
    m_uiAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, CComVariant(UIA_EditControlTypeId), &pControlTypeCondition);

    IUIAutomationElement* edit = nullptr;
    pCurrent->FindFirst(TreeScope_Descendants, pControlTypeCondition, &edit);

    pControlTypeCondition->Release();
    pCurrent->Release();

    return edit;
}

IUIAutomationElement* WeChatUtil::getSendBtn()
{
    if (m_uiAutomation == nullptr)
    {
        return nullptr;
    }

    IUIAutomationElement* pRoot = nullptr;
    m_uiAutomation->ElementFromHandle(m_hWnd, &pRoot);
    if (!pRoot)
    {
        qCritical("failed to get the root element");
        return nullptr;
    }

    VARIANT varName;
    varName.vt = VT_BSTR;
    varName.bstrVal = SysAllocString(L"发送(S)");
    IUIAutomationCondition* pNameCondition = NULL;
    m_uiAutomation->CreatePropertyCondition(UIA_NamePropertyId, varName, &pNameCondition);
    VariantClear(&varName);

    IUIAutomationCondition* pControlTypeCondition = NULL;
    m_uiAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, CComVariant(UIA_ButtonControlTypeId), &pControlTypeCondition);

    IUIAutomationCondition* pCombinedCondition = NULL;
    m_uiAutomation->CreateAndCondition(pNameCondition, pControlTypeCondition, &pCombinedCondition);

    IUIAutomationElement* button = nullptr;
    pRoot->FindFirst(TreeScope_Descendants, pCombinedCondition, &button);

    pNameCondition->Release();
    pControlTypeCondition->Release();
    pCombinedCondition->Release();
    pRoot->Release();

    return button;
}

bool WeChatUtil::clickElement(IUIAutomationElement* element)
{
    // 获取元素位置
    RECT rect;
    element->get_CurrentBoundingRectangle(&rect);

    // 计算中心点
    int x = rect.left + (rect.right - rect.left) / 2;
    int y = rect.top + (rect.bottom - rect.top) / 2;

    // 移动鼠标
    QCursor::setPos(QPoint(x, y));

    // 发送鼠标事件
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    // 左键按下
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    // 左键释放
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    int sendCount = SendInput(2, inputs, sizeof(INPUT));
    if (sendCount != 2)
    {
        qCritical("failed to send input, only send %d event, error: %d", sendCount, GetLastError());
        return false;
    }

    return true;
}

bool WeChatUtil::inputText(IUIAutomationElement* editControl, const QString& text)
{
    // 将文本内容复制到粘贴板
    QGuiApplication::clipboard()->setText(text);

    // 点击输入框
    if (!clickElement(editControl))
    {
        return false;
    }

    QThread::msleep(100);

    // 按Ctrl+V将发送内容粘贴到消息输入框
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));
    // 按下Ctrl
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    // 按下V
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';
    // 释放V
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    // 释放Ctrl
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT sentCount = SendInput(4, inputs, sizeof(INPUT));
    if (sentCount != 4)
    {
        qCritical("failed to send all keyboard inputs, only send %d inputs, error: %d", sentCount, GetLastError());
        return false;
    }
    
    return true;
}
