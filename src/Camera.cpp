#include "Camera.h"


void Camera::setPerspective(float a_fFov, float a_fAspectRatio, float a_fNear, float a_fFar)
{
	m_projectionTransform = glm::perspective(a_fFov, a_fAspectRatio, a_fNear, a_fFar);
}

void Camera::setLookAt(vec3 a_from, vec3 a_to, vec3 a_up)
{
	m_viewTransform = glm::lookAt(a_from, a_to, a_up);
}

void Camera::setPosition(vec3 a_position)
{
	m_worldTransform[3][0] = a_position[0];
	m_worldTransform[3][1] = a_position[1];
	m_worldTransform[3][2] = a_position[2];

}

mat4 Camera::getWorldTransform()
{
	return m_worldTransform;
}

mat4 Camera::getView()
{
	return m_viewTransform;
}

mat4 Camera::getProjection()
{
	return m_projectionTransform;
}

mat4 Camera::getProjectionView()
{
	return m_projectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{
	m_viewTransform = glm::inverse(m_worldTransform);
	m_projectionViewTransform = m_viewTransform * m_projectionTransform;
}

