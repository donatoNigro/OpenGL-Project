#include "SceneManagement.h"
#include "FlyCamera.h"





//planes array needs to be 6 elements large
void GetFrustumPlane(mat4 transform, vec4* planes)
{
	// right side
	planes[0] = vec4(transform[0][3] - transform[1][0],
		transform[1][3] - transform[1][0],
		transform[2][3] - transform[2][0],
		transform[3][3] - transform[3][0]);
	// left side
	planes[1] = vec4(transform[0][3] + transform[0][0],
		transform[1][3] + transform[1][0],
		transform[2][3] + transform[2][0],
		transform[3][3] + transform[3][0]);
	// top
	planes[2] = vec4(transform[0][3] - transform[0][1],
		transform[1][3] - transform[1][1],
		transform[2][3] - transform[2][1],
		transform[3][3] - transform[3][1]);
	// bottom
	planes[3] = vec4(transform[0][3] + transform[0][1],
		transform[1][3] + transform[1][1],
		transform[2][3] + transform[2][1],
		transform[3][3] + transform[3][1]);
	// far
	planes[4] = vec4(transform[0][3] - transform[0][2],
		transform[1][3] - transform[1][2],
		transform[2][3] - transform[2][2],
		transform[3][3] - transform[3][2]);
	// near
	planes[5] = vec4(transform[0][3] + transform[0][2],
		transform[1][3] + transform[1][2],
		transform[2][3] + transform[2][2],
		transform[3][3] + transform[3][2]);
	for (int i = 0; i < 6; i++)
		planes[i] = glm::normalize(planes[i]);

}

AABB GenerateAABB(vec3* positions, unsigned int count, unsigned int stride = 0)
{
	AABB result = {};
	result.min = positions[0];
	result.max = positions[0];

	if (stride == 0)
	{
		stride = sizeof(vec3);
	}

	for (unsigned int pos_index = 0; pos_index < count; ++pos_index)
	{

		if (positions->x < result.min.x)result.min.x = positions->x;
		if (positions->y < result.min.y)result.min.y = positions->y;
		if (positions->z < result.min.z)result.min.z = positions->z;

		if (positions->x > result.max.x)result.max.x = positions->x;
		if (positions->y > result.max.y)result.max.y = positions->y;
		if (positions->z > result.max.z)result.max.z = positions->z;

	}	

	return result;

}

bool OnPositivePlaneSide(vec4 plane, AABB aabb)
{

	vec3 plane_testA, plane_testB;

	vec3 normal = vec3(plane);
	if (plane.x >= 0)
	{
		plane_testA.x = aabb.min.x;
		plane_testB.x = aabb.max.x;
	}
	else
	{
		plane_testA.x = aabb.max.x;
		plane_testB.x = aabb.min.x;
	}

	if (plane.x >= 0)
	{
		plane_testA.y = aabb.min.y;
		plane_testB.y = aabb.max.y;
	}
	else
	{
		plane_testA.y = aabb.max.y;
		plane_testB.y = aabb.min.y;
	}

	if (plane.z >= 0)
	{
		plane_testA.z = aabb.min.z;
		plane_testB.z = aabb.max.z;
	}
	else
	{
		plane_testA.z = aabb.max.z;
		plane_testB.z = aabb.min.z;
	}

	float dA = glm::dot(vec3(plane), plane_testA) + plane.w;
	float dB = glm::dot(vec3(plane), plane_testB) + plane.w;

	return (dA >= 0 || dB >= 0);

}

