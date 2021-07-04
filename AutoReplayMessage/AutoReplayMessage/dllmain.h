#pragma once
#include "pch.h"
#include"resource.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

DWORD getWeChatWin();
CHAR* UnicodeToUTF8(const WCHAR* wideStr);
char* UnicodeToANSI(const wchar_t* str);
VOID SendTextMessage(wchar_t* wxid, wchar_t* message);
void HookGetMseeage(HWND hwndDlg, DWORD HookAdd);


INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD ThreadProc(HMODULE hModule);
