#ifndef _ANIM_H_
#define _ANIM_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"
#include "FBXFile.h"
#include "Vertex.h"
#include "Utility.h"

class Animation : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	void GenerateGLMeshes(FBXFile* fbx);

	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skelton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);



	float m_timer;
	unsigned int m_program_id;


	FBXFile* m_file;
	std::vector<OpenGLData> m_meshes;
	FlyCamera myCamera;

};



#endif