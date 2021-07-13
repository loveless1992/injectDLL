#include "pch.h"
#include"dllmain.h"

/*
 * 该模块向外暴露一个接口，该接口可以定时发送消息，用于自动提醒相关操作
 */

void* show(void* data)
{
	time_t startTime = time(NULL);
	
	while (true)
	{
		time_t nowTime = time(NULL);
		if (nowTime  - startTime >= 5)
		{
			SendTextMessage((wchar_t*)L"filehelper", (wchar_t*)L"666666666");
			startTime = nowTime;
		}

	}
	return NULL;
}

bool alreadyCreatThread = false;
//多少秒之后给谁发什么消息
void startThreadToDoSth(int time,wchar_t msg,wchar_t wxid)
{
	if(alreadyCreatThread == false)
	{
		pthread_t th;
		//创建线程
		pthread_create(&th, 0, show, NULL);
		//pthread_join会阻塞当前线程   pthread_detach不会阻塞当前线程
		//pthread_join(th, NULL);
		pthread_detach(th);
		alreadyCreatThread = true;
	}
}