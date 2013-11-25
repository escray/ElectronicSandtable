#include "StdAfx.h"
#include "ESTCoreOSG.h"
#include "planeUpdate.h"
#include "ribbonUpdate.h"

ESTCoreOSG::ESTCoreOSG(HWND hWnd) : m_hWnd(hWnd)
{
	estManipulator = new ESTManipulator;
	bhManipulator = new BHManipulator;
	southManipulator = new SouthManipulator;

	editpath = new ESTPickHandler();

	elm.setRadiusEquator(6378137);
	elm.setRadiusPolar(6378137);

	RadianPerDegree = osg::PI/180;

	// 飞机轴线位置，来自于飞机模型
	planeAxis = osg::Vec3d( -8.4, 32.3-4898.6, 476.4-550.4 );
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

	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(".\\data\\plane.ive");
	if (!model)
	{
		return;
	}
	// 飞机控制点
	// 与飘带共享
	std::vector<osg::Vec3d> vPos;

	osg::Vec3d vStart( 39.0*RadianPerDegree, 116.3*RadianPerDegree, 5000 );
	osg::Vec3d vStop( 41.5*RadianPerDegree, 116.3*RadianPerDegree, 5000 );
	// 飞机轴线，+或者-表示机头的朝向
	osg::Vec3d vAxis( (39.0+1.0/3600)*RadianPerDegree, 116.3*RadianPerDegree, 5000 );

	CreatePlane(model, vPos, vStart, vStop, vAxis);
	CreateRobbin(vPos);
	// 添加的飞机模型无法显示，考虑采用经纬度再试一次
	m_root->addChild(createMovingModel(osg::Vec3(-2190448, 4431758.5, 5000), 300.0f));

	//m_root->addChild(osgDB::readNodeFile(".\\data\\plane.ive"));

	//CreateHUD();

	

	//osg::Vec3 center();


	m_viewer->setSceneData(m_root.get());
	m_viewer->getCamera()->setCullingMode(m_viewer->getCamera()->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING );
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
	estManipulator->setViewer( m_viewer );
	m_viewer->setCameraManipulator( estManipulator.get() );


	//m_viewer->setCameraManipulator(keyswitchManipulator.get());	
	//m_viewer->setCameraManipulator(trackball.get());
	//m_viewer->setCamera(camera.get());
	//bhManipulator->setViewer(m_viewer);
	//m_viewer->setCameraManipulator(bhManipulator.get());
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

	
	//m_viewer->realize();
}

void ESTCoreOSG::Render( void* ptr )
{
	ESTCoreOSG* osg = (ESTCoreOSG*)ptr;
	osgViewer::Viewer* viewer = osg->getViewer();

	while (!viewer->done())
	{
		long k = viewer->getFrameStamp()->getFrameNumber();
		if ( k % 10 == 0)
		{
			planeCb1->setUpdate(true);
			ribbonCb1->setUpdate(true);
		}
		osg->PreFrameUpdate();
		viewer->frame();
		osg->PostFrameUpdate();
		Sleep(10);
	}

	_endthread();

}

//void ESTCoreOSG::run()
//{
//	m_viewer->realize();
//
//	while ( !m_viewer->done() )
//	{
//		long k = m_viewer->getFrameStamp()->getFrameNumber();
//		// 每10秒钟更新一次位置
//		if ( k % 10 == 0 )
//		{
//			Update();
//		}
//
//		m_viewer->frame();
//	}
//}
//
//void ESTCoreOSG::Update( void )
//{
//	planeCb1->setUpdate(true);
//	ribbonCb1->setUpdate(true);
//}


//void ESTCoreOSG::cancelThreads()
//{
//	cancel();
//	m_viewer->setDone(true);
//	m_viewer->stopThreading();
//}

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

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
	vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
	vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
	gm->setVertexArray(vertices.get());

	// 飘带颜色	
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
	color->push_back( osg::Vec4( 0.8, 0.8, 0.0, 0.5 ) );

	gm->setColorArray( color.get() );
	gm->setColorBinding( osg::Geometry::BIND_OVERALL );

	osg::ref_ptr<osg::DrawArrays> tileNode = new osg::DrawArrays( osg::DrawArrays::LINE_STRIP, 0, vertices->size() );
	gm->addPrimitiveSet( tileNode.get() );
	gm->setUseDisplayList( false );
	gm->setUseVertexBufferObjects( true );

	gm->getOrCreateStateSet()->setAttributeAndModes( new osg::BlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ), osg::StateAttribute::ON );
	gm->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	
	// 飘带宽度
	osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
	width->setWidth( 4 );
	gm->getOrCreateStateSet()->setAttribute( width );

	gm->setDataVariance( osg::Object::DYNAMIC );
	gm->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	// gm->getOrCreateStateSet()->setMode( GL_COLOR_MATERIAL, osg::StateAttribute::OFF );
	gm->getOrCreateStateSet()->setRenderBinDetails( 999, "RenderBin" );
	gm->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
	
	return gm.release();}

void ESTCoreOSG::CreateHUD()
{
	ESTCreateHUD hud;
	osgText::Text* updateText = new osgText::Text;
	editpath->setUpdateText(updateText);
	m_viewer->addEventHandler(editpath);
	m_root->addChild(hud.createTitleHUD());
	m_root->addChild(hud.createPositionHUD(updateText));
}

