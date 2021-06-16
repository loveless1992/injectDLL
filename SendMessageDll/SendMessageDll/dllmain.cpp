// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "dllmain.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//启动一个线程初始化界面，防止卡死
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, hModule, 0, NULL);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DWORD ThreadProc(HMODULE hModule)
{
	DialogBox(hModule, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
	return 0;
}

INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t wxid[0x100] = { 0 };
	wchar_t msg[0x300] = { 0 };
	switch (uMsg)
	{
	case WM_INITDIALOG://首次加载
		break;
	case WM_CLOSE://关闭事件
		EndDialog(hwndDlg, NULL);
		break;
	case WM_COMMAND://所有按钮的点击事件
		if (wParam == ID_SEND)
		{
			//发送文本消息
			sendTextMessage(wxid,msg);
		}
		break;
	default:
		break;
	}
	return FALSE;
}

