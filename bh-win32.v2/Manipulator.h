//these demo codes are designed,coded and tested  
//by jack 20130729
//QQ:771702827 

#pragma once

#include <osg/Quat>
#include <osg/Matrixd>

#include <osgGA/CameraManipulator>
#include <osgGA/OrbitManipulator>

#include <osgViewer/Viewer>

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/ReadWriteMutex>
#include <OpenThreads/ReentrantMutex>

#include <queue>

using namespace osg;
using namespace osgGA;


//todo:如何由矩阵反求观察点,中心点,向上向量
class  estreetGlobeManipulator : public osgGA::CameraManipulator
{
    public:

        estreetGlobeManipulator();

        virtual const char* className() const { return "Terrain"; }

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        virtual void setByMatrix(const osg::Matrixd& matrix);
    
		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
        virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }
    
		/** get the position of the manipulator as 4x4 Matrix.*/
        virtual osg::Matrixd getMatrix() const;
     
		/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
        virtual osg::Matrixd getInverseMatrix() const;

        /** Get the FusionDistanceMode. Used by SceneView for setting up stereo convergence.*/
        virtual osgUtil::SceneView::FusionDistanceMode getFusionDistanceMode() const { return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE; }

        /** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
        virtual float getFusionDistanceValue() const { return 1.0; }

		virtual void setNode(osg::Node*){};

        /** Return node if attached.*/
		virtual const osg::Node* getNode() const{return NULL;};

        /** Return node if attached.*/
		virtual osg::Node* getNode(){return NULL;};

        /** Move the camera to the default position. 
            May be ignored by manipulators if home functionality is not appropriate.*/
        virtual void home(const GUIEventAdapter& ea,GUIActionAdapter& us);
        
        /** Start/restart the manipulator.*/
		virtual void init(const GUIEventAdapter& ea,GUIActionAdapter& us){};

        /** handle events, return true if handled, false otherwise.*/
        virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& us);

        /** Get the keyboard and mouse usage of this manipulator.*/
		virtual void getUsage(osg::ApplicationUsage& usage) const{};
	
protected:
        virtual ~estreetGlobeManipulator();
public:

	void setViewer(osgViewer::Viewer *viewer){_Viewer=viewer;}

	void setLookAtByXyz(const osg::Vec3d eye,const osg::Vec3d lv,const osg::Vec3d up);
	//obj coor to screen coor
	void getLocalCoordFrame(osg::Vec3d center,double azimuth,osg::Vec3d &vN,osg::Vec3d &vL,osg::Vec3d &vUp);

	void resetPosition();
	
	void getLookAt(osg::Vec3d &eye,osg::Vec3d &center,osg::Vec3d &up);
	void setLookAtByBlh(osg::Vec3d eyeBlh,osg::Vec3d centerBlh,osg::Vec3d up);
	
	void objectXYZToScreenXy(osg::Vec3d ptO,osg::Vec3d &ptS);
	void screenXyToObjectXYZB(osg::Vec3d screenPt,osg::Vec3d &ptXYZ);

private:
	
	void  getUp(osg::Vec3d &vy);
public:       
	//观察点,视线中心
    osg::Vec3d              _eye,_center;
	//旋转参数
    osg::Quat               _rotation;

	osgViewer::Viewer* _Viewer;
	osg::EllipsoidModel _ellipsoid;
	
	double _a0;
	osg::Vec3d _vNe;

	//保存的沙盘观察参数
    osg::Vec3d              _eyeSaved;
    osg::Quat               _rotationSaved;
	bool _blSaved;

};



