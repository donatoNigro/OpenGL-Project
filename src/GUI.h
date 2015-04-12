#ifndef _GUI_H_
#define _GUI_H_


#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "AntTweakBar.h"

#include "glm_header.h"

#pragma once


class GUI
{
public:
	bool Init(const char* a_bar_name, int a_width, int a_height);
	
	bool Draw();

	TwBar* GetBar();

	void AddBarF(const char* bar_name, float* variable, const char* definition);
	void AddBarI(const char* bar_name, int* variable, const char* definition);

	void AddBarV3(const char* bar_name, vec3* variable, const char* definition);
	void AddBarV4(const char* bar_name, vec4* variable, const char* definition);
	
	GLFWwindow* window;

	TwBar* m_bar;

};




#endif