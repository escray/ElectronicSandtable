#include "StdAfx.h"
#include "osgRenderThread.h"


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
			/*plane1->setUpdate(true);
			plane2->setUpdate(true);
			plane3->setUpdate(true);
			plane4->setUpdate(true);*/

			// TODO: update ribbon

		}
	}

	viewer->frame();
}

