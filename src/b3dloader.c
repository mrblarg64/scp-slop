#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <glad/glad.h>


#include "b3dloader.h"
#include "scp-os.h"
#include "model.h"
#include "errormsg.h"

typedef struct {
	unsigned int sizevArray;
	unsigned int sizeIndices;
	unsigned flags; // same as b3d
	unsigned tex_coord_sets;
	unsigned tex_coord_set_size;
	float* vArray;
	int* indices;
} VRTS;


void TEXSparser(char* mem, size_t size){
	unsigned rtp;
  
	return;
}

void BRUSHparser(char* mem, size_t size){
  
  
	return;
}

__attribute__((always_inline)) static inline uint32_t readu32(char *addr)
{
	uint32_t retval;

	retval = *((uint32_t*)addr);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
	retval = __builtin_bswap32(retval);
#endif
	return retval;
}

// function user must free VatNode upon completion, unless there was an
// error then the function would handle it
int NODEparser(char* mem, size_t size, VRTS* VatNode, int nodecalls){
	unsigned vflags, tex_coord_sets, tex_coord_set_size;
	size_t vertexChunksize, meshChunksize, nodeChunksize, animChunksize, boneChunksize, trisChunksize; 
	unsigned memp, TRISp;
	unsigned meshstart;
	float* vertices; // a pickle for the knowing ones
	unsigned int numVertex;
	int* indexArray;
	uint32_t val; 
  
  
	// skip over info until we hit type of node
	memp = __builtin_strlen(mem) + 1 + 40;

        
	if (B3D_MESH_NODE == *((uint32_t*)&mem[memp]))
	{
		// MESH Node
		meshstart = memp;
		memp += 4;
		meshChunksize = readu32(&mem[memp]);
		// skip brush id
		memp += 8; // now at a vertix chunk, we can skip the name

		memp+=4;
		vertexChunksize = readu32(&mem[memp]);;

		memp += 4;
		vflags = readu32(&mem[memp]);
		memp += 4;
   
		tex_coord_sets = readu32(&mem[memp]);
		if(tex_coord_sets > 1){
			printf("WTF it's not a simple U/V, we are fucked\n");
		}
		memp += 4;

		tex_coord_set_size = readu32(&mem[memp]); 
		memp += 4;

		if(tex_coord_set_size > 2){
			printf("texture vertex greater than 2\n");
		}

       
		/* This may be useful, getting the number of vertices */  
		numVertex = (vertexChunksize - 12)/sizeof(float) / \
			(3 + 3 * (vflags & B3D_FLAG_NORMAL_PRESENT) + 4 *(vflags & B3D_FLAG_COLOUR_PRESENT) + tex_coord_sets * tex_coord_set_size);
    
		printf("The number of vertices %u\n", numVertex);


		printf("malloc for vert %u\n", vertexChunksize - 12);
		vertices = SCPMALLOC(vertexChunksize - 12);
		if(!vertices){
			printf("malloc for vertices failed\n");
			// SCPFREE(vertices);
			return -1; 
		}

		__builtin_memcpy(vertices, &mem[memp], vertexChunksize - 12);
		#if __FLOAT_WORD_ORDER__ ==  __ORDER_BIG_ENDIAN__
		// vertices is float, may need fixing
		for(size_t i = 0; i < numVertex; i++){
			vertices[i] = (float) __builtin_bswap32(*((int32_t*) &vertices[i]));
		}
		#endif

		VatNode->sizevArray = numVertex;
		VatNode->flags = vflags;
		VatNode->tex_coord_sets = tex_coord_sets;
		VatNode->tex_coord_set_size = tex_coord_set_size;
		VatNode->vArray = vertices;
    
		// we have completed with the vertex node, then we move to TRIS
		memp += vertexChunksize - 12;

		printf("%.4s\n", mem + memp);

    

		// allocate space for index vertices
		VatNode->sizeIndices = 0;
		printf("allocsize %u\n", meshChunksize - (memp - meshstart));
		indexArray = SCPMALLOC(meshChunksize - (memp - meshstart)); // waste of space but saves time
		if(!indexArray){
			errormsg("index array failed to allocated\n");
			SCPFREE(vertices);
			return -2; 
		}
    
		// parse the potential multiple triangles, ignoring brush id
		// really memp should never be over meshChunksize
		for(size_t i = 0; memp < (meshstart + meshChunksize); ++i){
			memp += 4; // skip over name
			// printf("%" PRIu32 "\n", memp);
			trisChunksize = readu32(&mem[memp]);
			printf("%" PRIu32 "\n");
			memp += 8; // skip over brush id
			// memcpy(&indexArray[i], mem + memp, trisChunksize - 4 /* remove brush id*/);
			for(size_t j = 0; j < (trisChunksize - 4) / 12; ++j)
			{
				#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				indexArray[j + i] = *(int32_t*)(mem + memp);
				#else
				indexArray[j + i] =  __builtin_bswap32(*(int32_t*)(mem + memp));
				#endif
				memp += 4; 
			}
      
			i += (trisChunksize - 4) / 12;
			VatNode->sizeIndices += (trisChunksize - 4) / 12; // 4 is the size of ints
			memp += trisChunksize - 4;
		}

		VatNode->indices = indexArray;
    
		// There is now a possibility for key, animes, or node chunks
		for(int i = 0; memp < size;)
		{
			switch(*((uint32_t *)(mem + memp))){

			case B3D_KEYS_NODE:
				printf("Here at keys\n");
				/* we will skip this for now as it's for animations */

				memp += readu32(&mem[memp + 4]);
				break;
			case B3D_NODE_NODE:
				/* Recursive case */
				memp += 4;
				nodeChunksize = readu32(&mem[memp]);;
				memp += 4;
				nodecalls += 1;
				nodecalls = NODEparser(mem + memp, nodeChunksize, VatNode + 1 + i * nodecalls, nodecalls);
				if(nodecalls < 0){
					SCPFREE(VatNode->indices);
					SCPFREE(VatNode->vArray);
					return -10; 
				}
				i = 1; // End of parsing this Node
				memp += nodeChunksize;
				break;


			case B3D_ANIM_NODE:
				/* Skipping for now as it's related to animation, we are not there yet. */
				memp += 4;
				animChunksize = readu32(&mem[memp]);
				memp += 4 + animChunksize;
				break;

			default:
				printf("%.4s\n", mem + memp);
				errormsg("Error: Mesh Node has an unidentified chunk name\n");
				SCPFREE(VatNode->indices);
				SCPFREE(VatNode->vArray);
				return -3;
       	
			} // end of switch
		}

	}
	else if(B3D_BONE_NODE == *((uint32_t *) (mem + memp)))
	{
		memp += 4;
		
		boneChunksize = readu32(&mem[memp]);
		memp += 4 + boneChunksize;
	}
	else
	{
		// type pivot, for now this is an error but doesn't have to be
		errormsg("node type is pivot\n");
		return -4;
	}
    
	return nodecalls;
}



