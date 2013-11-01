#pragma once
#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

class planeUpdate : public osg::NodeCallback
{
public:
	planeUpdate(void);
	~planeUpdate(void);

	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );

	void setUpdate( bool bl );

	void setPos( std::vector<osg::Vec3d> vPos )
	{
		_vPos = vPos;
	}

	void setAxis( osg::Vec3d axis )
	{
		_axis = axis;
	}

	void setDir( std::vector<osg::Vec3d> vDir )
	{
		_vDir = vDir;
	}

	void setScale( double scale )
	{
		_scale = scale;
	}

	void setA( double a )
	{
		_a = a;
	}

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
	double _a;

};

