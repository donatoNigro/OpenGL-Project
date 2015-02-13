#ifndef _LIGHTING_H_
#define _LIGHTING_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"

#include "tiny_obj_loader.h"

struct OpenGLData
{

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;

};

class Lighting : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void reloadShader();
	
	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);

	void cleanUpOpenGLBuffers();

	unsigned int m_program_id;

	FlyCamera myCamera;

	vec3 ambient_light;
	vec3 light_dir;
	vec3 light_color;
	vec3 material_color;

	float m_specular_power;

	std::vector<OpenGLData> m_gl_data;
	
};



#endif