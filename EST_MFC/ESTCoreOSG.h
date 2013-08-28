#pragma once
#include <string>

#include <osg/CameraNode>

#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/KeySwitchMatrixManipulator>

#include <osgText/Text>

#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/Win32/GraphicsWindowWin32>

#include "ESTManipulator.h"
#include "SouthManipulator.h"
#include "BHManipulator.h"
#include "ESTCreateHUD.h"
#include "ESTPickHandler.h"

// OSG 核心类，在这里执行 MFC 对 OSG 的管理功能
class ESTCoreOSG
{
public:
	// 构造函数，获取上一级窗口句柄
	ESTCoreOSG(HWND hWnd);
	~ESTCoreOSG(void);

	// 初始化 OSG
	// filename: 模型文件名称
	void InitOSG(std::string filename);

	void InitManipulators(void);

	void InitSceneGraph(void);	

	void InitCameraConfig(void);

	void SetupWindow(void);

	void SetupCamera(void);

	void PreFrameUpdate(void);

	void PostFrameUpdate(void);

	void setDone(bool value);

	void getDone(void);

	static void Render(void* ptr);

	void ReplaceSceneData(std::string filename);

	//PickHandler* getEditPath();

	osgViewer::Viewer* getViewer();

	// osg::Node* CreateLight(osg::StateSet* rootStateSet);

	void OptimizeModel();
	//osg::Node* createHUD( void );
private:

	bool m_done;
	std::string m_ModelName;
	HWND m_hWnd;
	osgViewer::Viewer* m_viewer;
	osg::ref_ptr<osg::Group> m_root;
	osg::ref_ptr<osg::Node> m_node;
	osg::ref_ptr<osgGA::TrackballManipulator> trackball;
	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;
	osg::ref_ptr<osgGA::CameraManipulator> cameraManipulator;

	osg::ref_ptr<ESTManipulator> estManipulator;
	osg::ref_ptr<BHManipulator> bhManipulator;
	SouthManipulator* southManipulator;


};

