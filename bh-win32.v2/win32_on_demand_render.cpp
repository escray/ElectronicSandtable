
//these demo codes are designed,coded and tested  
//by jack 20130729
//QQ:771702827 

#include "Manipulator.h"
#include "planeRoute.h"
#include "ribbon.h"

#include <windows.h>
#include <process.h>

#include <osgViewer/Viewer>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/StateSetManipulator>
#include <osg/MatrixTransform>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/AutoTransform>
#include <osg/StateSet>
#include <vector>
#include <osgDB/Registry>
#include <osg/BlendFunc>
#include <osg/LineWidth>



osgViewer::Viewer *g_viewer=new osgViewer::Viewer();;
bool g_finished;
HANDLE mThreadHandle ;
HWND hwnd;
osg::ref_ptr<planeUpdate >planeCb1=new planeUpdate;
osg::ref_ptr<planeUpdate >planeCb2=new planeUpdate;
osg::ref_ptr<planeUpdate >planeCb3=new planeUpdate;
osg::ref_ptr<planeUpdate >planeCb4=new planeUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb1=new ribbonUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb2=new ribbonUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb3=new ribbonUpdate;
osg::ref_ptr<ribbonUpdate>ribbonCb4=new ribbonUpdate;

//依据观察点,航迹点,生成飘带
//观察点是动态的
//动态的处理,交CALLBACK
osg::Geometry* createRibbonNode()
{	
	osg::ref_ptr<osg::Geometry> gm = new osg::Geometry();
 
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(); 
	vertices->push_back(osg::Vec3(0.0,0.0,0.0));
	vertices->push_back(osg::Vec3(0.0,0.0,0.0));
	gm->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
	color->push_back(osg::Vec4(0.8,0.8,0.0,0.5));

	gm->setColorArray(color.get());
	gm->setColorBinding(osg::Geometry::BIND_OVERALL);  
	
	osg::ref_ptr<osg::DrawArrays>tileNode=new osg::DrawArrays(osg::DrawArrays::LINE_STRIP,0,vertices->size());
	gm->addPrimitiveSet(tileNode.get());
	gm->setUseDisplayList(false);
	gm->setUseVertexBufferObjects(true);
	
	gm->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA),osg::StateAttribute::ON);
	gm->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);	
				
	osg::ref_ptr<osg::LineWidth>lw=new osg::LineWidth;
	lw->setWidth(6);
	gm->getOrCreateStateSet()->setAttribute(lw);
				
	gm->setDataVariance(osg::Object::DYNAMIC);
    gm->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   // gm->getOrCreateStateSet()->setMode(GL_COLOR_MATERIAL,osg::StateAttribute::OFF);
	gm->getOrCreateStateSet()->setRenderBinDetails(999,"RenderBin");
	gm->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);

	return gm.release();
}

