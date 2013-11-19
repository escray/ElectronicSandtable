#include "StdAfx.h"
#include "ESTRenderThread.h"


ESTRenderThread::ESTRenderThread(void)
{
}


ESTRenderThread::~ESTRenderThread(void)
{
}

void ESTRenderThread::setCoreOSG( ESTCoreOSG* osg )
{
	_osg = osg;
}

void ESTRenderThread::run()
{
	osgViewer::Viewer* viewer = _osg->getViewer();
	ESTCoreOSG* osg = _osg;
	viewer->realize();
	
	if ( !viewer->done() )
	{
		long k = viewer->getFrameStamp()->getFrameNumber();
		// 每10帧更新一次位置
		if ( k%10 )
		{
			_osg->Update();
		}

		viewer->frame();

	}
}
