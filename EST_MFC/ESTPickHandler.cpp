#include "StdAfx.h"
#include "ESTPickHandler.h"
#include <fstream>

// 图标的大小和每秒速度还需要调整
// 2013-11-22
ESTPickHandler::ESTPickHandler( void ) :
	m_bPick(false),
	m_bFirstPush(false),
	m_size(10000),
	sec(0.00001),
	m_height(5000)
{
	point = new osg::Vec3Array();	
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
				frame(m_viewer, ea);
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

void ESTPickHandler::frame(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea)
{

	std::string gdlist = "";
	std::ostringstream os;
	os<<"   Eye Position X: "<<(int)position[0]<<" Y: "<<(int)position[1]<<" Z: "<<(int)position[2] 
	<<"\nCenter Position X: "<<(int)center[0]<<" Y: "<<(int)center[1]<<" Z: "<<(int)center[2]
	<<"\n    Up Position X: "<<(double)up[0]<<" Y: "<<(double)up[1]<<" Z: "<<(double)up[2];
	//<<"\n  Pick Position X: "<<(int)temp[0]<<" Y: "<<(int)temp[1]<<" Z: "<<(int)temp[2];

	gdlist += os.str();
	
	setLabel(gdlist);
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
			//point->push_back( osg::Vec3f( 3, 3, 3 ) );
			ctrlPoints->addChild( DrawCtrlPoints(temp) );
		}
	}
	
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
	return ctrlPoints.get();
}

void ESTPickHandler::pushCardinal()
{
	osg::Group* root = m_viewer->getSceneData()->asGroup();
	root->addChild( CreateCardinal() );
}

osg::AnimationPath* ESTPickHandler::CreatePath( std::string pathName )
{
	osg::AnimationPath* path = new osg::AnimationPath();

	double looptime = sec * GetAllDistance();
	std::vector<osg::Vec3>::iterator iter = point->end();
	std::vector<osg::Vec3>::iterator iter2;
	iter--;
	iter2 = --iter;
	iter++;

	point->push_back( osg::Vec3( (*iter).x()-(*iter2).x(), (*iter).y()-(*iter2).y(), (*iter).z()-(*iter2).z() ) );
		
	path->setLoopMode( osg::AnimationPath::LOOP );
	

	int numSamples = point->size();

	float yaw = 0.0f;
	float yaw_delta = 0.f;
	float roll = osg::inDegrees(90.0f);

	double time = 0.0f;
	double time_delta = looptime/(double)numSamples;

	float angle = 0.0;

	iter = point->begin();

	for (int i = 1; i<numSamples; ++i, iter++)
	{
		osg::Vec3 position(*iter);
		iter++;
		if ( iter!= point->end() )
		{
			if ( (*iter).x() > position.x() )
			{
				angle = 1.57 - atan( ( (*iter).y() - position.y() ) / ((*iter).x() - position.x() ) );
				if ( angle < 0 )
				{
					angle += 1.57;
				}
			};
			if ( (*iter).x() < position.x() )
			{
				angle = -( 1.57 + atan( ((*iter).y() - position.y() ) / ( (*iter).x() - position.x() ) ) );
				if ( angle > 0 )
				{
					angle = -( 1.57 - angle );
				}
			};
			/*else
			{
				angle = 0.0;
			}*/
		};

		osg::Quat rotation( osg::Quat( roll, osg::Vec3( 1.0, 0.0, 0.0 ) ) * osg::Quat( -angle, osg::Vec3(0.0, 0.0, 1.0) ) );
		osg::Quat rotationY( osg::Quat( -( 3.1415926/6.0 ), osg::Vec3( 1.0, 0.0, 0.0 ) ));

		path->insert( time, osg::AnimationPath::ControlPoint( position, rotation ) );
		time += GetRunTime( position, *iter );

		iter--;

	}

	//std::ofstream fout( pathName.c_str() );
	//path->write( fout );
	//fout.close();

	std::ofstream fout(pathName.c_str());
	path->write(fout);
	fout.close();

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
	ctrlPoint->push_back( osg::Vec3( position.x()-2*m_size,		position.y(),				m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()-0.5*m_size,	position.y()-1.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()-0.5*m_size,	position.y()-3.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x(),				position.y()-4.0*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()+0.5*m_size,	position.y()-3.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()+0.5*m_size,	position.y()-1.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()+2.0*m_size,	position.y(),				m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()+0.5*m_size,	position.y()+1.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()+0.5*m_size,	position.y()+3.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x(),				position.y()+4.0*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()-0.5*m_size,	position.y()+3.5*m_size,	m_height ) );
	ctrlPoint->push_back( osg::Vec3( position.x()-0.5*m_size,	position.y()+1.5*m_size,	m_height ) );

	// fill the point in the geometry
	ctrlGeometry->setVertexArray( ctrlPoint.get() );

	// set up format
	osg::DrawElementsUInt* ctrlBase = new osg::DrawElementsUInt();
	ctrlBase->push_back(11);
	ctrlBase->push_back(10);
	ctrlBase->push_back(9);
	ctrlBase->push_back(8);
	ctrlBase->push_back(7);
	ctrlBase->push_back(6);
	ctrlBase->push_back(5);
	ctrlBase->push_back(4);
	ctrlBase->push_back(3);
	ctrlBase->push_back(2);
	ctrlBase->push_back(1);
	ctrlBase->push_back(0);

	// color
	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );

	// set color
	ctrlGeometry->setColorArray( colors );
	ctrlGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
	return (osg::Node*)ctrls;
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

	osg::ref_ptr<osg::Vec3Array> firstLabel = new osg::Vec3Array();
	firstLabel->push_back( osg::Vec3( position.x()-4*m_size, position.y(), m_height ) );
	firstLabel->push_back( osg::Vec3( position.x(), position.y()-4*m_size, m_height ) );
	firstLabel->push_back( osg::Vec3( position.x()+4*m_size, position.y(), m_height ) );
	firstLabel->push_back( osg::Vec3( position.x(), position.y()+4*m_size, m_height ) );

	// put the point into geometry
	firstGeometry->setVertexArray( firstLabel.get() );

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

double ESTPickHandler::GetAllDistance()
{
	float distant = 0.0;

	osg::Vec3Array::iterator iter = point->begin();

	const int size = point->size();

	if ( size <= 1 )
	{
		return 0;
	}
	else
	{
		for ( int i = 0; i < size - 1; i++, iter++ )
		{
			osg::Vec3 temp = *iter;
			iter++;
			distant += sqrt( ( temp.x() - (*iter).x() )*( temp.x() - (*iter).x() ) + ( temp.y() - (*iter).y() )*( temp.y() - (*iter).y() ) );
			iter--;
		};
	}

	return distant;
}

float ESTPickHandler::GetRunTime( osg::Vec3 res, osg::Vec3 des )
{
	float distant = sqrt( ( des.x()-res.x() )*( des.x()-res.x() ) + ( des.y()-res.y() )*( des.y()-res.y() ) );
	double init = sec;
	return (init * distant);
}

void ESTPickHandler::setSecondPerMeter( double second )
{
	sec = second;
}

double ESTPickHandler::getSecondsPerMeter()
{
	return sec;
}

void ESTPickHandler::setUpdateText( osgText::Text* updateText )
{
	m_updateText = updateText;
}
