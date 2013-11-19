#pragma once
#include <OpenThreads/Thread>

class ESTThread :
	public OpenThreads::Thread
{
public:
	ESTThread(void);
	~ESTThread(void);

	virtual int cancel();

	virtual void run() = 0;
protected:
	bool _done;
};

