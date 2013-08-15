// planeRoute.h

#pragma once
#include <vector>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>

class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate(void);
	~planeUpdate(void);

	// 跟新飞机位置
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

	void setUpdate(bool bUpdate);
	void setPos(std::vector<osg::Vec3d> vPos);
	void setDir(std::vector<osg::Vec3d> vDir);
	void setAxis(osg::Vec3d axis);
	void setScale(double scale);
	void setA(double a);
private:
	// 是否移动/更新飞机位置
	bool _updatePos;
	// 飞机在某一时刻的位置
	int _posIndex;
	// 位置
	std::vector<osg::Vec3d> _vPos;
	// 方向
	std::vector<osg::Vec3d> _vDir;
	// 轴向（从建模模型获取）
	osg::Vec3d _axis;
	// 缩放比例
	double _scale;
	// 角度
	double _angle;


};

