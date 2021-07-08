// DLL注射器Demo.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "DLL注射器Demo.h"
#include "Inject.h"


char* processName = (char*)"WeChat.exe";

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
	return 0;
}

INT_PTR CALLBACK Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG://首次加载
		//首次加载时需要给选框赋予默认值
		SetDlgItemText(hwndDlg, ID_DLL_PATH, (LPCSTR)"C:\\Users\\Ninga\\Desktop\\DLL\\AutoReplayMessage\\Debug\\AutoReplayMessage.dll");
		SetDlgItemText(hwndDlg, ID_PROCESS_NAME, (LPCSTR)"E:\\WeChat\\WeChat.exe");
		break;
	case WM_CLOSE://关闭事件
		EndDialog(hwndDlg, NULL);
		break;
	case WM_COMMAND://所有按钮的点击事件
		if (wParam == IN_DLL)
		{
			CHAR pathStr[0x100] = { 0 };
			GetDlgItemText(hwndDlg, ID_DLL_PATH, (LPSTR)pathStr,sizeof(pathStr));

			CHAR processNameStr[0x100] = { 0 };
			GetDlgItemText(hwndDlg, ID_PROCESS_NAME, (LPSTR)processNameStr, sizeof(processNameStr));
			//injectDll(pathStr, processNameStr);
			runWechat(pathStr, processNameStr);
			return TRUE;
		}
		else if (wParam == UN_DLL)
		{
			CHAR pathStr[0x100] = { 0 };
			GetDlgItemText(hwndDlg, ID_DLL_PATH, pathStr, sizeof(pathStr));

			CHAR processNameStr[0x100] = { 0 };
			GetDlgItemText(hwndDlg, ID_PROCESS_NAME, processNameStr, sizeof(processNameStr));
			//CHAR pathStr[0x100] = { "E:\\WritAndRead.dll" };
			DWORD dwPID = ProcessNameToFindPID((LPSTR)processNameStr);
			InjectDll2(dwPID, pathStr);

			return TRUE;
			//CHAR pathStr[0x100] = { NULL };
			//GetDlgItemText(hwndDlg, ID_DLL_PATH, (LPSTR)pathStr, sizeof(pathStr));
			//CHAR processNameStr[0x100] = { NULL };
			//GetDlgItemText(hwndDlg, ID_PROCESS_NAME, (LPSTR)processNameStr, sizeof(processNameStr));

			////char*转wchar_t
			//size_t origsize = strlen(pathStr) + 1;
			//const size_t newsize = 100;
			//size_t convertedChars = 0;
			//wchar_t wcstring[newsize];
			//mbstowcs_s(&convertedChars, wcstring, origsize, pathStr, _TRUNCATE);

			//DWORD dwPID = ProcessNameToFindPID((LPSTR)processNameStr);
			//int err;
			//bool flag =  UnInjectDll(dwPID, (LPSTR)wcstring, err);
			//if (!flag)
			//{
			//	//输出错误代码
			//	switch (err)
			//	{
			//	case 1:
			//		MessageBox(NULL, "找不到目标模块", "卸载错误", 0);
			//		break;
			//	case 2:
			//		MessageBox(NULL, "函数的地址获取失败", "卸载错误", 0);
			//		break;
			//	case 3:
			//		MessageBox(NULL, "无法创建远程线程", "卸载错误", 0);
			//		break;
			//	default:
			//		break;
			//	}
			//}
			//else
			//{
			//	MessageBox(NULL, "DLL卸载成功", "提示", 0);
			//}
		}
		break;
	default:
		break;
	}
	return FALSE;
}


//注入DLL
BOOL InjectDll2(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE hProcess = NULL;//进程句柄
	HANDLE hThread = NULL;//线程句柄
	HMODULE hMod = NULL;//模块句柄
	LPVOID pRemoteBuf = NULL;  // 存储在目标进程申请的内存地址  
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);  // 存储DLL文件路径所需的内存空间大小  
	LPTHREAD_START_ROUTINE pThreadProc;
	char cTmp[100] = { 0x00 };

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		MessageBox(NULL, "进程打开失败，可能权限不足或软件未开启", "错误", 0);
		return FALSE;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);  // 在目标进程空间中申请内存  
	if (pRemoteBuf == NULL)
	{
		MessageBox(NULL, "申请内存失败！！", "错误", 0);
		return FALSE;
	}
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL))  // 向在目标进程申请的内存空间中写入DLL文件的路径  
	{
		MessageBox(NULL, "创建远程线程失败", "错误", 0);
		return FALSE;
	}

	hMod = GetModuleHandle("kernel32.dll");
	if (!hMod)
	{
		MessageBox(NULL, "kernel32.dll句柄获取失败", "错误", 0);
		return FALSE;
	}
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");  // 获得LoadLibraryA()函数的地址  
	if (pThreadProc == NULL)
	{
		MessageBox(NULL, "LoadLibraryA()函数的地址获取失败", "错误", 0);
		return FALSE;
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	if (hThread == NULL)
	{
		return FALSE;
	}
	//做一下输出
	//打印注入的地址
	char temp[10000] = { 0 };
	sprintf_s(temp, "写入的地址为：%p", pRemoteBuf);//%p，以16进制的方式打印出来
	OutputDebugString(temp);


	WaitForSingleObject(hThread, INFINITE);//超时结束！
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}




