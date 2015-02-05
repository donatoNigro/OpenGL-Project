#ifndef _INTRO_GL_H_
#define _INTRO_GL_H_
#include "Application.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class IntroToOpenGL : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	float m_camera_x;
	float m_camera_y;
	float m_camera_z;
	float m_timer;

	mat4 m_projection;
	mat4 m_view;

};

#endif