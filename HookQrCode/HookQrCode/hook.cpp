#include "pch.h"
#include "dllMain.h"

#define HOOOK_LEN 5

//全局缓冲区，用于存放备份数据
BYTE backCode[HOOOK_LEN] = { 0 };
HWND hDlg = 0;

//获取模块基址
DWORD getWeChatWin()
{
	return (DWORD)LoadLibrary("WeChatWin.dll");
}

VOID saveImg(DWORD qrCode)
{
	//先取到ecx+4位置存储的数据，为图片长度
	DWORD picLen = qrCode + 0x4;
	size_t cpyLen = (size_t)*((LPVOID*)picLen);
	//图片的数据
	char picData[0xFFF] = { 0 };
	memcpy(picData, *((LPVOID*)qrCode), cpyLen);
	//打开或创建一个图片文件
	FILE* pFile;
	fopen_s(&pFile, "qrcode.png", "wb");
	//将数据写入文件
	fwrite(picData, sizeof(char), sizeof(picData), pFile);
	fclose(pFile);
	//显示图片
	CImage img;
	CRect rect;
	HWND PicHdl = GetDlgItem(hDlg, QR_CODE);
	GetClientRect(PicHdl, &rect);
	//载入图片
	img.Load("qrcode.png");
	img.Draw(GetDC(PicHdl), rect);
}

DWORD pEax = 0;
DWORD pEcx = 0;
DWORD pEdx = 0;
DWORD pEbx = 0;
DWORD pEbp = 0;
DWORD pEsp = 0;
DWORD pEsi = 0;
DWORD pEdi = 0;
DWORD retAdd = 0;

//hook函数体  __declspec(naked)声明其为一个裸函数，不会对堆栈进行操作，裸函数内不能声明任何变量
VOID __declspec(naked) show()
{
	//备份寄存器
	__asm{
		mov pEax, eax
		mov pEcx, ecx
		mov pEdx, edx
		mov pEbx, ebx
		mov pEbp, ebp
		mov pEsp, esp
		mov pEsi, esi
		mov pEdi, edi
	}
	//将ecx中的二维码数据储存
	saveImg(pEcx);
	retAdd = getWeChatWin()+ 0x5CABEF;
	//恢复寄存器
	__asm{
		mov eax,pEax
		mov ecx,pEcx
		mov edx,pEdx
		mov ebx,pEbx
		mov ebp,pEbp
		mov esp,pEsp
		mov esi,pEsi
		mov edi,pEdi
	    jmp retAdd
	}
}



//微信登陆二维码偏移 : WeChatWin.dll + 0x5CABEA
// hook返回的地址是: WeChatWin.dll + 0x5CABEF
//开始HOOK pram：所需要hook的函数的偏移   跳转的函数
bool startHook(DWORD hookA,LPVOID func, HWND hwndDlg)
{
	hDlg = hwndDlg;//这一步操作是为了让裸函数可以不用在内部申请变量，且不接受参数
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook的地址+偏移

    //组装数据
	BYTE jmpCode[HOOOK_LEN] = { 0 };
	//hook第一字节为类型，E9汇编中代表jump，E8为call
	jmpCode[0] = 0xE9;
	//第二字节代表所需要的地址，规则为：要跳转的地址-hook地址-5
	*(DWORD *)&jmpCode[1] = (DWORD)func - hookAdd - HOOOK_LEN;//这就是为啥我讨厌指针！！！！
	
	//备份原数据，卸载时候替换数据即可
	//获取自己的进程据句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (ReadProcessMemory(hProcess, (LPCVOID)hookAdd, backCode, HOOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "内存备份失败", "错误", NULL);
		return false;
	}

	//写入数据
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, jmpCode, HOOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "内存写入失败", "错误", NULL);
		return false;
	}
	return true;
}


//卸载HOOK
bool endHook(DWORD hookA)
{
	DWORD winAdd = getWeChatWin();
	DWORD hookAdd = winAdd + hookA;//hook的地址+偏移
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hProcess, (LPVOID)hookAdd, backCode, HOOOK_LEN, NULL) == 0)
	{
		MessageBox(NULL, "HOOK卸载成功", "提示", NULL);
		return false;
	}
	return true;
}