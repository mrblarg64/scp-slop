#include <string.h>
#include <math.h>

#include "player.h"
#include "matrix.h"
#include "camera.h"

// players should be drawn last and each player struct will have a "should draw" char
// if not dog GL_DEPTH_TEST should be enabled
// if dog GL_DEPTH_TEST will be disabled and should draw for players will have different logic
//void render(int width, int height, struct camera *c, struct players *p, struct world *w, struct doors *d, struct items *i
void render(int width, int height, struct scpCamera *camera, struct scpPlayer *players, unsigned vao,\
	    float lyaw, float lpitch)
{
	float tanfov;
	//float m[4][4];
	float v[4][4];
	float p[4][4];
	float mvp[4][4];
	float rot[3][3]; // matrix is only yaw and pitch
	unsigned char curuni;

	//proj matrix
	/* tanfov=__builtin_tanf(((camera->fov)/360.0f)*(M_PI*2.0f)); */
	/* memset(camera->proj,0,4*sizeof(float)); */
	/* camera->proj[0][0] = 1.0f/tanfov; */
	/* camera->proj[1][1]=((float)width/(float)height)/tanfov; */
	/* camera->proj[2][2]=(camera->zFar+camera->zNear)/(camera->zFar-camera->zNear); */
	/* camera->proj[2][3]=1.0f; */
	/* camera->proj[3][2]=(-2.0f*camera->zFar*camera->zNear)/(camera->zFar-camera->zNear); */

	//todo don't recalculate as much of this shit every frame
	//(fov, znear, etc..)
	//like some dumbass using glm and shit grade libraries
	//SCP_MATRIX_CAMERA_F((*camera), v);
	//SCP_MATRIX_PERSPECTIVE_F((*camera), width, height, p);
	//SCP_MATRIX_MULT_SQUARE_F(4, p, v, mvp);


	//comment this for legit builds
	// glClearColor(0,0,0,0);
	//glClear(GL_COLOR_BUFFER_BIT);

	
	//p is supposed to be a linked list
	//but this will do for now
	players = NULL; // don't need that shit right now
	if (players)
	{
		for(unsigned i = 0; i < players->model->num_of_meshes; i++)
		{
			glUseProgram(scpshad[players->model->sprog].program);
			curuni = 0;
			while (curuni != scpshad[players->model->sprog].unicount)
			{
				switch (scpshad[players->model->sprog].unitypes[curuni])
				{
				case SCP_GL_UNIFORM_MATRIX_4FV:
					glUniformMatrix4fv(scpshad[players->model->sprog].uniforms[curuni], 1, GL_FALSE, (float*)mvp);
					break;
				}
				curuni++;
			}
			glBindVertexArray(players->model->vabuff[i]);
			glDrawElements(GL_TRIANGLES, players->model->trigscount[i], GL_UNSIGNED_INT, 0);
		}
	}

	// skybox here, render last!
	// can use variables as to not recaluate
	rot[0][0] = __builtin_cos(-lyaw * M_PI / 180.0f) * __builtin_cos(-lpitch * M_PI / 180.0f);
	rot[0][1] = -1 * __builtin_sin(-lyaw * M_PI / 180.0f); 
	rot[0][2] = __builtin_cos(-lyaw * M_PI / 180.0f) * __builtin_sin(-lpitch * M_PI / 180.0f);
	rot[1][0] = __builtin_sin(-lyaw * M_PI / 180.0f) * __builtin_cos(-lpitch * M_PI / 180.0f);
	rot[1][1] = __builtin_cos(-lyaw * M_PI / 180.0f);
	rot[1][2] = __builtin_sin(-lpitch * M_PI / 180.0f) * __builtin_sin(-lyaw * M_PI / 180.0f);
	rot[2][0] = -1 *  __builtin_sin(-lpitch * M_PI / 180.0f);
	rot[2][1] = 0;
	rot[2][2] = __builtin_cos(-lpitch * M_PI / 180.0f);
		
	glUseProgram(scpshad[1].program);
	glUniformMatrix3fv(scpshad[1].uniforms[1], 1, GL_FALSE, &rot[0][0]);
	glBindVertexArray(vao); // sky vao
	glDrawElements(GL_TRIANGLES,12,GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
	

	return;
}
