#ifndef _CAMERA_H_
#define _CAMERA_H_

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "GLFW/glfw3.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
 protected:
	
	 mat4 m_worldTransform;
	 mat4 m_viewTransform;
	 mat4 m_projectionTransform;
	 mat4 m_projectionViewTransform;


 public:
	
	 virtual void update(float a_deltaTime, GLFWwindow* window) = 0;
	 void setPerspective(float a_fFov, float a_fAspectRatio, float a_fNear, float a_fFar);
	 void setLookAt(vec3 a_from, vec3 a_to, vec3 a_up);
	 void setPosition(vec3 a_position);
	 mat4 getWorldTransform();
	 mat4 getView();
	 mat4 getProjection();
	 mat4 getProjectionView();
	 void updateProjectionViewTransform();


};


#endif