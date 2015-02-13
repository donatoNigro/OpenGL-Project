#include "Texturing.h"

#include "Vertex.h"

#include "Utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Texturing::startup()
{
	if (Application::startup() == false)
	{
		return false;

	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	loadTexture("./crate.png");
	generateQuad(5.0f);
	LoadShader("./shaders/textured_vertex.glsl", "./shaders/texture_fragment.glsl", &m_program);

	myCamera.setSpeed(6.0f);
	myCamera.setSensitivity(2.0f);
	myCamera.setPosition(vec3(20, 20, 20));
	myCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	
	myCamera.setPerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);


	return true;
}

void Texturing::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool Texturing::update()
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

void Texturing::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);
	int view_proj_uniform = glGetUniformLocation(m_program, "projection_view");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&myCamera.getProjectionView());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	int diffuse_location = glGetUniformLocation(m_program, "diffuse");

	glUniform1i(diffuse_location, 0);



	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	Gizmos::draw(myCamera.getProjection(), myCamera.getView());
	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}

void Texturing::loadTexture(const char* filename)
{
	int width;
	int height;

	int channels;

	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

}

void Texturing::generateQuad(float size)
{

	VertexTexCoord vertex_data[4];
	
	
	vertex_data[0].position = vec4(-size, 0, -size, 1);
	vertex_data[1].position = vec4(-size, 0, size, 1);
	vertex_data[2].position = vec4(size, 0, size, 1);
	vertex_data[3].position = vec4(size, 0, -size, 1);


	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);



	unsigned int index_data[6] = { 0, 2, 1, 0, 3, 2 };

	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)* 4, vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //tex coord


	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}