// Manipulator.h

#pragma once
#include <osg/Quat>
#include <osg/Matrix>

#include <osgGA/CameraManipulator>
#include <osgGA/OrbitManipulator>

#include <osgViewer/Viewer>

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/ReadWriteMutex>
#include <OpenThreads/ReentrantMutex>


class ESTManipulator :
	public osgGA::CameraManipulator
{
public:
	ESTManipulator(void);
	~ESTManipulator(void);
};

