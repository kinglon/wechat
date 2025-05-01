#include "openwechat.h"
#include "util.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <QFile>
#include <QProcess>

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "Shell32")

ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(
    GetModuleHandleA("ntdll.dll"), "ZwQuerySystemInformation");
NTQUERYOBJECT NtQueryObject = (NTQUERYOBJECT)GetProcAddress(
    GetModuleHandleA("ntdll.dll"), "NtQueryObject");

//进程提权
BOOL ElevatePrivileges()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    tkp.PrivilegeCount = 1;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return FALSE;
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        return FALSE;
    }

    return TRUE;
}

HANDLE DuplicateHandleEx(DWORD pid, HANDLE h, DWORD flags)
{
    HANDLE hHandle = NULL;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProc)
    {
        if (!DuplicateHandle(hProc,
            (HANDLE)h, GetCurrentProcess(),
            &hHandle, 0, FALSE, /*DUPLICATE_SAME_ACCESS*/flags))
        {
            hHandle = NULL;
        }
    }

    CloseHandle(hProc);
    return hHandle;
}

int GetProcIds(LPCWSTR Name, DWORD* Pids)
{
    PROCESSENTRY32 pe32 = { sizeof(pe32) };
    int num = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap)
    {
        if (Process32First(hSnap, &pe32))
        {
            do {
                if (!_wcsicmp(Name, pe32.szExeFile))
                {
                    if (Pids)
                    {
                        Pids[num++] = pe32.th32ProcessID;
                    }
                }
            } while (Process32Next(hSnap, &pe32));
        }
        CloseHandle(hSnap);
    }

    return num;
}

BOOL IsTargetPid(DWORD Pid, DWORD* Pids, int num)
{
    for (int i = 0; i < num; i++)
    {
        if (Pid == Pids[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}

int PatchWeChat()
{
    DWORD dwSize = 0;
    POBJECT_NAME_INFORMATION pNameInfo;
    POBJECT_NAME_INFORMATION pNameType;
    PVOID pbuffer = NULL;
    NTSTATUS Status;
    unsigned int nIndex = 0;
    DWORD dwFlags = 0;
    char szType[128] = { 0 };
    char szName[512] = { 0 };
    PSYSTEM_HANDLE_INFORMATION1 pHandleInfo = NULL;
    DWORD Pids[100] = { 0 };
    int ret = -1;

    if (!ElevatePrivileges())
    {
        qCritical("failed to elevate privilege, error: %d", GetLastError());
    }

    DWORD Num = GetProcIds(L"WeChat.exe", Pids);
    if (Num == 0)
    {
        return ret;
    }

    if (!ZwQuerySystemInformation)
    {
        return ret;
    }

    pbuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
    if (!pbuffer)
    {
        return ret;
    }

    Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, 0x1000, &dwSize);
    if (!NT_SUCCESS(Status))
    {
        if (STATUS_INFO_LENGTH_MISMATCH != Status)
        {
            VirtualFree(pbuffer, 0, MEM_RELEASE);
            return ret;
        }

        if (dwSize * 2 > 0x4000000)  // MAXSIZE
        {
            VirtualFree(pbuffer, 0, MEM_RELEASE);
            return ret;
        }

        // 空间不足
        VirtualFree(pbuffer, 0, MEM_RELEASE);
        pbuffer = VirtualAlloc(NULL, dwSize * 2, MEM_COMMIT, PAGE_READWRITE);
        if (!pbuffer)
        {
            VirtualFree(pbuffer, 0, MEM_RELEASE);
            return ret;
        }

        Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, dwSize * 2, NULL);
        if (!NT_SUCCESS(Status))
        {
            VirtualFree(pbuffer, 0, MEM_RELEASE);
            return ret;
        }
    }

    pHandleInfo = (PSYSTEM_HANDLE_INFORMATION1)pbuffer;
    for (nIndex = 0; nIndex < pHandleInfo->NumberOfHandles; nIndex++)
    {
        if (IsTargetPid(pHandleInfo->Handles[nIndex].UniqueProcessId, Pids, Num))
        {
            HANDLE hHandle = DuplicateHandleEx(pHandleInfo->Handles[nIndex].UniqueProcessId,
                reinterpret_cast<HANDLE>(pHandleInfo->Handles[nIndex].HandleValue),
                DUPLICATE_SAME_ACCESS
            );
            if (hHandle == NULL) continue;

            Status = NtQueryObject(hHandle, ObjectNameInformation, szName, 512, &dwFlags);
            if (!NT_SUCCESS(Status))
            {
                CloseHandle(hHandle);
                continue;
            }

            Status = NtQueryObject(hHandle, ObjectTypeInformation, szType, 128, &dwFlags);
            if (!NT_SUCCESS(Status))
            {
                CloseHandle(hHandle);
                continue;
            }

            CloseHandle(hHandle);

            pNameInfo = (POBJECT_NAME_INFORMATION)szName;
            pNameType = (POBJECT_NAME_INFORMATION)szType;

            WCHAR TypName[1024] = { 0 };
            WCHAR Name[1024] = { 0 };

            wcsncpy_s(TypName, (WCHAR*)pNameType->Name.Buffer, pNameType->Name.Length / 2);
            wcsncpy_s(Name, (WCHAR*)pNameInfo->Name.Buffer, pNameInfo->Name.Length / 2);

            // 匹配是否为需要关闭的句柄名称
            if (0 == wcscmp(TypName, L"Mutant"))
            {
                //WeChat_aj5r8jpxt_Instance_Identity_Mutex_Name
                //if (wcsstr(Name, L"_WeChat_App_Instance_Identity_Mutex_Name"))
                if (wcsstr(Name, L"_WeChat_") &&
                    wcsstr(Name, L"_Instance_Identity_Mutex_Name"))
                {
                    hHandle = DuplicateHandleEx(pHandleInfo->Handles[nIndex].UniqueProcessId,
                        reinterpret_cast<HANDLE>(pHandleInfo->Handles[nIndex].HandleValue),
                        DUPLICATE_CLOSE_SOURCE
                    );

                    if (hHandle)
                    {
                        ret = ERROR_SUCCESS;
                        CloseHandle(hHandle);
                    }
                    else
                    {
                        ret = GetLastError();
                    }
                }
            }
        }
    }

    VirtualFree(pbuffer, 0, MEM_RELEASE);
    return ret;
}

bool startWeChat()
{
    QString wechatExePath = GetWeChatExePath();
    if (wechatExePath.isEmpty())
    {
        qCritical("failed to get the wechat exe path");
        return false;
    }

    if (!QFile(wechatExePath).exists())
    {
        qCritical("the wechat exe path not exist, path: %s", wechatExePath.toStdString().c_str());
        return false;
    }

    QProcess process;
    bool ok = process.startDetached(wechatExePath, QStringList());
    if (!ok)
    {
        qCritical("failed to start wechat, path: %s", wechatExePath.toStdString().c_str());
        return false;
    }

    return true;
}
