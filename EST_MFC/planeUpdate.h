#pragma once
#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate(void);
	~planeUpdate(void);

	// 更新位置
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

	// 设置是否更新
	void setUpdate( bool bl );

	// 设置位置
	void setPos( std::vector<osg::Vec3d> vPos );

	// 设置轴向
	void setAxis( osg::Vec3d axis );

	// 设置方向
	void setDir( std::vector<osg::Vec3d> vDir );

	// 设置放大缩小比例
	void setScale( double scale );

	// 设置角度
	void setAngle( double a );

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
	double _angle;

};

