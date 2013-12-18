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

	// for test
	
	initialPoints = new osg::Vec3Array();
	initialPoints->push_back(osg::Vec3(-2185423.25, 4437509, 5000));
	initialPoints->push_back(osg::Vec3(-2190448, 4431758.5, 5000));
	initialPoints->push_back(osg::Vec3(-2177271.25, 4426147.5, 5000));
	initialPoints->push_back(osg::Vec3(-2182646.75, 4402591, 5000));
	initialPoints->push_back(osg::Vec3(-2189121, 4335879, 5000));
}

ESTCoreOSG::~ESTCoreOSG(void)
{
	m_viewer->setDone(true);
	Sleep(1000);
	m_viewer->stopThreading();
	delete m_viewer;
}

// 全局变量，需要移除
osg::ref_ptr<planeUpdate> planeUpdate1 = new planeUpdate;
osg::ref_ptr<ribbonUpdate> ribbonUpdate1 = new ribbonUpdate;

osg::ref_ptr<planeUpdate> planeUpdate2 = new planeUpdate;
osg::ref_ptr<ribbonUpdate> ribbonUpdate2 = new ribbonUpdate;

osg::ref_ptr<planeUpdate >planeCb1=new planeUpdate;
osg::ref_ptr<planeUpdate >planeCb2=new planeUpdate;
osg::ref_ptr<planeUpdate >planeCb3=new planeUpdate;
osg::ref_ptr<planeUpdate >planeCb4=new planeUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb1=new ribbonUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb2=new ribbonUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb3=new ribbonUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb4=new ribbonUpdate;


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

	// planeCB1 的初始化是否可以放到函数里面？
	CreatePlane(model, vPos, vStart, vStop, vAxis, planeUpdate1);
	CreateRibbon(vPos, ribbonUpdate1);
	// 添加的飞机模型无法显示，考虑采用经纬度再试一次	
	osg::ref_ptr<osg::MatrixTransform> mt1=new osg::MatrixTransform;
	mt1->addChild(model);
	m_root->addChild(mt1);

	osg::ref_ptr<osg::MatrixTransform> mt2=new osg::MatrixTransform;
	mt2->addChild(model);
	m_root->addChild(mt2);
	double x,y,z;
	osg::EllipsoidModel elm;
	elm.setRadiusEquator(6378137);
	elm.setRadiusPolar(6378137);

	//机头指向-Y,翅膀X,
	{
		elm.convertLatLongHeightToXYZ(39.5*osg::PI/180.0,116.3*osg::PI/180.0,5000,x,y,z);

		double x1,y1,z1;
		elm.convertLatLongHeightToXYZ((39.5+1.0/3600.0)*osg::PI/180.0,116.3*osg::PI/180.0,5000,x1,y1,z1);
		osg::Vec3d v1=osg::Vec3d(x1,y1,z1)-osg::Vec3d(x,y,z);
		//飞机轴线位置
		osg::Vec3d v0=osg::Vec3d(-8.4,32.3-4898.6,476.4-550.4);

		mt1->setMatrix(osg::Matrix::rotate(v0,v1)*
			osg::Matrix::translate(x,y,z));

		planeCb1->setAxis(v0);

		std::vector<osg::Vec3d>vDir,vPos;
		vDir.push_back(v1);			
		vPos.push_back(osg::Vec3d(x,y,z));

		double x2,y2,z2;
		elm.convertLatLongHeightToXYZ((39.8+1.0/3600.0)*osg::PI/180.0,116.3*osg::PI/180.0,5000,x2,y2,z2);

		double dh=10.0;
		for(int i=0;i<100;i++)
		{
			double xt,yt,zt;
			xt=x+(x2-x)/100.0*i;
			yt=y+(y2-y)/100.0*i;
			zt=z+(z2-z)/100.0*i;

			osg::Vec3d pt=osg::Vec3d(xt,yt,zt);
			vPos.push_back(pt);
			vDir.push_back(v1);

		}
		planeCb1->setPos(vPos);
		planeCb1->setDir(vDir);
		planeCb1->setAngle(osg::PI);		

		mt1->setUpdateCallback(planeCb1);

		ribbonCb1->setPos(vPos);
		ribbonCb1->setNp(true);				
		ribbonCb1->setA(osg::PI);
		ribbonCb1->setEmp(estManipulator.get());

		osg::ref_ptr<osg::Geometry>gm=createRibbonNode();	
		osg::ref_ptr<osg::Geode>ge=new osg::Geode();
		ge->addDrawable(gm.get());
		gm->setUpdateCallback(ribbonCb1);	
		gm->setDataVariance(osg::Object::DYNAMIC);

		m_root->addChild(ge.get());

	}

	{
		elm.convertLatLongHeightToXYZ(39.0*osg::PI/180.0,116.2*osg::PI/180.0,5000,x,y,z);

		double x1,y1,z1;
		elm.convertLatLongHeightToXYZ((39.0+1.0/3600.0)*osg::PI/180.0,116.2*osg::PI/180.0,5000,x1,y1,z1);
		osg::Vec3d v1=osg::Vec3d(x1,y1,z1)-osg::Vec3d(x,y,z);
		//飞机轴线位置
		osg::Vec3d v0=osg::Vec3d(-8.4,32.3-4898.6,476.4-550.4);

		mt2->setMatrix(osg::Matrix::rotate(v0,v1)*
			osg::Matrix::translate(x,y,z));

		planeCb2->setAxis(v0);

		std::vector<osg::Vec3d>vDir,vPos;
		vDir.push_back(v1);			
		vPos.push_back(osg::Vec3d(x,y,z));

		double x2,y2,z2;
		elm.convertLatLongHeightToXYZ((41.5+1.0/3600.0)*osg::PI/180.0,116.2*osg::PI/180.0,5000,x2,y2,z2);

		for(int i=0;i<100;i++)
		{
			double xt,yt,zt;
			xt=x+(x2-x)/100.0*i;
			yt=y+(y2-y)/100.0*i;
			zt=z+(z2-z)/100.0*i;

			vPos.push_back(osg::Vec3d(xt,yt,zt));
			vDir.push_back(v1);

		}
		planeCb2->setPos(vPos);
		planeCb2->setDir(vDir);
		planeCb2->setAngle(osg::PI);

		mt2->setUpdateCallback(planeCb2);

		ribbonCb2->setPos(vPos);				
		ribbonCb2->setNp(true);				
		ribbonCb2->setA(osg::PI);
		ribbonCb2->setEmp(estManipulator);

		osg::ref_ptr<osg::Geometry>gm=createRibbonNode();	
		osg::ref_ptr<osg::Geode>ge=new osg::Geode();
		ge->addDrawable(gm.get());
		gm->setUpdateCallback(ribbonCb2);				
		gm->setDataVariance(osg::Object::DYNAMIC);

		m_root->addChild(ge.get());
	}



	CreateHUD();

	//osg::Vec3d vStart2( 40*RadianPerDegree, 116.5*RadianPerDegree, 5000 );
	//osg::Vec3d vStop2( 41.5*RadianPerDegree, 116.5*RadianPerDegree, 5000 );
	//osg::Vec3d vAxis2( (39.5+1.0/3600)*RadianPerDegree, 116.5*RadianPerDegree, 5000 );

	//CreatePlane(model, vPos, vStart2, vStop2, vAxis2, planeCb2);
	//CreateRobbin(vPos);

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

	double scale = 1.0;
	osg::Vec3 position;
	osg::Vec3 center;
	osg::Vec3 up;
	double currentHeight = 4015930.0;


	//scale = calculateScale()

	while (!viewer->done())
	{
		viewer->getCamera()->getViewMatrixAsLookAt(position, center, up);

		currentHeight = position[2];
		scale = calculateScale(currentHeight);

		long k = viewer->getFrameStamp()->getFrameNumber();
		if ( k % 10 == 0)
		{
			planeUpdate1->setUpdate(true);
			planeUpdate1->setScale(scale);
			ribbonUpdate1->setUpdate(true);

			planeCb1->setUpdate(true);
			ribbonCb1->setUpdate(true);

			planeCb2->setUpdate(true);
			ribbonCb2->setUpdate(true);
			/*planeCb3->setUpdate(true);			
			ribbonCb3->setUpdate(true);
			planeCb4->setUpdate(true);
			ribbonCb4->setUpdate(true);*/

		}
		osg->PreFrameUpdate();
		viewer->frame();
		osg->PostFrameUpdate();
		//Sleep(10);
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

// 路径计算部分可以独立出去
void ESTCoreOSG::CreatePlane( osg::ref_ptr<osg::Node> model, std::vector<osg::Vec3d>& vPos, osg::Vec3d vStart, osg::Vec3d vStop, osg::Vec3d vAxis, osg::ref_ptr<planeUpdate>& planecb )
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

	osg::AnimationPath* path = createSimpleAnimationPath(x, y, z, x2, y2, z2, v1);
	
	createSimplePlane(v1, path);

	//1. animation
	//2. particle

	//osg::ref_ptr<planeUpdate> planecallback = new planeUpdate();

	planecb->setAxis( planeAxis );
	// 在这里加入 Cardinal 点集合
	planecb->setPos( vPos );
	// 飞机的轴向一直没有变化
	planecb->setDir( vDir );
	planecb->setAngle( osg::PI );
	mat->setUpdateCallback(planecb);
}

