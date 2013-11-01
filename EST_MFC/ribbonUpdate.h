#pragma once
#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/SmoothingVisitor>

class ribbonUpdate :
	public osg::Drawable::UpdateCallback
{
public:
	ribbonUpdate(void);
	~ribbonUpdate(void);
};

