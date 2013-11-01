#pragma once
#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate(void);
	~planeUpdate(void);

	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

	void setUpdate( bool bl );

	void setPos( std::vector<osg::Vec3d> vPos )
	{
		_vPos = vPos;
	}

	void setAxis( osg::Vec3d axis )
	{
		_axis = axis;
	}

	void setDir( std::vector<osg::Vec3d> vDir )
	{
		_vDir = vDir;
	}

	void setScale( double scale )
	{
		_scale = scale;
	}

	void setA( double a )
	{
		_a = a;
	}

private:
	// 是否移动更新飞机位置
	bool _updatePos;
	// 某个时刻飞机的位置
	int _posIndex;
	// 位置信息
	std::vector<osg::Vec3d> _vPos;
	// 方向信息
	std::vector<osg::Vec3d> _vDir;
	// 飞机轴向信息
	osg::Vec3d _axis;
	// 缩放比例系数，用于夸张，使飞机可见
	double _scale;
	// ?
	double _a;

};

