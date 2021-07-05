#include "pch.h"
#include"dllmain.h"

#define GET_USER_LIST_HOOK_LEN 5 //HOOK的长度
BYTE getUserListBackCode[GET_USER_LIST_HOOK_LEN] = { 0 };//用于封装hook的代码
HWND getUserListHDlg = 0;//hoook界面的句柄
DWORD getUserListRetCallAdd = 0;//这个是在hook那行所需要call的函数
DWORD getUserListRetAdd = 0;//返回的地址
HWND getUserListGHwndList = 0;

char* list[10000];//存微信账号
char* nameList[10000];//存微信名
int listLen = 0;


char* ignoreList[10000];//添加进忽略列表的数据
int ignoreLen = 0;


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

	if(*((LPVOID*)wxidAdd) == NULL)
	{
		return;
	}
	if (*((LPVOID*)wxNameAdd) == NULL)
	{
		return;
	}
	if (*((LPVOID*)userNameAdd) == NULL)
	{
		return;
	}

	//去重
	bool flag = false;
	for (int i = 0; i < listLen; i++)
	{
		if (strcmp(UnicodeToUTF8(wxid), list[i]) == 0)
		{
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		list[listLen] = UnicodeToUTF8(wxid);
		nameList[listLen] = UnicodeToUTF8(userName);
		listLen++;

		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = (LPWSTR)wxid;
		ListView_InsertItem(getUserListGHwndList, &item);

		item.iSubItem = 1;
		item.pszText = (LPWSTR)wxName;
		ListView_SetItem(getUserListGHwndList, &item);

		item.iSubItem = 2;
		item.pszText = (LPWSTR)userName;
		ListView_SetItem(getUserListGHwndList, &item);
	}
}

//开始hook
/**
 * 参数一 hookAdd 想要hook的地址
 * 参数二 jmpAdd hook完回去的地址
**/
VOID GetUserListStartHook(DWORD hookAdd, LPVOID jmpAdd)
{
	BYTE JmpCode[GET_USER_LIST_HOOK_LEN] = { 0 };
	//我们需要组成一段这样的数据
	// E9 11051111(这里是跳转的地方这个地方不是一个代码地址 而是根据hook地址和跳转的代码地址的距离计算出来的)
	JmpCode[0] = 0xE9;
	//计算跳转的距离公式是固定的
	//计算公式为 跳转的地址(也就是我们函数的地址) - hook的地址 - hook的字节长度
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - GET_USER_LIST_HOOK_LEN;

	//hook第二步 先备份将要被我们覆盖地址的数据 长度为我们hook的长度 HOOK_LEN 5个字节

	//获取进程句柄
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

	//备份数据
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, getUserListBackCode, GET_USER_LIST_HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook地址的数据读取失败", L"读取失败", MB_OK);
		return;
	}

	//真正的hook开始了 把我们要替换的函数地址写进去 让他直接跳到我们函数里面去然后我们处理完毕后再放行吧！
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, GET_USER_LIST_HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook写入失败，函数替换失败", L"错误", MB_OK);
		return;
	}

}

DWORD cgEax = 0;
DWORD cgEcx = 0;
DWORD cgEdx = 0;
DWORD cgEbx = 0;
DWORD cgEsp = 0;
DWORD cgEbp = 0;
DWORD cgEsi = 0;
DWORD cgEdi = 0;

//存储寄存器，调用自己的方法，之后恢复寄存器
VOID __declspec(naked) GetUserListHookF()
{
	__asm {
		call getUserListRetCallAdd
		mov cgEax, eax
		mov cgEcx, ecx
		mov cgEdx, edx
		mov cgEbx, ebx
		mov cgEsp, esp
		mov cgEbp, ebp
		mov cgEsi, esi
		mov cgEdi, edi
	}
	//然后跳转到我们自己的处理函数 想干嘛干嘛
	insertUserLists(cgEax);
	__asm {
		mov eax, cgEax
		mov ecx, cgEcx
		mov edx, cgEdx
		mov ebx, cgEbx
		mov esp, cgEsp
		mov ebp, cgEbp
		mov esi, cgEsi
		mov edi, cgEdi
		jmp getUserListRetAdd
	}
}

VOID GetUserListHookStart(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	//添加特殊白名单
	wchar_t*  specialWxID = (wchar_t *)"q315319188";
	ignoreList[listLen] = UnicodeToUTF8(specialWxID);
	ignoreLen++;

	getUserListGHwndList = hwndList;
	DWORD WinAdd = getWeChatWin();
	getUserListHDlg = hwndDlg;
	getUserListRetCallAdd = WinAdd + 0x10D000;
	getUserListRetAdd = WinAdd + 0x55DEA2;
	GetUserListStartHook(WinAdd + HookAdd, &GetUserListHookF);
}
//卸载HOOK
bool endGetUserListHook(DWORD hookA)
{
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook的地址+偏移
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, getUserListBackCode, GET_USER_LIST_HOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, L"HOOK卸载成功", L"提示", NULL);
		return false;
	}
	return true;
}


//外部调用，检查玩家是否可以发送消息
BOOL checkCanSendMsg(wchar_t * wxid)
{
	bool flag = false;
	for (int i = 0; i < listLen; i++)
	{
		if (strcmp(UnicodeToUTF8(wxid), list[i]) == 0)
		{
			flag = true;
			break;
		}
	}
	if(!flag)
	{
		return flag;
	}

	for (int i = 0; i < ignoreLen; i++)
	{
		if (strcmp(UnicodeToUTF8(wxid), list[i]) == 0)
		{
			flag = false;
			break;
		}
	}
	return flag;
}
