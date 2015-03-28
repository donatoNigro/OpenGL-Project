#include "IntroToOpenGL.h"



bool IntroToOpenGL::startup()
{
	if (Application::startup() == false)
	{
		return false;

	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	m_camera_x = -10.0f;
	m_camera_y = -10.0f;
	m_camera_z = -10.0f;

	m_timer = 0.0f;


	return true;
}

void IntroToOpenGL::shutdown()
{
	Gizmos::destroy();
}

bool IntroToOpenGL::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_timer += dt;


	m_view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));

	m_camera_x = sinf(m_timer) * 10;
	m_camera_y = sinf(m_timer) * -5;
	m_camera_z = cosf(m_timer) * 10;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	vec4 white(1);

	vec4 black(0, 0, 0, 1);

	vec4 red(1, 0, 0, 1);

	vec4 green(0, 1, 0, 1);

	vec4 blue(0, 0, 1, 1);


	Gizmos::addTransform(mat4(1));
	for (int i = 0; i <= 20; ++i)
	{

		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	mat4 sun_transform = glm::rotate(m_timer, vec3(0, 1, 0));
	mat4 child_one_transform = sun_transform * glm::translate(vec3(3, 0, 0)) * glm::rotate(m_timer * 2.f, vec3(0, 1, 0));
	mat4 translation = glm::translate(vec3(5, 0, 0));
	mat4 transform = translation * sun_transform;
	mat4 child_two_transform = child_one_transform * glm::translate(vec3(3, 0, 0));
	mat4 child_three_transform = child_one_transform * glm::translate(vec3(-3, 0, 0));

	//Gizmos::addAABB(sun_transform[3].xyz, 1.0f, 25, 25, red, &sun_transform);

	Gizmos::addCylinderFilled(sun_transform[3].xyz, 1.0f, 1.0f, 10, red, &sun_transform);

	Gizmos::addSphere(child_one_transform[3].xyz, 1.5f, 25, 25, blue, &child_one_transform);
	Gizmos::addSphere(child_two_transform[3].xyz, 2.2f, 25, 25, green, &child_two_transform);
	Gizmos::addSphere(child_three_transform[3].xyz, 2.2f, 25, 25, green, &child_three_transform);


	Gizmos::draw(m_projection, m_view);





	return true;
}

void IntroToOpenGL::draw()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}