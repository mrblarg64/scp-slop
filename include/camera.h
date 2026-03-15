#ifndef SCP_CAMERA_H

#define SCP_CAMERA_H

#include "model.h"

struct scpCamera
{
	float pos[3];
	//float rot[4][4];
	float rot[2];//yaw[0] pitch[1] roll[2](todo)
	float proj[4][4];
	float fov;
	float znear;
	float zfar;
};

#endif
