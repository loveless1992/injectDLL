#pragma once

#include "pch.h"
#include "resource.h"
#include <Windows.h>
#include <stdio.h>
#include<atlimage.h>
#include<stdio.h>


INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD getWeChatWin();
bool startHook(DWORD hookA, LPVOID func, HWND hwndDlg);
bool endHook(DWORD hookA);
VOID show();
