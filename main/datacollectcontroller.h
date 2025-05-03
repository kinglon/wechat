#ifndef DATACOLLECTCONTROLLER_H
#define DATACOLLECTCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "../Utility/httpclientbase.h"

class DataCollectController : public HttpClientBase
{
    Q_OBJECT
public:
    explicit DataCollectController(QObject *parent = nullptr);

public:
    void sendEvent(QString eventName);

protected:
    virtual void onHttpResponse(QNetworkReply *reply) override;
};

#endif // DATACOLLECTCONTROLLER_H
