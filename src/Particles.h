#ifndef _PARTICLES_H_
#define _PARTICLES_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"
#include "Emitter.h"
#include "Utility.h"

class Particles : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	Emitter m_emitter;
	unsigned int m_program_id;
private:
	FlyCamera myCamera;

};



#endif