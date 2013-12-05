#pragma once
#include "BHManipulator.h"

#include <vector>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgUtil/SmoothingVisitor>

class ribbonUpdate :
	public osg::Drawable::UpdateCallback
{
public:
	ribbonUpdate(void);	
	~ribbonUpdate(void);

	// ����λ�ã�����и��±��
	virtual void update(osg::NodeVisitor* nv, osg::Drawable* dr);

	void setUpdate(bool positionUpdate);

	void setPos(std::vector<osg::Vec3d> vPos);
	// ���Ƶ��˳��ʽ
	void setNp(bool bl);
	void setA(double a);
	// zhaorui 2013-11-19
	// ����Ӧ�ÿ��Խ� BHmanipulator ��һ������Ϊ osgGA::CameraManipulator
	void setEmp(BHManipulator* emp);
	

public:
	// �Ƿ���·ɻ�λ�ã�Ʈ����Ҫ�ɻ�λ������
	bool _updatePos;
	// ĳ��ʱ�̷ɻ���λ��
	int _posIndex;
	// �ɻ����λ��
	std::vector<osg::Vec3d> _vPos;
	// Ʈ������ɫ
	osg::Vec4d _color;
	bool _blNp;
	double _a;
	// ���β�����
	osg::ref_ptr<BHManipulator> _emp;
};

