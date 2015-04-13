#include "Assignment.h"
#include "FlyCamera.h"

bool Assignment::startup()
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
	//myCamera.setLookAt(vec3(-1, -1, -1), vec3(-1, -1, -1), vec3(-1, -1, -1));
	myCamera.setPerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	g_pressed = false;

	grid_active = true;
	buildGrid(vec2(30, 30), glm::ivec2(128, 128));

	m_dims = glm::ivec2(128, 128);
	m_octaves = 6;
	m_persistence = 0.6f;
	m_scale = 12.0f;

	last_dims = m_dims;
	last_octaves = m_octaves;
	last_persistence = m_persistence;

	buildPerlinTexture(m_dims, m_octaves, m_persistence);

	LoadShader("./data/shaders/perlin_vertex.glsl", nullptr, "./data/shaders/perlin_fragment.glsl", &m_perlin_progam_id);


	TerrainWindow.Init("Terrain", 1280, 720);

	TwAddSeparator(TerrainWindow.GetBar(), "TERRAIN BAR", "");

	TerrainWindow.AddBarI("Octaves", &m_octaves, "min = 0 max = 24 step = 1");
	TerrainWindow.AddBarF("Persisistence", &m_persistence, "min = 0.0f max = 0.99f step = 0.03");
	TerrainWindow.AddBarF("Scale", &m_scale, "");

	m_model1 = new FBXFile();

	//loads fbx file which stores filepaths to all necessary files for loading the model
	m_model1->load("./data/models/characters/Enemyelite/Enemyelite.fbx");

	//loads textures needed for the model and creates openGL handles for the textures
	m_model1->initialiseOpenGLTextures();

	GenerateGLMeshes(m_model1);

	LoadShader("./data/shaders/skinned_vertex.glsl", nullptr, "./data/shaders/skinned_fragment.glsl", &m_fbx_program_id);

	srand(time(0));

	return true;
}


void Assignment::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool Assignment::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt;

	myCamera.update(dt, m_window);

	Gizmos::clear();

	Input();

	if (m_dims != last_dims || last_octaves != m_octaves || last_persistence != m_persistence)
	{
		buildPerlinTexture(m_dims, m_octaves, m_persistence);
		last_dims = m_dims;
		last_octaves = m_octaves;
		last_persistence = m_persistence;
	}

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	if (grid_active == true)
	{
		for (int i = 0; i <= 20; i++)
		{

			Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
				i == 10 ? white : black);

			Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
				i == 10 ? white : black);
		}
	}

	FBXSkeleton* skele = m_model1->getSkeletonByIndex(0);

	FBXAnimation* anim = m_model1->getAnimationByIndex(0);

	EvaluateSkeleton(anim, skele, m_timer);

	for (unsigned int i = 0; i < skele->m_boneCount; ++i)
	{

		skele->m_nodes[i]->updateGlobalTransform();
		mat4 node_global = skele->m_nodes[i]->m_globalTransform;
		vec3 node_pos = node_global[3].xyz;

		Gizmos::addAABBFilled(node_pos, vec3(0.5f), vec4(1, 0, 0, 1), &node_global);

		if (skele->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_pos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(node_pos, parent_pos, vec4(0, 1, 0, 1));

		}

	}

	return true;
}

void Assignment::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(myCamera.getProjectionView());

	//perlin noise shader program
	glUseProgram(m_perlin_progam_id);

	int perlin_view_proj_uniform =
		glGetUniformLocation(m_perlin_progam_id, "view_proj");
	glUniformMatrix4fv(perlin_view_proj_uniform, 1,
		GL_FALSE, (float*)&myCamera.getProjectionView());

	int perlin_texture_uniform =
		glGetUniformLocation(m_perlin_progam_id, "perlin_texture");
	glUniform1i(perlin_texture_uniform, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	int perlin_scale_uniform =
		glGetUniformLocation(m_perlin_progam_id, "scale");
	glUniform1f(perlin_scale_uniform, m_scale);

	int perlin_persistence_uniform =
		glGetUniformLocation(m_perlin_progam_id, "persistence");
	glUniform1f(perlin_persistence_uniform, m_persistence);

	int perlin_min_uniform =
		glGetUniformLocation(m_perlin_progam_id, "min");
	glUniform1f(perlin_min_uniform, MIN);

	int perlin_max_uniform =
		glGetUniformLocation(m_perlin_progam_id, "max");
	glUniform1f(perlin_max_uniform, MAX);

	glBindVertexArray(m_plane_mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_mesh.m_index_count, GL_UNSIGNED_INT, 0);

	//ant tweak bar draw
	TerrainWindow.Draw();

	//FBX shader program
	glUseProgram(m_fbx_program_id);

	int fbx_view_proj_uniform = glGetUniformLocation(m_fbx_program_id, "projection_view");
	glUniformMatrix4fv(fbx_view_proj_uniform, 1, GL_FALSE, (float*)&myCamera.getProjectionView());

	int fbx_diffuse_uniform = glGetUniformLocation(m_fbx_program_id, "diffuse");
	glUniform1i(fbx_diffuse_uniform, 0);

	FBXSkeleton* skeleton = m_model1->getSkeletonByIndex(0);

	UpdateBones(skeleton);

	int fbx_bones_uniform = glGetUniformLocation(m_fbx_program_id, "bones");

	glUniformMatrix4fv(fbx_bones_uniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
	
	//draw model to the screen
	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode* curr_mesh = m_model1->getMeshByIndex(i);

		FBXMaterial* mesh_material = curr_mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 world_transform = curr_mesh->m_globalTransform;

		int fbx_world_uniform = glGetUniformLocation(m_fbx_program_id, "world");
		glUniformMatrix4fv(fbx_world_uniform, 1, GL_FALSE, (float*)&world_transform);
		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_index_count, GL_UNSIGNED_INT, 0);

	}

	glfwSwapBuffers(m_window);
	glfwPollEvents();

}

