#include "pch.h"
#include"dllmain.h"

//ªÒ»°ƒ£øÈª˘÷∑
DWORD getWeChatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
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

wchar_t* getReplayMsg(wchar_t* msg)
{
	wchar_t retMsg[0x1000] = { 0 };
	swprintf_s(retMsg, L"%s", L"≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘≤‚ ‘");
	return retMsg;
}



