#include "StdAfx.h"
#include "ESTCoreOSG.h"



ESTCoreOSG::ESTCoreOSG(HWND hWnd) : m_hWnd(hWnd)
{
	estManipulator = new ESTManipulator;
	bhManipulator = new BHManipulator;
	southManipulator = new SouthManipulator;
}


ESTCoreOSG::~ESTCoreOSG(void)
{
	m_viewer->setDone(true);
	Sleep(1000);
	m_viewer->stopThreading();
	delete m_viewer;
}

void ESTCoreOSG::InitOSG( std::string filename )
{
	m_ModelName = filename;
	InitManipulators();
	InitSceneGraph();
	InitCameraConfig();
}

void ESTCoreOSG::InitManipulators( void )
{
	trackball = new osgGA::TrackballManipulator();
	keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

	keyswitchManipulator->addMatrixManipulator('1', "Trackball", trackball.get());	
	keyswitchManipulator->addMatrixManipulator('2', "BHManipulator", bhManipulator);
	keyswitchManipulator->addMatrixManipulator('3', "SouthManiputlator", southManipulator);
	//keyswitchManipulator->addMatrixManipulator('4', "ESTManipulator", estManipulator);

	keyswitchManipulator->selectMatrixManipulator(1);
}

void ESTCoreOSG::InitSceneGraph( void )
{
	m_root = new osg::Group;
	m_node = osgDB::readNodeFile("E:\\sourcecode\\bhdata-soft\\BH\\bh_11.ive");

	// 优化模型 Optimize
	//  [8/19/2013 zhaorui]
	// 加载地形的时候有时候会有问题
	// OptimizeModel();

	m_root->addChild(m_node.get());
}

void ESTCoreOSG::InitCameraConfig( void )
{
	RECT rect;
	::GetWindowRect(m_hWnd, &rect);

	m_viewer = new osgViewer::Viewer;
	m_viewer->addEventHandler(new osgViewer::StatsHandler);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;
	// COsg
	// traits->inheritedWindowData = windata.release();

	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;	
	camera->setGraphicsContext(gc);
	camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));
	m_viewer->addSlave(camera.get());
	m_viewer->setCameraManipulator(keyswitchManipulator.get());
	m_viewer->setSceneData(m_root.get());
	m_viewer->realize();
	//m_viewer->run();

}

void ESTCoreOSG::Render( void* ptr )
{
	ESTCoreOSG* osg = (ESTCoreOSG*)ptr;
	osgViewer::Viewer* viewer = osg->getViewer();

	while (!viewer->done())
	{
		osg->PreFrameUpdate();
		viewer->frame();
		osg->PostFrameUpdate();
		Sleep(10);
	}

	AfxMessageBox("程序退出");
	_endthread();

}

void ESTCoreOSG::ReplaceSceneData( std::string filename )
{
	
}

void ESTCoreOSG::OptimizeModel()
{
	osgUtil::Optimizer op;
	op.optimize(m_node.get());
	op.reset();
}

osgViewer::Viewer* ESTCoreOSG::getViewer()
{
	return m_viewer;
}

void ESTCoreOSG::PreFrameUpdate( void )
{

}

void ESTCoreOSG::PostFrameUpdate( void )
{

}
