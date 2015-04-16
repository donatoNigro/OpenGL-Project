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
#include "FBXFile.h"
#include "Utility.h"
#include "Vertex.h"
#include <vector>
#include "GUI.h"

class Assignment : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void Input();

	void buildGrid(vec2 real_dims, glm::ivec2 dims);
	void buildPerlinTexture(glm::ivec2 dims, int octaves, float persistence);
	void reloadShader();

	FlyCamera myCamera;

	bool grid_active;
	bool g_pressed;
	
	//perlin noise
	OpenGLData m_plane_mesh;
	unsigned int m_perlin_program_id;
	unsigned int m_perlin_texture;
	float* m_perlin_data;

	glm::ivec2 m_dims;
	int m_octaves;
	float m_persistence;
	float m_scale;

	glm::ivec2 last_dims;
	int last_octaves;
	float last_persistence;

	//gui
	GUI TerrainWindow;

	float MIN;
	float MAX;

	//fbx
	unsigned int m_fbx_program1;
	FBXFile* m_model1;

	unsigned int m_fbx_program2;
	FBXFile* m_model2;

	void GenerateGLMeshes(FBXFile* fbx, std::vector<OpenGLData> &meshes);
	void FBXDraw(unsigned int program_id, FBXFile* file, std::vector<OpenGLData> &meshes, int tex_num);
	void FBXUpdate(FBXFile* file);

	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skelton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);

	std::vector<OpenGLData> m_meshes;
	std::vector<OpenGLData> m_meshes2;
	float m_timer;

	void UpdateNormals(FBXFile* model, std::vector<OpenGLData> &meshes);

	//lighting

	float ambient_light_value;
	vec3 ambient_light;
	vec3 light_dir;
	vec3 light_color;
	vec3 material_color;

	float m_specular_power;

	void LoadTextures(const char* a_diffuse_file,
		const char* a_normal_file,
		const char* a_specular_file);

	unsigned int m_diffuse_texture;
	unsigned int m_normal_texture;
	unsigned int m_specular_texture;

};



#endif