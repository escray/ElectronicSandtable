#include "StdAfx.h"
#include "ESTPickHandler.h"

ESTPickHandler::ESTPickHandler( osgText::Text* updateText ) 
	: m_updateText(updateText),
	m_bPick(false),
	m_bFirstPush(false),
	m_size(1),
	m_height(5000)
{
}

ESTPickHandler::~ESTPickHandler(void)
{
}

bool ESTPickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	m_viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
			
			m_viewer->getCamera()->getViewMatrixAsLookAt(position, center, up);		

			if (m_viewer)
			{
				pick(m_viewer, ea);
			}
			return false;
		}
	case(osgGA::GUIEventAdapter::PUSH):
		if (m_bPick)
		{
			if (m_viewer)
			{
				pick(m_viewer, ea);
			}
			return false;
		}
	default:
		return false;
	}	
}

void ESTPickHandler::pick( osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea )
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	osg::Group* root = dynamic_cast<osg::Group*>(viewer->getSceneData());
	
	if (!root)
	{
		return;
	}

	osg::Vec3f temp;

	if (viewer->computeIntersections( ea.getX(), ea.getY(), intersections))
	{
		osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		temp = hitr->getWorldIntersectPoint();

		// this is not the first point
		if ( !m_bFirstPush )
		{
			point->clear();
			// height : 5000, maybe need modify
			point->push_back( osg::Vec3f( temp.x(), temp.y(), m_height ) );
			ctrlPoints = DrawFirstNode( temp );
			root->addChild(ctrlPoints.get());
			m_bFirstPush = true;
		}
		else
		{
			point->push_back( osg::Vec3f( temp.x(), temp.y(), m_height ) );
			ctrlPoints->addChild( DrawCtrlPoints(temp) );
		}
	}

	std::string gdlist = "";
	std::ostringstream os;
	os<<"   Eye Position X: "<<(int)position[0]<<" Y: "<<(int)position[1]<<" Z: "<<(int)position[2] 
	  <<"\nCenter Position X: "<<(int)center[0]<<" Y: "<<(int)center[1]<<" Z: "<<(int)center[2]
	  <<"\n    Up Position X: "<<(double)up[0]<<" Y: "<<(double)up[1]<<" Z: "<<(double)up[2]
	  <<"\n  Pick Position X: "<<(int)temp[0]<<" Y: "<<(int)temp[1]<<" Z: "<<(int)temp[2];

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

osg::Node* ESTPickHandler::CreateCardinal()
{
	osg::ref_ptr<osg::Vec3Array> allPoints = new osg::Vec3Array;
	Cardinal( allPoints.get() );

	osg::Group* root = new osg::Group();

	// 设置点的大小
	osg::ref_ptr<osg::LineWidth> lineSize = new osg::LineWidth();
	lineSize->setWidth( 1.0 );
	root->getOrCreateStateSet()->setAttributeAndModes( lineSize.get(), osg::StateAttribute::ON );

	// 设置几何结点
	osg::Geode* lineGeode = new osg::Geode();
	osg::Geometry* lineGeometry = new osg::Geometry();

	// 可画几何节点加入
	lineGeode->addDrawable( lineGeometry );
	root->addChild( lineGeode );

	// 把点充入画区
	lineGeometry->setVertexArray( allPoints.get() );

	// 设置所画格式
	osg::DrawElementsUInt * lineBase = new osg::DrawElementsUInt( osg::PrimitiveSet::LINE_LOOP, 0 );
	for ( int i = 0; i < (int)(allPoints->size()); i++ )
	{
		lineBase->push_back(i);
	}
	lineGeometry->addPrimitiveSet(lineBase);

	// 颜色
	osg::Vec4Array* colors = new osg::Vec4Array();
	// 蓝色
	colors->push_back( osg::Vec4( 0.4f, 0.0f, 0.0f, 0.5f ) );

	// 设置颜色
	lineGeometry->setColorArray( colors );
	lineGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

	return ( osg::Node* )root;
}

