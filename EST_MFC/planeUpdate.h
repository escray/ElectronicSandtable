// planeRoute.h

#pragma once
#include <vector>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>

class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate(void);
	~planeUpdate(void);

	// ���·ɻ�λ��
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

	void setUpdate(bool bUpdate);
	void setPos(std::vector<osg::Vec3d> vPos);
	void setDir(std::vector<osg::Vec3d> vDir);
	void setAxis(osg::Vec3d axis);
	void setScale(double scale);
	void setA(double a);
private:
	// �Ƿ��ƶ�/���·ɻ�λ��
	bool _updatePos;
	// �ɻ���ĳһʱ�̵�λ��
	int _posIndex;
	// λ��
	std::vector<osg::Vec3d> _vPos;
	// ����
	std::vector<osg::Vec3d> _vDir;
	// ���򣨴ӽ�ģģ�ͻ�ȡ��
	osg::Vec3d _axis;
	// ���ű���
	double _scale;
	// �Ƕ�
	double _angle;


};

