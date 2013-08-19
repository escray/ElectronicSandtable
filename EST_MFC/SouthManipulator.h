#pragma once
#include <osg/LineSegment>
#include <osgGA/CameraManipulator>
#include <osgUtil/IntersectVisitor>
#include <osgViewer/Viewer>

class SouthManipulator :
	public osgGA::CameraManipulator
{
public:
	SouthManipulator(void);
	~SouthManipulator(void);
private:
	// ���ֵ������������ײ���
	osg::ref_ptr<osg::Node> m_node;
	// ���������
	unsigned int m_nID;
	// �ƶ��ٶ�
	float m_fMoveSpeed;
	// λ��
	osg::Vec3 m_vPosition;
	// �Ƕ�
	osg::Vec3 m_vRotation;
	// ����Ƿ���
	bool m_bLeftButtonDown;
	// �������ʱ��Ļ����
	float m_fpushX;
	// ��ײ��⿪��״̬
	bool m_bCollisionDetection;
	// �Ҽ�����ʱ��Ļ����
	float m_fpushY;

public:
	// ��ײ����Ƿ���
	void setCollisionDetection(bool cd);
	// �õ���ײ���״̬
	bool getCollisionDetection();
	// ��ײ��⿪�أ������ײ��⿪����رգ�����ر�����
	void setInverseCollisionDetection();
	// �õ���ǰ�ٶ�
	float getSpeed();
	// ���õ�ǰ�ٶ�
	void setSpeed(float);
	// �����ӵ�λ��
	void SetPosition(osg::Vec3 &position);
	void SetPosition(double *);
	// �õ���ǰ�ӵ�
	osg::Vec3 GetPosition();
	// ����Ҫ������ײ��������
	virtual void setNode(osg::Node*);

	virtual void setByMatrix( const osg::Matrixd& matrix );

	virtual void setByInverseMatrix( const osg::Matrixd& matrix );

	virtual osg::Matrixd getMatrix() const;

	virtual osg::Matrixd getInverseMatrix() const;

	// �¼�������
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	// ��Ļ�Ƕ�
	float m_fAngle;

	// λ�ñ任
	void ChangePosition(osg::Vec3& delta);

	// ����ҵ�λ��
	void computeHomePosition();
};