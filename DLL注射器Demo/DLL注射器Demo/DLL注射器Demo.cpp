// DLL注射器Demo.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "DLL注射器Demo.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <string.h>


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
		break;
	case WM_CLOSE://关闭事件
		EndDialog(hwndDlg, NULL);
		break;
	case WM_COMMAND://所有按钮的点击事件
		if (wParam == IN_DLL)
		{
			CHAR pathStr[0x100] = { "E:\\WritAndRead.dll" };
			DWORD dwPID = ProcessNameToFindPID((LPSTR)"WeChat.exe");
			InjectDll(dwPID, pathStr);
		}
		else if (wParam == UN_DLL)
		{
			wchar_t pathStr[] = L"E:\\WritAndRead.dll";
			DWORD dwPID = ProcessNameToFindPID((LPSTR)"WeChat.exe");
			int a;
			bool flag =  UnInjectDll(dwPID, (LPCTSTR)pathStr, a);
			if (!flag)
			{
				//输出错误代码
				switch (a)
				{
				case 1:
					MessageBox(NULL, "找不到目标模块", "卸载错误", 0);
					break;
				case 2:
					MessageBox(NULL, "函数的地址获取失败", "卸载错误", 0);
					break;
				case 3:
					MessageBox(NULL, "无法创建远程线程", "卸载错误", 0);
					break;
				default:
					break;
				}
			}
			else
			{
				MessageBox(NULL, "DLL卸载成功", "提示", 0);
			}
		}
		break;
	default:
		break;
	}
	return FALSE;
}
//通过微信进程名找到PID，通过PID打开进程获取到进程句柄,由于微信3.2.X添加了子进程，所以这里直接捕获父进程PID
DWORD ProcessNameToFindPID(char* ProcessName)
{
	//获取到整个系统进程快照,需要#include<TlHelp32.h>
	HANDLE processAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	//遍历对比进程名称
	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	//这里将所有的进程ID以及父进ID保存下来
	int processIDArr[10] = { -1 };
	int processParentIDArr[10] = { -1 };
	int num = 0;
	while (Process32Next(processAll, &processInfo))
	{
		if (strcmp(processInfo.szExeFile, ProcessName) == 0)
		{
			processIDArr[num] = processInfo.th32ProcessID;
			processParentIDArr[num] = processInfo.th32ProcessID;
			num++;
		}
	}
	//遍历父句柄，如果子句柄数组中存在父句柄，那么此父句柄就是所需要的句柄
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			if (processParentIDArr[j] == processIDArr[i])
			{
				return  processParentIDArr[j];
			}
		}
	}
	return -999;
}
//注入DLL
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
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
//卸载DLL
BOOL UnInjectDll(DWORD dwPID, LPCTSTR szDllPath, int& nError)
{
	nError = 0;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;  // 存储在目标进程申请的内存地址  
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);  // 存储DLL文件路径所需的内存空间大小  
	LPTHREAD_START_ROUTINE pThreadProc;

	WCHAR* pDllName = (WCHAR*)wcsrchr((wchar_t *)szDllPath,L'\\');
	pDllName = pDllName + 1;

	//创建进程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	MODULEENTRY32 ME32 = { 0 };
	ME32.dwSize = sizeof(MODULEENTRY32);
	BOOL isNext = Module32First(hSnap, &ME32);
	BOOL flag = FALSE;
	while (isNext)
	{
		wchar_t ws[100];
		swprintf(ws, 100, L"%hs", ME32.szModule);

		if (wcscmp((wchar_t const*)ws, (wchar_t const*)pDllName) == 0)
		{
			flag = TRUE;
			break;
		}
		isNext = Module32Next(hSnap, &ME32);
	}
	if (flag == FALSE)
	{
		nError = 1;
		return FALSE;

	}

	// 获取目标进程句柄
	hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, dwPID);
	if (NULL == hProcess)
	{
		return false;
	}

	pThreadProc = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "FreeLibrary");
	if (pThreadProc == NULL)
	{
		nError = 2;
		return FALSE;
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, ME32.modBaseAddr, 0, NULL);
	if (hThread == NULL)
	{
		nError = 3;
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}
