#include "maincontroller.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QCoreApplication>
#include <QQuickWindow>
#include "settingmanager.h"
#include "../Utility/DumpUtil.h"

MainController::MainController(QObject *parent)
    : QObject{parent}
{

}

void MainController::run()
{
    connect(&m_wechatController, &WeChatController::wechatListChange, this, &MainController::onWeChatListChange);
    m_wechatController.run();
}

void MainController::quitApp()
{
    static bool first = true;
    if (!first)
    {
        return;
    }
    first = false;

    //
}

bool MainController::addAccount()
{
    return m_wechatController.startWeChat();
}

void MainController::mergeWeChat(bool merge)
{
    m_wechatController.mergeWeChat(merge);
}

void MainController::setCurrentWeChat(QString wechatId)
{
    m_wechatController.setCurrentWeChatId(wechatId);
}

void MainController::updateWeChatRect(int x, int y, int width, int height)
{
    QRect rect(x, y, width, height);
    m_wechatController.setWechatRect(rect);
}

void MainController::mainWndReady()
{
    if (m_qmlEngine == nullptr)
    {
        return;
    }    

    QList<QObject*> rootObjects = m_qmlEngine->rootObjects();
    QQuickWindow* targetWindow = nullptr;
    for (QObject* obj : rootObjects)
    {
        QQuickWindow* window = qobject_cast<QQuickWindow*>(obj);
        if (window && window->objectName() == "MainWindow")
        {
            targetWindow = window;
            break;
        }
    }
    if (targetWindow == nullptr || targetWindow->winId() == 0)
    {
        qCritical("failed to find the main window");
        return;
    }

    m_wechatController.setMainWindowHandle((HWND)targetWindow->winId());
    run();
}

void MainController::onWeChatListChange()
{
    QString currentWeChatId = m_wechatController.getCurrentWeChatId();
    const QVector<WeChat>& wechats = m_wechatController.getWeChats();
    QJsonArray wechatArray;
    for (const auto& wechat : wechats)
    {
        QJsonObject wechatObject;
        wechatObject["wechatId"] = wechat.m_id;
        wechatObject["nickName"] = wechat.m_nickName;
        wechatObject["avatarImg"] = QString("image://memory/avatar_") + wechat.m_id;
        if (currentWeChatId == wechat.m_id)
        {
            wechatObject["current"] = true;
        }
        else
        {
            wechatObject["current"] = false;
        }
        wechatArray.append(wechatObject);
    }
    QJsonDocument jsonDocument(wechatArray);
    emit wechatListChange(QString::fromUtf8(jsonDocument.toJson()));
}
