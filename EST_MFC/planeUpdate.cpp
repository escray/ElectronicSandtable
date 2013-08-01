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

// ���·ɻ�λ��
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
	// ��������
	em.convertXYZToLatLongHeight(_vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z(), b, l, h);

	osg::Vec3d n(_vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z());
	n.normalize();

	osg::Matrixd md;
	em.computeLocalToWorldTransformFromLatLongHeight(b, l, h, md);	

	// ���ŷɻ����ڸ߶ȱ仯��ʱ����Ҫ
	// ���÷ɻ����������ٰ���
	// TODO��1.0 Ӧ�ò��� _scale �滻
	md = osg::Matrixd::scale(1.0, 1.0, 1.0)*md*osg::Matrixd::rotate(_angle, n);

	mt->setMatrix(osg::Matrix::identity());
	mt->setMatrix(md);

	traverse(node, nv);

	_updatePos = false;
	_posIndex++;
	_posIndex %= _vPos.size();
}


