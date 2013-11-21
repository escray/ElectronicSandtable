#include "StdAfx.h"
#include "ESTCreateHUD.h"


ESTCreateHUD::ESTCreateHUD(void)
{
}


ESTCreateHUD::~ESTCreateHUD(void)
{
}

osg::Node* ESTCreateHUD::createTitleHUD()
{
	osg::Geode* geode = new osg::Geode;

	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	std::string mfont("STCAIYUN.TTF");
	osg::Vec3 position(50.0f, 720.0f, 0.0f);

	osgText::Text* text = new osgText::Text;
	geode->addDrawable(text);	

	text->setFont(mfont);
	text->setPosition(position);
	text->setText(L"军用电子沙盘系统");

	osg::Camera* camera = initialCamera();
	camera->addChild(geode);

	return camera;
}

// TODO: 视点位置正确，中心位置和Up位置不正确
osg::Node* ESTCreateHUD::createPositionHUD( osgText::Text* updateText )
{
	osg::Camera* camera = initialCamera();

	std::string timesFont("cour.ttf");
	// 下面字幕的位置
	osg::Vec3 position(10.0, 80.0, 0.0);
	osg::Geode* geode = new osg::Geode;
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->setName("The text label");
	geode->addDrawable(updateText);

	camera->addChild(geode);

	updateText->setCharacterSize(20.0f);
	updateText->setFont(timesFont);
	updateText->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	updateText->setText("");
	updateText->setPosition(position);
	updateText->setDataVariance(osg::Object::DYNAMIC);

	return camera;
}

osg::CameraNode* ESTCreateHUD::initialCamera()
{
	osg::Camera* camera = new osg::CameraNode;
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1024, 0, 768));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::CameraNode::POST_RENDER);
	return camera;
}
