#include "StdAfx.h"
#include "ESTPickHandler.h"

ESTPickHandler::ESTPickHandler( osgText::Text* updateText ) : m_updateText(updateText)
{

}


ESTPickHandler::~ESTPickHandler(void)
{
}

bool ESTPickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
			osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			viewer->getCamera()->getViewMatrixAsLookAt(position, center, up);
			

			if (viewer)
			{
				pick(viewer, ea);
			}
			return false;
		}
		default:
			return false;
	}	
}

void ESTPickHandler::pick( osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea )
{
	osgUtil::LineSegmentIntersector::Intersections intersectons;
	std::string gdlist = "";
	std::ostringstream os;
	os<<"   Eye Position X: "<<(int)position[0]<<" Y: "<<(int)position[1]<<" Z: "<<(int)position[2] 
	  <<"\nCenter Position X: "<<(int)center[0]<<" Y: "<<(int)center[1]<<" Z: "<<(int)center[2]
	  <<"\n    Up Position X: "<<(int)up[0]<<" Y: "<<(int)up[2]<<" Z: "<<(int)up[2];
	gdlist += os.str();
	setLabel(gdlist);
}

void ESTPickHandler::setLabel( const std::string& name )
{
	if (m_updateText.get())
	{
		m_updateText->setText(name);
	}
}
