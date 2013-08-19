#include "StdAfx.h"
#include "ESTManipulator.h"


ESTManipulator::ESTManipulator(void)
{
}


ESTManipulator::~ESTManipulator(void)
{
}

void ESTManipulator::setByMatrix( const osg::Matrixd& matrix )
{
	throw std::exception("The method or operation is not implemented.");
}

void ESTManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
	throw std::exception("The method or operation is not implemented.");
}

osg::Matrixd ESTManipulator::getMatrix() const
{
	throw std::exception("The method or operation is not implemented.");
}

osg::Matrixd ESTManipulator::getInverseMatrix() const
{
	throw std::exception("The method or operation is not implemented.");
}

void ESTManipulator::setNode( osg::Node* )
{
	throw std::exception("The method or operation is not implemented.");
}

bool ESTManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	throw std::exception("The method or operation is not implemented.");
}