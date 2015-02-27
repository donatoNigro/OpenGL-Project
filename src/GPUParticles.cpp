#include "GPUParticles.h"
#include "FlyCamera.h"

bool GPUParticles::startup()
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

	m_emitter.Init(10000, vec3(0, 0, 0), 0, 5, 5, 0.8f, 1.0f, 1, 0.01f, vec4(1, 0.4f, 0, 1), vec4(1, 0, 0, 1));
	
	m_time = 0;

	return true;
}

void GPUParticles::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool GPUParticles::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_time += dt;

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
	m_emitter.Draw(m_time, myCamera.m_worldTransform, myCamera.m_projectionViewTransform);
	Gizmos::draw(myCamera.getProjection(), myCamera.getView());

	return true;
}

void GPUParticles::draw()
{
	

	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}