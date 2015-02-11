#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "glm_header.h"

struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	GLFWwindow *m_window;

};

#endif