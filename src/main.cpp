#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "Assignment.h"


int main()
{
	Assignment app;
	if (app.startup() == false)
	{
		return -1;
	}

	while (app.update() == true)
	{
		app.draw();
	}

	app.shutdown();
	
	return 0;
}