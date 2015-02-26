#include "Particles.h"
#include "FlyCamera.h"

bool Particles::startup()
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
	

	LoadShader("./shaders/particle_vertex.glsl", "./shaders/particle_fragment.glsl", &m_program_id);
	m_emitter.Init(5000, vec4(0, 0, 0, 1), 1000, 1, 2, 1.0f, 1.5f, 1, 0.75f, vec4(0, 1, 0, 1), vec4(1, 0, 4, 0));
	

	return true;
}

void Particles::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool Particles::update()
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
	m_emitter.Update(dt);
	m_emitter.UpdateVertexData(myCamera.m_worldTransform[3].xyz, myCamera.m_worldTransform[2].xyz);
	Gizmos::draw(myCamera.getProjection(), myCamera.getView());

	return true;
}

void Particles::draw()
{
	glUseProgram(m_program_id);
	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&myCamera.m_projectionViewTransform);

	m_emitter.Render();
	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}