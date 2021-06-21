#pragma once
#include "pch.h"
#include"resource.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>


INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID sendTextMessage(wchar_t* wxid, wchar_t* msg);
DWORD ThreadProc(HMODULE hModule);

