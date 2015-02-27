#ifndef _GPU_P_H_
#define _GPU_P_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"

#include "GPUEmitter.h"

class GPUParticles : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();


	FlyCamera myCamera;

	GPUPointEmitter m_emitter;

	float m_time;

};



#endif