void ESTCoreOSG::CreatePlane( osg::ref_ptr<osg::Node> model, std::vector<osg::Vec3d>& vPos, osg::Vec3d vStart, osg::Vec3d vStop, osg::Vec3d vAxis )
{
	std::vector<osg::Vec3d> vDir;

	osg::ref_ptr<osg::MatrixTransform> mat = new osg::MatrixTransform;
	mat->addChild(model);
	m_root->addChild(mat);

	// 起点
	double x, y, z;
	elm.convertLatLongHeightToXYZ( vStart.x(), vStart.y(), vStart.z(), x, y, z );

	double x1, y1, z1;
	elm.convertLatLongHeightToXYZ( vAxis.x(), vAxis.y(), vAxis.z(), x1, y1, z1 );
	osg::Vec3d v1 = osg::Vec3d( x1, y1, z1 ) - osg::Vec3d( x, y, z );
	
	// 调整飞机的轴向和位置
	mat->setMatrix( osg::Matrix::rotate(planeAxis, v1) * osg::Matrix::translate(x, y, z) );	

	// 终点
	double x2, y2, z2;
	elm.convertLatLongHeightToXYZ( vStop.x(), vStop.y(), vStop.z(), x2, y2, z2 );	

	CreateControlPoint( x, y, z, x2, y2, z2, vPos, vDir, v1 );

	planeCb1->setAxis( planeAxis );
	// 在这里加入 Cardinal 点集合
	planeCb1->setPos( vPos );
	// 飞机的轴向一直没有变化
	planeCb1->setDir( vDir );
	planeCb1->setAngle( osg::PI );
	mat->setUpdateCallback(planeCb1);
}

void ESTCoreOSG::CreateRobbin( std::vector<osg::Vec3d> vPos )
{

	ribbonCb1->setPos( vPos );
	ribbonCb1->setNp( true );
	ribbonCb1->setA( osg::PI );
	ribbonCb1->setEmp( bhManipulator );

	osg::ref_ptr<osg::Geode> ge = new osg::Geode();
	osg::ref_ptr<osg::Geometry> gm = createRibbonNode();

	ge->addDrawable( gm.get() );
	gm->setUpdateCallback( ribbonCb1 );
	gm->setDataVariance( osg::Object::DYNAMIC );

	m_root->addChild( ge.get() );
}

void ESTCoreOSG::CreateControlPoint( double x, double y, double z, double x2, double y2, double z2, std::vector<osg::Vec3d> &vPos, std::vector<osg::Vec3d> &vDir, osg::Vec3d v1 )
{
	// 初始化
	vPos.push_back( osg::Vec3d( x, y, z ) );
	vDir.push_back( v1 );

	//TODO：控制点的数目100，这个数字需要替换掉，根据飞机的飞行距离来决定放多少个控制点
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
}

ESTPickHandler* ESTCoreOSG::getEditPath()
{
	return editpath;
}

void ESTCoreOSG::StopPath()
{
	if (cameraManipulator.valid())
	{
		m_viewer->setCameraManipulator(cameraManipulator);
	}
}

void ESTCoreOSG::PlayPath( osg::AnimationPath* path )
{
	osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator();
	apm->setAnimationPath(path);
	cameraManipulator = m_viewer->getCameraManipulator();
	m_viewer->setCameraManipulator(apm);
}

// 2013-11-25
osg::AnimationPath* ESTCoreOSG::createAnimationPath(const osg::Vec3& center, float radius, double looptime)
{
	osg::AnimationPath* animationPath = new osg::AnimationPath;	
	animationPath->setLoopMode(osg::AnimationPath::LOOP);

	int numSamples = 40;
	float yaw = 0.0f;
	float yaw_delta = 2.0f * osg::PI/((float)numSamples - 1.0f);
	float roll = osg::inDegrees(30.0f);

	double time = 0.0f;
	double time_delta = looptime/(double)numSamples;

	for (int i=0; i<numSamples; ++i)
	{
		osg::Vec3 position( center + osg::Vec3( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f ) );
		osg::Quat rotation( osg::Quat( roll, osg::Vec3( 0.0, 1.0, 0.0 ))*osg::Quat( -( yaw+osg::inDegrees(90.0f) ), osg::Vec3(0.0, 0.0, 1.0) ) ) ;

		animationPath->insert( time, osg::AnimationPath::ControlPoint( position, rotation ) );

		yaw += yaw_delta;
		time += time_delta;
	}
	return animationPath;
}

osg::Node* ESTCoreOSG::createMovingModel(const osg::Vec3& center, float radius)
{
	float animationLength = 100.0f;
	osg::Group* model = new osg::Group;
	osg::AnimationPath* animationPath = createAnimationPath(center, radius, animationLength);
	
	osg::Node* glider = osgDB::readNodeFile(".\\data\\plane.ive");
	if (glider)
	{
		const osg::BoundingSphere& bs = glider->getBound();
		float size = radius/bs.radius()*0.15f;

		osg::MatrixTransform* positioned = new osg::MatrixTransform;
		positioned->setDataVariance( osg::Object::STATIC );
		positioned->setMatrix( osg::Matrix::translate( -bs.center() )*
							   osg::Matrix::scale( 10.0, 10.0, 10.0 )*
							   osg::Matrix::rotate( osg::inDegrees(-90.0f), 0.0f, 0.0f, 1.0f ) );

		positioned->addChild(glider);
		
		osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;
		xform->setDataVariance(osg::Object::DYNAMIC);
		xform->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		xform->setUpdateCallback( new osg::AnimationPathCallback(animationPath, 0.0, 0.5) );
		xform->addChild( positioned );

		model->addChild( xform );

	}

	return model;
}