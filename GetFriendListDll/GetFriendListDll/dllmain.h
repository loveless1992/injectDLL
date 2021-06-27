#pragma once
#include "pch.h"
#include"resource.h"
#include <Windows.h>
#include <stdio.h>
#include "commctrl.h"
#include <TlHelp32.h>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include <map>
#include <iostream>

DWORD ThreadProc(HMODULE hModule);
INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID HookWechatQrcode(HWND hwndDlg, HWND hwndList, DWORD HookAdd);
void InitListContrl(HWND List);
bool endHook(DWORD hookA);
