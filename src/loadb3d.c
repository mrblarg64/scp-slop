#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>

#include <assert.h>

#include "scp-os.h"
#include "model.h"

struct scpmodel *loadb3d(char *file, char modeltype)
{
	FILE_T fd;
        uint64_t fsize;
	char *finram;
	unsigned val;
	unsigned rootchunksize;
	unsigned fpos;
	unsigned chunkend;
	unsigned subchunkend;
	unsigned subsubchunkend;
	char foundtexs=0;
	char foundbrus=0;
	char foundnode=0;
	unsigned vflags;
	unsigned vtcords;
	unsigned vtcordssize;
	float *verts;
	unsigned vertslen;
	float *vnorms=NULL;
	unsigned vnormslen;
	float *vcolour=NULL;
	unsigned vcolourlen;
	float *vtcorddata;
	unsigned vtcorddatalen;
	unsigned *trigs;
	unsigned trigslen;
	unsigned reg;
	unsigned subreg;
	unsigned mallocfactor;
	//unsigned glbuf;
	//unsigned glibuf;
	struct scpmodel *retval=NULL;

	fd = scpopen(file);
	fsize = scpgetfsize(fd);

	if (fsize<8)
	{
		printf("file is too small to be genuine\n");
		CLOSEFILE(fd);
		return retval;
	}
        finram = scpmapfile(fd, fsize);
	CLOSEFILE(fd);

	#ifdef __ORDER_LITTLE_ENDIAN__
	val = 0x44334242;
	#else
	val = 0x42423344;
	#endif
	//BB3D
	if ((*((unsigned*)(finram)))!=val)
	{
		printf("no b3d header detectected\nexpected: %x\ngot:      %x\n",val,(*((unsigned*)(finram))));
		scpunmapfile(finram, fsize);
		return retval;
	}
	#ifdef __ORDER_LITTLE_ENDIAN__
	rootchunksize = (*((unsigned*)(&finram[4])));
	#else
	rootchunksize = (*((unsigned*)(&finram[4])));
	rootchunksize = __builtin_bswap32(rootchunksize);
	#endif
	if ((rootchunksize+8)>fsize)
	{
		printf("the file seems to be incomplete, the header reports a size of %'u bytes but the file is " U64_PF " bytes\n",(rootchunksize+8), fsize);
		scpunmapfile(finram, fsize);
		return retval;
	}

