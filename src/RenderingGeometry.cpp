#include "RenderingGeometry.h"
#include "Vertex.h"
#include "glm_header.h"
#include "Utility.h"

bool RenderingGeometry::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	generateShader();
	generateGrid(10, 10);
	glClearColor(0.3f, 0.3f, 0.3f, 0.3f);

	Gizmos::create();
	myCamera = new FlyCamera;	
	myCamera->setSpeed(6.0f);
	myCamera->setSensitivity(2.0f);
	myCamera->setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	myCamera->setPerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
			
	glEnable(GL_DEPTH_TEST);
	

	return true;
}

void RenderingGeometry::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool RenderingGeometry::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	
	Gizmos::clear();
	vec4 white(1);

	vec4 black(0, 0, 0, 1);
	myCamera->update(dt, glfwGetCurrentContext());
	Gizmos::addTransform(mat4(1));
	for (int i = 0; i <= 20; ++i)
	{

		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}
//	Gizmos::draw(myCamera->getProjection(), myCamera->getView());
	return true;
}

void RenderingGeometry::draw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program_id);
	unsigned int proj_view_handle = glGetUniformLocation(m_program_id, "projection_view");
	if (proj_view_handle >= 0)
	{ 
		glUniformMatrix4fv(proj_view_handle, 1, false, (float*)&myCamera->getProjectionView());
	}
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
	Gizmos::draw(myCamera->getProjection(), myCamera->getView());
	 
	Application::draw();

	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}

void RenderingGeometry::generateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertex_array = new Vertex[(rows + 1) * (cols + 1)];

	for (unsigned int r = 0; r < rows + 1; ++r)
	{

		for (unsigned int c = 0; c < cols + 1; ++c)
		{
			vec4 pos = vec4((float)c, 0, (float)r, 1);
			vertex_array[r * (cols + 1) + c].position = pos;
			
			vec4 color = vec4((float)c / (cols + 1), 0, (float)r / (rows + 1), 1);
			vertex_array[c + r * (cols + 1)].color = color;

		}
	}
	m_index_count = rows * cols * 6;
	unsigned int *index_array = new unsigned int[m_index_count];

	int index_location = 0;

	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			
			index_array[index_location + 0] = r * (cols + 1) + c;
			index_array[index_location + 1] = (r + 1) * (cols + 1) + c;
			index_array[index_location + 2] = r * (cols + 1) + (c + 1);

			index_array[index_location + 3] = r * (cols + 1) + (c + 1);
			index_array[index_location + 4] = (r + 1) * (cols + 1) + c;
			index_array[index_location + 5] = (r + 1) * (cols + 1) + (c + 1);

			index_location += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (cols + 1) * (rows + 1) * sizeof(Vertex), vertex_array, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof (vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count, index_array, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] index_array;
	delete[] vertex_array;



}

void RenderingGeometry::generateShader()
{

	const char * vs_source = "#version 410\n"
		"layout(location = 0) in vec4 position;"
		"layout(location = 1) in vec4 color;\n"
		"out vec4 out_color;\n"
		"uniform mat4 projection_view;"
		"void main()"
		"{"
		"out_color = color;\n"
		"gl_Position = projection_view * position;"
		"}";

	const char * fs_source = "#version 410 \n "
							 "in vec4 out_color;\n"  
							 "out vec4 frag_color; \n"
							 "void main()\n "
							 "{\n "
							 
							 "frag_color = out_color; \n"
							 
							 "} \n";

	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader, 1, &vs_source, 0);
	glCompileShader(vertex_shader);

	glShaderSource(fragment_shader, 1, &fs_source, 0);
	glCompileShader(fragment_shader);

	m_program_id = glCreateProgram();
	glAttachShader(m_program_id, vertex_shader);
	glAttachShader(m_program_id, fragment_shader);
	glLinkProgram(m_program_id);

	int success = GL_FALSE;
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int log_length = 0;
		glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &log_length);

		char* log = new char[log_length];
		glGetProgramInfoLog(m_program_id, log_length, 0,log);

		printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");
		printf("%s", log);
		delete[] log;
	}

	glGetShaderiv(vertex_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int log_length = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);

		char* log = new char[log_length];
		glGetShaderInfoLog(vertex_shader, log_length, 0, log);

		printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");
		printf("%s", log);
		delete[] log;
	}

	glGetShaderiv(fragment_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int log_length = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);

		char* log = new char[log_length];
		glGetShaderInfoLog(fragment_shader, log_length, 0, log);

		printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");
		printf("%s", log);
		delete[] log;
	}


	

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

}