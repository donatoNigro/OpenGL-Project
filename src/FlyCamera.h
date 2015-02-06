#ifndef _FLY_CAMERA_H_
#define _FLY_CAMERA_H_
#include "Camera.h"

class FlyCamera : public Camera
{
 private:

	 float m_fSensitivity;
	 float m_fSpeed;
	 vec3 m_Up;


 public:
	  
	 virtual void update(float a_deltaTime, GLFWwindow* window);
	 void setSpeed(float a_fSpeed);
	 void setSensitivity(float a_fSensitivity);

};


#endif