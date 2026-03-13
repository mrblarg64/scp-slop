#ifndef SCP_PLAYER_H

#define SCP_PLAYER_H

#include "model.h"

struct scpPlayer
{
	//todo defuculate how orientation is stored
	//want to have halo style gun aimed at orientation and skeletal animation
	//eventuallay
	float position[3];
	float rotation[2];//yaw[0] pitch[1]
	char type;
	struct scpmodel model;
};

#endif
