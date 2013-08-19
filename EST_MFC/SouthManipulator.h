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
	// 结点值，用来测试碰撞检测
	osg::ref_ptr<osg::Node> m_node;
	// 相机操作器
	unsigned int m_nID;
	// 移动速度
	float m_fMoveSpeed;
	// 位置
	osg::Vec3 m_vPosition;
	// 角度
	osg::Vec3 m_vRotation;
	// 左键是否按下
	bool m_bLeftButtonDown;
	// 左键点下时屏幕坐标
	float m_fpushX;
	// 碰撞检测开启状态
	bool m_bCollisionDetection;
	// 右键点下时屏幕坐标
	float m_fpushY;

public:
	// 碰撞检测是否开启
	void setCollisionDetection(bool cd);
	// 得到碰撞检测状态
	bool getCollisionDetection();
	// 碰撞检测开关，如果碰撞检测开启则关闭，如果关闭则开启
	void setInverseCollisionDetection();
	// 得到当前速度
	float getSpeed();
	// 设置当前速度
	void setSpeed(float);
	// 设置视点位置
	void SetPosition(osg::Vec3 &position);
	void SetPosition(double *);
	// 得到当前视点
	osg::Vec3 GetPosition();
	// 设置要进行碰撞检测的数据
	virtual void setNode(osg::Node*);

	virtual void setByMatrix( const osg::Matrixd& matrix );

	virtual void setByInverseMatrix( const osg::Matrixd& matrix );

	virtual osg::Matrixd getMatrix() const;

	virtual osg::Matrixd getInverseMatrix() const;

	// 事件控制器
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	// 屏幕角度
	float m_fAngle;

	// 位置变换
	void ChangePosition(osg::Vec3& delta);

	// 计算家的位置
	void computeHomePosition();
};