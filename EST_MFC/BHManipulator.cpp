#include "StdAfx.h"
#include "BHManipulator.h"

using namespace std;
using namespace osg;
using namespace osgGA;

BHManipulator::BHManipulator(void)
{
	_ellipsoid.setRadiusEquator(6378137);
	_ellipsoid.setRadiusPolar(6378137);


	setAutoComputeHomePosition(false);

	_vNe.set(-999,-999,-999.0);

	_blSaved=false;
}


BHManipulator::~BHManipulator(void)
{
}

void BHManipulator::home(const GUIEventAdapter& ,GUIActionAdapter& us)
{
	resetPosition();
}

//��������ʼ����λ
void BHManipulator::resetPosition()
{
	double r=_ellipsoid.getRadiusEquator();

	setLookAtByBlh(osg::Vec3d(38.914702,116.391731,15000.0),osg::Vec3d(39.914702,116.391731,0.0),osg::Vec3d(-999,-999,-999));	
	osg::Vec3d eye,center,up;
	getLookAt(eye,center,up);
	setHomePosition(eye,center,up);

	_a0=0.0;

	double x,y,z;
	_ellipsoid.convertLatLongHeightToXYZ(39.914702*osg::PI/180.0,116.391731*osg::PI/180.0,0.0,x,y,z);
	_center.set(x,y,z);

}

//�¼��Ĵ���
bool BHManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
{		
	switch(ea.getEventType())
	{
	case(GUIEventAdapter::KEYDOWN):
		{		
			//����
			if (ea.getKey()=='R')
			{					
				if(_blSaved)return false;;

				double a0;
				_a0+=1.0;
				a0=1.0;

				osg::Vec3d axis(_center.x(),_center.y(),_center.z());
				axis.normalize();

				osg::Matrixd matrix=
					osg::Matrixd::translate(-_center)
					*osg::Matrixd::rotate(-a0*osg::PI/180.0,axis)
					*osg::Matrixd::translate(_center);
				_eye=_eye*matrix;				

				osg::Matrixd matrix2=
					osg::Matrixd::translate(-_center)
					*osg::Matrixd::rotate(-_a0*osg::PI/180.0,axis)
					*osg::Matrixd::translate(_center);

				osg::Vec3d vn=_vNe*matrix2;
				setLookAtByXyz(_eye,_center,vn);

				us.requestRedraw();
				us.requestContinuousUpdate(false);

				return false;
			} 

			//Χ�ƹ۲��,���͹۲�"�߶�",����ת����
			if (ea.getKey()=='D')
			{	
				double x,y,z,b,l,h;
				_ellipsoid.convertXYZToLatLongHeight(_eye.x(),_eye.y(),_eye.z(),b,l,h);
				h-=h*0.1;
				_ellipsoid.convertLatLongHeightToXYZ(b,l,h,x,y,z);
				_eye.set(x,y,z);					

				osg::Vec3d axis(_center.x(),_center.y(),_center.z());
				axis.normalize();

				osg::Matrixd matrix=
					osg::Matrixd::translate(-_center)
					*osg::Matrixd::rotate(-_a0*osg::PI/180.0,axis)
					*osg::Matrixd::translate(_center);				
				osg::Vec3d vn=_vNe*matrix;
				setLookAtByXyz(_eye,_center,vn);

				us.requestRedraw();
				us.requestContinuousUpdate(false);

				return false;
			}

			//Χ�ƹ۲��,�����۲�"�߶�",����ת����
			if (ea.getKey()=='I')
			{							
				double x,y,z,b,l,h;
				_ellipsoid.convertXYZToLatLongHeight(_eye.x(),_eye.y(),_eye.z(),b,l,h);
				h+=h*0.1;
				_ellipsoid.convertLatLongHeightToXYZ(b,l,h,x,y,z);
				_eye.set(x,y,z);		

				osg::Vec3d axis(_center.x(),_center.y(),_center.z());
				axis.normalize();

				osg::Matrixd matrix=
					osg::Matrixd::translate(-_center)
					*osg::Matrixd::rotate(-_a0*osg::PI/180.0,axis)
					*osg::Matrixd::translate(_center);				
				osg::Vec3d vn=_vNe*matrix;

				setLookAtByXyz(_eye,_center,vn);

				us.requestRedraw();
				us.requestContinuousUpdate(false);

				return false;
			} 

			//�����I/D����Ӧ�Ĳ���,�Ѿ������ϸ߶ȸı�,���������Ӹ߶ȸı�
			//�����ǽ���"���Ӹ߶�"����ʾ����,����������ʾ���� 
			//�ص��潵�͹۲�߶�,���ﲻ��ɳ����ʾ�Ĺ۲�ģʽ,ֻ����ʾ����
			//����֮ǰ,Ҫ������ǰ��"Χ�ƹ۲��,��תģʽ������,�Ա�ָ�,����ɳ�̲�����Э��

			//-1�������
			if (ea.getKey()=='X')
			{
				if(_blSaved)return false;

				_eyeSaved=_eye;
				_rotationSaved=_rotation;

				_blSaved=true;

				return false;
			}

			//-2���͸߶�
			if (ea.getKey()=='T')
			{
				//���û�н���״̬����,��ô�����и߶ȸı�,��Ҫ��Ϊ"ɳ�̻���"��г����
				if(!_blSaved)return false;;

				double x,y,z,b,l,h,hEye;
				_ellipsoid.convertXYZToLatLongHeight(_eye.x(),_eye.y(),_eye.z(),b,l,h);
				hEye=h;
				if(ea.getKey()=='t')hEye-=h/100.0;
				if(ea.getKey()=='T')hEye+=h/100.0;

				_ellipsoid.convertXYZToLatLongHeight(_center.x(),_center.y(),_center.z(),b,l,h);

				_ellipsoid.convertLatLongHeightToXYZ(b,l,hEye,x,y,z);
				_eye.set(x,y,z);		

				osg::Vec3d axis(_center.x(),_center.y(),_center.z());
				axis.normalize();

				osg::Matrixd matrix=
					osg::Matrixd::translate(-_center)
					*osg::Matrixd::rotate(-_a0*osg::PI/180.0,axis)
					*osg::Matrixd::translate(_center);				
				osg::Vec3d vn=_vNe*matrix;

				setLookAtByXyz(_eye,_center,vn);

				us.requestRedraw();
				us.requestContinuousUpdate(false);

				return false;

			}

			//-3�ָ�����
			if (ea.getKey()=='G')
			{
				if(!_blSaved)return false;

				_eye=_eyeSaved;
				_rotation=_rotationSaved;

				_blSaved=false;

				return false;

			}

		}  
	default:			
		return false;
	}
	return true;
}


