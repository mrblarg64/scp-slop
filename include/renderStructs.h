#ifndef SCP_RENDER_STRUCTS_H

#define SCP_RENDER_STRUCTS_H

#include <GLAD/gl.h>

struct scpcam
{
	float position[3];
	float rotation[4][4];
	float projection[4][4];
	float fov;
	float znear;
	float zfar;
};

#endfi
