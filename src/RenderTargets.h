#ifndef _RENDER_H_
#define _RENDER_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"
#include "Utility.h"
#include "Vertex.h"


class RenderTargets : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void GenerateFrameBuffer();
	void GeneratePlane();


	OpenGLData m_plane;

	unsigned int m_program_id;

	FlyCamera myCamera;

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

};



#endif