#include "StdAfx.h"
#include "SouthManipulator.h"


SouthManipulator::SouthManipulator(void): m_fMoveSpeed(100.0f), m_bLeftButtonDown(false), m_fpushX(0), m_fpushY(0), m_fAngle(2.5), m_bCollisionDetection(false)
{
	m_vPosition = osg::Vec3(0.0f, 0.0f, 5.0f);
	m_vRotation = osg::Vec3(osg::PI_2, 0.0f, 0.0f);
}


SouthManipulator::~SouthManipulator(void)
{
}

void SouthManipulator::setByMatrix( const osg::Matrixd& matrix )
{

}

void SouthManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{

}

osg::Matrixd SouthManipulator::getMatrix() const
{
	// 得到旋转后的矩阵，也就是视口矩阵，用此控制场景
	osg::Matrix mat;
	mat.makeRotate(m_vRotation._v[0], osg::Vec3(1.0f, 0.0f, 0.0f), m_vRotation._v[1], osg::Vec3(0.0f, 1.0f, 0.0f), m_vRotation._v[2], osg::Vec3(0.0f, 0.0f, 1.0f));
	return mat*osg::Matrixd::translate(m_vPosition);
}

osg::Matrixd SouthManipulator::getInverseMatrix() const
{
	osg::Matrixd mat;
	mat.makeRotate(m_vRotation._v[0], osg::Vec3(1.0f, 0.0f, 0.0f), m_vRotation._v[1], osg::Vec3(0.0f, 1.0f, 0.0f), m_vRotation._v[2], osg::Vec3(0.0f, 0.0f, 1.0f));
	return osg::Matrixd::inverse(mat * osg::Matrix::translate(m_vPosition));
}

void SouthManipulator::setNode( osg::Node* node)
{
	m_node = node;
}

bool SouthManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	float mouseX = ea.getX();
	float mouseY = ea.getY();

	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			// 如果是空格，重绘
			if (ea.getKey() == 0x20)
			{
				aa.requestRedraw();
				aa.requestContinuousUpdate();
				return true;
			}
			// 如果是 home，则视点向上移动
			if (ea.getKey() == 0xFF50)
			{
				ChangePosition(osg::Vec3(0, 0, m_fMoveSpeed));
				return true;
			}
			// 如果是 end，则视点向下移动
			if (ea.getKey() == 0xFF57)
			{
				ChangePosition(osg::Vec3(0, 0, -m_fMoveSpeed));
				return true;
			}
			// 如果是 +，则加速
			if (ea.getKey() == 0x2B)
			{
				m_fMoveSpeed += 1.0f;
				return true;
			}
			// 如果是 -，则减速
			if (ea.getKey() == 0x2D)
			{
				m_fMoveSpeed -= 1.0f;
				if (m_fMoveSpeed < 1.0f)
				{
					m_fMoveSpeed = 1.0f;
				}
				return true;
			}

			// 如果是 w 或 UP，则向前走
			if (ea.getKey() == 0xFF52 || ea.getKey() == 0x57 || ea.getKey() == 0x77)
			{
				ChangePosition(osg::Vec3(0, m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0));
				ChangePosition(osg::Vec3(m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0, 0));
				return true;
			}

			// 如果是 s 或 DOWN，则向后走
			if (ea.getKey() == 0xFF54 || ea.getKey() == 0x53 || ea.getKey() == 0x73)
			{
				ChangePosition(osg::Vec3(0, -m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0));
				ChangePosition(osg::Vec3(-m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0, 0));
				return true;
			}
			// 如果是 a 
			if (ea.getKey() == 0x41 || ea.getKey() == 0x61)
			{
				ChangePosition(osg::Vec3(0, m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0));
				ChangePosition(osg::Vec3(-m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0, 0));
				return true;
			}
			// 如果是 d 
			if (ea.getKey() == 0x44 || ea.getKey() == 0x64)
			{
				ChangePosition(osg::Vec3(0, -m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0));
				ChangePosition(osg::Vec3(m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0, 0));
				return true;
			}
			// 如果是 RIGHT
			if (ea.getKey() == 0xFF53)
			{
				m_vRotation._v[2] -= osg::DegreesToRadians(m_fAngle);
			}
			// 如果是 LEFT
			if (ea.getKey() == 0xFF51)
			{
				m_vRotation._v[2] += osg::DegreesToRadians(m_fAngle);
			}
			// 如果是 f
			if (ea.getKey() == 0x46 || ea.getKey() == 0x66)
			{
				computeHomePosition();
				m_fAngle -= 0.2;
				return true;
			}
			// 如果是 g
			if (ea.getKey() == 0x47 || ea.getKey() == 0x67)
			{
				m_fAngle += 0.2;
				return true;
			}
			return false;
			break;
		}	
	case(osgGA::GUIEventAdapter::PUSH):
		if (ea.getButton() == 1)
		{
			m_fpushX = mouseX;
			m_fpushY = mouseY;
			m_bLeftButtonDown = true;
		}
		return false;
		break;
	case(osgGA::GUIEventAdapter::DRAG):
		if (m_bLeftButtonDown)
		{
			m_vRotation._v[2] -= osg::DegreesToRadians(m_fAngle * (mouseX - m_fpushX));
			m_vRotation._v[0] += osg::DegreesToRadians(1.1 * (mouseY - m_fpushY));
			if (m_vRotation._v[0] >= 3.14)
			{
				m_vRotation._v[0] = 3.14;
			}
			if (m_vRotation._v[0] <= 0)
			{
				m_vRotation._v[0] = 0;
			}			
		}
		return false;

		break;
	case(osgGA::GUIEventAdapter::RELEASE):
		if (ea.getButton() == 1)
		{
			m_bLeftButtonDown = false;
		}
		return false;
		break;
	default:
		return false;
		break;
	}
}

