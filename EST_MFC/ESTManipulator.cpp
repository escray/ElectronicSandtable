#include "StdAfx.h"
#include "ESTManipulator.h"


ESTManipulator::ESTManipulator(void)
{
	m_ellipsoid.setRadiusEquator(6378137);
	m_ellipsoid.setRadiusPolar(6378137);

	setAutoComputeHomePosition(false);

	m_vNe.set(-999.0, -999.0, -999.0);

	m_blSaved = false;
}


ESTManipulator::~ESTManipulator(void)
{
}

// 设置观察参数，参数由矩阵确定，唯一确定视点位置和旋转参数
void ESTManipulator::setByMatrix( const osg::Matrixd& matrix )
{
	osg::Vec3d lookVector(-matrix(2, 0), -matrix(2, 1), -matrix(2, 2));
	osg::Vec3d eye(matrix(3, 0), matrix(3, 1), matrix(3, 3));

	m_eye = eye;
    m_rotation = matrix.getRotate().inverse();
}

void ESTManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
	// tricky
	setByMatrix(osg::Matrixd::inverse(matrix));
}

// 观察视图矩阵，由试点位置、旋转参数确定
osg::Matrixd ESTManipulator::getMatrix() const
{
	return osg::Matrixd::rotate(m_rotation)*osg::Matrixd::translate(m_eye);
}

osg::Matrixd ESTManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate(-m_eye)*osg::Matrixd::rotate(m_rotation.inverse());
}

void ESTManipulator::setNode( osg::Node* )
{
	//TODO: m_node = node;
}


