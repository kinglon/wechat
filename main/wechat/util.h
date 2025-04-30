#pragma once

#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <QString>

// 获取微信安装目录
QString GetWeChatInstallPath();

// 获取微信程序路径
QString GetWeChatExePath();

// 获取微信核心dll路径
QString GetWeChatDllPath();

// 获取微信版本号
QString GetWeChatVersion();

// 获取文件版本
bool GetFileVersion(LPTSTR lpszFilePath, LPTSTR version);
