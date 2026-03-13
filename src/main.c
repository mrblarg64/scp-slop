
#include <stdio.h>

#include "scp-os.h"
#include "render.h"
#include "errormsg.h"
#include "b3dloader.h"
#include "camera.h"
#include "player.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

// this function needs to be in haskell notation, not readable as is
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

	case GLFW_KEY_ESCAPE:
	  if(action == GLFW_PRESS)
	    glfwSetWindowShouldClose(window, true);

	  break;
	}
}


void framebufferSizeCallback(GLFWwindow* w, int width, int height)
{
  glViewport(0,0,width,height);
}


int loadUp(GLFWwindow* w, struct scpCamera* cam, struct scpPlayer* player)
{  
	(cam->pos)[0]=0;
	(cam->pos)[1]=0;
	(cam->pos)[2]=0;
	cam->fov=65.0f;
	cam->zNear=0.01f;
	cam->zFar=100.0f;

	
	if(B3DLoader("resource/models/096/scp096.b3d", &(player->model)))
	{
	  // will have to clean up player allocations
	  errormsg("B3D loader error\n");
	  return -1;
	}
	
  
  if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
  {
    errormsg("Failed to initialize GLAD\n");
    return -1;
  }

  //window=glfwCreateWindow(1366,768,"hello glfw-3",NULL,NULL);
  
  glViewport(0,0,640,480);
  //glfwEnable(GLFW_STICKY_KEYS); 
  glfwSetFramebufferSizeCallback(w, framebufferSizeCallback);
  glfwSetKeyCallback(w, keyCallbackFunction);
    //vsync...
  glfwSwapInterval(1);

  if(compileshaders())
  {
    // failed to compile
    return -1;
  }

  return 0;
}


GLFWwindow* openWindow()
{
	GLFWwindow* retval;

	glfwInit();
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES,16);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	/*
	  For Mac OS X to compile we need
	  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	 */
	
	retval = glfwCreateWindow(640,480,"SCP",NULL,NULL);
	if(retval == NULL)
	{
	  errormsg("Failed to create Window\n");
	  glfwTerminate();
	  return NULL;
	}

	glfwMakeContextCurrent(retval);
	
	return retval;
}

void cleanUp(struct scpPlayer* p)
{
	printf("k bye\n");
	glfwTerminate();
	SCPFREE(p->model.trigscount);
	SCPFREE(p->model.vabuff);
	printf("it worked bitch\n");
}

int main(int argc, char *argv[])
{
  	int width;
	int height;
	double time;
	float fps;
	struct scpPlayer player;
	//delete variables below this comment
	//they are temporary
	struct scpCamera cam;

	GLFWwindow *window;

	__builtin_memset(&player, 0, sizeof(player));
	
	window = openWindow();
	if(window == NULL)
	  return -1;
	
	if(loadUp(window, &cam, &player))
	{
	  // error
	  glfwTerminate();
	  return -1;
	}

	osinit();

	/* main loop  */
	while (!glfwWindowShouldClose(window))
	{
		time=glfwGetTime();
		

	        render(width,height,&cam,&player);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/* --------- */
		
	cleanUp(&player);
	return 0;
}
