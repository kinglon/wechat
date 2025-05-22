#include "huashumanager.h"
#include <QFile>
#include "../Utility/ImPath.h"
#include "../Utility/ImCharset.h"
#include "../Utility/LogMacro.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

HuaShuManager::HuaShuManager()
{
    loadMyHuaShu();
    loadTemplateHuaShu();
}

HuaShuManager* HuaShuManager::get()
{
    static HuaShuManager* instance = new HuaShuManager();
    return instance;
}

void HuaShuManager::loadMyHuaShu()
{
    // 如果data目录没有数据，就先拷贝一份默认的
    std::wstring filePath = CImPath::GetDataPath() + L"quickreply_my.json";
    QFile file(QString::fromStdWString(filePath));
    if (!file.exists())
    {
        std::wstring defaultFilePath = CImPath::GetConfPath() + L"quickreply_my.json";
        ::CopyFile(defaultFilePath.c_str(), filePath.c_str(), FALSE);
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        LOG_ERROR(L"failed to open the my hua shu data file : %s", filePath.c_str());
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    QJsonObject root = jsonDocument.object();
    QJsonArray huaShuJsonArray = root["quickreply"].toArray()[0].toObject()["category"].toArray();
    parseHuaShu(huaShuJsonArray, m_myHuaShuList);
}

void HuaShuManager::loadTemplateHuaShu()
{
    std::wstring filePath = CImPath::GetConfPath() + L"quickreply_template.json";
    QFile file(QString::fromStdWString(filePath));
    if (!file.open(QIODevice::ReadOnly))
    {
        LOG_ERROR(L"failed to open the template hua shu data file : %s", filePath.c_str());
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    QJsonObject root = jsonDocument.object();
    QJsonArray huaShuJsonArray = root["quickreply"].toArray()[0].toObject()["category"].toArray();
    parseHuaShu(huaShuJsonArray, m_templateHuaShuList);
}

void HuaShuManager::parseHuaShu(const QJsonArray& huaShuJsonArray, QVector<HuaShuGroup>& huaShuList)
{
    for (auto huaShuJsonItem : huaShuJsonArray)
    {
        QJsonObject huaShuJsonObject = huaShuJsonItem.toObject();
        HuaShuGroup huaShuGroup;
        huaShuGroup.m_huaShuGroupId = QUuid::createUuid().toString().remove('{').remove('}');
        huaShuGroup.m_huaShuGroupName = huaShuJsonObject["name"].toString();

        QJsonArray contentsJsonArray = huaShuJsonObject["contents"].toArray();
        for (auto contentsJsonItem : contentsJsonArray)
        {
            QJsonObject contentsJsonObject = contentsJsonItem.toObject();
            HuaShu huaShu;
            huaShu.m_huaShuId = QUuid::createUuid().toString().remove('{').remove('}');
            huaShu.m_huaShuGroupName = huaShuGroup.m_huaShuGroupName;
            huaShu.m_huaShuTitle = contentsJsonObject["title"].toString();
            huaShu.m_huaShuContent = contentsJsonObject["content"].toString();
            huaShuGroup.m_huaShuList.append(huaShu);
        }

        huaShuList.append(huaShuGroup);
    }
}

bool HuaShuManager::save()
{
    QJsonArray huaShuJsonArray;
    for (const auto& huaShuGroup : m_myHuaShuList)
    {
        QJsonObject huaShuJsonObject;
        huaShuJsonObject["name"] = huaShuGroup.m_huaShuGroupName;

        QJsonArray contentsJsonArray;
        for (const auto& huaShu : huaShuGroup.m_huaShuList)
        {
            QJsonObject contentsJsonObject;
            contentsJsonObject["title"] = huaShu.m_huaShuTitle;
            contentsJsonObject["content"] = huaShu.m_huaShuContent;
            contentsJsonArray.append(contentsJsonObject);
        }

        huaShuJsonObject["contents"] = contentsJsonArray;
        huaShuJsonArray.append(huaShuJsonObject);
    }

    QJsonObject categoryJsonObject;
    categoryJsonObject["category"] = huaShuJsonArray;

    QJsonArray quickReplyJsonArray;
    quickReplyJsonArray.append(categoryJsonObject);

    QJsonObject root;
    root["quickreply"] = quickReplyJsonArray;

    QJsonDocument jsonDocument(root);
    QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Indented);
    std::wstring filePath = CImPath::GetDataPath() + L"quickreply_my.json";
    QFile file(QString::fromStdWString(filePath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical("failed to save my hua shu");
        return false;
    }
    file.write(jsonData);
    file.close();
    return true;
}

QString HuaShuManager::searchHuaShu(QString keyWord, QString groupName)
{
    // 按关键词搜索
    QVector<HuaShuGroup> results;
    if (keyWord.isEmpty())
    {
        results = m_myHuaShuList;
    }
    else
    {
        for (const auto& item : m_myHuaShuList)
        {
            HuaShuGroup huaShuGroup;
            huaShuGroup.m_huaShuGroupName = item.m_huaShuGroupName;
            for (const auto& content : item.m_huaShuList)
            {
                if (content.m_huaShuContent.contains(keyWord))
                {
                    huaShuGroup.m_huaShuList.append(content);
                }
            }

            if (!huaShuGroup.m_huaShuList.empty())
            {
                results.append(huaShuGroup);
            }
        }
    }

    if (results.isEmpty())
    {
        return "";
    }

    // 获取当前分组
    int currentGroupIndex = 0;
    for (int i=0; i<results.length(); i++)
    {
        if (results[i].m_huaShuGroupName == groupName)
        {
            currentGroupIndex = i;
            break;
        }
    }

    // 构造成json串
    QJsonArray groupJsonArray;
    for (int i=0; i<results.size(); i++)
    {
        const auto& item = results[i];
        QJsonObject groupJsonObject;
        groupJsonObject["groupName"] = item.m_huaShuGroupName;
        groupJsonObject["current"] = i==currentGroupIndex;
        groupJsonArray.append(groupJsonObject);
    }

    QJsonArray huaShuJsonArray;
    for (int i=0; i<results[currentGroupIndex].m_huaShuList.size(); i++)
    {
        const auto& item = results[currentGroupIndex].m_huaShuList[i];
        QJsonObject huaShuJsonObject;
        huaShuJsonObject["huaShuIndex"] = QString("%1").arg(i+1, 2, 10, QChar('0'));
        huaShuJsonObject["huaShuId"] = item.m_huaShuId;
        huaShuJsonObject["huaShuTitle"] = item.m_huaShuTitle;
        huaShuJsonObject["huaShuContent"] = item.m_huaShuContent;
        huaShuJsonArray.append(huaShuJsonObject);
    }

    QJsonObject root;
    root["group"] = groupJsonArray;
    root["huashu"] = huaShuJsonArray;

    QJsonDocument doc(root);
    return QString::fromUtf8(doc.toJson());
}

QString HuaShuManager::getHuaShuGroupList()
{
    QJsonArray myGroupJsonArray;
    for (int i=0; i<m_myHuaShuList.size(); i++)
    {
        const auto& item = m_myHuaShuList[i];
        QJsonObject groupJsonObject;
        groupJsonObject["groupId"] = item.m_huaShuGroupId;
        groupJsonObject["groupName"] = item.m_huaShuGroupName;
        groupJsonObject["isMine"] = true;
        groupJsonObject["huaShuCount"] = item.m_huaShuList.length();
        myGroupJsonArray.append(groupJsonObject);
    }

    QJsonArray templateGroupJsonArray;
    for (int i=0; i<m_templateHuaShuList.size(); i++)
    {
        const auto& item = m_templateHuaShuList[i];
        QJsonObject groupJsonObject;
        groupJsonObject["groupId"] = item.m_huaShuGroupId;
        groupJsonObject["groupName"] = item.m_huaShuGroupName;
        groupJsonObject["isMine"] = false;
        groupJsonObject["huaShuCount"] = item.m_huaShuList.length();
        templateGroupJsonArray.append(groupJsonObject);
    }

    QJsonObject root;
    root["myGroup"] = myGroupJsonArray;
    root["templateGroup"] = templateGroupJsonArray;

    QJsonDocument doc(root);
    return QString::fromUtf8(doc.toJson());
}

QString HuaShuManager::getHuaShuList(QString groupId)
{
    bool isMine = true;
    const HuaShuGroup* huaShuGroup = nullptr;
    for (const auto& item : m_myHuaShuList)
    {
        if (item.m_huaShuGroupId == groupId)
        {
            huaShuGroup = &item;
            break;
        }
    }

    for (const auto& item : m_templateHuaShuList)
    {
        if (item.m_huaShuGroupId == groupId)
        {
            huaShuGroup = &item;
            isMine = false;
            break;
        }
    }

    QJsonArray huaShuJsonArray;
    if (huaShuGroup)
    {
        for (int i=0; i<huaShuGroup->m_huaShuList.size(); i++)
        {
            const auto& item = huaShuGroup->m_huaShuList[i];
            QJsonObject huaShuJsonObject;
            huaShuJsonObject["groupId"] = huaShuGroup->m_huaShuGroupId;
            huaShuJsonObject["huaShuId"] = item.m_huaShuId;
            huaShuJsonObject["huaShuIndex"] = QString("%1").arg(i+1, 2, 10, QChar('0'));
            huaShuJsonObject["huaShuTitle"] = item.m_huaShuTitle;
            huaShuJsonObject["huaShuContent"] = item.m_huaShuContent;
            huaShuJsonObject["isMine"] = isMine;
            huaShuJsonArray.append(huaShuJsonObject);
        }
    }

    QJsonObject root;
    root["huaShuList"] = huaShuJsonArray;

    QJsonDocument doc(root);
    return QString::fromUtf8(doc.toJson());
}

void HuaShuManager::addHuaShuGroup(QString groupName)
{
    if (groupName.isEmpty())
    {
        return;
    }

    HuaShuGroup huaShuGroup;
    huaShuGroup.m_huaShuGroupId = QUuid::createUuid().toString().remove('{').remove('}');
    huaShuGroup.m_huaShuGroupName = groupName;
    m_myHuaShuList.append(huaShuGroup);

    save();
}

void HuaShuManager::deleteHuaShuGroup(QString groupId)
{
    if (groupId.isEmpty())
    {
        return;
    }

    for (int i=0; i<m_myHuaShuList.size(); i++)
    {
        if (m_myHuaShuList[i].m_huaShuGroupId == groupId)
        {
            m_myHuaShuList.remove(i);
            break;
        }
    }

    save();
}

void HuaShuManager::addHuaShu(QString groupId, QString title, const QString& content)
{
    for (int i=0; i<m_myHuaShuList.size(); i++)
    {
        if (m_myHuaShuList[i].m_huaShuGroupId == groupId)
        {
            HuaShu huaShu;
            huaShu.m_huaShuId = QUuid::createUuid().toString().remove('{').remove('}');
            huaShu.m_huaShuGroupName = m_myHuaShuList[i].m_huaShuGroupName;
            huaShu.m_huaShuTitle = title;
            huaShu.m_huaShuContent = content;
            m_myHuaShuList[i].m_huaShuList.append(huaShu);
            break;
        }
    }

    save();
}

void HuaShuManager::deleteHuaShu(QString groupId, QString huaShuId)
{
    for (int i=0; i<m_myHuaShuList.size(); i++)
    {
        if (m_myHuaShuList[i].m_huaShuGroupId == groupId)
        {
            for (int j=0; j<m_myHuaShuList[i].m_huaShuList.size(); j++)
            {
                if (m_myHuaShuList[i].m_huaShuList[j].m_huaShuId == huaShuId)
                {
                    m_myHuaShuList[i].m_huaShuList.remove(j);
                    break;
                }
            }
            break;
        }
    }

    save();
}

void HuaShuManager::editHuaShu(QString groupId, QString huaShuId, QString newGroupId, QString title, const QString& content)
{
    if (groupId == newGroupId)
    {
        // 分组没变，改下内容
        for (int i=0; i<m_myHuaShuList.size(); i++)
        {
            if (m_myHuaShuList[i].m_huaShuGroupId == groupId)
            {
                for (int j=0; j<m_myHuaShuList[i].m_huaShuList.size(); j++)
                {
                    if (m_myHuaShuList[i].m_huaShuList[j].m_huaShuId == huaShuId)
                    {
                        m_myHuaShuList[i].m_huaShuList[j].m_huaShuTitle = title;
                        m_myHuaShuList[i].m_huaShuList[j].m_huaShuContent = content;
                        break;
                    }
                }
                break;
            }
        }
    }
    else
    {
        // 分组变了，先删除，再添加
        deleteHuaShu(groupId, huaShuId);
        addHuaShu(newGroupId, title, content);
    }

    save();
}
