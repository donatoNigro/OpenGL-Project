#include "Animation.h"
#include "FlyCamera.h"

bool Animation::startup()
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

	m_file = new FBXFile();
	//loads fbx file which stores filepaths to all necessary files for loading the pyro model
	m_file->load("./models/characters/enemynormal/EnemyNormal.fbx");

	//loads textures needed for the model and creates openGL handles for the textures
	m_file->initialiseOpenGLTextures();

	GenerateGLMeshes(m_file);

	LoadShader("./shaders/skinned_vertex.glsl", "./shaders/skinned_fragment.glsl", &m_program_id);

	return true;
}

void Animation::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
	m_file->unload();
	delete m_file;
}

bool Animation::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt;
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

	

	FBXSkeleton* skele = m_file->getSkeletonByIndex(0);

	FBXAnimation* anim = m_file->getAnimationByIndex(0);

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

void Animation::draw()
{
	Gizmos::draw(myCamera.getProjection(), myCamera.getView());
	glUseProgram(m_program_id);

	int view_proj_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&myCamera.getProjectionView());

	int diffuse_uniform = glGetUniformLocation(m_program_id, "diffuse");
	glUniform1i(diffuse_uniform, 0);


	FBXSkeleton* skeleton = m_file->getSkeletonByIndex(0);

	UpdateBones(skeleton);

	int bones_uniform = glGetUniformLocation(m_program_id, "bones");

	glUniformMatrix4fv(bones_uniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);



	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode* curr_mesh = m_file->getMeshByIndex(i);
		

		FBXMaterial* mesh_material = curr_mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 world_transform = curr_mesh->m_globalTransform;

		int world_uniform = glGetUniformLocation(m_program_id, "world");
		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&world_transform);
		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_index_count, GL_UNSIGNED_INT, 0);

	}
	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}

void Animation::GenerateGLMeshes(FBXFile* fbx)
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

void Animation::EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer)
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





void Animation::UpdateBones(FBXSkeleton* skeleton)
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
			skeleton->m_bones[bone_index] = skeleton->m_nodes[parent_index]->m_localTransform * skeleton->m_bones[bone_index];	

		}
		


	}

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{

	//multiply the global transform by the inverse bind pose
	skeleton->m_bones[bone_index] = skeleton->m_bindPoses[bone_index] * skeleton->m_bones[bone_index];
		
	}

}