#include "StdAfx.h"
#include "ribbonUpdate.h"



ribbonUpdate::ribbonUpdate(void)
{
	_updatePos = true;
	_posIndex = 0;
	// 飘带颜色
	_color = osg::Vec4d(1.0, 0.0, 0.0, 0.5);
	_blNp = false;
}


ribbonUpdate::~ribbonUpdate(void)
{
}

void ribbonUpdate::update( osg::NodeVisitor* nv, osg::Drawable* dr )
{
	if (!_updatePos)
	{
		return;
	}

	osg::Geometry *gm = dynamic_cast<osg::Geometry*>(dr);
	osg::Vec3Array *v3a = dynamic_cast<osg::Vec3Array*>(gm->getVertexArray());

	// zhaorui 2013-11-19
	// 这一段代码重复了好几次，应该想办法替换掉
	osg::EllipsoidModel elm;
	elm.setRadiusEquator(6378137);
	elm.setRadiusPolar(6378137);

	// 飞机位置
	double b, l, h;
	// 将数字坐标转换为LatLongHeight
	elm.convertXYZToLatLongHeight(_vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z(), b, l, h);
	osg::Vec3d n(_vPos[_posIndex].x(), _vPos[_posIndex].y(), _vPos[_posIndex].z());
	n.normalize();

	// 飞机安置变换
	osg::Matrixd md;
	elm.computeLocalToWorldTransformFromLatLongHeight(b, l, h, md);
	md = md * osg::Matrixd::rotate(_a, n);

	// 如果是第一个飞机的位置，清零
	if ( _posIndex == 0 )
	{
		v3a->clear();
	}

	osg::Vec3d oPos;

	// 尾部安置点
	if ( _blNp )
	{
		oPos = osg::Vec3d(0.0, 4898.6, 550.4) * md;
	}
	else
	{
		oPos = osg::Vec3d(0.0, -4898.6, 550.4 ) * md;
	}

	v3a->push_back(osg::Vec3d(oPos));
	
	osg::ref_ptr<osg::DrawArrays> tileNode = new osg::DrawArrays(osg::DrawArrays::LINE_STRIP, 0, v3a->size());
	gm->removePrimitiveSet(0);
	gm->addPrimitiveSet(tileNode.get());

	v3a->dirty();
	gm->dirtyBound();

	_updatePos = false;
	_posIndex++;
	_posIndex %= _vPos.size();



}

void ribbonUpdate::setUpdate( bool positionUpdate )
{
	_updatePos = positionUpdate;
}

void ribbonUpdate::setPos( std::vector<osg::Vec3d> vPos )
{
	_vPos = vPos;
}

void ribbonUpdate::setNp( bool bl )
{
	_blNp = bl;
}

// 设置角度
void ribbonUpdate::setA( double a )
{
	_a = a;
}

void ribbonUpdate::setEmp( ESTManipulator* emp )
{
	m_estmanipulator = emp;
}
