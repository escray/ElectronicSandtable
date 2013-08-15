#pragma once
#include <osg/Drawable>


class ribbonUpdate :
	public osg::Drawable::UpdateCallback
{
public:
	ribbonUpdate(void);
	~ribbonUpdate(void);

public:
	bool _updatePos;
	int _posIndex;
	std::vector<osg::Vec3d> _vPos;
	osg::Vec4d _color;
	bool _blNp;
	double _a;
	//osg::ref_ptr<bhManipulator> _emp;
};

