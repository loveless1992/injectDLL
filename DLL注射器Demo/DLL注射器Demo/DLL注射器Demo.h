#pragma once

#include"resource.h"



INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD ProcessNameToFindPID(char* ProcessName);
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);
BOOL UnInjectDll(DWORD dwPID, LPCTSTR szDllPath, int& nError);
VOID setWindowWechat(HWND hwndDlg);