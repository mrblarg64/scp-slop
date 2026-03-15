#ifndef SCP_MATRIX_H

#define SCP_MATRIX_H

#define SCP_MATRIX_MULT_SQUARE_F(size, a, b, sum) do{			\
		float accum;						\
		unsigned char r = 0;					\
		unsigned char c;					\
		unsigned char x;					\
		while (r != size)					\
		{							\
			c=0;						\
			while (c != size)				\
			{						\
				x = 0;					\
				accum = 0.0f;				\
				while (x != size)			\
				{					\
					accum = __builtin_fmaf(a[r][x], b[x][c], accum); \
					x++;				\
				}					\
				sum[r][c] = accum;			\
				c++;					\
			}						\
			r++;						\
		}							\
	} while (0)

#define SCP_MATRIX_PERSPECTIVE_F(c, width, height, retval) do{		\
		float htanfov;						\
									\
		htanfov = __builtin_tanf(c.fov/2.0f);			\
		__builtin_memset(retval, 0, sizeof(float)*4*4);		\
		retval[0][0] = 1.0f/((((float)width)/((float)height)) * htanfov); \
		retval[1][1] = 1.0f/htanfov;				\
		retval[2][2] = c.zfar/(c.znear - c.zfar);		\
		retval[2][3] = -1.0f;					\
		retval[3][2] = -((c.zfar * c.znear)/(c.zfar - c.znear)); \
	} while(0)

//fuck cross, dot, and vector normalization! hand assemble this shit
//s, u, f = new basis vectors
//s = our left bound to the ground plane
//u = up (affected by pitch)
//f = look direction
//z up!
//retval[0][0] = s.x
//retval[1][0] = s.y
//retval[0][1] = u.x
//retval[1][1] = u.y
//retval[2][1] = u.z
//retval[0][2] = -f.x
//retval[1][2] = -f.y
//retval[2][2] = -f.z
//retval[3][0] = position in new basis vectors -(pos.s)
//retval[3][1] = position in new basis vectors -(pos.u)
//retval[3][2] = position in new basis vectors (pos.f)

#define SCP_MATRIX_CAMERA_F(c, retval) do{				\
	float ugpmag;/*u ground plane magintude*/			\
	__builtin_memset(retval, 0, sizeof(float)*4*4);			\
	ugpmag = -__builtin_sinf(c.rot[1]);				\
	retval[0][0] = -__builtin_sinf(c.rot[0]);			\
	retval[1][0] = __builtin_cosf(c.rot[0]);			\
	retval[0][1] = ugpmag * retval[1][0];				\
	retval[1][1] = ugpmag * (-retval[0][0]);			\
	retval[2][1] = __builtin_cosf(c.rot[1]);			\
	retval[0][2] = -(retval[1][0] * retval[2][1]);			\
	retval[1][2] = retval[0][0] * retval[2][1];			\
	retval[2][2] = ugpmag;						\
	retval[3][0] = -((retval[0][0] * c.pos[0]) + (retval[1][0] * c.pos[1])); \
	retval[3][1] = -((retval[0][1] * c.pos[0]) + (retval[1][1] * c.pos[1]) * (retval[2][1] * c.pos[2])); \
	retval[3][2] = ((-retval[0][2] * c.pos[0]) + (-retval[1][2] * c.pos[1]) * (-retval[2][2] * c.pos[2])); \
	} while(0)
#endif
