#include "ThreadHandler.h"

#include <HAL/RunnableThread.h>

#pragma region Main Thread Code
// This code will be run on the thread that invoked this thread (i.e. game thread)

FThreadHandler::FThreadHandler()
{
	// 스레드 생성 전에 데이터가 초기화 되어야함
	bThreadWrite = true;
	
	LOG_WARN(TEXT("Thread Handler Init"));

	Thread = FRunnableThread::Create(this, TEXT(""));
}

FThreadHandler::~FThreadHandler()
{
	if(Thread != nullptr)
	{
		Thread->Kill();
		delete Thread;
	}
}

#pragma endregion
// The code below will run on the new thread.

bool FThreadHandler::Init()
{
	// Return false if you want to abort the thread
	bThreadWrite = true;

	return true;
}

uint32 FThreadHandler::Run()
{
	// Peform your processor intensive task here. 

	return -1;
}

void FThreadHandler::Exit()
{
	bThreadWrite = false;
}