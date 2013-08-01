#include "StdAfx.h"
#include "planeUpdate.h"


planeUpdate::planeUpdate(void)
{
	_updatePos = true;
	_posIndex = 0;
	_scale = 1.0;
}


planeUpdate::~planeUpdate(void)
{
}

void planeUpdate::setUpdate( bool bUpdate )
{
	_updatePos = true;
}
void planeUpdate::setPos( std::vector<osg::Vec3d> vPos )
{
	_vPos = vPos;
}

void planeUpdate::setDir( std::vector<osg::Vec3d> vDir )
{
	_vDir = vDir;
}

void planeUpdate::setAxis( osg::Vec3d axis )
{
	_axis = axis;
}

void planeUpdate::setScale( double scale )
{
	_scale = scale;
}

void planeUpdate::setA( double a )
{
	_angle = a;
}

// 更新飞机位置
void planeUpdate::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	if (!_updatePos)
	{
		return;
	}

	osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);

	osg::EllipsoidModel em;
	// question: why 6378137
	em.setRadiusEquator(6378137);
	em.setRadiusPolar(6378137);

	double b, l, h;
	// 法线轴线
	em.convertXYZToLatLongHeight(_vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z(), b, l, h);

	osg::Vec3d n(_vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z());
	n.normalize();

	osg::Matrixd md;
	em.computeLocalToWorldTransformFromLatLongHeight(b, l, h, md);	

	// 缩放飞机，在高度变化的时候需要
	// 放置飞机，先缩放再安放
	// TODO：1.0 应该采用 _scale 替换
	md = osg::Matrixd::scale(1.0, 1.0, 1.0)*md*osg::Matrixd::rotate(_angle, n);

	mt->setMatrix(osg::Matrix::identity());
	mt->setMatrix(md);

	traverse(node, nv);

	_updatePos = false;
	_posIndex++;
	_posIndex %= _vPos.size();
}


