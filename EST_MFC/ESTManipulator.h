#pragma once
#include <osgGA/CameraManipulator>
#include <osgGA/GUIEventAdapter>

class ESTManipulator :
	public osgGA::CameraManipulator
{
public:
	ESTManipulator(void);
	~ESTManipulator(void);

	virtual void setByMatrix( const osg::Matrixd& matrix );

	virtual void setByInverseMatrix( const osg::Matrixd& matrix );

	virtual osg::Matrixd getMatrix() const;

	virtual osg::Matrixd getInverseMatrix() const;

	virtual void setNode( osg::Node* );

	virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );	

private:
	osg::ref_ptr<osg::Node> m_node;

public:

};

