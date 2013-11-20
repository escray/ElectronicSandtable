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

void planeUpdate::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	if ( !_updatePos )
	{
		return;
	}

	osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>( node );
	osg::EllipsoidModel elm;
	elm.setRadiusEquator( 6378137 );
	elm.setRadiusPolar( 6378137 );


	double b, l, h;
	// ��������
	elm.convertXYZToLatLongHeight( _vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z(), b, l, h );
	osg::Vec3d n( _vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z() );
	n.normalize();

	osg::Matrixd md;
	elm.computeLocalToWorldTransformFromLatLongHeight( b, l, h, md );
	// ���ŷɻ�
	// ���÷ɻ����ȿ��ţ��ٰ���
	// TODO: �ɻ������ӵ�λ�÷Ŵ���С
	md = osg::Matrixd::scale( 1.0, 1.0, 1.0 ) * md * osg::Matrixd::rotate( _angle, n );

	mt->setMatrix( osg::Matrix::identity() );
	mt->setMatrix( md );

	traverse( node, nv );

	_updatePos = false;
	_posIndex++;
	_posIndex %= _vPos.size();
}

void planeUpdate::setUpdate( bool bl )
{
	_updatePos = bl;
}

void planeUpdate::setPos( std::vector<osg::Vec3d> vPos )
{
	_vPos = vPos;
}

void planeUpdate::setAxis( osg::Vec3d axis )
{
	_axis = axis;
}

void planeUpdate::setDir( std::vector<osg::Vec3d> vDir )
{
	_vDir = vDir;
}

void planeUpdate::setScale( double scale )
{
	_scale = scale;
}

void planeUpdate::setAngle( double angle )
{
	_angle = angle;
}
