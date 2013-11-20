#pragma once
#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate(void);
	~planeUpdate(void);

	// ����λ��
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

	// �����Ƿ����
	void setUpdate( bool bl );

	// ����λ��
	void setPos( std::vector<osg::Vec3d> vPos );

	// ��������
	void setAxis( osg::Vec3d axis );

	// ���÷���
	void setDir( std::vector<osg::Vec3d> vDir );

	// ���÷Ŵ���С����
	void setScale( double scale );

	// ���ýǶ�
	void setAngle( double a );

private:
	// �Ƿ��ƶ����·ɻ�λ��
	bool _updatePos;
	// ĳ��ʱ�̷ɻ���λ��
	int _posIndex;
	// λ����Ϣ
	std::vector<osg::Vec3d> _vPos;
	// ������Ϣ
	std::vector<osg::Vec3d> _vDir;
	// �ɻ�������Ϣ
	osg::Vec3d _axis;
	// ���ű���ϵ�������ڿ��ţ�ʹ�ɻ��ɼ�
	double _scale;
	// ?
	double _angle;

};