void render( void* )
{
	g_viewer->realize();
	
	while (!g_viewer->done())
	{
		long k=g_viewer->getFrameStamp()->getFrameNumber();
		//每10帧更新一次位置
		if(k%10==0)
		{
			planeCb1->setUpdate(true);
			planeCb2->setUpdate(true);
			planeCb3->setUpdate(true);
			planeCb4->setUpdate(true);
			
			ribbonCb1->setUpdate(true);
			ribbonCb2->setUpdate(true);
			ribbonCb3->setUpdate(true);
			ribbonCb4->setUpdate(true);
		}

		g_viewer->frame();
	}

    g_finished = true;
	g_viewer->setDone(true);
	//如果窗口进行了隐藏,那么在运行一个固定的时间后,要发送窗口结束消息,以结束线程
	SendMessage(hwnd,WM_DESTROY,0,0);
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch ( message )
    {
        case WM_CREATE:
        {            	
			osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(hwnd);
   
			osg::GraphicsContext::Traits *traits = new osg::GraphicsContext::Traits;
            traits->x = 0;
            traits->y = 0;
            traits->width = 1000;
            traits->height = 800;
            traits->windowDecoration = false;
            traits->doubleBuffer = true;
            traits->samples=3.0;

			osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits);
         
            osg::ref_ptr<osg::Camera> camera = new osg::Camera;
            camera->setGraphicsContext( gc );
            camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
			g_viewer->setCamera( camera.get() );
			g_viewer->getCamera()->setProjectionMatrixAsPerspective(23.0f,(double)traits->width/(double)traits->height,.1, 6378137*6);
			g_viewer->getCamera()->setNearFarRatio(0.00001);

			//g_viewer->setSceneData( osgDB::readNodeFile("cessna.osg") );
            g_viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );
		
			g_viewer->setRunFrameScheme(osgViewer::Viewer::ON_DEMAND);
				
			osg::Group *root= new osg::Group;
			root->addChild(osgDB::readNodeFile("E:\\BH\\bh_13.ive"));
			//root->addChild(osgDB::readNodeFile("e:\\pldTest.ive"));
		
			osg::ref_ptr<osg::Node>nd=osgDB::readNodeFile("f:\\plane.ive");

			//4架飞机
			osg::ref_ptr<osg::MatrixTransform> mt1=new osg::MatrixTransform;
			osg::ref_ptr<osg::MatrixTransform> mt2=new osg::MatrixTransform;
			osg::ref_ptr<osg::MatrixTransform> mt3=new osg::MatrixTransform;
			osg::ref_ptr<osg::MatrixTransform> mt4=new osg::MatrixTransform;
			mt1->addChild(nd);
			mt2->addChild(nd);
			mt3->addChild(nd);
			mt4->addChild(nd);
		
			root->addChild(mt1);
			root->addChild(mt2);
			root->addChild(mt3);
			root->addChild(mt4);
						
			osg::ref_ptr<estreetGlobeManipulator>manip=new estreetGlobeManipulator();	
			manip->setViewer(g_viewer);
			g_viewer->setCameraManipulator( manip.get() );
	
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
				planeCb1->setA(osg::PI);
				
				mt1->setUpdateCallback(planeCb1);

				ribbonCb1->setPos(vPos);
				ribbonCb1->setNp(true);				
				ribbonCb1->setA(osg::PI);
				ribbonCb1->setEmp(manip);

				osg::ref_ptr<osg::Geometry>gm=createRibbonNode();	
				osg::ref_ptr<osg::Geode>ge=new osg::Geode();
				ge->addDrawable(gm.get());
				gm->setUpdateCallback(ribbonCb1);	
				gm->setDataVariance(osg::Object::DYNAMIC);

				root->addChild(ge.get());

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
				planeCb2->setA(osg::PI);

				mt2->setUpdateCallback(planeCb2);
				
				ribbonCb2->setPos(vPos);				
				ribbonCb2->setNp(true);				
				ribbonCb2->setA(osg::PI);
				ribbonCb2->setEmp(manip);

				osg::ref_ptr<osg::Geometry>gm=createRibbonNode();	
				osg::ref_ptr<osg::Geode>ge=new osg::Geode();
				ge->addDrawable(gm.get());
				gm->setUpdateCallback(ribbonCb2);				
				gm->setDataVariance(osg::Object::DYNAMIC);

				root->addChild(ge.get());
			}

			{
				elm.convertLatLongHeightToXYZ(39.5*osg::PI/180.0,116.4*osg::PI/180.0,5000,x,y,z);
				double x1,y1,z1;
				elm.convertLatLongHeightToXYZ((39.5-1.0/3600.0)*osg::PI/180.0,116.4*osg::PI/180.0,5000,x1,y1,z1);
				osg::Vec3d v1=osg::Vec3d(x1,y1,z1)-osg::Vec3d(x,y,z);
				//飞机轴线位置
				osg::Vec3d v0=osg::Vec3d(-8.4,32.3-4898.6,476.4-550.4);
				
				mt3->setMatrix(osg::Matrix::rotate(v0,v1)*
					osg::Matrix::translate(x,y,z));
					
				planeCb3->setAxis(v0);

				std::vector<osg::Vec3d>vDir,vPos;
				vDir.push_back(v1);			
				vPos.push_back(osg::Vec3d(x,y,z));

				double x2,y2,z2;
				elm.convertLatLongHeightToXYZ(39.0*osg::PI/180.0,116.4*osg::PI/180.0,5000,x2,y2,z2);
				
				for(int i=0;i<100;i++)
				{
					double xt,yt,zt;
					xt=x+(x2-x)/100.0*i;
					yt=y+(y2-y)/100.0*i;
					zt=z+(z2-z)/100.0*i;

					vPos.push_back(osg::Vec3d(xt,yt,zt));
					vDir.push_back(v1);

				}
				planeCb3->setPos(vPos);
				planeCb3->setDir(vDir);
				planeCb3->setA(0.0);

				mt3->setUpdateCallback(planeCb3);
				
				ribbonCb3->setPos(vPos);				
				ribbonCb3->setNp(false);				
				ribbonCb3->setA(osg::PI);
				ribbonCb3->setEmp(manip);

				osg::ref_ptr<osg::Geometry>gm=createRibbonNode();	
				osg::ref_ptr<osg::Geode>ge=new osg::Geode();
				ge->addDrawable(gm.get());
				gm->setUpdateCallback(ribbonCb3);				
				gm->setDataVariance(osg::Object::DYNAMIC);

				root->addChild(ge.get());

			}

			{
				elm.convertLatLongHeightToXYZ(41.5*osg::PI/180.0,116.5*osg::PI/180.0,5000,x,y,z);
				
				double x1,y1,z1;
				elm.convertLatLongHeightToXYZ((41.5-1.0/3600.0)*osg::PI/180.0,116.5*osg::PI/180.0,5000,x1,y1,z1);
				osg::Vec3d v1=osg::Vec3d(x1,y1,z1)-osg::Vec3d(x,y,z);
				//飞机轴线位置
				osg::Vec3d v0=osg::Vec3d(-8.4,32.3-4898.6,476.4-550.4);
				
				mt4->setMatrix(osg::Matrix::rotate(v0,v1)*
					osg::Matrix::translate(x,y,z));
					
				planeCb4->setAxis(v0);

				std::vector<osg::Vec3d>vDir,vPos;
				vDir.push_back(v1);			

				vPos.push_back(osg::Vec3d(x,y,z));
				double x2,y2,z2;
				elm.convertLatLongHeightToXYZ(39.0*osg::PI/180.0,116.5*osg::PI/180.0,5000,x2,y2,z2);
				
				for(int i=0;i<100;i++)
				{
					double xt,yt,zt;
					xt=x+(x2-x)/100.0*i;
					yt=y+(y2-y)/100.0*i;
					zt=z+(z2-z)/100.0*i;

					vPos.push_back(osg::Vec3d(xt,yt,zt));
					vDir.push_back(v1);

				}
				planeCb4->setPos(vPos);
				planeCb4->setDir(vDir);				
				planeCb4->setA(0.0);


				mt4->setUpdateCallback(planeCb4);
				
				ribbonCb4->setPos(vPos);				
				ribbonCb4->setNp(false);				
				ribbonCb4->setA(osg::PI);
				ribbonCb4->setEmp(manip);

				osg::ref_ptr<osg::Geometry>gm=createRibbonNode();	
				osg::ref_ptr<osg::Geode>ge=new osg::Geode();
				ge->addDrawable(gm.get());
				gm->setUpdateCallback(ribbonCb4);				
				gm->setDataVariance(osg::Object::DYNAMIC);

				root->addChild(ge.get());

			}

			g_viewer->setSceneData(root);
			g_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
			g_viewer->getCamera()->setCullingMode(g_viewer->getCamera()->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING );


            g_finished = false;
            mThreadHandle = (HANDLE)_beginthread( render, 0, NULL );
            return 0;
        }

        case WM_DESTROY:  
			    	
			g_viewer->setRunFrameScheme(osgViewer::ViewerBase::CONTINUOUS);
			   
			g_viewer->stopThreading();
			g_viewer->setDone( true );
			while(!g_finished)Sleep(10);
			delete g_viewer;

            PostQuitMessage( 0 );
            return 0;
        default:
            break;
    }
    return DefWindowProc( hwnd, message, wParam, lParam );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
    static TCHAR szAppName[] = TEXT("gui");
    
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;
    if ( !RegisterClass(&wndclass) )
        return 0;
    
    //HWND 
	hwnd = CreateWindow( szAppName, // window class name
                              TEXT ("OSG and Win32 API Window"), // window caption
                              WS_OVERLAPPEDWINDOW, // window style
                              100, // initial x position
                              100, // initial y position
                              800, // initial x size
                              600, // initial y size
                              NULL, // parent window handle
                              NULL, // window menu handle
                              hInstance, // program instance handle
                              NULL ); // creation parameters
   // ShowWindow( hwnd, iCmdShow );
    ShowWindow( hwnd, SW_HIDE );

    UpdateWindow( hwnd );
    
	////
    MSG msg;
	//if set hwnd, then threads contniue to run when exit.
    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return (int)msg.wParam;
}
