#include "datacollectcontroller.h"
#include "settingmanager.h"
#include <QJsonDocument>
#include <QJsonObject>

#define URI_DATA_COLLECT "/wechat/dc"

DataCollectController::DataCollectController(QObject *parent)
    : HttpClientBase{parent}
{

}

void DataCollectController::sendEvent(QString eventName)
{
    if (eventName.isEmpty())
    {
        return;
    }

    qInfo("send an event: %s", eventName.toStdString().c_str());

    QNetworkRequest request;
    QUrl url(SettingManager::getInstance()->m_host + URI_DATA_COLLECT);
    request.setUrl(url);
    addCommonHeader(request);

    QJsonObject bodyJson;
    bodyJson["event_name"] = eventName;
    QByteArray bodyString = QJsonDocument(bodyJson).toJson();

    // 自定义属性: 发送的次数和body
    request.setAttribute(QNetworkRequest::User, QVariant(1));
    request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User+1), QVariant(bodyString));
    m_networkAccessManager.post(request, bodyString);
}

void DataCollectController::onHttpResponse(QNetworkReply *reply)
{
    QString url = reply->request().url().toString();
    if (url.indexOf(URI_DATA_COLLECT) >= 0)
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qCritical("failed to send the event request, error: %d", reply->error());
            QNetworkRequest request = reply->request();
            int sendTimes = request.attribute(QNetworkRequest::User).toInt();
            if (sendTimes < 3)
            {
                qInfo("retry to send it");
                request.setAttribute(QNetworkRequest::User, QVariant(sendTimes+1));
                QByteArray bodyString = request.attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User+1)).toByteArray();
                m_networkAccessManager.post(request, bodyString);
            }
            return;
        }
    }
}
