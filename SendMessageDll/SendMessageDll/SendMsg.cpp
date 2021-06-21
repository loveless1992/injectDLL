#include "pch.h"
#include "dllmain.h"

struct wxStr
{
	wchar_t* pStr;
	int straLen;
	int straPerLen;
	int end;
};


//获取模块基址
DWORD getWeChatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}


VOID sendTextMessage(wchar_t* wxid,wchar_t* msg)
{
	//发送消息CALL
	DWORD sendCall = getWeChatWin() + 0x11EECB;
	//组装微信ID数据 
	wxStr pWxID = { 0x0 };
	memset(&pWxID, 0, sizeof(pWxID));
	pWxID.pStr = wxid;
	pWxID.straLen = wcslen(wxid);//wcslen，取宽字符串长度
	pWxID.straPerLen = wcslen(wxid);
	pWxID.end = 0x0;
	//组装文本数据
	wxStr pWxMsg = { 0x0 };
	memset(&pWxMsg, 0, sizeof(pWxMsg));
	pWxMsg.pStr = msg;
	pWxMsg.straLen = wcslen(msg);//wcslen，取宽字符串长度
	pWxMsg.straPerLen = wcslen(msg);
	pWxMsg.end = 0x0;

	//赋值
	/*
		push 0x1
		push edi   //0
		push ebx   //内容
		lea edx,dword ptr ss:[ebp-0x88] //ID
		lea ecx,dword ptr ss:[ebp-0x5F0]  //0
		call WeChatWi.63503930                   
		add esp,0xC
	*/
	char* asmWxID = (char*)&pWxID.pStr;
	char* asmWxMsg = (char*)&pWxMsg.pStr;

	char ecxBuff[0x5F0] = { 0 };
	char edxBuff[0x88] = { 0 };
	char ediBuff[0x10] = { 0 };
	char eaxBuff[0x20] = { 0 };
	__asm{
		push 0x1
		push edi
		mov ebx, asmWxMsg
		push ebx
		mov edx,asmWxID
		lea ecx, ecxBuff
		lea eax, eaxBuff
		call sendCall
		add esp,0xC
	}



}