	fpos=12;
	while (fpos!=fsize)
	{
		//printf("%c%c%c%c\n",finram[fpos],finram[fpos+1],finram[fpos+2],finram[fpos+3]);
		switch (*((unsigned*)(&finram[fpos])))
		{
			#ifdef __ORDER_LITTLE_ENDIAN__
		case 0x53584554:
			#else
		case 0x54455853:
			#endif
			//TEXS
			//ignore textures for now
			printf("ignoring textures for now\n");
			fpos+=4;
			#ifdef __ORDER_LITTLE_ENDIAN__
			fpos +=4 + *((unsigned*)(&finram[fpos]));
			#else
			fpos += 4 + __builtin_bswap32(*((unsigned*)(&finram[fpos])));
			#endif
			break;
			#ifdef __ORDER_LITTLE_ENDIAN__
		case 0x53555242:
			#else
		case 0x42525553:
			#endif
			//BRUS
			//ignore brushes for now
			printf("ignoring brushes for now\n");
			fpos+=4;
			#ifdef __ORDER_LITTLE_ENDIAN__
			fpos +=4 + *((unsigned*)(&finram[fpos]));
			#else
			fpos += 4+  __builtin_bswap32(*((unsigned*)(&finram[fpos])));
			#endif
			break;
			#ifdef __ORDER_LITTLE_ENDIAN__
		case 0x45444f4e:
			#else
		case 0x4e4f4445:
			#endif
			//NODE
			//ignore nodes for now
			printf("ignoring nodes for now\n");
			fpos+=4;
			#ifdef __ORDER_LITTLE_ENDIAN__
			chunkend =fpos +4 + *((unsigned*)(&finram[fpos]));
			#else
			chunkend =fpos+ 4+  __builtin_bswap32(*((unsigned*)(&finram[fpos])));
			#endif
			fpos+=4;
			printf("found node name %s\n",&finram[fpos]);
			fpos += 1+strlen(&finram[fpos]);
			//the following printfs ONLY WORK ON LITTLE ENDIAN
			printf("pos: %f %f %f\n",*((float*)(&finram[fpos])),*((float*)(&finram[fpos+4])),*((float*)(&finram[fpos+8])));
			fpos+=12;
			printf("scale: %f %f %f\n",*((float*)(&finram[fpos])),*((float*)(&finram[fpos+4])),*((float*)(&finram[fpos+8])));
			fpos+=12;
			printf("rot: %f %f %f %f\n",*((float*)(&finram[fpos])),*((float*)(&finram[fpos+4])),*((float*)(&finram[fpos+8])),*((float*)(&finram[fpos+12])));
			fpos+=16;
			//printf("%c%c%c%c\n",finram[fpos],finram[fpos+1],finram[fpos+2],finram[fpos+3]);
			switch (*((unsigned*)(&finram[fpos])))
			{
				#ifdef __ORDER_LITTLE_ENDIAN__
			case 0x4853454d:
				#else
			case 0x4d455348:
				#endif
				printf("mesh detected\n");
				fpos+=4;
				#ifdef __ORDER_LITTLE_ENDIAN__
				subchunkend =fpos +4 + *((unsigned*)(&finram[fpos]));
				#else
				subchunkend =fpos+ 4+  __builtin_bswap32(*((unsigned*)(&finram[fpos])));
				#endif
				fpos+=4;
				fpos+=4;//skip over brush_id
				while (fpos!=subchunkend)
				{
					switch (*((unsigned*)(&finram[fpos])))
					{
						#ifdef __ORDER_LITTLE_ENDIAN__
					case 0x53545256:
						#else
					case 0x56525453:
						#endif
						printf("found vrts\n");
						fpos+=4;
						#ifdef __ORDER_LITTLE_ENDIAN__
						subsubchunkend =fpos +4 + *((unsigned*)(&finram[fpos]));
						vflags = *((unsigned*)(&finram[fpos+4]));
						vtcords = *((unsigned*)(&finram[fpos+8]));
						vtcordssize = *((unsigned*)(&finram[fpos+12]));
						#else
						subsubchunkend =fpos+ 4+  __builtin_bswap32(*((unsigned*)(&finram[fpos])));
						vflags = __builtin_bswap32(*((unsigned*)(&finram[fpos+4])));
						vtcords = __builtin_bswap32(*((unsigned*)(&finram[fpos+8])));
						vtcordssize = __builtin_bswap32(*((unsigned*)(&finram[fpos+12])));
						#endif
						fpos+=16;
						mallocfactor = ((subsubchunkend-fpos)/(sizeof(float)))/(3+(+(vflags&1)*3)+(((vflags>>1)&1)*4)+(vtcords*vtcordssize));
						vertslen = sizeof(float) * 3 * mallocfactor;
						verts = SCPMALLOC(vertslen);
						if (!verts)
						{
							printf("malloc verts failed\n");
							scpunmapfile(finram, fsize);
							return retval;
						}
						if (vflags&1)
						{
							vnormslen = sizeof(float) * 3 * mallocfactor;
							vnorms = SCPMALLOC(vnormslen);
							if (!vnorms)
							{
								printf("malloc vnorms failed\n");
								SCPFREE(verts);
								scpunmapfile(finram, fsize);
								return retval;
							}
						}
						if (vflags&2)
						{
							vcolourlen = sizeof(float) * 4 * mallocfactor;
							vcolour = SCPMALLOC(vcolourlen);
							if (!vcolour)
							{
								printf("malloc vcolour failed");
								if (vnorms)
								{
									SCPFREE(vnorms);
								}
								SCPFREE(verts);
								scpunmapfile(finram, fsize);
								return retval;
							}
						}
						vtcorddatalen = sizeof(float) * (vtcords*vtcordssize) * mallocfactor;
						vtcorddata = SCPMALLOC(vtcorddatalen);
						printf("vtcords: %u\nvtcordssize: %u\n",vtcords,vtcordssize);
						reg=0;
						while (fpos!=subsubchunkend)
						{
							//printf("%u/%u\n",fpos,subsubchunkend);
							memcpy(&verts[reg*3], &finram[fpos], 3*sizeof(float));
							#ifdef __ORDER_BIG_ENDIAN__
							verts[reg*3] = __builtin_bswap32(verts[reg]);
							verts[(3*reg)+1] = __builtin_bswap32(verts[(3*reg)+1]);
							verts[(3*reg)+2] = __builtin_bswap32(verts[(3*reg)+2]);
							#endif
							//if (((3*reg)+2)>((subsubchunkend-fpos)/(sizeof(float))) * (3/(3+(+(vflags&1)*3)+(((vflags>>1)&1)*4)+(vtcords*vtcordssize))))
							//{
							//	printf("vert malloc error\n");
							//}
							fpos+=3*sizeof(float);
							//printf("%u/%u\n",fpos,subsubchunkend);
							if (vnorms)
							{
								memcpy(&vnorms[reg*3], &finram[fpos], 3*sizeof(float));
								#ifdef __ORDER_BIG_ENDIAN__
								vnorms[(reg*3)] = __builtin_bswap32(vnorms[reg*3]);
								vnorms[(reg*3)+1] = __builtin_bswap32(vnorms[(reg*3)+1]);
								vnorms[(reg*3)+2] = __builtin_bswap32(vnorms[(reg*3)+2]);
								#endif
								fpos+=3*sizeof(float);
							}
							if (vcolour)
							{
								memcpy(&vcolour[reg*4], &finram[fpos], 4*sizeof(float));
								#ifdef __ORDER_BIG_ENDIAN__
								vcolour[(reg*4)] = __builtin_bswap32(vcolour[reg*4]);
								vcolour[(reg*4)+1] = __builtin_bswap32(vcolour[(reg*4)+1]);
								vcolour[(reg*4)+2] = __builtin_bswap32(vcolour[(reg*4)+2]);
								vcolour[(reg*4)+3] = __builtin_bswap32(vcolour[(reg*4)+3]);
								#endif
								fpos+=4*sizeof(float);
							}
							memcpy(&vtcorddata[reg*vtcords*vtcordssize], &finram[fpos], vtcords*vtcordssize*sizeof(float));
							#ifdef __ORDER_BIG_ENDIAN__
							subreg = 0;
							while (subreg!=(vtcords*vtcordssize))
							{
								vtcorddata[subreg+reg*vtcords*vtcordssize] = __builtin_bswap32(vtcorddata[subreg+reg*vtcords*vtcordssize]);
								subreg++;
							}
							#endif
							reg++;
							fpos+=vtcords*vtcordssize*sizeof(float);
						}
						//fpos = subsubchunkend;
						break;
						#ifdef __ORDER_LITTLE_ENDIAN__
					case 0x53495254:
						#else
					case 0x54524953:
						#endif
						printf("found tris\n");
						fpos+=4;
						#ifdef __ORDER_LITTLE_ENDIAN__
						subsubchunkend =fpos +4 + *((unsigned*)(&finram[fpos]));
						#else
						subsubchunkend =fpos+ 4+  __builtin_bswap32(*((unsigned*)(&finram[fpos])));
						#endif
						fpos+=4;
						//;
						assert((subsubchunkend-fpos)==(((subsubchunkend - fpos)/sizeof(unsigned)))*sizeof(unsigned));
						trigslen = subsubchunkend-fpos;
						trigs = SCPMALLOC(trigslen);
						memcpy(trigs, &finram[fpos], (subsubchunkend-fpos));
						fpos = subsubchunkend;
						break;
					}
				}
				break;
				#ifdef __ORDER_LITTLE_ENDIAN__
			case 0x454e4f42:
				#else
			case 0x424f4e45:
				#endif
				fpos+=4;
				#ifdef __ORDER_LITTLE_ENDIAN__
				subchunkend =fpos +4 + *((unsigned*)(&finram[fpos]));
				#else
				subchunkend =fpos+ 4+  __builtin_bswap32(*((unsigned*)(&finram[fpos])));
				#endif
				printf("bone detected\n");
				break;
			}
			fpos=chunkend;
			break;
		}
	}

