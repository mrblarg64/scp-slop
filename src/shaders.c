#include "shaders.h"
#include "errormsg.h"

const char *fshaders[] = {"#version 330 core\
in vec4 inColor;			    \
out vec3 color;				    \
					    \
\void main(void)			    \
{					    \
	color = inColor;		    \
}",
	NULL};

const char *vshaders[] = {"#version 330 core		\
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
			return -1;
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
			return -1;
		}


		fshader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fshader)
		{
			errormsg("unable to create shader");
			return -1;
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
			return -1;
		}

		scpshad.program = glCreateProgram();
		glAttachShader(scpshad.program, fshader);
		glAttachShader(scpshad.program, vshader);
		glLinkProgram(scpshad.program);
		

		x++;
	}

	return 0;
}
