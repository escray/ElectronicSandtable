#pragma once
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/AnimationPath>
#include <osg/io_utils>

#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>


#include <osgText/Text>
#include <osgViewer/Viewer>

#include <sstream>



class ESTPickHandler : public osgGA::GUIEventHandler
{
public:
	ESTPickHandler(void);
	ESTPickHandler(osgText::Text* updateText);
	~ESTPickHandler(void);

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void frame(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea);
	virtual void pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea);

	osg::Group* DrawFirstNode(osg::Vec3f position);
	osg::Node* DrawCtrlPoints(osg::Vec3f position);

	void setPick(bool pick);
	bool getPick();

	
	double GetAllDistance();
	float GetRunTime( osg::Vec3 res, osg::Vec3 des );

	void setSecondPerMeter( double second );
	double getSecondsPerMeter();

	osg::AnimationPath* CreatePath(std::string pathName);

	void setLabel(const std::string& name);

	// ���߲��ֿ���Ҫ���� ESTCoreOSG.cpp ��
	osg::Node* CreateCardinal();

	void Cardinal( osg::ref_ptr<osg::Vec3Array> temp );

	float GetCoefficient( float p0, float p1, float p2, float p3, float t );

	// �������й켣
	void pushCardinal();

	osg::Group* getCtrlPointGroup();
	void setUpdateText( osgText::Text* updateText );

protected:
	osg::Vec3 position;
	osg::Vec3 center;
	osg::Vec3 up;
	osg::ref_ptr<osgText::Text> m_updateText;

	osgViewer::Viewer* m_viewer;

	// height of the point
	int m_height;

	// size of the point
	int m_size;

	// �Ƿ�ѡ���
	bool m_bPick;
	// �Ƿ��һ����
	bool m_bFirstPush;
	// ��ѡ�ĵ㼯��
	osg::ref_ptr<osg::Vec3Array> point;
	// ���Ƶ�
	osg::ref_ptr<osg::Group> ctrlPoints;

	int size;
	double sec;

};

