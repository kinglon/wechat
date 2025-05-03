#include <QCoreApplication>
#include <Windows.h>

std::wstring getSoftInstallPath()
{
    static std::wstring strSoftInstallPath;
    if (!strSoftInstallPath.empty())
    {
        return strSoftInstallPath;
    }

    wchar_t szModulePath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szModulePath, MAX_PATH);
    for (int i = wcslen(szModulePath); i >= 0; --i)
    {
        if (szModulePath[i] == '\\')
        {
            szModulePath[i] = 0;
            break;
        }
    }

    strSoftInstallPath = std::wstring(szModulePath) + L"\\";

    return strSoftInstallPath;
}

// 注册/删除应用信息：安装路径
void registerAppInfo(bool isInstall)
{
    HKEY hRootKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_WRITE, &hRootKey) == ERROR_SUCCESS)
    {
        HKEY softwareKey;
        if (RegOpenKeyEx(hRootKey, L"SOFTWARE", 0, KEY_WRITE, &softwareKey) == ERROR_SUCCESS)
        {
            LPCWSTR subKey = L"DuoKai";
            RegDeleteTree(softwareKey, subKey);

            if (isInstall)
            {
                HKEY hKey;
                if (RegCreateKeyEx(softwareKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
                {
                    std::wstring installPath = getSoftInstallPath();
                    if (RegSetValueEx(hKey, L"InstallPath", 0, REG_SZ, (const BYTE*)installPath.c_str(), (installPath.length()+1)*2) != ERROR_SUCCESS)
                    {
                        qCritical("Failed to set install path value.");
                    }
                    RegCloseKey(hKey);
                }
                else
                {
                    qCritical("Failed to create akool key");
                }
            }

            RegCloseKey(softwareKey);
        }

        RegCloseKey(hRootKey);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc < 2)
    {
        qCritical("too less params");
        return 1;
    }

    bool isInstall = true;
    if (strcmp(argv[1], "install") == 0)
    {
        isInstall = true;
    }
    else if (strcmp(argv[1], "uninstall") == 0)
    {
        isInstall = false;
    }
    else
    {
        qCritical("wrong param");
        return 2;
    }

    // 注册应用信息
    registerAppInfo(isInstall);

    return 0;
}
