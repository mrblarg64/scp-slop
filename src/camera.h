#ifndef SCP_CAMERA_H

#define SCP_CAMERA_H

#include "model.h"

struct scpCamera
{
	float pos[3];
	float rot[4][4];
	float proj[4][4];
	float fov;
	float zNear;
	float zFar;
};

#endif
