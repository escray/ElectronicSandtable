#include "StdAfx.h"
#include "osgScene.h"
#include "planeUpdate.h"

// TODO: �滻Ϊѭ����
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
	
}

void osgScene::initializeOsgScene()
{

}

void osgScene::resetClearTest()
{

}


void osgRenderThread::run()
{
	osgViewer::Viewer* viewer = _oScene->getViewer();
	osgScene* osg = _oScene;
	viewer->realize();

	while (!viewer->done())
	{
		long k = viewer->getFrameStamp()->getFrameNumber();
		// ÿ 10 ֡����һ��λ��
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