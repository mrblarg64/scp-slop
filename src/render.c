#include <string.h>
#include <math.h>

#include "player.h"
#include "matrix.h"
#include "camera.h"

// players should be drawn last and each player struct will have a "should draw" char
// if not dog GL_DEPTH_TEST should be enabled
// if dog GL_DEPTH_TEST will be disabled and should draw for players will have different logic
//void render(int width, int height, struct camera *c, struct players *p, struct world *w, struct doors *d, struct items *i
void render(int width, int height, struct scpCamera *camera, struct scpPlayer *p)
{
	float tanfov;
	float pmat[4][4];

	//THIS NEEDS HEAVY DEFUCKULATION BUT WORKS FOR NOW
	//COPIED THIS FROM CNMGR (cnmgr.c)
	
	//proj matrix
	tanfov=tanf(((camera->fov)/360.0f)*(M_PI*2.0f));
	memset(camera->proj,0,4*sizeof(float));
	camera->proj[0][0] = 1.0f/tanfov;
	camera->proj[1][1]=((float)width/(float)height)/tanfov;
	camera->proj[2][2]=(camera->zFar+camera->zNear)/(camera->zFar-camera->zNear);
	camera->proj[2][3]=1.0f;
	camera->proj[3][2]=(-2.0f*camera->zFar*camera->zNear)/(camera->zFar-camera->zNear);

	

	//comment this for legit builds
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (p)
	{
	  for(unsigned i = 0; i < p->model.num_of_meshes; i++)
	    {
	      	glUseProgram(p->model.sprog.program);
		glBindVertexArray(p->model.vabuff[i]);
		glDrawElements(GL_TRIANGLES, p->model.trigscount[i], GL_UNSIGNED_INT, 0);
       	    }
	}
	return;
}
