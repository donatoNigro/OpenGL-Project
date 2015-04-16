#include "Assignment.h"
#include "FlyCamera.h"
#include "stb_image.h"

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

	//lighting
	ambient_light_value = 0.1f;

	ambient_light = vec3(ambient_light_value);
	light_dir = vec3(0, -1, 0);
	light_color = vec3(1, 1, 1);
	material_color = vec3(1);

	m_specular_power = 10.0f;

	//perlin noise
	m_dims = glm::ivec2(128, 128);
	m_octaves = 6;
	m_persistence = 0.8f;
	m_scale = 1.0f;

	last_dims = m_dims;
	last_octaves = m_octaves;
	last_persistence = m_persistence;

	buildPerlinTexture(m_dims, m_octaves, m_persistence);

	//perlin shader
	LoadShader("./data/shaders/perlin_vertex.glsl", nullptr, "./data/shaders/perlin_fragment.glsl", &m_perlin_program_id);

	//gui
	TerrainWindow.Init("Terrain", 1280, 720);

	TwAddSeparator(TerrainWindow.GetBar(), "TERRAIN BAR", "");

	TerrainWindow.AddBarI("Octaves", &m_octaves, "min = 0 max = 24 step = 1");
	TerrainWindow.AddBarF("Persisistence", &m_persistence, "min = 0.0f max = 0.99f step = 0.03");
	TerrainWindow.AddBarF("Specular Power", &m_specular_power, "min = 0.0f");
	TerrainWindow.AddBarF("Ambient light", &ambient_light_value, "min = 0.0f step = 0.5");
	TerrainWindow.AddBarV3("Light Direction", &light_dir, "");
	srand(time(0));

	//load models
	m_model1 = new FBXFile();
	//loads fbx file which stores filepaths to all necessary files for loading the pyro model
	m_model1->load("./data/models/characters/enemynormal/EnemyNormal.fbx");

	//loads textures needed for the model and creates openGL handles for the textures
	m_model1->initialiseOpenGLTextures();

	GenerateGLMeshes(m_model1, m_meshes);

	m_model2 = new FBXFile();
	//loads fbx file which stores filepaths to all necessary files for loading the pyro model
	m_model2->load("./data/models/characters/Marksman/Marksman.fbx");

	//loads textures needed for the model and creates openGL handles for the textures
	m_model2->initialiseOpenGLTextures();

	GenerateGLMeshes(m_model2, m_meshes2);

	//model shaders
	LoadShader("./data/shaders/skinned_vertex.glsl", nullptr, "./data/shaders/skinned_fragment.glsl", &m_fbx_program1);

	LoadShader("./data/shaders/skinned_vertex.glsl", nullptr, "./data/shaders/skinned_fragment.glsl", &m_fbx_program2);

	LoadTextures("./data/rock_diffuse.tga",
		"./data/rock_normal.tga",
		"./data/rock_specular.tga");

	return true;
}


void Assignment::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();

	m_model1->unload();
	delete m_model1;

}

bool Assignment::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	m_timer += dt;
	glfwSetTime(0.0f);

	myCamera.update(dt, m_window);
	
	Gizmos::clear();

	Input();
	ambient_light = vec3(ambient_light_value);
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

	FBXUpdate(m_model1);
	FBXUpdate(m_model2);
	UpdateNormals(m_model1, m_meshes);
	UpdateNormals(m_model2, m_meshes2);

	return true;
}

void Assignment::FBXUpdate(FBXFile* file)
{

	FBXSkeleton* skele = file->getSkeletonByIndex(0);

	FBXAnimation* anim = file->getAnimationByIndex(0);

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
}

