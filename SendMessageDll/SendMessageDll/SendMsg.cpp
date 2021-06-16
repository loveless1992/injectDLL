#include "pch.h"
#include "dllmain.h"

struct wxStr
{
	wchar_t* pStr;
	int straLen;
	int straPerLen;
};


//获取模块基址
DWORD getWeChatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}


VOID sendTextMessage(wchar_t* wxid,wchar_t* msg)
{
	//发送消息CALL
	DWORD sendCall = getWeChatWin() + 0x1343BA;
	//组装微信ID数据
	wxStr pWxID = { 0 };
	pWxID.pStr = wxid;
	pWxID.straLen = wcslen(wxid);//wcslen，取宽字符串长度
	pWxID.straPerLen = wcslen(wxid) * 2;
	//组装文本数据
	wxStr pWxMsg = { 0 };
	pWxMsg.pStr = msg;
	pWxMsg.straLen = wcslen(msg);//wcslen，取宽字符串长度
	pWxMsg.straPerLen = wcslen(msg) * 2;

	//赋值
	//mov ecx, dword ptr ds : [esi + 0xB60]
	//lea eax, dword ptr ss : [ebp - 0x24]
	//push eax
	//add ecx, 0x1528
	//call WeChatWi.78BFECB0

	char* asmWxID = (char*)&pWxID.pStr;
	char* asmWxMsg = (char*)&pWxMsg.pStr;
	_asm
	{
		mov ecx, asmWxID
		lea eax, dword ptr ss : [ebp - 0x24]
	}



}