#pragma once
#include <osgGA/CameraManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/Viewer>

class ESTManipulator :
	public osgGA::CameraManipulator
{
public:
	ESTManipulator(void);
	~ESTManipulator(void);

	virtual const char* className() const { return "ESTManipulator"; }

	// set the position of the matrix manipulator using a 4x4 Matrix
	virtual void setByMatrix( const osg::Matrixd& matrix );

	// 
	virtual void setByInverseMatrix( const osg::Matrixd& matrix );

	// get the position of the manipulator as a 4x4 Matrix
	virtual osg::Matrixd getMatrix() const;

	// get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix
	virtual osg::Matrixd getInverseMatrix() const;	

	// move the camera to the default position
	// may be ignored by manipulators if home functionality is not appropriate
	virtual void home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);	

	// handle events, return true if handled, false otherwise
	virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );	

	void setViewer(osgViewer::Viewer* viewer) { m_viewer = viewer; }

	void setLookAtByXyz(const osg::Vec3d eye, const osg::Vec3d lv, const osg::Vec3d up);

	// obj coordinate to screen coordinate
	void getLocalCoordFrame(osg::Vec3d center, double azimuth, osg::Vec3d& vN, osg::Vec3d& vL, osg::Vec3d &vUp);

	void resetPosition();

	void getLookAt(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up);
	void setLookAtByBlh(osg::Vec3d eyeBlh, osg::Vec3d centerBlh, osg::Vec3d up);

	void objectXYZToScreenXy(osg::Vec3d ptO, osg::Vec3d& ptS);
	void screenXyToObjectXYZB(osg::Vec3d screenPt, osg::Vec3d &ptXYZ);

	void LoadParam();

	void SaveParam();

	osg::Vec3d getCenter();


	// NO USED
	//////////////////////////////////////////////////////////////////////////

	virtual void setNode( osg::Node* );

	virtual const osg::Node* getNode() const { return NULL; }

	// get the FusionDistanceMode. Used by SceneView for setting up stereo convergence
	virtual osgUtil::SceneView::FusionDistanceMode getFusionDistanceMode() const 
	{ 
		return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE; 
	}

	// get the FusionDistanceValue. Used by SceneView for setting up stereo convergence
	virtual float getFusionDistanceValue() const { return 1.0; }

	// start/restart the manipulator
	virtual void init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {};

	// get the keyboard and mouse usage of this manipulator
	virtual void getUsage(osg::ApplicationUsage& usage) const {};



private:
	osg::ref_ptr<osg::Node> m_node;

public:
	// 视点
	osg::Vec3d m_eye;
	// 观察点
	osg::Vec3d m_center;
	// 旋转参数
	osg::Quat m_rotation;
	// 观察器
	osgViewer::Viewer* m_viewer;
	// 椭球模式
	osg::EllipsoidModel m_ellipsoid;

	double m_a0;

	osg::Vec3d m_vNe;

	// 保存的沙盘观察参数
	osg::Vec3d m_eyeSaved;
	osg::Quat m_rotationSaved;
	bool m_blSaved;
};