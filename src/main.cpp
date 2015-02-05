#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include <cstdio>
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;


int main()
{

	if (glfwInit() == false)
	{
		return -1;
	}
	
	

	GLFWwindow* window = glfwCreateWindow(1280, 720, "!!!", nullptr, nullptr);
	
	if (window == nullptr)
	{
		return -2;
	}

	

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	int major_version = ogl_GetMajorVersion();

	int minor_version = ogl_GetMinorVersion();
	printf("successfully loaded OpenGL version %d.%d\n", major_version, minor_version);


	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f/720.0f, 0.1f, 1000.0f);

	float camera_x = -10.0f;
	float camera_y = -10.0f;
	float camera_z = -10.0f;

	float timer = 0.0f;

	glfwSetTime(0.0);
	
	while (glfwWindowShouldClose(window) == false)
	{
		float dt = glfwGetTime();
		glfwSetTime(0.0);

		
			
			
				timer += dt;
			
			
		
		view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
		
		camera_x = sinf(timer) * 10;
		camera_y = sinf(timer) * -5;
		camera_z = cosf(timer) * 10;
		
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

		mat4 sun_transform = glm::rotate(timer, vec3(0, 1, 0));
		mat4 child_one_transform = sun_transform * glm::translate(vec3(3, 0, 0)) * glm::rotate(timer * 2.f, vec3(0, 1, 0));
		mat4 translation = glm::translate( vec3(5, 0, 0));
		mat4 transform = translation * sun_transform;
		mat4 child_two_transform = child_one_transform * glm::translate(vec3(3, 0, 0));
		mat4 child_three_transform = child_one_transform * glm::translate(vec3(-3, 0, 0));

		//Gizmos::addAABB(sun_transform[3].xyz, 1.0f, 25, 25, red, &sun_transform);

		Gizmos::addCylinderFilled(sun_transform[3].xyz, 1.0f, 1.0f, 10, red, &sun_transform);

		Gizmos::addSphere(child_one_transform[3].xyz, 1.5f, 25, 25, blue, &child_one_transform);
		Gizmos::addSphere(child_two_transform[3].xyz, 2.2f, 25, 25, green, &child_two_transform);
		Gizmos::addSphere(child_three_transform[3].xyz, 2.2f, 25, 25, green, &child_three_transform);


	Gizmos::draw(projection, view);


		

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	Gizmos::destroy();

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}