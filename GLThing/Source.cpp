#include <iostream>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <aie\Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "Application.h"

using namespace glm;

int main()
{
	Application *app = new DemoApp();
	if (app->init() == true)
	{
		while (app->update() == true)
		{
			app->draw();
		}
		app->exit();
	}

	delete app;
	return 0;
}