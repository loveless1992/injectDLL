#include "pch.h"
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <string>

//微信3.3
//偏移
	//微信昵称：0x1DDD55C
	//手机号：0x1DDD590
	//微信ID：0x1DDD6C0
	//微信UID：0x1DDD978
    //手机型号：0x1DDD9FA
	//微信头像：0x1DDD824


//首先获取WeChatWin.dll基址
DWORD getWechatWinAdd()
{
	HMODULE winApp = LoadLibrary("WeChatWin.dll");
	return (DWORD)winApp;
}
//读取内存数据
void readWeChatData(HWND hwndDlg)
{
	DWORD winApp = getWechatWinAdd();
	//微信ID
	char wxid[0x100] = { 0 };
	sprintf_s(wxid, "%s", winApp + 0x1DDD6C0);
	SetDlgItemText(hwndDlg, TEXT_ID, wxid);

	//微信名
	char wxName[0x100] = { 0 };
	sprintf_s(wxName, "%s", winApp + 0x1DDD55C);
	SetDlgItemText(hwndDlg, TEXT_NAME, wxName);

	//手机号
	char phone[0x100] = { 0 };
	sprintf_s(phone, "%s", winApp + 0x1DDD590);
	SetDlgItemText(hwndDlg, TEXT_PHONE, phone);

	//微信头像
	char wxPic[0x200] = { 0 };
	DWORD pPic = winApp + 0x1DDD824;
	sprintf_s(wxPic, "%s", *((DWORD *)pPic));
	SetDlgItemText(hwndDlg, TEXT_PIC, wxPic);
}

//写内存
void writeWeChatData(HWND hwndDlg)
{
	DWORD winApp = getWechatWinAdd();
	//微信ID
	DWORD wxNameP = winApp + 0x1DDD55C;
	char wxName[0x100] = { 0 };
	GetDlgItemText(hwndDlg, TEXT_NAME, wxName,sizeof(wxName));
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)wxNameP, wxName, sizeof(wxName), NULL);

}