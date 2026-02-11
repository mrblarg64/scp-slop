#include "shaders.h"
#include <stdlib.h>

const char *fshaders[] = {"#version 330\
				       \
out vec3 color;			       \
				       \
\void main(void)		       \
{				       \
	color=vec3(0.0,0.0,1.0);       \
}",
	NULL};

const char *vshaders[] = {"#version 3300 core		\
							\
in vec3 vertex;						\
uniform mat4 uberMatrix;				\
							\
void main(void)						\
{							\
	gl_Position=uberMatrix*vec4(vertex,1.0);	\
}							\
							\
",
	NULL};

struct scpshader scpvshads;
struct scpshader scpfshads;

int compileshaders()
{
	unsigned x;
	GLuint fshader;
	GLuint vshader;
	GLint slen;
	GLint worked;

	x=0;
	while (x!=SCP_SHADER_COUNT)
	{
		vshader = glCreateShader(GL_VERTEX_SHADER);
		if (!vshader)
		{
			errormsg("unable to create shader");
			exit(1);
		}
		slen = __builtin_strlen(vshaders[x]);
		glShaderSource(vshader, 1, &vshaders[x], &slen);
		glCompileShader(vshader);
		glGetShaderiv(vshader, GL_COMPILE_STATUS, &worked);

		if (!worked)
		{
			GLchar err[1024];
			glGetShaderInfoLog(vshader, 1024, NULL, err);
			errormsg(err);
			exit(1);
		}


		fshader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fshader)
		{
			errormsg("unable to create shader");
			exit(1);
		}
		slen = __builtin_strlen(fshaders[x]);
		glShaderSource(fshader, 1, &fshaders[x], &slen);
		glCompileShader(fshader);
		glGetShaderiv(fshader, GL_COMPILE_STATUS, &worked);

		if (!worked)
		{
			GLchar err[1024];
			glGetShaderInfoLog(fshader, 1024, NULL, err);
			errormsg(err);
			exit(1);
		}

		scpshad.program = glCreateProgram();
		glAttachShader(scpshad.program, fshader);
		glAttachShader(scpshad.program, vshader);
		glLinkProgram(scpshad.program);
		

		x++;
	}
}
