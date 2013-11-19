#pragma once
#include "ESTthread.h"
#include "ESTCoreOSG.h"

class ESTRenderThread :
	public ESTThread
{
public:
	ESTRenderThread(void);
	~ESTRenderThread(void);

	virtual void run();
	void setCoreOSG(ESTCoreOSG* osg);

private:
	ESTCoreOSG* _osg;
};