void Assignment::FBXDraw(unsigned int program_id, FBXFile* file, std::vector<OpenGLData> &meshes, int tex_num)
{
	glUseProgram(program_id);

	int view_proj_uniform = glGetUniformLocation(program_id, "projection_view");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&myCamera.getProjectionView());

	int diffuse_uniform = glGetUniformLocation(program_id, "diffuse");
	glUniform1i(diffuse_uniform, 0);

	FBXSkeleton* skeleton = file->getSkeletonByIndex(0);

	UpdateBones(skeleton);

	int bones_uniform = glGetUniformLocation(program_id, "bones");

	glUniformMatrix4fv(bones_uniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	int ambient_uniform = glGetUniformLocation(program_id, "ambient_light");
	int light_dir_uniform = glGetUniformLocation(program_id, "light_dir");
	int light_color_uniform = glGetUniformLocation(program_id, "light_color");
	int material_uniform = glGetUniformLocation(program_id, "material_color");

	int eye_pos_uniform = glGetUniformLocation(program_id, "eye_pos");

	int specular_power_uniform = glGetUniformLocation(program_id, "specular_power");
	glUniform3fv(material_uniform, 1, (float*)&material_color);
	glUniform3fv(ambient_uniform, 1, (float*)&ambient_light);
	glUniform3fv(light_dir_uniform, 1, (float*)&light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);


	vec3 camera_pos = myCamera.getWorldTransform()[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);

	glUniform1f(specular_power_uniform, m_specular_power);

	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		FBXMeshNode* curr_mesh = file->getMeshByIndex(i);


		FBXMaterial* mesh_material = curr_mesh->m_material;

		
		glActiveTexture(GL_TEXTURE0);
		

		glBindTexture(GL_TEXTURE_2D, mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 world_transform = curr_mesh->m_globalTransform;

		int world_uniform = glGetUniformLocation(program_id, "world");
		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&world_transform);
		glBindVertexArray(meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, meshes[i].m_index_count, GL_UNSIGNED_INT, 0);

	}
}

void Assignment::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(myCamera.getProjectionView());


	glUseProgram(m_perlin_program_id);

	int view_proj_uniform =
		glGetUniformLocation(m_perlin_program_id, "view_proj");
	glUniformMatrix4fv(view_proj_uniform, 1,
		GL_FALSE, (float*)&myCamera.getProjectionView());

	int perlin_texture_uniform =
		glGetUniformLocation(m_perlin_program_id, "perlin_texture");
	glUniform1i(perlin_texture_uniform, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);


	int scale_uniform =
		glGetUniformLocation(m_perlin_program_id, "scale");
	glUniform1f(scale_uniform, m_scale);

	int persistence_uniform =
		glGetUniformLocation(m_perlin_program_id, "persistence");
	glUniform1f(persistence_uniform, m_persistence);

	int min_uniform =
		glGetUniformLocation(m_perlin_program_id, "MIN");
	glUniform1f(min_uniform, MIN);

	int max_uniform =
		glGetUniformLocation(m_perlin_program_id, "MAX");
	glUniform1f(max_uniform, MAX);

	int ambient_uniform = glGetUniformLocation(m_perlin_program_id, "ambient_light");
	int light_dir_uniform = glGetUniformLocation(m_perlin_program_id, "light_dir");
	int light_color_uniform = glGetUniformLocation(m_perlin_program_id, "light_color");
	int material_uniform = glGetUniformLocation(m_perlin_program_id, "material_color");

	int eye_pos_uniform = glGetUniformLocation(m_perlin_program_id, "eye_pos");

	int specular_power_uniform = glGetUniformLocation(m_perlin_program_id, "specular_power");
	glUniform3fv(material_uniform, 1, (float*)&material_color);
	glUniform3fv(ambient_uniform, 1, (float*)&ambient_light);
	glUniform3fv(light_dir_uniform, 1, (float*)&light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);


	vec3 camera_pos = myCamera.getWorldTransform()[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);

	glUniform1f(specular_power_uniform, m_specular_power);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_normal_texture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);

	int diffuse_location =
		glGetUniformLocation(m_perlin_program_id, "diffuse_tex");

	int normal_location =
		glGetUniformLocation(m_perlin_program_id, "normal_tex");

	int specular_location =
		glGetUniformLocation(m_perlin_program_id, "specular_tex");

	glUniform1i(diffuse_location, 0);
	glUniform1i(normal_location, 1);
	glUniform1i(specular_location, 2);


	glBindVertexArray(m_plane_mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_mesh.m_index_count, GL_UNSIGNED_INT, 0);

	TerrainWindow.Draw();

	FBXDraw(m_fbx_program1, m_model1, m_meshes, 1);
	FBXDraw(m_fbx_program2, m_model2, m_meshes2, 2);

	glfwSwapBuffers(m_window);
	glfwPollEvents();

}

