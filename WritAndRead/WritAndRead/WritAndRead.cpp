#include "pch.h"
#include "resource.h"
#include <stdio.h>
#include <string.h>
#include <string>

//3.x版本
//微信名：05BC1BAC = WeChatWin.dll + 1AD 1BAC
//微信账号：05BC1D10 = WeChatWin.dll + 1AD 1D10
//头像：05BC1E74 = = WeChatWin.dll + 1AD 1E74
//手机号：05BC1BE0 = WeChatWin.dll + 1AD 1BE0
//微信2.x版本
//进程：weChatStore.exe
//偏移：
//微信名：05BC1BAC = WeChatWin.dll + 104F52C
//微信账号：05BC1D10 = WeChatWin.dll+104F690
//头像：05BC1E74 = = WeChatWin.dll + 104F7F4
//手机号：05BC1BE0 = WeChatWin.dll + 104F560


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
	sprintf_s(wxid, "%s", winApp + 0x104F690);
	SetDlgItemText(hwndDlg, TEXT_ID, wxid);

	//微信名
	char wxName[0x100] = { 0 };
	sprintf_s(wxName, "%s", winApp + 0x104F52C);
	SetDlgItemText(hwndDlg, TEXT_NAME, wxName);

	//手机号
	char phone[0x100] = { 0 };
	sprintf_s(phone, "%s", winApp + 0x104F560);
	SetDlgItemText(hwndDlg, TEXT_PHONE, phone);

	//微信头像
	char wxPic[0x200] = { 0 };
	DWORD pPic = winApp + 0x104F7F4;
	sprintf_s(wxPic, "%s", *((DWORD *)pPic));
	SetDlgItemText(hwndDlg, TEXT_PIC, wxPic);
}

//写内存
void writeWeChatData(HWND hwndDlg)
{
	DWORD winApp = getWechatWinAdd();
	//微信ID
	DWORD wxIdP = winApp + 0x1AD1D10;
	char wxid[0x100] = { 0 };
	GetDlgItemText(hwndDlg, TEXT_ID, wxid,sizeof(wxid));
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)wxIdP, wxid, sizeof(wxid), NULL);

}