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

	//����λ��,��������˸��±�־
	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
	{
		if(!_updatePos)return;

		osg::MatrixTransform* mt =dynamic_cast<osg::MatrixTransform*>( node );
			
		osg::EllipsoidModel elm;
		elm.setRadiusEquator(6378137);
		elm.setRadiusPolar(6378137);

		double b,l,h;
		//��������
		elm.convertXYZToLatLongHeight(_vPos[_posIndex].x(),_vPos[_posIndex].y(),_vPos[_posIndex].z(),b,l,h);
		osg::Vec3d n(_vPos[_posIndex].x(),_vPos[_posIndex].y(),_vPos[_posIndex].z());
		n.normalize();

		osg::Matrixd md;
		elm.computeLocalToWorldTransformFromLatLongHeight(b,l,h,md );
		//���ŷɻ�
		//���÷ɻ�,�ȿ���,�ٰ���
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
	//�Ƿ��ƶ����·ɻ�λ��
	bool _updatePos;
	//ĳ��ʱ��,�ɻ�λ��
	int _posIndex;
	//λ����Ϣ
	std::vector<osg::Vec3d>_vPos;
	//������Ϣ
	std::vector<osg::Vec3d>_vDir;
	//�ɻ���,�ɻ�������Ϣ,����ӽ�ģģ�������ȡ
	osg::Vec3d _axis;
	//���ű���ϵ��,���ڿ���֮��,ʹ�ɻ��ɼ�
	double _scale;
	double _a;
};