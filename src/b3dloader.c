#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <glad/glad.h>


#include "b3dloader.h"
#include "scp-os.h"
#include "model.h"
#include "errormsg.h"

typedef struct {
	uint32_t size_of_vertex;
        unsigned num_of_floats;
	uint32_t size_of_indices;
	uint32_t flags; // same as b3d
	uint32_t tex_coord_sets;
	uint32_t tex_coord_set_size;
	float* vArray;
	uint32_t* indices;
} VRTS;

static void ClearAllocation(int number_of_meshes, VRTS* meshp) {
	for(int i = 0; i < number_of_meshes; i++){
	        unsigned int  size_of_array = (meshp[i]).num_of_floats;
		for(unsigned int j = 0; j < size_of_array; j += 3){
		  printf("%f %f %f\n",				\
			       (meshp[i]).vArray[j],	\
			       (meshp[i]).vArray[j+1],	\
			       (meshp[i]).vArray[j+2]);
		}
		SCPFREE((meshp[i]).vArray);
		unsigned int size_of_indices = (meshp[i]).size_of_indices;
		for(unsigned int j = 0; j < size_of_indices; j += 3){
		  printf("%" PRIu32 " %" PRIu32 " %" PRIu32 "\n", \
			 (meshp[i]).indices[j],		   \
			 (meshp[i]).indices[j+1],		   \
			 (meshp[i]).indices[j+2]);
		}
		SCPFREE((meshp[i]).indices);
	}
	

	if(number_of_meshes > 0) SCPFREE(meshp);

	return;
  
}


void TEXSparser(char* mem, size_t size){
	unsigned rtp;
  
	return;
}

void BRUSHparser(char* mem, size_t size){
  
  
	return;
}

// __attribute__((always_inline))

__attribute__((always_inline)) static inline uint32_t readu32(char *addr)
{
	uint32_t retval;

	retval = *((uint32_t*)addr);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
	printf("BIG ENDIAN DETECTION\n");
	retval = __builtin_bswap32(retval);
#endif
	return retval;
}

