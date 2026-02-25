#ifndef SCP_MODEL_H
#define SCP_MODEL_H

#include <glad/glad.h>
#include "shaders.h"

struct scpmodel
{
	//todo defuculate how orientation is stored
	//want to have halo style gun aimed at orientation (as it it tracks up and down unlike SCP:SL) and skeletal animation
	//eventualay
	float position[3];
	float rotation[4][4];
	char type;
	unsigned vertcount;
	unsigned trigscount;
	GLuint vbuff;
	GLuint varray;
	GLuint ibuff;
	struct scpshader *sprog;
};

#endif
