#ifndef SCP_SHADER_H

#define SCP_SHADER_H

#include <GL/gl.h>
#include <stddef.h>

#define SCP_SHADER_COUNT 1

extern const char *fshaders[];
extern const char *vshaders[];

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
