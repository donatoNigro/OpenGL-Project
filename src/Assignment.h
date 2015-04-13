#ifndef _ASSIGNMENT_H_
#define _ASSIGNMENT_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"

#include "Utility.h"
#include "Vertex.h"
#include "FBXFile.h"
#include "GUI.h"

class Assignment : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera myCamera;

	//perlin noise terrain stuff
	void Input();
	
	void buildGrid(vec2 real_dims, glm::ivec2 dims);
	void buildPerlinTexture(glm::ivec2 dims, int octaves, float persistence);
	void reloadShader();

	bool grid_active;
	bool g_pressed;

	OpenGLData m_plane_mesh;
	unsigned int m_perlin_progam_id;
	unsigned int m_perlin_texture;
	float* m_perlin_data;

	glm::ivec2 m_dims;
	int m_octaves;
	float m_persistence;
	float m_scale;

	glm::ivec2 last_dims;
	int last_octaves;
	float last_persistence;

	//ant tweak bar
	GUI TerrainWindow;

	float MIN;
	float MAX;

	//FBX
	void GenerateGLMeshes(FBXFile* fbx);
	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skelton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);

	float m_timer;
	unsigned int m_fbx_program_id;

	FBXFile* m_model1;
	std::vector<OpenGLData> m_meshes;



};



#endif