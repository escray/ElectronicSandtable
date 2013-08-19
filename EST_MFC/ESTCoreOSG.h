#pragma once
#include <string>

#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/Win32/GraphicsWindowWin32>

#include "ESTManipulator.h"
#include "SouthManipulator.h"
#include "BHManipulator.h"

// OSG �����࣬������ִ�� MFC �� OSG �Ĺ�����
class ESTCoreOSG
{
public:
	// ���캯������ȡ��һ�����ھ��
	ESTCoreOSG(HWND hWnd);
	~ESTCoreOSG(void);

	// ��ʼ�� OSG
	// filename: ģ���ļ�����
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

	ESTManipulator* estManipulator;
	BHManipulator* bhManipulator;
	SouthManipulator* southManipulator;


};

