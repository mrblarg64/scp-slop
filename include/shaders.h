#ifndef SCP_SHADER_H
#define SCP_SHADER_H

#include <glad/glad.h>
#include <stddef.h>

const char * const fshaders[] = {"#version 330 core\
in vec4 inColor;			    \
out vec3 color;				    \
					    \
\void main(void)			    \
{					    \
	color = inColor;		    \
}",
	NULL};

const char * const vshaders[] = {"#version 330 core		\
layout (location = 0) in vec3 pos;			\
layout (location = 1) in vec3 normal;			\
layout (location = 2) in vec4 color;		        \
layout (location = 3) in vec2 tex;			\
uniform mat4 uberMatrix;                                \
  						        \
out ourColor;					        \
						        \
void main(void)						\
{							\
  gl_Position = 0.5f * vec4(pos, 2.0);			\
  ourColor = color;					\
}",
  NULL};

#define SCP_SHADER_COUNT ((sizeof(vshaders)/sizeof(vshaders[0])) - 1)

extern GLuint shaderprogs[SCP_SHADER_COUNT];

struct scpuni
{
	int type;
	void *data;
	GLuint uniform;
	GLchar *name;
};

struct scpshader
{
	GLuint program;
	unsigned char unicount;
	struct scpuni *uniforms;
};


extern struct scpshader scpshad;

//int compileshaders(struct scpuni *uniforms, struct scpshader *fshaders, struct scpshader *vshaders);
int compileshaders();

#endif
