#include "StdAfx.h"
#include "ESTCoreOSG.h"
#include "planeUpdate.h"
#include "ribbonUpdate.h"

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

// 全局变量，需要移除
osg::ref_ptr<planeUpdate> planeCb1 = new planeUpdate;
osg::ref_ptr<ribbonUpdate> ribbonCb1 = new ribbonUpdate;


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
	camera->setGraphicsContext( gc );
	camera->setViewport( new osg::Viewport(traits->x, traits->y, traits->width, traits->height) );

	m_viewer->addSlave( camera.get() );
	m_viewer->getCamera()->setProjectionMatrixAsPerspective( 46.0f, (double)traits->width/(double)traits->height, 0.1, 6378137*6 );
	m_viewer->getCamera()->setNearFarRatio( 0.00001 );


	// set up manipulator
	//osg::ref_ptr<BHManipulator> manipulator = new BHManipulator;
	bhManipulator->setViewer( m_viewer );
	m_viewer->setCameraManipulator( bhManipulator.get() );


	//m_viewer->setCameraManipulator(keyswitchManipulator.get());
	
	//m_viewer->setCameraManipulator(trackball.get());
	//m_viewer->setCamera(camera.get());
	//bhManipulator->setViewer(m_viewer);
	//m_viewer->setCameraManipulator(bhManipulator.get());

	// first plane ?
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("E:\\sourcecode\\ElectronicSandtable\\EST_MFC\\data\\plane.ive");

	if (!model)
	{
		return;
	}

	osg::ref_ptr<osg::MatrixTransform> mat = new osg::MatrixTransform;

	mat->addChild(model);

	m_root->addChild(mat);

	double x, y, z;
	osg::EllipsoidModel elm;
	elm.setRadiusEquator(6378137);
	elm.setRadiusPolar(6378137);

	elm.convertLatLongHeightToXYZ( 39.5*osg::PI/180.0, 116.3*osg::PI/180.0, 5000, x, y, z );
	// TODO: 在这里做一个断点，读出 x, y, z 的值；后面考虑采用数值代替经纬度

	double x1, y1, z1;
	elm.convertLatLongHeightToXYZ( (39.5+1.0/3600.0)*osg::PI/180, 116.3*osg::PI/180.0, 5000, x1, y1, z1 );
	osg::Vec3d v1 = osg::Vec3d( x1, y1, z1 ) - osg::Vec3d( x, y, z );
	// 飞机轴线位置
	osg::Vec3d v0 = osg::Vec3d( -8.4, 32.3-4898.6, 476.4-550.4 );
	mat->setMatrix( osg::Matrix::rotate(v0, v1) * osg::Matrix::translate(x, y, z) );

	planeCb1->setAxis( v0 );
	std::vector<osg::Vec3d> vDir, vPos;
	vDir.push_back( v1 );
	vPos.push_back( osg::Vec3d( x, y, z ) );

	double x2, y2, z2;
	elm.convertLatLongHeightToXYZ( (39.8+1.0/3600.0)*osg::PI/180.0, 116.3*osg::PI/180.0, 5000, x2, y2, z2 );

	double dh = 10.0;

	for ( int i=0; i<100; i++ )
	{
		double xt, yt, zt;
		xt = x + ( x2-x )/100 * i;
		yt = y + ( y2-y )/100 * i;
		zt = z + ( z2-z )/100 * i;

		osg::Vec3d pt = osg::Vec3d( xt, yt, zt );
		vPos.push_back( pt );
		vDir.push_back( v1 );
	}

	planeCb1->setPos( vPos );
	planeCb1->setDir( vDir );
	planeCb1->setA( osg::PI );
	mat->setUpdateCallback(planeCb1);

	ribbonCb1->setPos( vPos );
	ribbonCb1->setNp( true );
	ribbonCb1->setA( osg::PI );
	ribbonCb1->setEmp( bhManipulator );
	
	//osg::ref_ptr<osg::Geometry> gm = createRibbonNode();
	//osg::ref_ptr<osg::Geode> ge = new osg::Geode;
	//ge->addDrawable( gm.get() );
	//gm->setUpdateCallback( ribbonCb1 );
	//gm->setDataVariance( osg::Object::DYNAMIC );

	//m_root->addChild( ge.get() );




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
	m_viewer->getCamera()->setCullingMode(m_viewer->getCamera()->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING );
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

// 依据观察点，航迹点，生成飘带
// 观察点是动态的，动态处理部分，交给 Callback
osg::Geometry* ESTCoreOSG::createRibbonNode()
{
	osg::ref_ptr<osg::Geometry> gm = new osg::Geometry();



	return gm.get();
}