bool SceneManagement::startup()
{
	if (Application::startup() == false)
	{
		return false;

	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	myCamera.setSpeed(6.0f);
	myCamera.setSensitivity(2.0f);
	myCamera.setPosition(vec3(20, 20, 20));
	myCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	myCamera.setPerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	//load meshes

	m_meshes.push_back(LoadMesh("./models/stanford/bunny.obj")  );


	return true;
}

MeshObject SceneManagement::LoadMesh(char* obj_filename)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	tinyobj::LoadObj(shapes, materials, obj_filename);

	unsigned int mesh_count = m_meshes.size();

	m_meshes.resize(m_meshes.size() + shapes.size());

	for (unsigned int shape_index = 0; shape_index < shapes.size; ++shape_index)
	{	
		unsigned int mesh_count = m_meshes.size();
		unsigned int mesh_index = mesh_count + shape_index;
		std::vector<float> vertex_data;

		unsigned int float_count = shapes[shape_index].mesh.positions.size();
		float_count += shapes[shape_index].mesh.normals.size();
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(), shapes[shape_index].mesh.positions.begin(), shapes[shape_index].mesh.positions.end());
		vertex_data.insert(vertex_data.end(), shapes[shape_index].mesh.normals.begin(), shapes[shape_index].mesh.normals.end());

		m_meshes[mesh_index].m_index_count = shapes[shape_index].mesh.indices.size();

		glGenVertexArrays(1, &m_meshes[mesh_index].m_vao);
		glGenBuffers(1, &m_meshes[mesh_index].m_vbo);
		glGenBuffers(1, &m_meshes[mesh_index].m_ibo);

		glBindVertexArray(m_meshes[mesh_index].m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[mesh_index].m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* float_count, vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[mesh_index].m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shape_index].mesh.indices.size() * sizeof(unsigned int), shapes[shape_index].mesh.indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)* shapes[shape_index].mesh.positions.size()));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_meshes[mesh_index].m_aabb = GenerateAABB((vec3*)&shapes[shape_index].mesh.positions[0], shapes[shape_index].mesh.positions.size() / 3);		

	}

}

void SceneManagement::DrawMesh(MeshObject mesh)
{
	glBindVertexArray(mesh.m_vao);
	glDrawElements(GL_TRIANGLES, mesh.m_index_count, GL_UNSIGNED_INT, 0);

}

void SceneManagement::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool SceneManagement::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();

	vec4 white(1);

	vec4 black(0, 0, 0, 1);

	vec4 red(1, 0, 0, 1);

	vec4 green(0, 1, 0, 1);

	vec4 blue(0, 0, 1, 1);
	myCamera.update(dt, glfwGetCurrentContext());

	Gizmos::addTransform(mat4(1));
	for (int i = 0; i <= 20; ++i)
	{

		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	Gizmos::draw(myCamera.getProjection(), myCamera.getView());

	return true;
}

void SceneManagement::draw()
{
	vec4 planes[6];
	GetFrustumPlane(myCamera.getProjectionView(), planes);

	for (unsigned int mesh_index = 0; mesh_index < m_meshes.size(); ++mesh_index)
	{

		bool is_in_frustum = true;

		for (unsigned int plane_index = 0; plane_index < 0; ++plane_index)
	{
			if (OnPositivePlane(planes[plane_index], m_meshes[mesh_index.m_aabb))
			{
				is_in_frustum = false;
				break;
			}
		
			if (is_in_frustum)
			{
				DrawMesh(m_meshes[mesh_index]);
			}

	}

	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program_id);

	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&myCamera.getProjectionView());


	int ambient_uniform = glGetUniformLocation(m_program_id, "ambient_light");
	int light_dir_uniform = glGetUniformLocation(m_program_id, "light_dir");
	int light_color_uniform = glGetUniformLocation(m_program_id, "light_color");
	int material_uniform = glGetUniformLocation(m_program_id, "material_color");

	int eye_pos_uniform = glGetUniformLocation(m_program_id, "eye_pos");

	int specular_power_uniform = glGetUniformLocation(m_program_id, "specular_power");
	glUniform3fv(material_uniform, 1, (float*)&material_color);
	glUniform3fv(ambient_uniform, 1, (float*)&ambient_light);
	glUniform3fv(light_dir_uniform, 1, (float*)&light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);


	vec3 camera_pos = myCamera.getWorldTransform()[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);

	glUniform1f(specular_power_uniform, m_specular_power);


	for (unsigned int mesh_index = 0; mesh_index < m_gl_data.size(); ++mesh_index)
	{

		glBindVertexArray(m_gl_data[mesh_index].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[mesh_index].m_index_count, GL_UNSIGNED_INT, 0);
	}


	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}