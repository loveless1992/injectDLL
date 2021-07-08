// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include"dllmain.h"


HWND gUserListView = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //启动一个线程初始化界面，防止卡死
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, hModule, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DWORD ThreadProc(HMODULE hModule)
{
    //MessageBox(NULL, L"开始hook", L"提示", NULL);
    //HookWechatQrcode(hModule, 0x55DE9D);
    DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG1), NULL, &Dlgproc);
    return 0;
}

INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hListView = NULL;
    switch (uMsg)
    {
    case WM_INITDIALOG://首次加载
        hListView = GetDlgItem(hwndDlg, USER_LISTS);
        gUserListView = hListView;
        InitListContrl(hListView);
        break;
    case WM_CLOSE://关闭事件
        EndDialog(hwndDlg, NULL);
        break;
    case WM_COMMAND://所有按钮的点击事件
        if (wParam == IDOK)
        {
            HookGetMseeage(hwndDlg, 0x4111DB);
            GetUserListHookStart(hwndDlg, gUserListView, 0x55DE9D);
            MessageBox(NULL, L"HOOK成功", L"提示", MB_OK);
            SetDlgItemText(hwndDlg, ID_STATE, L"已正常HOOK");
        }
        else if (wParam == IDCANCEL)
        {
            endGetUserListHook(0x55DE9D);
            endHook(0x4111DB);
            MessageBox(NULL, L"HOOK卸载成功", L"提示", MB_OK);
            SetDlgItemText(hwndDlg, ID_STATE, L"已卸载HOOK");
        }
        break;
    default:
        break;
    }
    return FALSE;
}

void InitListContrl(HWND List)
{
    LVCOLUMN pcol = { 0 };
    LPCWSTR titleBuffer[] = { L"ID",L"微信账号",L"微信昵称" };
    pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    pcol.fmt = LVCFMT_LEFT;
    pcol.cx = 120;
    for (int i = 0; i < 3; i++) {
        //创建list成员信息
        pcol.pszText = (LPWSTR)titleBuffer[i];
        ListView_InsertColumn(List, i, &pcol);
    }
}