void Assignment::Input()
{
	GLFWwindow* window = glfwGetCurrentContext();

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && g_pressed == false)
	{
		g_pressed = true;
		if (grid_active == true)
		{
			grid_active = false;
		}
		else
			grid_active = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		g_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		reloadShader();
	}
}

void Assignment::GenerateGLMeshes(FBXFile* fbx)
{
	unsigned int mesh_count = fbx->getMeshCount();
	m_meshes.resize(mesh_count);
	for (unsigned int mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
	{
		FBXMeshNode * curr_mesh = fbx->getMeshByIndex(mesh_index);
		m_meshes[mesh_index].m_index_count = curr_mesh->m_indices.size();
		m_meshes.resize(mesh_count);
		glGenBuffers(1, &m_meshes[mesh_index].m_VBO);
		glGenBuffers(1, &m_meshes[mesh_index].m_IBO);
		glGenVertexArrays(1, &m_meshes[mesh_index].m_VAO);

		glBindVertexArray(m_meshes[mesh_index].m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* curr_mesh->m_vertices.size(), curr_mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* curr_mesh->m_indices.size(), curr_mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);//pos
		glEnableVertexAttribArray(1);//texcoord
		glEnableVertexAttribArray(2);//bone indices
		glEnableVertexAttribArray(3);//bone weight

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


}

void Assignment::EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer)
{
	float fps = 24.f;
	int current_frame = (int)(timer * fps);

	//loop through tracks
	for (unsigned int track_index = 0; track_index < anim->m_trackCount; ++track_index)
	{

		//wrap back to the start of the track if we've gone too far
		int track_frame_count = anim->m_tracks[track_index].m_keyframeCount;
		int track_frame = current_frame % track_frame_count;

		//find what keyframes are currently affecting the bones
		FBXKeyFrame curr_frame = anim->m_tracks[track_index].m_keyframes[track_frame];
		FBXKeyFrame next_frame = anim->m_tracks[track_index].m_keyframes[(track_frame + 1) % track_frame_count];



		float time_since_frame_flip = timer - (current_frame / fps);
		float t = time_since_frame_flip * fps;


		//interpolate between those keyframes to generate the matrix for the current pose

		vec3 new_pos = glm::mix(curr_frame.m_translation, next_frame.m_translation, t);
		vec3 new_scale = glm::mix(curr_frame.m_scale, next_frame.m_scale, t);
		glm::quat new_rot = glm::slerp(curr_frame.m_rotation, next_frame.m_rotation, t);

		mat4 local_transform = glm::translate(new_pos) * glm::toMat4(new_rot) * glm::scale(new_scale);
		int bone_index = anim->m_tracks[track_index].m_boneIndex;


		//set the FBXNode's local transform
		if (bone_index < skeleton->m_boneCount)
		{
			skeleton->m_nodes[bone_index]->m_localTransform = local_transform;
		}

	}

}

void Assignment::UpdateBones(FBXSkeleton* skeleton)
{
	//rattle me bones!	

	//loop through the nodes in the skeleton

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		//generate their global transforms 

		int parent_index = skeleton->m_parentIndex[bone_index];

		if (parent_index == -1)
		{
			skeleton->m_bones[bone_index] = skeleton->m_nodes[bone_index]->m_localTransform;
		}
		else
		{
			skeleton->m_bones[bone_index] = skeleton->m_bones[parent_index] * skeleton->m_nodes[bone_index]->m_localTransform;
		}

	}

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{

		//multiply the global transform by the inverse bind pose
		skeleton->m_bones[bone_index] = skeleton->m_bones[bone_index] * skeleton->m_bindPoses[bone_index];

	}

}


