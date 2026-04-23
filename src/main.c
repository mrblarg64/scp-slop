
#include <stdio.h>

#include "scp-os.h"
#include "render.h"
#include "errormsg.h"
#include "b3dloader.h"
#include "camera.h"
#include "player.h"
#include "stb_image.h"


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
	int bretval;

	(cam->pos)[0]=0;
	(cam->pos)[1]=0;
	(cam->pos)[2]=0;
	cam->fov=1.650f;
	cam->znear=0.01f;
	cam->zfar=100.0f;

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		errormsg("Failed to initialize GLAD\n");
		return -1;
	}

	if(compileshaders())
	{
		// failed to compile
		return -1;
	}

	player->model = SCPMALLOC(sizeof(struct scpmodel));
	// bretval = B3DLoader("resource/models/096/scp096.b3d", player->model);
	/*if(bretval)
	{
		printf("%i\n", bretval);
		// will have to clean up player allocations
		errormsg("B3D loader error\n");
		return -1;
	} */
	

	//window=glfwCreateWindow(1366,768,"hello glfw-3",NULL,NULL);
  
	glViewport(0,0,640,480);
	//glfwEnable(GLFW_STICKY_KEYS); 
	glfwSetFramebufferSizeCallback(w, framebufferSizeCallback);
	glfwSetKeyCallback(w, keyCallbackFunction);
	//vsync...
	glfwSwapInterval(1);


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

       
	#ifdef __APPLE__  
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
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
	//SCPFREE(p->model.trigscount);
	//SCPFREE(p->model.vabuff);
	printf("it worked bitch\n");
}

int main(int argc, char *argv[])
{
        
  	int width = 1024;
	int height = 1024;
	double time;
	float fps;
	struct scpPlayer player;
	//delete variables below this comment
	//they are temporary
	struct scpCamera cam = {0};

	GLFWwindow *window;

	/* sky box shit, can relocate to better spot brain */
	unsigned int skyVBO, skyEBO, skyVAO;
	int skybox_width, skybox_height, skybox_nrChannels;
	unsigned char *data;
	unsigned int texture_skybox_id;
	const float sky_vcoord[] = {
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f		
	};

	const int sky_indices[] = {
		0,1,2,
		0,2,3, // front face
		1,5,6,
		6,2,1, // right face
		4,5,6,
		6,7,4, // back face
		3,0,4,
		4,7,3 // left face		
	};

	glGenVertexArrays(1, &skyVAO);

	glGenBuffers(1, &skyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sky_vcoord),\
		     sky_vcoord, GL_STATIC_DRAW);
		
	glGenBuffers(1, &skyEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sky_indices),\
		     sky_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
        // delocate
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
       	
	const char* texture_faces[] = {
	  "resource/skybox/right.jpg",
	  "resource/skybox/left.jpg",
	  "resource/skybox/top.jpg",
	  "resource/skybox/bottom.jpg",
	  "resource/skybox/back.jpg",
	  "resource/skybox/front.jpg"};
	glGenTextures(1, &texture_skybox_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox_id);

	for(int i = 0; i < 6; i++)
	{
		
		data = stbi_load(texture_faces[i], &skybox_width, &skybox_height, \
				 &skybox_nrChannels, 0);
		if(!data)
		{
			errormsg("invalid path for cubemap");
			stbi_image_free(data);
			return -1;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, \
			     skybox_width, skybox_height, 0,		\
			     GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	  
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	/*---------------------------------------------------- */

	__builtin_memset(&player, 0, sizeof(player));

	cam.fov = 1.65f;
	cam.znear = 0.1f;
	cam.zfar = 100.0f;
	
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
