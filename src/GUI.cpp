#include "GUI.h"
#include "Utility.h"

void OnMouseButton(GLFWwindow* window, int button, int pressed, int mod_keys)
{
	TwEventMouseButtonGLFW(button, pressed);
}

void OnMousePosition(GLFWwindow* window, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}

void OnMouseScroll(GLFWwindow* window, double x, double y)
{
	TwEventMouseWheelGLFW((int)y);
}

void OnKey(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys)
{
	TwEventKeyGLFW(key, pressed);
}

void OnChar(GLFWwindow* window, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}

//When the Window gets resized
void OnWindowResize(GLFWwindow* window, int width, int height)
{
	TwWindowSize(width, height);
	glViewport(0, 0, width, height);
}

bool GUI::Init(const char* a_bar_name, int a_width, int a_height) // START UP //
{
	window = glfwGetCurrentContext();

	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(a_width, a_height);

	m_bar = TwNewBar(a_bar_name);

	return true;
}

bool GUI::Draw()
{
	TwDraw();

	return true;
}

TwBar* GUI::GetBar()
{
	return m_bar;
}

void GUI::AddBarF(const char* bar_name, float* variable, const char* definition)
{
	TwAddVarRW(m_bar, bar_name, TW_TYPE_FLOAT, variable, definition);
}

void GUI::AddBarI(const char* bar_name, int* variable, const char* definition)
{
	TwAddVarRW(m_bar, bar_name, TW_TYPE_INT32, variable, definition);
}

void GUI::AddBarV3(const char* bar_name, vec3* variable, const char* definition)
{
	TwAddVarRW(m_bar, bar_name, TW_TYPE_DIR3F, variable, definition);
}

void GUI::AddBarV4(const char* bar_name, vec4* variable, const char* definition)
{
	TwAddVarRW(m_bar, bar_name, TW_TYPE_COLOR4F, variable, definition);
}