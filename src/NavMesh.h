#ifndef _NAVMESH_H_
#define _NAVMESH_H_
#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "FlyCamera.h"
#include "Vertex.h"
#include <vector>
#include "Utility.h"

struct NavMeshNode
{
	vec3 position;

	vec3 corners[3];

	NavMeshNode*edges[3];

	unsigned int flags;
	float edge_costs[3];


};

class NavMesh : public Application
{

public:

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	//program for the sponza shader
	//store the sponza mesh we're rendering

	unsigned int m_program_id;
	OpenGLData m_sponza_mesh;
	std::vector<NavMeshNode> m_nav_nodes;

	FlyCamera myCamera;

};



#endif