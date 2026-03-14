#include "shaders.h"
#include "errormsg.h"



struct scpshader scpvshads;
struct scpshader scpfshads;

struct scpshader scpshad;

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