void Assignment::buildGrid(vec2 real_dims, glm::ivec2 dims)
{
	//compute how many vertices we need
	unsigned int vertex_count = (dims.x + 1) * (dims.y + 1);

	//allocate vertex data
	VertexTexCoord* vertex_data = new VertexTexCoord[vertex_count];

	//compute how many indices we need
	unsigned int index_count = dims.x * dims.y * 6;

	//allocate index data
	unsigned int* index_data = new unsigned int[index_count];

	//create two nested FOR loops
	float curr_y = -real_dims.y / 2.0f;
	for (int y = 0; y < dims.y + 1; ++y)
	{
		float curr_x = -real_dims.x / 2.0f;
		for (int x = 0; x < dims.x + 1; ++x)
		{
			//create points
			vertex_data[y * (dims.x + 1) + x].position =
				vec4(curr_x, 0, curr_y, 1);

			vertex_data[y * (dims.x + 1) + x].tex_coord =
				vec2((float)x / (float)dims.x, (float)y / (float)dims.y);

			curr_x += real_dims.x / (float)dims.x;
		}
		curr_y += real_dims.y / (float)dims.y;
	}

	//generate our index_data
	int curr_index = 0;
	for (int y = 0; y < dims.y; ++y)
	{
		for (int x = 0; x < dims.x; x++)
		{
			//create 6 indices for each quad

			//traingle 1
			index_data[curr_index++] = y * (dims.x + 1) + x;
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + x;
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);

			//triangle 2
			index_data[curr_index++] = (y + 1) * (dims.x + 1) + (x + 1);
			index_data[curr_index++] = y * (dims.x + 1) + (x + 1);
			index_data[curr_index++] = y * (dims.x + 1) + x;
		}
	}

	m_plane_mesh.m_index_count = index_count;

	//create VertexArrayObject, buffers etc
	glGenVertexArrays(1, &m_plane_mesh.m_VAO);
	glGenBuffers(1, &m_plane_mesh.m_VBO);
	glGenBuffers(1, &m_plane_mesh.m_IBO);

	//bind and fill buffers
	glBindVertexArray(m_plane_mesh.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane_mesh.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_mesh.m_IBO);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(VertexTexCoord) * vertex_count, vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * index_count, index_data, GL_STATIC_DRAW);



	//tell opengl about our vertex structure
	glEnableVertexAttribArray(0);	//position
	glEnableVertexAttribArray(1);	//tex coord

	//Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(VertexTexCoord), 0);

	//Tex Coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(VertexTexCoord), (void*)sizeof(vec4));

	//unbind stuff
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//free data
	delete[] vertex_data;
	delete[] index_data;
}

void Assignment::buildPerlinTexture(glm::ivec2 dims, int octaves, float persistence)
{
	//set scale
	float scale = (1.0f / dims.x) * 3.0f;

	//allocate memory for perlin data
	m_perlin_data = new float[dims.x * dims.y];

	MIN = FLT_MAX;
	MAX = -FLT_MAX;

	//loop through all the pixels
	for (int y = 0; y < dims.y; y++)
	{
		for (int x = 0; x < dims.x; x++)
		{
			float amplitude = 1;
			float frequency = 1;

			//generate the perlin noise
			m_perlin_data[y * dims.x + x] = 0;

			//loop over  anumber of octaves
			for (int o = 0; o < octaves; o++)
			{
				//each octave will double the frequency of the previous one
				//we scale the points to achieve this
				float perlin_sample =
					glm::perlin(vec2(x, y) * scale * frequency) * 0.5f + 0.5f; //call glm::perlin function

				//each subsequent octave will contribute less and less to the final height
				//to achieve this, mutliply by a smaller and smaller number

				perlin_sample *= amplitude;
				m_perlin_data[y * dims.x + x] += perlin_sample;

				if (m_perlin_data[y * dims.x + x] < MIN)
				{
					MIN = m_perlin_data[y * dims.x + x];
				}
				if (m_perlin_data[y * dims.x + x] > MAX)
				{
					MAX = m_perlin_data[y * dims.x + x];
				}

				amplitude *= persistence;
				frequency *= 2.0f;
			}
		}
	}

	//generate opengl texture handles
	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);


	//pass perlin data to texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dims.x, dims.y, 0,
		GL_RED, GL_FLOAT, m_perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Assignment::reloadShader()
{
	glDeleteProgram(m_perlin_progam_id);

	LoadShader("./data/shaders/perlin_vertex.glsl", nullptr, "./data/shaders/perlin_fragment.glsl", &m_perlin_progam_id);
}