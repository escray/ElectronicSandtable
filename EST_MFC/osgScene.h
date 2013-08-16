// COsg.h
//  [8/15/2013 zhaorui]
// TODO: rename to coreOSG.h

#pragma once
#include <OpenThreads/Thread>
#include <osgViewer/Viewer>
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include "planeUpdate.h"

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

class osgScene
{
public:
	osgScene( HWND hWnd );
	~osgScene(void);

	osgViewer::Viewer* getViewer() { return m_Viewer; }
	osg::Group *getOsgSceneRootNode() { return m_osgRootRendNxyGroup.get(); }

	void resetClearTest();
	void initializeOsgScene();
	void cancelThreads();

public:
	bool m_Done;
	HWND m_hWnd;
	osgViewer::Viewer* m_Viewer;
	osg::ref_ptr<osg::Group> m_osgRootRendNxyGroup;
	osgRenderThread m_threadOsgRender;


};