//////////////////////////////////////////////////////////////////////////
// f 环视
// w 前进 
// s 后退
// a 左
// d 右
// + 加速
// - 减速
// e 视点上升
// c 视点下降
// z 保存参数
// x 读取参数
// r 升高正视高毒
// v 降低正视高度
// 鼠标拖转
// 滚轮
//////////////////////////////////////////////////////////////////////////
bool ESTManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		// 环视
		if (ea.getKey() == 'F' || ea.getKey() == 'f')
		{
			double a0;
			m_a0 += 1.0;
			a0 = 1.0;

			osg::Vec3d axis(m_center.x(), m_center.y(), m_center.z());
			axis.normalize();

			osg::Matrixd matrix = osg::Matrixd::translate(-m_center) * osg::Matrixd::rotate(-a0 * osg::PI/180.0, axis) * osg::Matrixd::translate(m_center);
			m_eye = m_eye*matrix;

			//  [8/27/2013 zhaorui]
			// TODO: matrix2 可以使用 matrix 替换
			osg::Matrixd matrix2 = osg::Matrixd::translate(-m_center) * osg::Matrixd::rotate(-m_a0 * osg::PI/180.0, axis) * osg::Matrixd::translate(m_center);
			osg::Vec3d vn = m_vNe * matrix2;
			setLookAtByXyz(m_eye, m_center, vn);

			us.requestRedraw();
			us.requestContinuousUpdate(false);
			return false;
		}
		// 围绕观察点，降低观察高度
		if (ea.getKey() == 'C' || ea.getKey() == 'c')
		{
			double x, y, z, b, l, h;
			m_ellipsoid.convertXYZToLatLongHeight(m_eye.x(), m_eye.y(), m_eye.z(), b, l, h);
			h -= h*0.1;
			m_ellipsoid.convertLatLongHeightToXYZ(b, l, h, x, y, z);
			m_eye.set(x, y, z);
			
			// 加旋转
			//osg::Vec3d axis(m_center.x(), m_center.y(), m_center.z());
			//osg::Matrix matrix = osg::Matrixd::translate(-m_center) * osg::Matrixd::rotate(-m_a0*osg::PI/180.0, axis) * osg::Matrixd::translate(m_center);
			//osg::Vec3d vn = m_vNe * matrix;
			//setLookAtByXyz(m_eye, m_center, vn);

			us.requestRedraw();
			us.requestContinuousUpdate(false);

			return false;
		}
		// 围绕观察点升高，可以旋转
		if (ea.getKey() == 'E' || ea.getKey() == 'e')
		{
			double x, y, z, b, l, h;
			m_ellipsoid.convertXYZToLatLongHeight(m_eye.x(), m_eye.y(), m_eye.z(), b, l, h);
			h += h*0.1;
			m_ellipsoid.convertLatLongHeightToXYZ(b, l, h, x, y, z);
			m_eye.set(x, y, z);

			osg::Vec3d axis(m_center.x(), m_center.y(), m_center.z());
			axis.normalize();

			osg::Matrixd matrix = osg::Matrixd::translate(-m_center)*osg::Matrixd::rotate(-m_a0*osg::PI/180.0, axis)*osg::Matrixd::translate(m_center);
			osg::Vec3d vn = m_vNe * matrix;

			setLookAtByXyz(m_eye, m_center, vn);
			
			us.requestRedraw();
			us.requestContinuousUpdate(false);

			return false;
		}
		// 保存参数
		// 保存围绕观察点旋转模式的数据，以便恢复
		if (ea.getKey() == 'Z' || ea.getKey() == 'z')
		{
			if (m_blSaved)
			{
				return false;
			}
			SaveParam();

			return false;
		}
		// 读取参数
		if (ea.getKey() == 'X' || ea.getKey() == 'x')
		{
			if(!m_blSaved)
			{
				return false;
			}

			LoadParam();

			return false;
		}
		// 降低正视高度
		if (ea.getKey() == 'V' || ea.getKey() == 'v')
		{
			if (!m_blSaved)
			{
				SaveParam();
			}

			double x, y, z, b, l, h, he;
			m_ellipsoid.convertXYZToLatLongHeight(m_eye.x(), m_eye.y(), m_eye.z(), b, l, h);
			he = h;

			he -= h/100.0;

			m_ellipsoid.convertXYZToLatLongHeight(m_center.x(), m_center.y(), m_center.z(), b, l, h);
			m_ellipsoid.convertLatLongHeightToXYZ(b, l, he, x, y, z);

			m_eye.set(x, y, z);

			osg::Vec3d axis(m_center.x(), m_center.y(), m_center.z());
			axis.normalize();

			osg::Matrixd matrix = osg::Matrixd::translate(-m_center) * osg::Matrixd::rotate( -m_a0 * osg::PI/180.0, axis) * osg::Matrixd::translate(m_center);
			osg::Vec3d vn = m_vNe * matrix;

			setLookAtByXyz(m_eye, m_center, vn);

			us.requestRedraw();
			us.requestContinuousUpdate();

			return false;
		}
		// 升高正视高度
		if (ea.getKey() == 'R')
		{
			if (!m_blSaved)
			{
				SaveParam();
			}

			double x, y, z, b, l, h, hEye;
			m_ellipsoid.convertXYZToLatLongHeight(m_eye.x(), m_eye.y(), m_eye.z(), b, l, h);
			hEye = h;
			hEye += h/100.0;

			m_ellipsoid.convertXYZToLatLongHeight(m_center.x(), m_center.y(), m_center.z(), b, l, h);
			m_ellipsoid.convertLatLongHeightToXYZ(b, l, hEye, x, y, z);

			m_eye.set(x, y, z);

			osg::Vec3d axis(m_center.x(), m_center.y(), m_center.z());
			axis.normalize();

			osg::Matrixd matrix = osg::Matrixd::translate(-m_center) * osg::Matrixd::rotate(-m_a0*osg::PI/180.0, axis) * osg::Matrixd::translate(m_center);
			osg::Vec3d vn = m_vNe * matrix;

			setLookAtByXyz(m_eye, m_center, vn);

			us.requestRedraw();
			us.requestContinuousUpdate(false);

			return false;
		}
		if (ea.getKey() == 0x20)
		{
			home(ea, us);
			return false;
		}

	default:
		return false;
	}
	return true;
}

void ESTManipulator::home( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	resetPosition();
}

void ESTManipulator::setLookAtByXyz( const osg::Vec3d eye, const osg::Vec3d center, const osg::Vec3d up )
{
	m_eye = eye;
	osg::Matrixd matrix;
	matrix.makeLookAt(eye, center , up);
	m_rotation = matrix.getRotate().inverse();
}

void ESTManipulator::setLookAtByBlh( osg::Vec3d eyeBlh, osg::Vec3d centerBlh, osg::Vec3d up )
{
	osg::Vec3d eyeXYZ;
	osg::Vec3d centerXYZ;
	double rb = osg::DegreesToRadians(eyeBlh.x());
	double rl = osg::DegreesToRadians(eyeBlh.y());
	double h = eyeBlh.z();

	m_ellipsoid.convertLatLongHeightToXYZ(rb, rl, h, eyeXYZ.x(), eyeXYZ.y(), eyeXYZ.z());
	m_eye = eyeXYZ;

	rb = osg::DegreesToRadians(centerBlh.x());
	rl = osg::DegreesToRadians(centerBlh.y());
	m_ellipsoid.convertLatLongHeightToXYZ(rb, rl, 0, centerXYZ.x(), centerXYZ.y(), centerXYZ.z());

	osg::Vec3d lookAt = centerXYZ - eyeXYZ;
	lookAt.normalize();
	osg::Vec3d vNe = up;
	osg::Vec3d vLe;
	osg::Vec3d vUpe;
	osg::Vec3d center = m_eye + lookAt;

	// why?
	if(up.length() >= 9.0)
	{
		getLocalCoordFrame(center, 0, vNe, vLe, vUpe);
	}

	vNe.normalize();

	setLookAtByXyz(m_eye, center, vNe);

	if (m_vNe.length() > 9.0)
	{
		m_vNe = vNe;
	}
}

