//these demo codes are designed,coded and tested  
//by jack 20130729
//QQ:771702827 

#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>


class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate()
	{
		_updatePos=true;
		_posIndex=0;
		_scale=1.0;
	}

	//更新位置,如果设置了更新标志
	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
	{
		if(!_updatePos)return;

		osg::MatrixTransform* mt =dynamic_cast<osg::MatrixTransform*>( node );
			
		osg::EllipsoidModel elm;
		elm.setRadiusEquator(6378137);
		elm.setRadiusPolar(6378137);

		double b,l,h;
		//法线轴线
		elm.convertXYZToLatLongHeight(_vPos[_posIndex].x(),_vPos[_posIndex].y(),_vPos[_posIndex].z(),b,l,h);
		osg::Vec3d n(_vPos[_posIndex].x(),_vPos[_posIndex].y(),_vPos[_posIndex].z());
		n.normalize();

		osg::Matrixd md;
		elm.computeLocalToWorldTransformFromLatLongHeight(b,l,h,md );
		//夸张飞机
		//安置飞机,先夸张,再安置
		md=osg::Matrixd::scale(1.0,1.0,1.0)*md*osg::Matrixd::rotate(_a,n);

		mt->setMatrix(osg::Matrix::identity());
		mt->setMatrix(md);

		traverse( node, nv );		
		
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

	void setAxis(osg::Vec3d axis)
	{
		_axis=axis;
	}

	void setDir(std::vector<osg::Vec3d>vDir)
	{
		_vDir=vDir;
	}
	void setScale(double scale)
	{
		_scale=scale;
	}
	void setA(double a){_a=a;}
private:
	//是否移动更新飞机位置
	bool _updatePos;
	//某个时刻,飞机位置
	int _posIndex;
	//位置信息
	std::vector<osg::Vec3d>_vPos;
	//方向信息
	std::vector<osg::Vec3d>_vDir;
	//飞机轴,飞机轴向信息,必须从建模模型那里获取
	osg::Vec3d _axis;
	//缩放比例系数,用于夸张之类,使飞机可见
	double _scale;
	double _a;
};