// function user must free VatNode upon completion, unless there was an
// error then the function would handle it
static int NODEparser(char* mem, uint32_t size, VRTS* VatNode){
	uint32_t vflags, tex_coord_sets, tex_coord_set_size;
	uint32_t vertexChunksize, meshChunksize, nodeChunksize, animChunksize, boneChunksize, trisChunksize; 
	unsigned memp, TRISp;
	unsigned meshstart;
	float* vertices; // a pickle for the knowing ones
	unsigned int numVertex;
	unsigned num_floats;
	uint32_t* indexArray;
	unsigned temp_num_of_indices;
	int nodecalls = 0, node_result;
	
  
	// skip over info until we hit type of node
	memp = __builtin_strlen(mem) + 1 + 40;

        
	if (B3D_MESH_CHUNK == (*(uint32_t*) (&mem[memp])))
	{
	  	printf("Found a Mesh Node\n");
		// MESH Node
		meshstart = memp;
		memp += 4;
		meshChunksize = readu32(&mem[memp]);
		// skip brush id
		memp += 8; // now at a vertix chunk, we can skip the name

		memp+=4;
		vertexChunksize = readu32(&mem[memp]);

		memp += 4;
		vflags = readu32(&mem[memp]);
		memp += 4;
   
		tex_coord_sets = readu32(&mem[memp]);
		if(tex_coord_sets > 1){
			errormsg("WTF it's not a simple U/V, we are fucked\n");
			return -1;
		}
		
		memp += 4;

		tex_coord_set_size = readu32(&mem[memp]); 
		memp += 4;

		if(tex_coord_set_size > 2){
			errormsg("texture vertex greater than 2\n");
			return -1;
		}

		num_floats = (vertexChunksize - 12) / sizeof(float);

       
		/* This may be useful, getting the number of vertices */  
		numVertex = num_floats / (3 + 3 * (vflags & B3D_FLAG_NORMAL_PRESENT) \
					  + 4 *(vflags & B3D_FLAG_COLOUR_PRESENT)\
					  + tex_coord_sets * tex_coord_set_size);

		
    
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
		for(size_t i = 0; i < (vertexChunksize - 12) / sizeof(float); i++){
			vertices[i] = (float) __builtin_bswap32(*((int32_t*) &vertices[i]));
		}
		#endif

		VatNode->size_of_vertex = numVertex;
		VatNode->flags = vflags;
		VatNode->tex_coord_sets = tex_coord_sets;
		VatNode->tex_coord_set_size = tex_coord_set_size;
		VatNode->vArray = vertices;
		VatNode->num_of_floats = num_floats;
    
		// we have completed with the vertex node, then we move to TRIS
		memp += vertexChunksize - 12;

		//printf("%.4s\n", mem + memp);

		// allocate space for index vertices
		VatNode->size_of_indices = 0;
	       
		//printf("allocsize %u\n", meshChunksize - (memp - meshstart));
		/* to save memory we can parse for the sizes excusivesly */
		indexArray = SCPMALLOC(meshChunksize - (memp - meshstart)); // waste of space but saves time
		if(!indexArray){
			errormsg("index array failed to allocated\n");
			SCPFREE(vertices);
			return -2; 
		}

		printf("Size of allocated for index array %" PRIu32 "\n", meshChunksize - (memp - meshstart));

		
		size_t i = 0;
		// parse the potential multiple triangles, ignoring brush id
		// really memp should never be over meshChunksize
		while(memp < (meshChunksize + meshstart)){
		        printf("%.4s\n", &mem[memp]);
			memp += 4; // skip over name
			trisChunksize = readu32(&mem[memp]);
			printf("tris chunk size: %" PRIu32 "\n",  trisChunksize);
			temp_num_of_indices = trisChunksize / sizeof(uint32_t) - 1;
			printf("number of indices: %u"  "\n", temp_num_of_indices);
			memp += 8; // skip over brush id
			
			for(size_t j = 0; j < temp_num_of_indices; ++j){
				
				indexArray[i + j] = readu32(mem + memp);
				// printf(" at hand: %" PRIu32 "\n", indexArray[i + j]);
				memp += 4; 
			}

		        i += temp_num_of_indices; 
			VatNode->size_of_indices += temp_num_of_indices; 
			// memp += trisChunksize - 4;
		}

		VatNode->indices = indexArray;
    
		// There is now a possibility for key, animes, or node chunks
		while(memp < size){
		        switch((*(uint32_t*) (&mem[memp]))){

			case B3D_KEYS_CHUNK:
				printf("Here at keys\n");
				/* we will skip this for now as it's for animations */

				memp += readu32(&mem[memp + 4]) + 8;
				break;
			case B3D_NODE_CHUNK:
				/* Recursive case */
			        printf("Found another node, recursion time\n");
				memp += 4;
				nodeChunksize = readu32(&mem[memp]);
				memp += 4;
				nodecalls += 1;
				node_result = NODEparser(mem + memp, nodeChunksize, VatNode + nodecalls);
				if(node_result < 0){
				  SCPFREE(VatNode->indices);
				  SCPFREE(VatNode->vArray);
					return -10; 
				}

				nodecalls += node_result;
				 
				memp += nodeChunksize;
				break;


			case B3D_ANIM_CHUNK:
			        printf("Here at ANIM\n");
				/* Skipping for now as it's related to animation, we are not there yet. */
				animChunksize = readu32(&mem[memp + 4]);
				printf("animChunksize %" PRIu32 "\n", animChunksize);
				memp += 8 + animChunksize;
				break;

			case B3D_SEQS_CHUNK:
			  	/* chunk not in documentation */ 
			  	printf("Mysterous SEQS chunk found\n");
				memp += readu32(mem + memp + 4) + 8;
				break;
 
			default:
				printf("%.4s\n", mem + memp);
				printf("is it a chunk? %" PRIu32 "\n", readu32(mem + memp + 4));
				errormsg("Error: Node has an unidentified chunk name\n");
				SCPFREE(VatNode->indices); 
				SCPFREE(VatNode->vArray); 
				return -10;
       	
			} // end of switch
		}

	}
	else if(B3D_BONE_CHUNK == (*(uint32_t*) (&mem[memp])))
	{
	  	printf("Found a Bone Chunk, skipping for now\n");
		// memp += 4;		
		// boneChunksize = readu32(&mem[memp]);
		// memp += 4 + boneChunksize;
	}
	else
	{
		// type pivot, custom type for node
	  	printf("Found Pivot type in file with name %.4s\n", mem + memp);
		// memp += readu32(mem + memp +4) + 8;
		
		
	}
    
	return nodecalls;
}



/*
 * parser will load info into gpu as a side effect
 * retval is a struct that needs to be filled out
 * returns status as int, negative if error, 0 for good
 */
