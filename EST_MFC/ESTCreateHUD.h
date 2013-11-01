#pragma once
#include <osgText/Text>
#include <osg/geode>
#include <osg/GraphicsContext>
#include <osg/Camera>
#include <osg/StateSet>
#include <osg/CameraNode>

class ESTCreateHUD
{
public:
	ESTCreateHUD(void);
	~ESTCreateHUD(void);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits;

	osg::Node* createTitleHUD();

	osg::CameraNode* initialCamera();

	osg::Node* createPositionHUD(osgText::Text* updateText);
};

