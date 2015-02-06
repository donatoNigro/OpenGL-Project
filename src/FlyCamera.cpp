#include "FlyCamera.h"

void FlyCamera::update(float a_deltaTime, GLFWwindow* window)
{

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_worldTransform[2] * m_fSpeed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[2] * m_fSpeed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_worldTransform[0] * m_fSpeed * a_deltaTime;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_worldTransform[0] * m_fSpeed * a_deltaTime;
	}

	double x_delta, y_delta;

	glfwGetCursorPos(window, &x_delta, &y_delta);
	glfwSetCursorPos(window, 1280.f / 2.f, 720.f / 2.f);

	x_delta -= (1280.f / 2.f);
	y_delta -= (720.f / 2.f);

	x_delta /= (1280.f / 2.f);
	y_delta /= (720.f / 2.f);

	x_delta *= m_fSensitivity;
	y_delta *= m_fSensitivity;

	if (glfwGetMouseButton(window, 1))
	{

		vec3 camera_right = (vec3)m_worldTransform[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rot = yaw * pitch;

		m_worldTransform[0] = rot * m_worldTransform[0];
		m_worldTransform[1] = rot * m_worldTransform[1];
		m_worldTransform[2] = rot * m_worldTransform[2];

	}

	updateProjectionViewTransform();

}


void FlyCamera::setSpeed(float a_fSpeed)
{

	m_fSpeed = a_fSpeed;
}

void FlyCamera::setSensitivity(float a_fSensitivity)
{

	m_fSensitivity = a_fSensitivity;
}