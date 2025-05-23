#ifndef HUASHUMANAGER_H
#define HUASHUMANAGER_H

#include <QString>
#include <QVector>
#include <QJsonArray>

// 话术
class HuaShu
{
public:
    // id
    QString m_huaShuId;

    // 分组
    QString m_huaShuGroupName;

    // 标题
    QString m_huaShuTitle;

    // 内容
    QString m_huaShuContent;
};

// 话术分组
class HuaShuGroup
{
public:
    // 分组ID
    QString m_huaShuGroupId;

    // 分组
    QString m_huaShuGroupName;

    // 话术列表
    QVector<HuaShu> m_huaShuList;
};

class HuaShuManager
{
protected:
    HuaShuManager();

public:
    static HuaShuManager* get();

    bool save();

    // 搜索话术，keyWord关键词，groupName分组名字，如果该分组没有话术就取第一个分组
    QString searchHuaShu(QString keyWord, QString groupName);

    // 获取话术分组列表
    QString getHuaShuGroupList();

    // 获取指定分组下的话术列表
    QString getHuaShuList(QString groupId);

    // 新增话术分组
    void addHuaShuGroup(QString groupName);

    // 删除话术分组
    void deleteHuaShuGroup(QString groupId);

    // 新增话术
    void addHuaShu(QString groupId, QString title, const QString& content);

    // 删除话术
    void deleteHuaShu(QString huaShuId);

    // 修改话术
    void editHuaShu(QString huaShuId, QString groupId, QString title, const QString& content);

    // 移动话术
    void moveHuaShu(QString huaShuId, bool up, bool top);

private:
    void loadMyHuaShu();
    void loadTemplateHuaShu();

    void parseHuaShu(const QJsonArray& huaShuJsonArray, QVector<HuaShuGroup>& huaShuList);

public:
    // 我的话术
    QVector<HuaShuGroup> m_myHuaShuList;

    // 话术模板
    QVector<HuaShuGroup> m_templateHuaShuList;
};

#endif // HUASHUMANAGER_H
