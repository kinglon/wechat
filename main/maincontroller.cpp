﻿#include "maincontroller.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QCoreApplication>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include "settingmanager.h"
#include "../Utility/DumpUtil.h"

MainController::MainController(QObject *parent)
    : QObject{parent}
{

}

void MainController::run()
{
    connect(&m_wechatController, &WeChatController::wechatListChange, this, &MainController::onWeChatListChange);
    connect(&m_wechatController, &WeChatController::wechatStatusChange, this, &MainController::onWeChatStatusChange);
    m_wechatController.run();

    connect(&m_licenseController, &LicenseController::licenseEnd, this, &MainController::licenseEnd);
    m_licenseController.run();
}

void MainController::quitApp()
{
    static bool first = true;
    if (!first)
    {
        return;
    }
    first = false;

    m_wechatController.stop();
    CDumpUtil::Enable(false);
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
    m_dataCollectController.sendEvent("launch");

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

QVariant MainController::getPrimaryScreenRect()
{
    QRect primaryScreenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    return QVariant::fromValue(primaryScreenGeometry);
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
        wechatObject["hasNewMsg"] = wechat.m_hasNewMsg;
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

void MainController::onWeChatStatusChange()
{
    const QVector<WeChat>& wechats = m_wechatController.getWeChats();
    QJsonArray wechatArray;
    for (const auto& wechat : wechats)
    {
        QJsonObject wechatObject;
        wechatObject["wechatId"] = wechat.m_id;
        wechatObject["hasNewMsg"] = wechat.m_hasNewMsg;
        wechatArray.append(wechatObject);
    }
    QJsonDocument jsonDocument(wechatArray);
    emit wechatStatusChange(QString::fromUtf8(jsonDocument.toJson()));
}
