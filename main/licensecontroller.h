#ifndef LICENSECONTROLLER_H
#define LICENSECONTROLLER_H

#include <QObject>

// 授权相关的控制器
class LicenseController: public QObject
{
    Q_OBJECT

public:
    LicenseController();

public:
    void run();

signals:
    // 授权到期
    void licenseEnd();

private:
    void onMainTimer();
};

#endif // LICENSECONTROLLER_H
