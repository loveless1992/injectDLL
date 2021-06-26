// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include"dllmain.h"

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
	switch (uMsg)
	{
	case WM_INITDIALOG://首次加载
		break;
	case WM_CLOSE://关闭事件
		EndDialog(hwndDlg, NULL);
		break;
	case WM_COMMAND://所有按钮的点击事件
		if (wParam == IDOK)
		{
            HookWechatQrcode(hwndDlg, 0x55DE9D);
		}
		break;
	default:
		break;
	}
	return FALSE;
}

