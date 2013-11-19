#include "StdAfx.h"
#include "ESTThread.h"

ESTThread::ESTThread( void )
{
	_done = false;
}


ESTThread::~ESTThread(void)
{
	cancel();
}

int ESTThread::cancel()
{
	_done = true;
	while ( isRunning() )
	{
		OpenThreads::Thread::YieldCurrentThread();
	}
	return 0;
}
