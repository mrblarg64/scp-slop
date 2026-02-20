
#include <stdio.h>

#include "scp-os.h"
#include "render.h"
#include "errormsg.h"
#include "loadb3d.h"
#include "camera.h"
#include "player.h"
#include <GLFW/glfw3.h>
void keyCallbackFunction(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
		case GLFW_KEY_DOWN:
			if (action==GLFW_PRESS)//||action==GLFW_REPEAT)
			{
				printf("go down\n");
				//loadb3d("resource/models/173/173_2.b3d");
			}
			break;
	}
}

GLFWwindow *openWindow()
{
	GLFWwindow *retval;

	glfwInit();
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES,16);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	retval = glfwCreateWindow(640,480,"SCP",NULL,NULL);
	//window=glfwCreateWindow(1366,768,"hello glfw-3",NULL,NULL);
	glfwMakeContextCurrent(retval);
	//glfwEnable(GLFW_STICKY_KEYS);
	//vsync...
	glfwSetKeyCallback(retval,&keyCallbackFunction);
	glfwSwapInterval(1);

	return retval;
}

void mainloop(GLFWwindow *window)
{
	int width;
	int height;
	double time;//lol
	float fps;
	struct scpPlayer player;
	//delete variables below this comment
	//they are temporary
	struct scpCamera cam;
	(cam.pos)[0]=0;
	(cam.pos)[1]=0;
	(cam.pos)[2]=0;
	cam.fov=65.0f;
	cam.zNear=0.01f;
	cam.zFar=100.0f;

	__builtin_memset(&player, 0, sizeof(player));
	player.model = loadb3d("resource/models/096/scp096.b3d", 1);
	while (!glfwWindowShouldClose(window))
	{
		time=glfwGetTime();
		glfwGetWindowSize(window,&width,&height);
		glViewport(0,0,width,height);

		render(width,height,&cam,&player);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void cleanUp()
{
	printf("k bye\n");
	glfwTerminate();
	errormsg("it worked bitch");
}

int main(int argc, char *argv[])
{
	GLFWwindow *window;
	
	window = openWindow();

	osinit();
	
	mainloop(window);
	cleanUp();
	return 0;
}
