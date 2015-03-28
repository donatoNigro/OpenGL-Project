#ifndef _SHADOWS_H_
#define _SHADOWS_H_
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


#include "tiny_obj_loader.h"
#include <vector>

class Shadows : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void buildMeshes();

	void buildShadowMap();

	OpenGLData m_bunny;
	OpenGLData m_plane;

	unsigned int m_fbo;
	unsigned int m_fbo_depth;

	unsigned int m_shadowmap_program;
	unsigned int m_diffuse_shadowed_program;

	vec3 m_light_dir;
	mat4 m_light_matrix;

	FlyCamera myCamera;

};



#endif