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
	keyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
	keyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
	keyswitchManipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator());
	keyswitchManipulator->addMatrixManipulator('5', "FirstPerson", new osgGA::FirstPersonManipulator());
	keyswitchManipulator->addMatrixManipulator('6', "SouthManiputlator", southManipulator);
	keyswitchManipulator->addMatrixManipulator('7', "BHManipulator", bhManipulator.get());	
	keyswitchManipulator->addMatrixManipulator('8', "ESTManipulator", estManipulator.get());

	keyswitchManipulator->selectMatrixManipulator(7);
}

void ESTCoreOSG::InitSceneGraph( void )
{
	m_root = new osg::Group;
	m_node = osgDB::readNodeFile(m_ModelName);
	//m_node = osgDB::readNodeFile("E:\\sourcecode\\bhdata-soft\\BH\\bh_11.ive");
	//m_node = osgDB::readNodeFile("E:\\sourcecode\\ElectronicSandtable\\Debug\\data\\1.ive");
	//m_node = osgDB::readNodeFile("1.ive");

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
	//m_viewer->setCameraManipulator(trackball.get());
	//m_viewer->setCamera(camera.get());
	//bhManipulator->setViewer(m_viewer);
	//m_viewer->setCameraManipulator(bhManipulator.get());

	ESTCreateHUD hud;
	osgText::Text* updateText = new osgText::Text;

	m_viewer->addEventHandler(new ESTPickHandler(updateText));

	m_root->addChild(hud.createTitleHUD());
	m_root->addChild(hud.createPositionHUD(updateText));

	//  [8/28/2013 zhaorui]
	// 如果是其他的 manipulator 如何处理？
	//osg::Vec3d center;
	//if(m_viewer->getCameraManipulator().getName() == "ESTManipulator")
	//{
	//	center = estManipulator->getCenter();
	//	m_root->addChild(hud.createPositionHUD(updateText, center));
	//}	
	//else
	//{
	//m_root->addChild(hud.createPositionHUD(updateText);
	//	}

	m_viewer->setSceneData(m_root.get());
	m_viewer->realize();
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