void ESTCoreOSG::CreateRibbon( std::vector<osg::Vec3d> vPos, osg::ref_ptr<ribbonUpdate>& ribboncb )
{
	ribboncb->setPos( vPos );
	ribboncb->setNp( true );
	ribboncb->setA( osg::PI );
	ribboncb->setEmp( estManipulator );

	osg::ref_ptr<osg::Geode> ge = new osg::Geode();
	osg::ref_ptr<osg::Geometry> gm = createRibbonNode();

	ge->addDrawable( gm.get() );
	gm->setUpdateCallback( ribboncb );
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

osg::AnimationPath* ESTCoreOSG::createCardinalAnimationPath( osg::ref_ptr<osg::Vec3Array> controls )
{
	osg::AnimationPath* animationPath = new osg::AnimationPath;	
	animationPath->setLoopMode(osg::AnimationPath::LOOP);

	double looptime = 10;
	int numSamples = 100;
	double time = 0.0f;
	double time_delta = looptime/(double)numSamples;

//	osg::Vec3 startPoint = controls->begin();

//	std::vector<osg::Vec3>::iterator = controls->begin();
	osg::ref_ptr<osg::Vec3Array> controlPoints = new osg::Vec3Array;



	// 求两点间距离

	// 求采样点个数

	// 得到轨迹点


	return animationPath;
}

osg::AnimationPath* ESTCoreOSG::createSimpleAnimationPath( double x, double y, double z, double x2, double y2, double z2, osg::Vec3d v1 )
{
	osg::AnimationPath* animationPath = new osg::AnimationPath;	
	animationPath->setLoopMode(osg::AnimationPath::LOOP);
	
	double looptime = 10;

	int numSamples = 100;
	double time = 0.0f;
	double time_delta = looptime/(double)numSamples;

	// begin for test
	//osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();

	//points->push_back(osg::Vec3(-2185423.25, 4437509, 5000));
	//points->push_back(osg::Vec3(-2190448, 4431758.5, 5000));
	//points->push_back(osg::Vec3(-2177271.25, 4426147.5, 5000));
	//points->push_back(osg::Vec3(-2182646.75, 4402591, 5000));
	//points->push_back(osg::Vec3(-2189121, 4335879, 5000));

	//osg::ref_ptr<osg::Vec3Array> cardinalPoints = Cardinal(points);

	//std::vector<osg::Vec3>::iterator iter = cardinalPoints->begin();

	//for( ; iter!=cardinalPoints->end(); *iter++)
	//{
	//	time += time_delta;
	//	osg::Vec3 p = *iter++;
	//	osg::Quat r(osg::inDegrees(0.0f), v1);

	//	animationPath->insert( time, osg::AnimationPath::ControlPoint(p, r));
	//}

	// end for test

	for ( int i=0; i<numSamples; i++ )
	{
	double xt, yt, zt;
	xt = x + ( x2-x )/numSamples * i;
	yt = y + ( y2-y )/numSamples * i;
	zt = z + ( z2-z )/numSamples * i;
	time += time_delta;
	osg::Vec3 position(xt, yt, zt);
	osg::Quat rotation(osg::inDegrees(0.0f), v1);

	animationPath->insert( time, osg::AnimationPath::ControlPoint(position, rotation));

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

void ESTCoreOSG::createSimplePlane( osg::Vec3d v1, osg::AnimationPath* path )
{
	osg::Node* plane = osgDB::readNodeFile(".\\data\\plane.ive");
	if (plane)
	{
		float radius = 300.0f;
		const osg::BoundingSphere& bs = plane->getBound();
		float size = radius/bs.radius()*0.15f;

		osg::MatrixTransform* positioned = new osg::MatrixTransform;
		positioned->setDataVariance( osg::Object::STATIC );
		positioned->setMatrix( osg::Matrix::translate( -bs.center() )*
			osg::Matrix::scale( 1.0, 1.0, 1.0 )*
			osg::Matrix::rotate(planeAxis, v1) );

		positioned->addChild(plane);

		osg::PositionAttitudeTransform* xform = new osg::PositionAttitudeTransform;
		xform->setDataVariance(osg::Object::DYNAMIC);
		xform->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
		xform->setUpdateCallback( new osg::AnimationPathCallback(path, 0.0, 0.5) );
		xform->addChild( positioned );

		m_root->addChild( xform );

	}
}


osg::ref_ptr<osg::Vec3Array> ESTCoreOSG::Cardinal( osg::ref_ptr<osg::Vec3Array> point )
{
	// 这个 pointer 应该替换为 vPos，就是控制点的集合
	// 2013-11-20
	// 处理头尾两个节点
	std::vector<osg::Vec3>::iterator iter = point->begin();
	osg::ref_ptr<osg::Vec3Array> ctrlTwo = new osg::Vec3Array();
	ctrlTwo->push_back( osg::Vec3( (*iter).x()-1, (*iter).y()-1, (*iter).z() ) );
	ctrlTwo->insert( ++(ctrlTwo->begin() ), point->begin(), point->end() );
	iter = point->end();
	iter--;
	ctrlTwo->push_back( osg::Vec3( (*iter).x()-1, (*iter).y()-1, (*iter).z()-1 ) );
	iter = ctrlTwo->begin();

	ref_ptr<osg::Vec3Array> cardinal = new osg::Vec3Array();

	for (; iter!=ctrlTwo->end(); *iter++)
	{
		osg::Vec3 p0 = *iter++;
		osg::Vec3 p1 = *iter++;
		osg::Vec3 p2 = *iter++;
		if (iter == ctrlTwo->end() )
		{
			return cardinal;
		}
		osg::Vec3 p3 = *iter;

		iter--;
		iter--;
		iter--;

		float t = 0;
		for ( ; t<=1; t=t+0.1 )
		{
			// 高度 5000 不变
			cardinal->push_back( osg::Vec3( GetCoefficient( p0.x(), p1.x(), p2.x(), p3.x(), t ), GetCoefficient( p0.y(), p1.y(), p2.y(), p3.y(), t ), 5000 ) );
		}
	}

	return cardinal;

}

float ESTCoreOSG::GetCoefficient( float p0, float p1, float p2, float p3, float t )
{
	float d = p1;
	float c = 0.5 * ( 1 - t ) * ( p2 - p0 );
	float a = 0.5 * ( t - 1 ) * p0 + ( 1.5 + 0.5 * t ) * p1 - ( 1.5 + 0.5 * t ) * p2 + 0.5 * ( 1 - t ) * p3;
	float b = p2 - a - d - c;
	return ( a * t * t * t + b * t * t + c * t + d );
}

double ESTCoreOSG::GetAllDistance()
{
	float distant  = 0.0 ;
	osg::Vec3Array::iterator iter = initialPoints ->begin() ;

	const int size = initialPoints->size () ;
	if (size <= 1)
		return 0;

	else
	{
		for (int i = 0 ;i < size-1 ; i++,iter ++)
		{
			osg::Vec3 temp = *iter ;
			iter ++  ;
			distant += sqrt ( (temp.x () - (*iter).x())*(temp.x () - (*iter).x())   +  (temp.y ()-(*iter).y ()) * (temp.y ()-(*iter).y())) ;
			iter -- ;
		} ;
	}
	return distant ;
}

osg::AnimationPath* ESTCoreOSG::CreatePath(std::string pathName)
{
	double sec = 1.0;
	double looptime = sec * GetAllDistance() ;
	std::vector <osg::Vec3 > ::iterator iter =initialPoints->end ();
	std::vector <osg::Vec3 > ::iterator iter2 ;
	iter--;
	iter2 = --iter ;
	iter ++ ;

	initialPoints ->push_back (osg::Vec3 ((*iter).x()-(*(iter2)).x (), (*(iter)).y()-(*(iter2)).y(), (*(iter)).z()-(*(iter2)).z())) ;

	osg::AnimationPath* animationPath = new osg::AnimationPath;
	animationPath->setLoopMode(osg::AnimationPath::LOOP);

	int numSamples = initialPoints ->size ();


	float yaw = 0.0f;
	float yaw_delta = 0.5;
	float roll = osg::inDegrees(90.0f);

	double time=0.0f;
	double time_delta = looptime/(double)numSamples;

	float angle = 0.0 ;

	iter = initialPoints  ->begin () ;
	for(int i=1;i<numSamples;++i, iter ++)
	{

		osg::Vec3 position(*iter);
		iter ++ ;
		if (iter != initialPoints ->end ())
		{
			//这只是一种情况
			if ((*iter).x() > position.x())
			{
				angle = 1.57-atan(  ((*iter).y () - position.y()) /((*iter).x() - position.x())) ;
				if (angle < 0)
					angle = 1.57 + angle ;
			} ;

			if ((*iter).x() < position.x())
			{
				angle = -(1.57+atan(  ((*iter).y () - position.y()) /((*iter).x() - position.x()))) ;
				if (angle > 0)
					angle = -(1.57 - angle) ;
			}
			//对于X=X的情况

		} ;

		osg::Quat rotation(  osg::Quat(roll,osg::Vec3(1.0,0.0,0.0)) * osg::Quat(-angle,osg::Vec3(0.0,0.0,1.0)));
		osg::Quat rotationY(osg::Quat ( -(3.1415926/6.0), osg::Vec3 (1.0, 0.0, 0.0))) ;
		//rotation *= rotationY ;


		animationPath->insert(time,osg::AnimationPath::ControlPoint(position,rotation));
		time += GetRunTime (position, *iter) ;

		//yaw -= yaw_delta;

		iter -- ;
	}
		
	//std::ofstream fout(pathName.c_str());
	//animationPath->write(fout);
	//fout.close();
	return animationPath;    
}

float ESTCoreOSG::GetRunTime(osg::Vec3 res, osg::Vec3 des)
{
	float distant = sqrt (  (des.x () - res.x())*(des.x () - res.x())   + (des.y ()-res.y())*(des.y ()-res.y()) ) ;
	double sec = 1.0f;
	double init = sec ;
	return (init * distant);	
}

double ESTCoreOSG::calculateScale( double currentHeight )
{
	double scale = 1.0;
	double maxHeight = 4300000.0;
	double maxScale = 1.5;
	double minScale = 0.75;

	double left = sqrt(1-minScale/(maxScale + minScale)) * maxHeight;
	double right = sqrt(1-maxScale/(maxScale + minScale)) * maxHeight;

	if ( currentHeight <= left)
	{
		scale = maxScale;
	}
	else if (currentHeight >= right)
	{
		scale = minScale;
	}
	else
	{
		scale = (1 - pow((currentHeight/maxHeight), 2))*maxScale;
	}

	return scale;
}