//���ù۲�����������ɾ���ȷ����Ψһȷ���۾�λ�ü���ת 
void BHManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	osg::Vec3d lookVector(- matrix(2,0),-matrix(2,1),-matrix(2,2));
	osg::Vec3d eye(matrix(3,0),matrix(3,1),matrix(3,2));
	_eye=eye;

	_rotation=matrix.getRotate().inverse();

}

//�۲���ͼ���� ���۲�λ�á���ת����ȷ�� 
osg::Matrixd BHManipulator::getMatrix() const
{
	return osg::Matrixd::rotate(_rotation)
		*osg::Matrixd::translate(_eye);

}

osg::Matrixd BHManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate(-_eye)
		*osg::Matrixd::rotate(_rotation.inverse());
}

void BHManipulator::setLookAtByXyz(const osg::Vec3d eye,const osg::Vec3d center,const osg::Vec3d up)
{
	_eye=eye;

	osg::Matrixd matrix;
	matrix.makeLookAt(eye,center,up);

	_rotation=matrix.getRotate().inverse();
}

void BHManipulator::setLookAtByBlh(osg::Vec3d eyeBlh,osg::Vec3d centerBlh,osg::Vec3d up)
{ 	
	osg::Vec3d eyeXyz,centerXyz;
	double rb=osg::DegreesToRadians(eyeBlh.x()),rl=osg::DegreesToRadians(eyeBlh.y()),h=eyeBlh.z();
	_ellipsoid.convertLatLongHeightToXYZ(rb,rl,h,eyeXyz.x(),eyeXyz.y(),eyeXyz.z());

	rb=osg::DegreesToRadians(centerBlh.x());
	rl=osg::DegreesToRadians(centerBlh.y());
	//todo:
	_ellipsoid.convertLatLongHeightToXYZ(rb,rl,0,centerXyz.x(),centerXyz.y(),centerXyz.z());

	_eye=eyeXyz;
	osg::Vec3d lookAt=centerXyz-eyeXyz;
	lookAt.normalize();
	osg::Vec3d vNe=up,vLe,vUpe;
	osg::Vec3d center=_eye+lookAt;
	//todo:
	if(up.length()>=9.0)
		getLocalCoordFrame(center,0,vNe,vLe,vUpe);

	vNe.normalize();

	setLookAtByXyz(_eye,center,vNe);	
	if(_vNe.length()>9.0)_vNe=vNe;

}

