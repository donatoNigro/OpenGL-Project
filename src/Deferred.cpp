#include "Deferred.h"
#include "FlyCamera.h"

#include "tiny_obj_loader.h"

bool Deferred::startup()
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

	buildMesh();
	buildGbuffer();
	buildLightbuffer();
	buildQuad();

	LoadShader("./data/shaders/composite_vertex.glsl", 0, "./data/shaders/composite_fragment.glsl", &m_composite_program);

	LoadShader("./data/shaders/gbuffer_vertex.glsl", 0, "./data/shaders/gbuffer_fragment.glsl", &m_gbuffer_program);

	LoadShader("./data/shaders/composite_vertex.glsl", 0, "./data/shaders/directional_light_fragment.glsl", &m_directional_light_program);

	return true;
}

void Deferred::renderDirectionalLight(vec3 light_dir, vec3 light_color)
{
	light_dir = vec3(0, -1, 0);

	vec4 viewspace_light_dir = myCamera.m_viewTransform * vec4(light_dir, 0);

	light_color = vec3(1, 0, 1);

	int light_dir_uniform = glGetUniformLocation(m_directional_light_program, "light_dir");

	int light_color_uniform = glGetUniformLocation(m_directional_light_program, "light_color");

	glUniform3fv(light_dir_uniform, 1, (float*)&viewspace_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Deferred::buildMesh()
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	tinyobj::LoadObj(shapes, materials, "./data/models/stanford/bunny.obj");

	m_bunny.m_index_count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	std::vector<float> vertex_data;

	vertex_data.reserve(mesh->positions.size() + mesh->normals.size());

	vertex_data.insert(vertex_data.end(), mesh->positions.begin(), mesh->positions.end());

	vertex_data.insert(vertex_data.end(), mesh->normals.begin(), mesh->normals.end());
	

	glGenVertexArrays(1, &m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindVertexArray(m_bunny.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(float)* vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Deferred::buildQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 720) * 0.5f;
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f;

	float vertexData[] =
	{
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
	};

	glGenVertexArrays(1, &m_screenspace_quad.m_VAO);
	glBindVertexArray(m_screenspace_quad.m_VAO);
	glGenBuffers(1, &m_screenspace_quad.m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_screenspace_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Deferred::buildLightbuffer()
{

	//create fbo
	glGenFramebuffers(1, &m_light_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);

	//create light tex
	glGenTextures(1, &m_light_texture);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//attach tex

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_light_texture, 0);

	GLenum light_target = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &light_target);

	//check frame buffer status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error in light framebuffer/n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::buildGbuffer()
{
	glGenFramebuffers(1, &m_gbuffer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);

	glDrawBuffer(GL_COLOR_ATTACHMENT2);
	glClearColor(0, 0, 0, 0);

	glUseProgram(m_gbuffer_program);


	glGenTextures(1, &m_albedo_texture);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_position_texture);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glGenTextures(1, &m_normals_texture);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				  
	glGenRenderbuffers(1, &m_gbuffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gbuffer_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedo_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_position_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normals_texture, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gbuffer_depth);

	GLenum targets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(3, targets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("gbuffer error\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Deferred::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool Deferred::update()
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

void Deferred::draw()
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);
	glClearColor(0, 0, 0, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	vec4 clear_color = vec4(0, 0, 0, 0);
	vec4 clear_normal = vec4(0.5f, 0.5f, 0.5f, 0.5f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 1, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 2, (float*)&clear_normal);

	glUseProgram(m_gbuffer_program);

	int view_uniform = glGetUniformLocation(m_gbuffer_program, "view");

	int view_proj_uniform = glGetUniformLocation(m_gbuffer_program, "view_proj");

	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float*)&myCamera.m_viewTransform);
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&myCamera.m_projectionViewTransform);

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	//light accumulation

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_directional_light_program);

	int position_tex_uniform = glGetUniformLocation(m_directional_light_program, "position_tex");

	int normals_tex_uniform = glGetUniformLocation(m_directional_light_program, "normals_tex");

	glUniform1i(position_tex_uniform, 0);
	glUniform1i(normals_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);

	renderDirectionalLight(vec3(0, -1, 0), vec3(1, 0, 0));



	//composite pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.3f, 0.3f, 0.3f, 1);

	glUseProgram(m_composite_program);

	int albedo_tex_uniform = glGetUniformLocation(m_composite_program, "albedo_tex");

	int light_tex_uniform = glGetUniformLocation(m_composite_program, "light_tex");


	glUniform1i(albedo_tex_uniform, 0);
	glUniform1i(light_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);

	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	



	


	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}