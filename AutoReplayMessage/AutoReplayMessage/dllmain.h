#pragma once
#include "pch.h"
#include"resource.h"
#include "commctrl.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

DWORD getWeChatWin();
CHAR* UnicodeToUTF8(const WCHAR* wideStr);
char* UnicodeToANSI(const wchar_t* str);
VOID SendTextMessage(wchar_t* wxid, wchar_t* message);
void HookGetMseeage(HWND hwndDlg, DWORD HookAdd);
VOID GetUserListHookStart(HWND hwndDlg, HWND hwndList, DWORD HookAdd);
BOOL checkCanSendMsg(wchar_t* wxid);
wchar_t* getReplayMsg(wchar_t* msg);

bool endGetUserListHook(DWORD hookA); 
bool endHook(DWORD hookA);


INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD ThreadProc(HMODULE hModule);
void InitListContrl(HWND List);
