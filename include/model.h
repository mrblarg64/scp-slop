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
  	unsigned num_of_meshes;
  //	unsigned vertcount;
  	unsigned* trigscount; // dependant on the number of meshes in the model
  //unsigned* vbuff;
	GLuint* vabuff;
  //	unsigned* ibuff;
        unsigned char sprog;
};

#endif