void Assignment::UpdateNormals(FBXFile* model, std::vector<OpenGLData> &meshes)
{
	unsigned int mesh_count = model->getMeshCount();
	meshes.resize(mesh_count);
	for (unsigned int mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
	{
		FBXMeshNode * curr_mesh = model->getMeshByIndex(mesh_index);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* curr_mesh->m_vertices.size(), curr_mesh->m_vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);

	}
	
}

void Assignment::GenerateGLMeshes(FBXFile* fbx, std::vector<OpenGLData> &meshes)
{
	unsigned int mesh_count = fbx->getMeshCount();
	meshes.resize(mesh_count);
	for (unsigned int mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
	{
		FBXMeshNode * curr_mesh = fbx->getMeshByIndex(mesh_index);
		meshes[mesh_index].m_index_count = curr_mesh->m_indices.size();
		meshes.resize(mesh_count);
		glGenBuffers(1, &meshes[mesh_index].m_VBO);
		glGenBuffers(1, &meshes[mesh_index].m_IBO);
		glGenVertexArrays(1, &meshes[mesh_index].m_VAO);

		glBindVertexArray(meshes[mesh_index].m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* curr_mesh->m_vertices.size(), curr_mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* curr_mesh->m_indices.size(), curr_mesh->m_indices.data(), GL_STATIC_DRAW);

		

		glEnableVertexAttribArray(0);//pos
		glEnableVertexAttribArray(1);//tex coord
		glEnableVertexAttribArray(2);//normal
		glEnableVertexAttribArray(3);//bone weight
		glEnableVertexAttribArray(4);//bone indices
	

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
		
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


}

void Assignment::LoadTextures(const char* a_diffuse_file, const char* a_normal_file, const char* a_specular_file)
{
	int width = 0;
	int height = 0;
	int channels;

	//DIFFUSE
	unsigned char* data = stbi_load(a_diffuse_file,
		&width, &height,
		&channels, STBI_default);

	glGenTextures(1, &m_diffuse_texture);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);

	//type of channel, actual data providing it (image)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);


	//NORMAL
	data = stbi_load(a_normal_file,
		&width, &height,
		&channels, STBI_default);

	glGenTextures(1, &m_normal_texture);
	glBindTexture(GL_TEXTURE_2D, m_normal_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);


	//SPECULAR
	data = stbi_load(a_specular_file,
		&width, &height,
		&channels, STBI_default);

	glGenTextures(1, &m_specular_texture);
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);

	//,type of channel, actual data providing it (image)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	//Texture Filtering Options			(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR or GL_NEAREST_MIPMAP_NEAREST)
	//What to use when pixels are too big 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//What to use when pixels are too small
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

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

void Assignment::buildGrid(vec2 real_dims, glm::ivec2 dims)
{
	//compute how many vertices we need
	unsigned int vertex_count = (dims.x + 1) * (dims.y + 1);
	
	//allocate vertex data
	VertexNormal* vertex_data = new VertexNormal[vertex_count];

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

			vertex_data[y * (dims.x + 1) + x].normal = vec4(0, 1, 0, 0);
			vertex_data[y * (dims.x + 1) + x].tangent = vec4(1, 0, 0, 0);

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

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal)* vertex_count, vertex_data, GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * index_count, index_data, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);	//position
	glEnableVertexAttribArray(1);	//tex coord
	glEnableVertexAttribArray(2);   //normal
	glEnableVertexAttribArray(3);   //tangent

	//Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0);

	//Tex Coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void*)sizeof(vec4));

	//normal
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
		sizeof(VertexNormal), (void*)(sizeof(vec4)* 3));

	//tangent
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
		sizeof(VertexNormal), (void*)(sizeof(vec4)* 3));

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
	glDeleteProgram(m_perlin_program_id);

	LoadShader("./data/shaders/perlin_vertex.glsl", nullptr, "./data/shaders/perlin_fragment.glsl", &m_perlin_program_id);
}