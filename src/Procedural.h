#ifndef _PROCEDURAL_H_
#define _PROCEDURAL_H_
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

class Procedural : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();
	
	void buildGrid(vec2 real_dims, glm::ivec2 dims);

	void buildPerlinTexture();

	OpenGLData m_plane_mesh;
	unsigned int m_program_id;
	unsigned int m_perlin_texture;
	float* m_perlin_data;

	FlyCamera myCamera;

};



#endif