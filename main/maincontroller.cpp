#include "maincontroller.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QCoreApplication>
#include "settingmanager.h"
#include "../Utility/DumpUtil.h"

MainController::MainController(QObject *parent)
    : QObject{parent}
{

}

void MainController::run()
{
    //
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
