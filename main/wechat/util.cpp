#include <strsafe.h>
#include "util.h"
#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#pragma comment(lib, "Version.lib")

bool GetFileVersion(LPCTSTR lpszFilePath, LPTSTR version)
{
    if (wcslen(lpszFilePath) > 0 && PathFileExists(lpszFilePath))
    {
        VS_FIXEDFILEINFO *pVerInfo = NULL;
        DWORD dwTemp, dwSize;
        BYTE *pData = NULL;
        UINT uLen;

        dwSize = GetFileVersionInfoSize(lpszFilePath, &dwTemp);
        if (dwSize == 0)
        {
            return false;
        }

        pData = new BYTE[dwSize + 1];
        if (pData == NULL)
        {
            return false;
        }

        if (!GetFileVersionInfo(lpszFilePath, 0, dwSize, pData))
        {
            delete[] pData;
            return false;
        }

        if (!VerQueryValue(pData, TEXT("\\"), (void **)&pVerInfo, &uLen))
        {
            delete[] pData;
            return false;
        }

        DWORD verMS = pVerInfo->dwFileVersionMS;
        DWORD verLS = pVerInfo->dwFileVersionLS;
        DWORD major = HIWORD(verMS);
        DWORD minor = LOWORD(verMS);
        DWORD build = HIWORD(verLS);
        DWORD revision = LOWORD(verLS);
        delete[] pData;

        StringCbPrintf(version, 0x20, TEXT("%d.%d.%d.%d"), major, minor, build, revision);

        return true;
    }

    return false;
}


QString GetWeChatInstallPath()
{
    static QString wechatInstallPath;
    if (!wechatInstallPath.isEmpty())
    {
        return wechatInstallPath;
    }

    HKEY hKey = NULL;
    HRESULT ret = RegOpenKey(HKEY_CURRENT_USER, L"Software\\Tencent\\WeChat", &hKey);
    if (ERROR_SUCCESS != ret)
    {        
        qCritical("failed to open key: Software\\Tencent\\WeChat, error: 0x%x", ret);
        return "";
    }

    DWORD Type = REG_SZ;
    WCHAR Path[MAX_PATH] = { 0 };
    DWORD cbData = MAX_PATH * sizeof(WCHAR);
    ret = RegQueryValueEx(hKey, L"InstallPath", 0, &Type, (LPBYTE)Path, &cbData);
    if (ERROR_SUCCESS != ret)
    {
        qCritical("failed to query key: InstallPath, error: 0x%x", ret);
        RegCloseKey(hKey);
        return "";
    }

    RegCloseKey(hKey);

    wechatInstallPath = QString::fromWCharArray(Path);
    qInfo("wechat install path: %s", wechatInstallPath.toStdString().c_str());
    return wechatInstallPath;

    PathAppend(Path, L"WeChat.exe");
    return QString::fromWCharArray(Path);
}

QString GetWeChatExePath()
{
    QString wechatInstallPath = GetWeChatInstallPath();
    if (wechatInstallPath.isEmpty())
    {
        return "";
    }

    QString exePath = QFileInfo(wechatInstallPath, "WeChat.exe").absoluteFilePath();
    return exePath;
}

QString GetWeChatDllPath()
{
    QString wechatInstallPath = GetWeChatInstallPath();
    if (wechatInstallPath.isEmpty())
    {
        return "";
    }

    QString dllPath = QFileInfo(wechatInstallPath, "WeChatWin.dll").absoluteFilePath();
    return dllPath;
}

QString GetWeChatVersion()
{
    static QString versionValue = "";
    if (!versionValue.isEmpty())
    {
        return versionValue;
    }

    QString dllPath = GetWeChatDllPath();
    if (!QFile(dllPath).exists())
    {
        return "";
    }

    std::wstring path = dllPath.toStdWString();
    wchar_t version[MAX_PATH];
    if (!GetFileVersion(path.c_str(), version))
    {
        return "";
    }

    versionValue = QString::fromWCharArray(version);
    return versionValue;
}
