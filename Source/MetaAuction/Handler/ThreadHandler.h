#pragma once

#include <HAL/Runnable.h>

/**
 * 멀티 쓰레딩으로 fbx, texture 파일을 읽어주는 handler
 * 이었으나 플러그인 변경으로 인해 싹 날렸음.
 * 추후 쓰레드가 사용될 수 있으니 틀만 남겨두었습니다.
 */
class FThreadHandler : public FRunnable
{
public:
	// Constructor, create the thread by calling this
	FThreadHandler();

	// Destructor
	virtual ~FThreadHandler() override;
	
	virtual bool Init() override; // Do your setup here, allocate memory, ect.
	
	virtual uint32 Run() override; // Main data processing happens here

	virtual void Exit() override; // Run이 끝난 후
	
private:
	bool _ReadDirectory();
	
	// Thread handle. Control the thread using this, with operators like Kill and Suspend
	FRunnableThread* Thread;

	// 스레드가 쓰기 작업을 하고있나?
	uint8 bThreadWrite:1;
};
