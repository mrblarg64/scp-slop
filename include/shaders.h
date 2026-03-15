#ifndef SCP_SHADER_H
#define SCP_SHADER_H

#include <glad/glad.h>
#include <stddef.h>

#define SCP_SHADER_COUNT 1
#define SCP_MAX_UNIFORM_PER_SHADER 1

#define SCP_GL_UNIFORM_1F 0
#define SCP_GL_UNIFORM_2F 1
#define SCP_GL_UNIFORM_3F 2
#define SCP_GL_UNIFORM_4F 3
#define SCP_GL_UNIFORM_1I 4
#define SCP_GL_UNIFORM_2I 5
#define SCP_GL_UNIFORM_3I 6
#define SCP_GL_UNIFORM_4I 7
#define SCP_GL_UNIFORM_1UI 8
#define SCP_GL_UNIFORM_2UI 9
#define SCP_GL_UNIFORM_3UI 10
#define SCP_GL_UNIFORM_4UI 11
#define SCP_GL_UNIFORM_1FV 12
#define SCP_GL_UNIFORM_2FV 13
#define SCP_GL_UNIFORM_3FV 14
#define SCP_GL_UNIFORM_4FV 15
#define SCP_GL_UNIFORM_1IV 16
#define SCP_GL_UNIFORM_2IV 17
#define SCP_GL_UNIFORM_3IV 18
#define SCP_GL_UNIFORM_4IV 19
#define SCP_GL_UNIFORM_1UIV 20
#define SCP_GL_UNIFORM_2UIV 21
#define SCP_GL_UNIFORM_3UIV 22
#define SCP_GL_UNIFORM_4UIV 23
#define SCP_GL_UNIFORM_MATRIX_2FV 24
#define SCP_GL_UNIFORM_MATRIX_3FV 25
#define SCP_GL_UNIFORM_MATRIX_4FV 26
#define SCP_GL_UNIFORM_MATRIX_2X3FV 27
#define SCP_GL_UNIFORM_MATRIX_3X2FV 28
#define SCP_GL_UNIFORM_MATRIX_2X4FV 29
#define SCP_GL_UNIFORM_MATRIX_4X2FV 30
#define SCP_GL_UNIFORM_MATRIX_3X4FV 31
#define SCP_GL_UNIFORM_MATRIX_4X3FV 32



struct scpshader
{
	GLuint program;
	unsigned char unicount;
	GLuint uniforms[SCP_MAX_UNIFORM_PER_SHADER];
	unsigned char unitypes[SCP_MAX_UNIFORM_PER_SHADER];
};

struct scpshadersrc
{
        const char *const vsrc;
	const char *const fsrc;
	unsigned char unicount;
	char *const uninames[SCP_MAX_UNIFORM_PER_SHADER];//variable name in shader code
	unsigned char unitypes[SCP_MAX_UNIFORM_PER_SHADER];
};

extern struct scpshader scpshad[SCP_SHADER_COUNT];


//int compileshaders(struct scpuni *uniforms, struct scpshader *fshaders, struct scpshader *vshaders);
int compileshaders();

#endif