	retval=SCPMALLOC(sizeof(struct scpmodel));
	if (!retval)
	{
		scpunmapfile(finram, fsize);
		//todo free other things
		//this leaks memory if malloc failes
		printf("loadb3d() scpmodel malloc() failed\n");
		//avoid memory leak via nuke
		exit(1);
	}

	retval->sprog = SCPMALLOC(sizeof(struct scpshader));
	if (!retval->sprog)
	{
		scpunmapfile(finram, fsize);
		//todo free other things
		//this leaks memory if malloc failes
		printf("loadb3d() scpmodel malloc() failed\n");
		//avoid memory leak via nuke
		exit(1);
	}

	retval->type = modeltype;
	retval->vertcount = vertslen/(3*sizeof(float));
	retval->trigscount = trigslen/(3*sizeof(float));

	glGenVertexArrays(1,&(retval->varray));
	glGenBuffers(1, &(retval->vbuff));
	glGenBuffers(1, &(retval->ibuff));

	glBindVertexArray(retval->varray);
	glBindBuffer(GL_ARRAY_BUFFER, retval->vbuff);
	glBufferData(GL_ARRAY_BUFFER, vertslen, verts, GL_STATIC_DRAW);




	
	//glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, retval->ibuff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, trigslen, trigs, GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	
	printf("finished, freeing finram and returning\n");
	scpunmapfile(finram, fsize);
	SCPFREE(verts);
	if (vnorms)
	{
		SCPFREE(vnorms);
	}
	if (vcolour)
	{
		SCPFREE(vcolour);
	}
	SCPFREE(vtcorddata);
	SCPFREE(trigs);
	return retval;
}
