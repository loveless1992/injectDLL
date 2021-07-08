#include "pch.h"
#include"Inject.h"

VOID setWindow(HWND thisWindow)
{
	HWND wechatWindow = FindWindow("WeChatMainWndForPC", NULL);
	//上：20 下：620 左：10 右：720
	//MoveWindow(wechatWindow, 10, 20, 100, 600, TRUE);

	RECT wechatHandle = { 0 };
	GetWindowRect(wechatWindow, &wechatHandle);
	LONG width = wechatHandle.right - wechatHandle.left;
	LONG height = wechatHandle.bottom - wechatHandle.top;
	MoveWindow(thisWindow, wechatHandle.left - 230, wechatHandle.top, 240, height, TRUE);
	SetWindowPos(thisWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	char buff[0x100] = {};
	sprintf_s(buff, "上：%d 下：%d 左：%d 右：%d\r\n", wechatHandle.top, wechatHandle.bottom, wechatHandle.left, wechatHandle.right);
	OutputDebugString(buff);
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
		if (strcmp((const char*)processInfo.szExeFile, ProcessName) == 0)
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

//注入dll
VOID injectDll(char* dllPath ,char* ProcessName)
{
	char buff[0x100] = { 0 };
	//获取目标进程PID
	DWORD PID = ProcessNameToFindPID(ProcessName);
	if (PID == 0) {
		MessageBox(NULL, "没有找到该进程，可能为启动该软件", "没有找到", MB_OK);
		return;
	}
	else {
		//找到pid我们就打开进程
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
		if (NULL == hProcess) {
			MessageBox(NULL, "进程打开失败", "错误", MB_OK);
			return;
		}
		else {
			DWORD strSize = (DWORD)(_tcslen(dllPath) + 1) * sizeof(TCHAR);
			//进程打开后我们把我们的dll路径存进去
			//首先申请一片内存用于储存dll路径
			LPVOID allocRes = VirtualAllocEx(hProcess, NULL, strSize, MEM_COMMIT, PAGE_READWRITE);
			if (NULL == allocRes) {
				MessageBox(NULL, "内存申请失败", "错误", MB_OK);
				return;
			}

			//申请好后 我们写入路径到目标内存当中
			if (WriteProcessMemory(hProcess, allocRes, (LPVOID)dllPath, strSize, NULL) == 0) {
				MessageBox(NULL, "DLL路径写入失败", "错误", MB_OK);
				return;
			}
			//路径写入 成功后我们现在获取LoadLibraryW 基址
			//LoadLibraryW 在Kernel32.dll里面 所以我们先获取这个dll的基址
			HMODULE hModule = GetModuleHandle("Kernel32.dll");
			LPVOID address = GetProcAddress(hModule, "LoadLibraryA");
			sprintf_s(buff, "loadLibrary=%p path=%p", address, allocRes);
			OutputDebugString(buff);
			//通过远程线程执行这个函数 参数传入 我们dll的地址
			//开始注入dll
			HANDLE hRemote = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, allocRes, 0, NULL);
			if (NULL == hRemote) {
				MessageBox(NULL, "远程执行失败", "错误", MB_OK);
				return;
			}
		}
	}
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

	WCHAR* pDllName = (WCHAR*)wcsrchr((wchar_t*)szDllPath, '\\');
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

//启动微信
//CreateProcess 创建目标进程 创建时即挂起该进程.
//然后注入
//然后再ResumeThread 让目标进程运行
VOID runWechat(TCHAR* dllPath, TCHAR* wechatPath)
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;//SW_SHOW
	CreateProcess(NULL, wechatPath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	LPVOID Param = VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	TCHAR add[0x100] = { 0 };
	if (WriteProcessMemory(pi.hProcess, Param, dllPath, strlen(dllPath) * 2 + sizeof(char), NULL) == 0) 
	{
		MessageBox(NULL, "DLL路径写入失败", "错误", MB_OK);
		return;
	}
	TCHAR buff[0x100] = { 0 };
	HMODULE hModule = GetModuleHandle("Kernel32.dll");
	LPVOID address = GetProcAddress(hModule, "LoadLibraryA");
	//通过远程线程执行这个函数 参数传入 我们dll的地址
	//开始注入dll
	HANDLE hRemote = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, Param, 0, NULL);
	if (NULL == hRemote) {
		MessageBox(NULL, "远程执行失败", "错误", MB_OK);
		return;
	}
	DWORD TIME = ResumeThread(pi.hThread);
}