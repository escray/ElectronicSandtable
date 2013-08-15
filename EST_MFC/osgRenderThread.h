#pragma once

#include "stdafx.h"
#include "OpenThreads/Thread"
#include <osgViewer/Viewer>
#include "osgScene.h"

class osgScene;

class bhThread : public OpenThreads::Thread
{
public:
	bhThread() : _done(false) {}
	~bhThread() { cancel(); }
	virtual int cancel()
	{
		_done = true;
		while(isRunning())
		{
			OpenThreads::Thread::YieldCurrentThread();
		}
		return 0;
	}

	virtual void run() = 0;
protected:
	bool _done;
};


class osgRenderThread : public bhThread
{
public:
	virtual void run();
	void setOsgScene(osgScene *pOsgScene) { _oScene = pOsgScene; }
private:
	osgScene *_oScene;
};
