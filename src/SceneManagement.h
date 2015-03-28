#ifndef _SCENE_M_H_
#define _SCENE_M_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"
#include <vector>
#include "Utility.h"
#include "tiny_obj_loader.h"

struct AABB
{
	vec3 min, max;

};

struct MeshObject
{
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;
	unsigned int m_index_count;
	AABB m_aabb;

};

class SceneManagement : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();

	MeshObject LoadMesh(char* filename);
	void DrawMesh(MeshObject mesh);

	virtual bool update();
	virtual void draw();

	std::vector<MeshObject> m_meshes;

	unsigned int m_program_id;

private:
	FlyCamera myCamera;

};



#endif