//ground location's directionN,directionE,directionUp
//b->b+2sec
//l->l+2sec.
//vl^vn
//todo:azimuth?
void BHManipulator::getLocalCoordFrame(osg::Vec3d center,double azimuth,
	osg::Vec3d &vN,osg::Vec3d &vL,osg::Vec3d &vUp)
{
	//1.degree azimuth to radian. 
	azimuth=osg::DegreesToRadians(azimuth);

	//2.location's xyz to blh.
	double b,l,h;	
	_ellipsoid.convertXYZToLatLongHeight(center.x(),center.y(),center.z(),b,l,h);	

	//3.directions are get by increasing a small amount(2 second).
	double xb,yb,zb,xe,ye,ze;
	xb=center.x();
	yb=center.y();
	zb=center.z();
	//4.directionN:b->b+2sec.
	_ellipsoid.convertLatLongHeightToXYZ(b+osg::DegreesToRadians(2.0/3600.0),l,h,xe,ye,ze);
	vN.set(xe-xb,ye-yb,ze-zb);
	vN/=vN.length();
	//5.directionE:l->l+2sec.
	_ellipsoid.convertLatLongHeightToXYZ(b,l+osg::DegreesToRadians(2.0/3600.0),h,xe,ye,ze);
	vL.set(xe-xb,ye-yb,ze-zb);
	vL/=vL.length();

	//6.directionUp:vL^vN
	vUp=vL^vN;
	vUp/=vUp.length();
}

//�����ƽ���UP����,������Ļ�������߷���
void BHManipulator::getUp(osg::Vec3d &vy)
{	
	//��Ļ����
	//������ȵ�ѡȡ���ɡ�����ռ�Z=C��Ӧ����ռ�ƽ��
	osg::Vec3d screenPt1(50,0,0),screenPt2(50,50,0);	 
	osg::Vec3d obj1,obj2;
	Matrix VPW = _Viewer->getCamera()->getViewMatrix() *
		_Viewer->getCamera()->getProjectionMatrix() *
		_Viewer->getCamera()->getViewport()->computeWindowMatrix();
	Matrix inverseVPW;
	inverseVPW.invert(VPW);
	//�õ���Ӧ�������
	obj1=screenPt1* inverseVPW;
	obj2=screenPt2* inverseVPW;

	vy.set(obj2-obj1);
	vy.normalize();
}

void BHManipulator::getLookAt(osg::Vec3d &eye,osg::Vec3d &center,osg::Vec3d &up)
{
	osg::Matrixd  mt=getMatrix();
	//�۲�ϵ,�ֱ�ָ��۲��,��,��,����ϵ
	osg::Vec3d vx=osg::Vec3d (mt(2,0),mt(2,1),mt(2,2));
	osg::Vec3d vy=osg::Vec3d (mt(0,0),mt(0,1),mt(0,2));
	osg::Vec3d vz=osg::Vec3d (mt(1,0),mt(1,1),mt(1,2));

	eye=_eye;
	center=eye-vx;
	//getUp(up);
	up=vz;
}

//�����ƽ���UP����,������Ļ�������߷���
void BHManipulator::objectXYZToScreenXy(osg::Vec3d ptO,osg::Vec3d &ptS)
{	
	Matrix VPW = _Viewer->getCamera()->getViewMatrix() *
		_Viewer->getCamera()->getProjectionMatrix() *
		_Viewer->getCamera()->getViewport()->computeWindowMatrix();

	ptS=ptO*VPW;
}

void BHManipulator::screenXyToObjectXYZB(osg::Vec3d screenPt,osg::Vec3d &ptXYZ)
{	
	osg::Vec3d screenPt0;
	screenPt0.set(screenPt.x(),screenPt.y(),0.5);

	Matrix VPW = _Viewer->getCamera()->getViewMatrix() *
		_Viewer->getCamera()->getProjectionMatrix() *
		_Viewer->getCamera()->getViewport()->computeWindowMatrix();
	Matrix inverseVPW;
	inverseVPW.invert(VPW);
	//
	ptXYZ=screenPt0* inverseVPW;
}