void ESTPickHandler::Cardinal( osg::ref_ptr<osg::Vec3Array> temp )
{
	// 这个 pointer 应该替换为 vPos，就是控制点的集合
	// 2013-11-20
	// 处理头尾两个节点
	std::vector<osg::Vec3>::iterator iter = point->begin();
	osg::ref_ptr<osg::Vec3Array> ctrlTwo = new osg::Vec3Array();
	ctrlTwo->push_back( osg::Vec3( (*iter).x()-1, (*iter).y()-1, (*iter).z() ) );
	ctrlTwo->insert( ++(ctrlTwo->begin() ), point->begin(), point->end() );
	iter = point->end();
	iter--;
	ctrlTwo->push_back( osg::Vec3( (*iter).x()-1, (*iter).y()-1, (*iter).z()-1 ) );
	iter = ctrlTwo->begin();

	for (; iter!=ctrlTwo->end(); *iter++)
	{
		osg::Vec3 p0 = *iter++;
		osg::Vec3 p1 = *iter++;
		osg::Vec3 p2 = *iter++;
		if (iter == ctrlTwo->end() )
		{
			return;
		}
		osg::Vec3 p3 = *iter;

		iter--;
		iter--;
		iter--;

		float t = 0;
		for ( ; t<=1; t=t+0.1 )
		{
			// 高度 5000 不变
			temp->push_back( osg::Vec3( GetCoefficient( p0.x(), p1.x(), p2.x(), p3.x(), t ), GetCoefficient( p0.y(), p1.y(), p2.y(), p3.y(), t ), m_height ) );
		}
	}

}

float ESTPickHandler::GetCoefficient( float p0, float p1, float p2, float p3, float t )
{
	float d = p1;
	float c = 0.5 * ( 1 - t ) * ( p2 - p0 );
	float a = 0.5 * ( t - 1 ) * p0 + ( 1.5 + 0.5 * t ) * p1 - ( 1.5 + 0.5 * t ) * p2 + 0.5 * ( 1 - t ) * p3;
	float b = p2 - a - d - c;
	return ( a * t * t * t + b * t * t + c * t + d );
}

osg::Group* ESTPickHandler::getCtrlPointGroup()
{
	osg::ref_ptr<osg::Group> group;
	return group.get();
}

void ESTPickHandler::pushCardinal()
{

}

osg::AnimationPath* ESTPickHandler::CreatePath( std::string pathName )
{
	osg::AnimationPath* path = new osg::AnimationPath();
	return path;
}

bool ESTPickHandler::getPick()
{
	return m_bPick;
}

void ESTPickHandler::setPick( bool pick )
{
	m_bPick = pick;
}

osg::Node* ESTPickHandler::DrawCtrlPoints( osg::Vec3f position )
{
	osg::Group* ctrls = new osg::Group();
	
	// setup size of point
	osg::ref_ptr<osg::Point> pointSize = new osg::Point();
	pointSize->setSize(5.0);
	ctrls->getOrCreateStateSet()->setAttributeAndModes( pointSize.get(), osg::StateAttribute::ON );

	// setup geode
	osg::Geode* ctrlGeode = new osg::Geode();
	osg::Geometry* ctrlGeometry = new osg::Geometry();

	// add drawable node
	ctrlGeode->addDrawable( ctrlGeometry );
	ctrls->addChild( ctrlGeode );

	osg::ref_ptr<osg::Vec3Array> ctrlPoint = new osg::Vec3Array();
	
	




	return ctrls;
}

osg::Group* ESTPickHandler::DrawFirstNode( osg::Vec3f position )
{
	osg::Group* first = new osg::Group();
	
	// setup geometry point
	osg::Geode* firstGeode = new osg::Geode();
	osg::Geometry* firstGeometry = new osg::Geometry();

	// add drawable point
	firstGeode->addDrawable( firstGeometry );
	first->addChild( firstGeode );

	osg::ref_ptr<osg::Vec3Array> trangle = new osg::Vec3Array();
	trangle->push_back( osg::Vec3( position.x()-4*m_size, position.y(), m_height ) );
	trangle->push_back( osg::Vec3( position.x(), position.y()-4*m_size, m_height ) );
	trangle->push_back( osg::Vec3( position.x()+4*m_size, position.y(), m_height ) );
	trangle->push_back( osg::Vec3( position.x(), position.y()+4*m_size, m_height ) );

	// put the point into geometry
	firstGeometry->setVertexArray( trangle.get() );

	// setup the format of the point
	osg::DrawElementsUInt* pyramidBase = new osg::DrawElementsUInt( osg::PrimitiveSet::LINE_LOOP, 0 );

	pyramidBase->push_back(3);
	pyramidBase->push_back(2);
	pyramidBase->push_back(1);
	pyramidBase->push_back(0);
	firstGeometry->addPrimitiveSet( pyramidBase );

	// color
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
	
	// setup color
	firstGeometry->setColorArray( colors );
	firstGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

	return first;
}
