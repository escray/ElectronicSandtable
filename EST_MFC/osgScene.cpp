#include "StdAfx.h"
#include "osgScene.h"
#include "planeUpdate.h"

// TODO: 替换为循环？
osg::ref_ptr<planeUpdate> plane1 = new planeUpdate;
osg::ref_ptr<planeUpdate> plane2 = new planeUpdate;
osg::ref_ptr<planeUpdate> plane3 = new planeUpdate;
osg::ref_ptr<planeUpdate> plane4 = new planeUpdate;

osgScene::osgScene( HWND hWnd ) : m_hWnd(hWnd)
{
}

osgScene::~osgScene(void)
{
	m_threadOsgRender.cancel();
	m_Viewer->setDone(true);
	m_Viewer->stopThreading();

	delete m_Viewer;
}

void osgScene::cancelThreads()
{
	m_threadOsgRender.cancel();
	m_Viewer->setDone(true);
	m_Viewer->stopThreading();
}

void osgScene::initializeOsgScene()
{	
	//  [8/15/2013 zhaorui] CONTINUED
}

void osgScene::resetClearTest()
{
	osg::Group* root = new osg::Group;
	m_osgRootRendNxyGroup = root;
	m_Viewer = new osgViewer::Viewer();
	m_Viewer->setRunMaxFrameRate(60.0);

	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	osg::GraphicsContext::Traits* traits = new osg::GraphicsContext::Traits;
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

}


void osgRenderThread::run()
{
	osgViewer::Viewer* viewer = _oScene->getViewer();
	osgScene* osg = _oScene;
	viewer->realize();

	while (!viewer->done())
	{
		long k = viewer->getFrameStamp()->getFrameNumber();
		// 每 10 帧更新一次位置
		if ( k%10 == 0 )
		{
			plane1->setUpdate(true);
			plane2->setUpdate(true);
			plane3->setUpdate(true);
			plane4->setUpdate(true);

			// TODO: update ribbon

		}
	}

	viewer->frame();
}