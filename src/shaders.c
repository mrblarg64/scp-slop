#include <stdio.h>

#include "shaders.h"
#include "errormsg.h"


const char *const fshaders[] = {
	"#version 330 core\n\n"
	"in vec4 inColor;\n"
	"out vec3 color;\n"
	"\n"
	"void main(void){\n"
	"  color = vec3(inColor.x, inColor.y, inColor.z);\n"
	"}"};

const char *const vshaders[] = {
	"#version 330 core\n\n"
	"layout (location = 0) in vec3 pos;\n"
	"layout (location = 1) in vec3 normal;\n"
	"layout (location = 2) in vec4 color;\n"
	"layout (location = 3) in vec2 tex;\n"
	"uniform mat4 mvp;\n"
	"\n"
	"\n"
	"void main(void){\n"
	"  gl_Position = mvp * vec4(pos, 1.0);\n"
	"}"};


//GLuint shaderprogs[SCP_SHADER_COUNT];

struct scpshader scpvshads;
struct scpshader scpfshads;

struct scpshader scpshad[SCP_SHADER_COUNT];

const struct scpshadersrc shadersrc[] = {
	{
		.vsrc = vshaders[0],
		.fsrc = fshaders[0],
		.unicount = 1,
		.uninames = {"mvp"},
		.unitypes = {SCP_GL_UNIFORM_MATRIX_4FV}
	},
	NULL
};

int compileshaders()
{
	unsigned x;
	unsigned char curuni;
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
		slen = __builtin_strlen(shadersrc[x].vsrc);
		glShaderSource(vshader, 1, &shadersrc[x].vsrc, &slen);
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
		slen = __builtin_strlen(shadersrc[x].fsrc);
		glShaderSource(fshader, 1, &shadersrc[x].fsrc, &slen);
		glCompileShader(fshader);
		glGetShaderiv(fshader, GL_COMPILE_STATUS, &worked);

		if (!worked)
		{
			GLchar err[1024];
			glGetShaderInfoLog(fshader, 1024, NULL, err);
			errormsg(err);
			puts(shadersrc[x].fsrc);
			return -1;
		}

		scpshad[x].program = glCreateProgram();
		if (!scpshad[x].program)
		{
			errormsg("Failed to glCreateProgram()");
			//todo clean up?
			//realistically fuck it though
			return -1;
		}
		glAttachShader(scpshad[x].program, fshader);
		glAttachShader(scpshad[x].program, vshader);
		glLinkProgram(scpshad[x].program);

		scpshad[x].unicount = shadersrc[x].unicount;
		curuni = 0;
		while (curuni != scpshad[x].unicount)
		{
			scpshad[x].uniforms[curuni] = glGetUniformLocation(scpshad[x].program, shadersrc[x].uninames[curuni]);
			if (scpshad[x].uniforms[curuni] == -1)
			{
				errormsg("glGetUniformLocation() FUCK!");
				return -1;
			}
			curuni++;
		}

		x++;
	}

	return 0;
}
