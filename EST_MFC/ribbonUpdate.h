#pragma once
#include "BHManipulator.h"

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

	// 更新位置，如果有更新标记
	virtual void update(osg::NodeVisitor* nv, osg::Drawable* dr);

	void setUpdate(bool positionUpdate);

	void setPos(std::vector<osg::Vec3d> vPos);
	// 控制点的顺序方式
	void setNp(bool bl);
	void setA(double a);
	// zhaorui 2013-11-19
	// 这里应该可以将 BHmanipulator 进一步抽象为 osgGA::CameraManipulator
	void setEmp(BHManipulator* emp);
	

public:
	// 是否更新飞机位置，飘带需要飞机位置修正
	bool _updatePos;
	// 某个时刻飞机的位置
	int _posIndex;
	// 飞机相关位置
	std::vector<osg::Vec3d> _vPos;
	// 飘带的颜色
	osg::Vec4d _color;
	bool _blNp;
	double _a;
	// 漫游操作器
	osg::ref_ptr<BHManipulator> _emp;
};

