#ifndef _DEFFERED_H_
#define _DEFFERED_H_
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

class Deferred : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void renderDirectionalLight(vec3 light_dir, vec3 light_color);


	void buildMesh();
	void buildQuad();
	void buildGbuffer();
	void buildLightbuffer();

	//mesh
	OpenGLData m_bunny;
	OpenGLData m_screenspace_quad;


	//generate g buffer
	unsigned int m_gbuffer_fbo;
	unsigned int m_albedo_texture;
	unsigned int m_position_texture;
	unsigned int m_normals_texture;
	unsigned int m_gbuffer_depth;
	
	//render lights
	unsigned int m_light_fbo;
	unsigned int m_light_texture;

	//shaders
	unsigned int m_gbuffer_program;
	unsigned int m_directional_light_program;
	unsigned int m_point_light_program;
	unsigned int m_spot_light_program;
	unsigned int m_composite_program;



	FlyCamera myCamera;

};



#endif