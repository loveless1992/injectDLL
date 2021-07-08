#pragma once
#include "pch.h"
#include <stdio.h>
#include <Windows.h>
#include <wchar.h>
#include <TlHelp32.h>
#include <string.h>

//…Ë÷√¥∞ÃÂŒª÷√
VOID setWindow(HWND thisWindow);
DWORD ProcessNameToFindPID(char* ProcessName);
VOID injectDll(char* dllPath, char* ProcessName);
BOOL UnInjectDll(DWORD dwPID, LPCTSTR szDllPath, int& nError);
VOID runWechat(TCHAR* dllPath, TCHAR* wechatPath);
BOOL InjectDll2(DWORD dwPID, LPCTSTR szDllPath);