/*
 * parser will load info into gpu as a side effect
 * retval is a struct that needs to be filled out
 * returns status as int, negative if error, 0 for good
 */
int B3DLoader(char* file, struct scpmodel* retval){
	FILE_T fdb3d;
	uint32_t val;
	uint64_t fsize;
	unsigned fp;
	char* finram;
	uint32_t totalChunkSize, SubChunkSize;
	VRTS* mesh_buff; // contains all meshs from the mesh nodes
	int numNodes;

	fdb3d = scpopen(file);
	fsize = scpgetfsize(fdb3d);
	if (fsize > 0xffffffffL)
	{
		errormsg("FILE DUMMY THICCCCC\n");
		CLOSEFILE(fdb3d);
		return -10;
	}
	// printf("%u\n", fsize);

	if(fsize < 8){
		errormsg("file size is too small for type b3d\n");
		CLOSEFILE(fdb3d);
		return -1;
	}

	finram = scpmapfile(fdb3d, fsize);
	CLOSEFILE(fdb3d);

	//parse  B3D Node

  
	// I don't think this is truly portable, starting
	// to think that java is the true portable language
	#if __BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__
	val = 0x44334242;
	#else
	val = 0x42423344;
	#endif
	if(*((uint32_t*)(finram)) != val){
		// no b3d header
		errormsg("no b3d header\n");
		scpunmapfile(finram, fsize);
		return -2;
	}

	#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
	printf("BIG CHUNGUS ALERT\n");
	#endif

	totalChunkSize = readu32(&finram[4]); 
	if(totalChunkSize + 8 != fsize){
		errormsg("file size doesn't match header file size\n");
		scpunmapfile(finram, fsize);
		return -3;
	}
	if(fsize <=  12){
		errormsg("file only contains b3d chunk\n");
		scpunmapfile(finram, fsize);
		return -4;
	}


	fp = 12;
	while (fp < fsize){
		// figure out the type of chunk
		switch(*((uint32_t *) &finram[fp])){
			#if __BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__
		case 0x53584554: //TEXS CHUNK
			#else
		case 0x54455853:  
			#endif
			// printf("here at TEXS, this is the value of fp %u\n", fp);
			SubChunkSize = readu32(&finram[fp+4]);

			printf("sub chunk texs%" PRIu32 "\n", SubChunkSize);
	
			//TEXSparser(&finram[fp], SubChunkSize);
			fp += SubChunkSize + 8;
			break;

			#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		case 0x53555242: // BRUS CHUNCK
			#else
		case 0x42525553:
			#endif

			SubChunkSize = readu32(&finram[fp + 4]); 
			//  BRUSHparser(&finram[fp], SubChunkSize);
			fp += SubChunkSize + 8;
			break;

			#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		case 0x45444f4e: //NODE CHUNK
			#else
		case 0x4e4f4445:
			#endif
			// printf("here the value of fp is %u\n", fp);

			SubChunkSize = readu32(&finram[fp+4]);

			printf("subchunk node %" PRIu32 " \n", SubChunkSize);
      
			fp += 8;
			/* This is a waste of memory but it's not the worst */
			mesh_buff = (VRTS*) SCPMALLOC(SubChunkSize); 
			if(!mesh_buff){
				printf("failed to allocate mem for mesh\n");
				scpunmapfile(finram, fsize);
				return -5;
			}
			numNodes = NODEparser(&finram[fp], SubChunkSize,mesh_buff, 1);
			if(numNodes < 0){
				printf("Error Occured under Nodeparser\n");
				SCPFREE(mesh_buff);
				scpunmapfile(finram, fsize);
				return -4;
			}
			fp += SubChunkSize;
			break;
      
		default:
			printf("unknown chuck detected\n");
			return -5;
			break;
		}
	}

	// everything is parsed now need to load info into gpu
	// then load into scp model

	/* Just testing below can remove once complete */
	for(int i = 0; i < numNodes; i++){
		unsigned int  size_of_array = (mesh_buff[i]).sizevArray * 3; // times 3 need to change for later.
		for(unsigned int j = 0; j < size_of_array; j++){
			printf("%f %f %f\n", (mesh_buff[i]).vArray[j], (mesh_buff[i]).vArray[j+1], (mesh_buff[i]).vArray[j+2]);
		}
		SCPFREE((mesh_buff[i]).vArray);
		unsigned int size_of_indices = (mesh_buff[i]).sizeIndices;
		for(unsigned int j = 0; j < size_of_indices; j++){
			printf("%u\n", (mesh_buff[i]).indices[j]);
		}
		SCPFREE((mesh_buff[i]).indices);
	}

	/* testing code segment is done */
  
	return 0;
}
