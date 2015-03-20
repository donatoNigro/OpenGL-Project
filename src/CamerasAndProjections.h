#ifndef _CAM_PROJ_H_
#define _CAM_PROJ_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"

#include "Utility.h"

class CamerasAndProjections : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();


	FlyCamera myCamera;

};



#endif