int B3DLoader(char* file, struct scpmodel* p){
	FILE_T fdb3d;
	uint32_t val;
	uint64_t fsize;
	unsigned fp;
	char* finram;
	uint32_t totalChunkSize, SubChunkSize, header_name;
	VRTS* mesh_buff; // contains all meshs from the mesh nodes
	int num_mesh = 0;

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

	header_name = readu32(finram);
	
	//parse B3D Node
	if( header_name != B3D_CHUNK){
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
		// printf("name as is: %#x\n", *(uint32_t*)(finram + fp));
		// printf("header name is: %#x\n", header_name);
		// figure out the type of chunk
	        switch (*(uint32_t*) (&finram[fp])) {
			
		case B3D_TEXS_CHUNK: 
		        printf("here at TEXS\n");
			SubChunkSize = readu32(&finram[fp+4]);
			printf("sub chunk texs %" PRIu32 "\n", SubChunkSize);
			//TEXSparser(&finram[fp], SubChunkSize);
			fp += SubChunkSize + 8;
			break;
			
		case B3D_BRUS_CHUNK: // BRUS CHUNCK
		        printf("At Brush Chunk\n");
			SubChunkSize = readu32(&finram[fp + 4]); 
			//  BRUSHparser(&finram[fp], SubChunkSize);
			fp += SubChunkSize + 8;
			break;

			
		case B3D_NODE_CHUNK: //NODE CHUNK
		        SubChunkSize = readu32(&finram[fp+4]);
			printf("Node chunk size %" PRIu32 " \n", SubChunkSize);    
			fp += 8;
			/* This is a waste of memory but it's not the worst */
			mesh_buff = (VRTS*) SCPMALLOC(SubChunkSize);  
			if(!mesh_buff){
				printf("failed to allocate mem for mesh\n");
				scpunmapfile(finram, fsize);
				return -5;
			}
			num_mesh = NODEparser(&finram[fp], SubChunkSize, mesh_buff);
			if(num_mesh < 0){
				printf("Error Occured under Nodeparser\n");
				SCPFREE(mesh_buff);
				scpunmapfile(finram, fsize);
				return -4;
			}
			fp += SubChunkSize;
			break;
      
		default:
			errormsg("unknown chuck detected with name %" PRIu32 "\n");
			return -5;
			break;
		}
	}
	// everything is parsed now need to load info into gpu
	// and into scp model
	p->num_of_meshes = num_mesh;
	p->vabuff = SCPMALLOC(sizeof(GLuint) * num_mesh);
	p->trigscount = SCPMALLOC(sizeof(unsigned) * num_mesh);

	if ((!p->vabuff) || (!p->trigscount)){
		
	  ClearAllocation(num_mesh, mesh_buff);
	  errormsg("malloc failed to allocate for vabuff\n");
	  return -1;
	}


	// one mesh <-> one VAO
	glGenVertexArrays(num_mesh, p->vabuff);


	
	for(int i = 0; i < num_mesh; i++)
	{
	  	GLuint VBO, EBO;
		glBindVertexArray(p->vabuff[i]);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh_buff[i].num_of_floats * sizeof(float),\
			     mesh_buff[i].vArray, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_buff[i].size_of_indices * sizeof(uint32_t), \
			      mesh_buff[i].indices, GL_STATIC_DRAW);

		p->trigscount[i] = mesh_buff[i].size_of_indices;

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh_buff[i].size_of_vertex * sizeof(float) \
				      , (void*) 0);
		glEnableVertexAttribArray(0);

		if(mesh_buff[i].flags & B3D_FLAG_NORMAL_PRESENT)
		{
		  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,\
					mesh_buff[i].size_of_vertex * sizeof(float), \
					(void*) (3 * sizeof(float)));
		  glEnableVertexAttribArray(1);
		}

		if(mesh_buff[i].flags & B3D_FLAG_COLOUR_PRESENT)
		{
		  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, \
					mesh_buff[i].size_of_vertex * sizeof(float), \
					(void*) ((3 + 3*(mesh_buff[i].flags & B3D_FLAG_NORMAL_PRESENT)) * sizeof(float)));
		  glEnableVertexAttribArray(2);
		}

		if((mesh_buff[i].tex_coord_sets > 0) & (mesh_buff[i].tex_coord_set_size > 0))
	        {
		  glVertexAttribPointer(3, \
					mesh_buff[i].tex_coord_sets * mesh_buff[i].tex_coord_set_size, \
					GL_FLOAT,\
					GL_FALSE,\
					mesh_buff[i].size_of_vertex * sizeof(float), \
					(void*) ((3 + 3*(mesh_buff[i].flags & B3D_FLAG_NORMAL_PRESENT) \
						  + 4*(mesh_buff[i].flags & B3D_FLAG_COLOUR_PRESENT)) \
						 * sizeof(float))
					);
		  glEnableVertexAttribArray(3);
		}		
	}

	
		
	p->sprog = 0;	

	ClearAllocation(num_mesh, mesh_buff);


	return 0;
}