void SouthManipulator::computeHomePosition()
{
	if (m_node.get())
	{
		const osg::BoundingSphere& boundingShere = m_node->getBound();
		osg::Vec3 bp = boundingShere._center;
		SetPosition(bp);
	}
}

void SouthManipulator::ChangePosition( osg::Vec3& delta )
{
	if (m_bCollisionDetection)
	{
		osg::Vec3 newPos = m_vPosition + delta;
		osgUtil::IntersectVisitor iv;
		osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(newPos, m_vPosition);
		osg::ref_ptr<osg::LineSegment> lineZ = new osg::LineSegment(newPos + osg::Vec3(0.0f, 0.0f, m_fMoveSpeed), newPos - osg::Vec3(0.0f, 0.0f, m_fMoveSpeed));
		iv.addLineSegment(lineZ.get());
		iv.addLineSegment(line.get());
		m_node->accept(iv);
		if (!iv.hits())
		{
			m_vPosition += delta;
		}
	}
	else
	{
		m_vPosition += delta;
	}
}

void SouthManipulator::setCollisionDetection( bool cd )
{
	m_bCollisionDetection = true;
}

bool SouthManipulator::getCollisionDetection()
{
	return m_bCollisionDetection;
}

void SouthManipulator::setInverseCollisionDetection()
{
	m_bCollisionDetection = !m_bCollisionDetection;
}

float SouthManipulator::getSpeed()
{
	return m_fMoveSpeed;
}

void SouthManipulator::setSpeed( float sp)
{
	m_fMoveSpeed = sp;
}

void SouthManipulator::SetPosition( osg::Vec3 &position )
{
	m_vPosition = position;
}

void SouthManipulator::SetPosition( double* position)
{
	m_vPosition._v[0] = position[0];
	m_vPosition._v[1] = position[1];
	m_vPosition._v[2] = position[2];
}

osg::Vec3 SouthManipulator::GetPosition()
{
	return m_vPosition;
}
