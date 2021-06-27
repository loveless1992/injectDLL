#include "pch.h"
#include"dllmain.h"

#define HOOK_LEN 5 //HOOK的长度
BYTE backCode[HOOK_LEN] = { 0 };//用于封装hook的代码
HWND hDlg = 0;//hoook界面的句柄
DWORD hookData = 0;
DWORD WinAdd = 0;
DWORD retCallAdd = 0;//这个是在hook那行所需要call的函数
DWORD retAdd = 0;//返回的地址
HWND gHwndList = 0;

char* list[10000];//存微信账号
char* nameList[10000];//存微信名
int listLen = 0;

//获取模块基址
DWORD getWechatWin()
{
	return (DWORD)LoadLibrary("WeChatWin.dll");
}



CHAR* UnicodeToUTF8(const WCHAR* wideStr)
{
	char* utf8Str = NULL;
	int charLen = -1;
	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
	utf8Str = (char*)malloc(charLen);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);
	return utf8Str;
}

char* UnicodeToANSI(const wchar_t* str)
{
	char* result;
	int textlen = 0;
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char*)malloc((textlen + 1) * sizeof(char));
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}
//显示好友列表
VOID insertUserLists(DWORD userData)
{
	//微信ID
	wchar_t wxid[0x100] = { 0 };
	DWORD wxidAdd = userData + 0x38;
	if (swprintf_s(wxid, L"%s", *((LPVOID*)wxidAdd)) == -1)
	{
		return;
	}

	//微信号
	wchar_t wxName[0x100] = { 0 };
	DWORD wxNameAdd = userData + 0x4C;
	if (swprintf_s(wxName, L"%s", *((LPVOID*)wxNameAdd)) == -1)
	{
		return;
	}

	//用户昵称
	wchar_t userName[0x100] = { 0 };
	DWORD userNameAdd = userData + 0x94;
	if (swprintf_s(userName, L"%s", *((LPVOID*)userNameAdd)) == -1)
	{
		return;
	}

	//去重
	bool flag = false;
	for (int i = 0; i < listLen; i++)
	{
		if (strcmp(UnicodeToANSI(wxid), list[i]) == 0)
		{
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		list[listLen] = UnicodeToANSI(wxid);
		nameList[listLen] = UnicodeToANSI(userName);
		listLen++;


		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = (LPSTR)UnicodeToANSI(wxid);
		ListView_InsertItem(gHwndList, &item);

		item.iSubItem = 1;
		item.pszText = UnicodeToANSI(wxName);
		ListView_SetItem(gHwndList, &item);

		item.iSubItem = 2;
		item.pszText = UnicodeToANSI(userName);
		ListView_SetItem(gHwndList, &item);
	}
}



//开始hook
/**
 * 参数一 hookAdd 想要hook的地址
 * 参数二 jmpAdd hook完回去的地址
**/
VOID StartHook(DWORD hookAdd, LPVOID jmpAdd)
{
	BYTE JmpCode[HOOK_LEN] = { 0 };
	//我们需要组成一段这样的数据
	// E9 11051111(这里是跳转的地方这个地方不是一个代码地址 而是根据hook地址和跳转的代码地址的距离计算出来的)
	JmpCode[0] = 0xE9;
	//计算跳转的距离公式是固定的
	//计算公式为 跳转的地址(也就是我们函数的地址) - hook的地址 - hook的字节长度
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - HOOK_LEN;

	//hook第二步 先备份将要被我们覆盖地址的数据 长度为我们hook的长度 HOOK_LEN 5个字节

	//获取进程句柄
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

	//备份数据
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, "hook地址的数据读取失败", "读取失败", MB_OK);
		return;
	}

	//真正的hook开始了 把我们要替换的函数地址写进去 让他直接跳到我们函数里面去然后我们处理完毕后再放行吧！
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, "hook写入失败，函数替换失败", "错误", MB_OK);
		return;
	}

}

DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;
DWORD userData = 0;

//存储寄存器，调用自己的方法，之后恢复寄存器
VOID __declspec(naked) HookF()
{
	__asm {
		call retCallAdd
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
	}
	//然后跳转到我们自己的处理函数 想干嘛干嘛
	insertUserLists(cEax);
	__asm {
		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi
		jmp retAdd
	}
}

VOID HookWechatQrcode(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	gHwndList = hwndList;
	WinAdd = getWechatWin();
	hDlg = hwndDlg;
	retCallAdd = WinAdd + 0x10D000;
	retAdd = WinAdd + 0x55DEA2;
	MessageBox(NULL, "HOOK开始", "提示", MB_OK);
	StartHook(WinAdd + HookAdd, &HookF);
}

//卸载HOOK
bool endHook(DWORD hookA)
{
	DWORD winAdd = getWechatWin();
	DWORD hookAdd = winAdd + hookA;//hook的地址+偏移
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "HOOK卸载成功", "提示", NULL);
		return false;
	}
	return true;
}