#ifndef _POST_PROC_H_
#define _POST_PROC_H_
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

#include "glm_header.h"

class PostProcess : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void generateFrameBuffer();
	void generateScreenspaceQuad();

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	unsigned int m_post_program_id;

	OpenGLData m_quad;
	

	FlyCamera myCamera;

};



#endif