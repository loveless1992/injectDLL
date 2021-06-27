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
        //注入的时候
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
    DialogBox(hModule, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
    return 0;
}

INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hListView = NULL;
	switch (uMsg)
	{
	case WM_INITDIALOG://首次加载
        hListView = GetDlgItem(hwndDlg, USER_LISTS);
		MessageBox(NULL, "开始InitListContrl", "提示", MB_OK);
        gUserListView = hListView;
        InitListContrl(hListView);
		MessageBox(NULL, "结束InitListContrl", "提示", MB_OK);
		break;
	case WM_CLOSE://关闭事件
		EndDialog(hwndDlg, NULL);
		break;
	case WM_COMMAND://所有按钮的点击事件
		if (wParam == IDOK)
		{
            HookWechatQrcode(hwndDlg, gUserListView,0x55DE9D);
		}
		else if (wParam == IDCANCEL)
		{
			endHook(0x55DEA2);
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
	LPCSTR titleBuffer[] = { "wxid","微信账号","微信昵称" };
	pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	pcol.fmt = LVCFMT_LEFT;
	pcol.cx = 120;
	for (int i = 0; i < 3; i++) {
		//创建list成员信息
		pcol.pszText = (LPSTR)titleBuffer[i];
		ListView_InsertColumn(List, i, &pcol);
	}
}


