#ifndef _QUATERNIONS_H_
#define _QUATERNIONS_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"
struct Keyframe
{
	vec3 pos;
	glm::quat rot;
};


class Quaternions : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	float m_timer;
	
	mat4 m_hip_bone;
	mat4 m_knee_bone;
	mat4 m_ankle_bone;

	Keyframe m_hip_frames[2];
	Keyframe m_knee_frames[2];
	Keyframe m_ankle_frames[2];

private:
	FlyCamera myCamera;

};



#endif