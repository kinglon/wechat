#include "licensecontroller.h"
#include <QTimer>
#include <QDateTime>

LicenseController::LicenseController()
{

}

void LicenseController::run()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LicenseController::onMainTimer);
    timer->start(10000);
}

void LicenseController::onMainTimer()
{
    QDateTime targetDate(QDate(2025, 6, 1), QTime(0, 0, 0));
    QDateTime current = QDateTime::currentDateTime();
    if (current > targetDate)
    {
        emit licenseEnd();

        // 销毁定时器
        QTimer* timer = qobject_cast<QTimer*>(sender());
        if (timer)
        {
            timer->stop();
            timer->deleteLater();
        }
    }
}
