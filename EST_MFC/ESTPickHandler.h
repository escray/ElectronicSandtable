#pragma once

#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>


#include <osgText/Text>
#include <osgViewer/Viewer>

#include <sstream>



class ESTPickHandler : public osgGA::GUIEventHandler
{
public:
	ESTPickHandler(osgText::Text* updateText);
	~ESTPickHandler(void);

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual void pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea);

	void setLabel(const std::string& name);

protected:
	osg::Vec3 position;
	osg::Vec3 center;
	osg::Vec3 up;
	osg::ref_ptr<osgText::Text> m_updateText;
};

