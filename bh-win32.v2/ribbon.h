//these demo codes are designed,coded and tested  
//by jack 20130729
//QQ:771702827 

#include "Manipulator.h"

#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/SmoothingVisitor>



//飘带绘制渲染
class ribbonUpdate : public osg::Drawable::UpdateCallback
{
public:
	ribbonUpdate()
	{
		_updatePos=true;
		_posIndex=0;
		_color=osg::Vec4d(1.0,0.0,0.0,0.5);
		_blNp=false;
	}

	//更新位置,如果设置了更新标志
	virtual void update( osg::NodeVisitor* nv, osg::Drawable* dr )
	{
		if(!_updatePos)return;

		osg::Geometry *gm=dynamic_cast<osg::Geometry *>(dr);
		osg::Vec3Array *v3a=dynamic_cast<osg::Vec3Array *>(gm->getVertexArray());
		
		osg::EllipsoidModel elm;
		elm.setRadiusEquator(6378137);
		elm.setRadiusPolar(6378137);

		//飞机位置
		double b,l,h;
		elm.convertXYZToLatLongHeight(_vPos[_posIndex].x(),_vPos[_posIndex].y(),_vPos[_posIndex].z(),b,l,h);
		osg::Vec3d n(_vPos[_posIndex].x(),_vPos[_posIndex].y(),_vPos[_posIndex].z());
		n.normalize();
		//飞机安置变换
		osg::Matrixd md;
		elm.computeLocalToWorldTransformFromLatLongHeight(b,l,h,md );
		md=md*osg::Matrixd::rotate(_a,n);

		//如果是第一个飞机位置,那么首先清0
		if(_posIndex==0)v3a->clear();

		osg::Vec3d oPos;
		//尾部安置点
		if(_blNp)oPos=osg::Vec3d(0.0,4898.6,550.4)*md;
		else oPos=osg::Vec3d(0.0,-4898.6,550.4)*md;
	
		v3a->push_back(osg::Vec3d(oPos));

		osg::ref_ptr<osg::DrawArrays>tileNode=new osg::DrawArrays(osg::DrawArrays::LINE_STRIP,0,v3a->size());
		gm->removePrimitiveSet(0);
		gm->addPrimitiveSet(tileNode.get());

		v3a->dirty();
		gm->dirtyBound();

		_updatePos=false;
		_posIndex++;
		_posIndex%=_vPos.size();

	}
	
	void setUpdate(bool bl)
	{
		_updatePos=bl;
	}

	void setPos(std::vector<osg::Vec3d>vPos)
	{
		_vPos=vPos;
	}
	//控制点的顺序方式
	void setNp(bool bl){_blNp=bl;}	
	void setA(double a){_a=a;}
	void setEmp(estreetGlobeManipulator * emp){_emp=emp;}

//private:
public:
	//是否移动更新飞机位置,飘带是需要随飞机位置修正的
	bool _updatePos;
	//某个时刻,飞机位置
	int _posIndex;
	//位置信息,这里采用飞机相关的位置信息
	std::vector<osg::Vec3d>_vPos;
	osg::Vec4d _color;
	bool _blNp;
	double _a;
	osg::ref_ptr<estreetGlobeManipulator> _emp;

	
};