// ground location's directionN, directionE, directionUp
void ESTManipulator::getLocalCoordFrame( osg::Vec3d center, double azimuth, osg::Vec3d& vN, osg::Vec3d& vL, osg::Vec3d &vUp )
{
	// 1. degree azimuth to radian
	azimuth = osg::DegreesToRadians(azimuth);

	// 2 location's xyz to blh
	double b, l, h;
	m_ellipsoid.convertXYZToLatLongHeight(center.x(), center.y(), center.z(), b, l, h);	

	// 3. directions are get by increaing a small amount(2 second)
	double xb, yb, zb, xe, ye, ze;
	xb = center.x();
	yb = center.y();
	zb = center.z();

	// 4. directionN: b->b+2sec
	m_ellipsoid.convertLatLongHeightToXYZ(b+osg::DegreesToRadians(2.0/3600.0), l, h, xe, ye, ze);
	vN.set(xe-xb, ye-yb, ze-zb);
	vN /= vN.length();
	
	// 5. directionE: l->l+2sec
	m_ellipsoid.convertLatLongHeightToXYZ(b, l+osg::DegreesToRadians(2.0/3600.0), h, xe, ye, ze);
	vL.set(xe-xb, ye-yb, ze-zb);
	vL /= vL.length();

	// 6. directionUp: vl^vN
	vUp = vL^vN;
	vUp /= vUp.length();

}

// 操作器初始化复位
void ESTManipulator::resetPosition()
{
	double r = m_ellipsoid.getRadiusEquator();

	setLookAtByBlh(osg::Vec3d(38.914702, 116.391731, 15000.0), osg::Vec3d(39.914702, 116.391731, 0.0), osg::Vec3d(-999.0, -999.0, -999.0));

	osg::Vec3d eye, center, up;
	getLookAt(eye, center, up);
	setHomePosition(eye, center, up);

	m_a0 = 0.0;

	double x, y, z;
	m_ellipsoid.convertLatLongHeightToXYZ(39.914702*osg::PI/180.0, 116.391731*osg::PI/180.0, 0.0, x, y, z);
	m_center.set(x, y, z);

}

void ESTManipulator::getLookAt( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up )
{
	osg::Matrixd mt = getMatrix();
	// 观察系，分别指向观察点
	osg::Vec3d vx = osg::Vec3d(mt(2, 0), mt(2, 1), mt(2, 2));
	osg::Vec3d vy = osg::Vec3d(mt(0, 0), mt(0, 1), mt(0, 2));
	osg::Vec3d vz = osg::Vec3d(mt(1, 0), mt(1, 1), mt(1, 2));

	eye = m_eye;
	center = m_center;
	up = vz;
}

// 求成像平面的up向量，依据屏幕坐标竖线方向
void ESTManipulator::objectXYZToScreenXy( osg::Vec3d ptO, osg::Vec3d& ptS )
{
	osg::Matrix vpw = m_viewer->getCamera()->getViewMatrix() * m_viewer->getCamera()->getProjectionMatrix() * m_viewer->getCamera()->getViewport()->computeWindowMatrix();

	ptS = ptO * vpw;
}

void ESTManipulator::screenXyToObjectXYZB( osg::Vec3d screenPt, osg::Vec3d &ptXYZ )
{
	osg::Vec3d screenPt0;
	screenPt0.set(screenPt.x(), screenPt.y(), 0.5);
	osg::Matrix vpw = m_viewer->getCamera()->getViewMatrix() * m_viewer->getCamera()->getProjectionMatrix() * m_viewer->getCamera()->getViewport()->computeWindowMatrix();

	osg::Matrix inverseVPW;
	inverseVPW.inverse(vpw);

	ptXYZ = screenPt0 * inverseVPW;
}

void ESTManipulator::SaveParam()
{
	m_eyeSaved = m_eye;
	m_rotationSaved = m_rotation;

	m_blSaved = true;
}

void ESTManipulator::LoadParam()
{
	m_eye = m_eyeSaved;
	m_rotation = m_rotationSaved;

	m_blSaved = false;
}

osg::Vec3d ESTManipulator::getCenter()
{
	return m_center;
}
