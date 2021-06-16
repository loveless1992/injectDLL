// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "dllMain.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DialogBox(hModule, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
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
		if (wParam == HOOK_START)
		{
			//开始HOOK
			if (startHook((DWORD)0x5CABEA, show, hwndDlg) == true)
			{
				SetDlgItemText(hwndDlg, ID_STATE, "已加载HOOK");
			}
			else
			{
				SetDlgItemText(hwndDlg, ID_STATE, "加载HOOK失败");
			}
		}
		else if (wParam == HOO_END)
		{
			//卸载HOOK
			if (endHook((DWORD)0x5CABEA))
			{
				SetDlgItemText(hwndDlg, ID_STATE, "已卸载HOOK");
			}
			else
			{
				SetDlgItemText(hwndDlg, ID_STATE, "卸载HOOK失败");
			}
			
		}
		break;
		break;
	default:
		break;
	}
	return FALSE;
}

