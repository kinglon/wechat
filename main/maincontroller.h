#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <qqml.h>
#include "wechatcontroller.h"
#include <QQmlApplicationEngine>
#include "datacollectcontroller.h"
#include "huashumanager.h"

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
    Q_INVOKABLE void exitAccount(QString wechatId) { m_wechatController.exitAccount(wechatId); }
    Q_INVOKABLE void moveAccount(bool up, QString wechatId) { m_wechatController.moveAccount(up, wechatId); }

    Q_INVOKABLE void mergeWeChat(bool merge);

    Q_INVOKABLE void setCurrentWeChat(QString wechatId);

    Q_INVOKABLE void updateWeChatRect(int x, int y, int width, int height);

    // 主窗口已创建
    Q_INVOKABLE void mainWndReady();

    // 获取主屏区域
    Q_INVOKABLE QVariant getPrimaryScreenRect();

    // 发送消息
    Q_INVOKABLE void sendMessage(QString message) { m_wechatController.sendMessage(message); }

    // 搜索话术，keyWord关键词，groupName分组名字，如果该分组没有话术就取第一个分组
    Q_INVOKABLE QString searchHuaShu(QString keyWord, QString groupId) { return HuaShuManager::get()->searchHuaShu(keyWord, groupId); }

    // 获取话术分组列表
    Q_INVOKABLE QString getHuaShuGroupList() { return HuaShuManager::get()->getHuaShuGroupList(); }

    // 获取指定分组下的话术列表
    Q_INVOKABLE QString getHuaShuList(QString groupId) { return HuaShuManager::get()->getHuaShuList(groupId); }

    // 新增话术分组
    Q_INVOKABLE void addHuaShuGroup(QString groupName) { HuaShuManager::get()->addHuaShuGroup(groupName); }

    // 删除话术分组
    Q_INVOKABLE void deleteHuaShuGroup(QString groupId) { HuaShuManager::get()->deleteHuaShuGroup(groupId); }

    // 新增话术
    Q_INVOKABLE void addHuaShu(QString groupId, QString title, const QString& content) { HuaShuManager::get()->addHuaShu(groupId, title, content); }

    // 删除话术
    Q_INVOKABLE void deleteHuaShu(QString huaShuId) { HuaShuManager::get()->deleteHuaShu(huaShuId); }

    // 修改话术
    Q_INVOKABLE void editHuaShu(QString huaShuId, QString groupId, QString title, const QString& content) { HuaShuManager::get()->editHuaShu(huaShuId, groupId, title, content); }

    // 移动话术
    Q_INVOKABLE void moveHuaShu(QString huaShuId, bool up, bool top) { HuaShuManager::get()->moveHuaShu(huaShuId, up, top); }

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

    DataCollectController m_dataCollectController;
};

#endif // MAINCONTROLLER_H
