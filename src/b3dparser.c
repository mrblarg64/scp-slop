#include <stdio.h>
#include <string.h>
#include <glad/glad.h>
#include "scp-os.h"
#include "model.h"

typedef struct {
  size_t sizevArray;
  size_t sizeIndices;
  size_t flags; // same as b3d
  size_t tex_coord_sets;
  size_t tex_coord_set_size;
  float* vArray;
  int* indices;
} VRTS;


void TEXSparser(char* mem, unsigned size){
  unsigned rtp;
  
  return;
}

void BRUSHparser(char* mem, unsigned size){
  
  
  return;
}

// function user must free VatNode upon completion, unless there was an
// error then the function would handle it
int NODEparser(char* mem, unsigned size, VRTS* VatNode, int nodecalls){
  size_t val, vflags, tex_coord_sets, tex_coord_set_size;
  size_t vertexChunksize, meshChunksize, nodeChunksize, animChucksize, boneChunksize; 
  size_t memp, TRISp;
  float* vertices;
  size_t numVertex;
  
  
  // skip over info until we hit type of node
  memp = strlen(mem) + 1 + 40; // this may only work for little endian
#ifdef __ORDER_LITTLE_ENDIAN__
  val = 0x4853454d;
#else
  val = 0x4d455348;
#endif
  if(val == *((size_t*)&mem[memp])){ // MESH Node
    memp += 4;
    meshChunksize = *((size_t*)&mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
    meshChunksize = __builtin_bswap32(meshChunksize);
#endif
    // skip brush id
    memp += 8; // now at a vertix chunk, we can skip the name
    vertexChunkSize = *((size_t *) &mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
    vertexChunkSize = __builtin_bswap32(vertexChunkSize);
#endif
    memp += 4;
    vflags = *((size_t *) &mem[memp]);
#if __ORDER_BIG_ENDIAN__
    vflags = __builtin_bswap32(vflags);
#endif
    memp += 4;
    tex_coord_sets = *((size_t *) &mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
    tex_coord_sets = __builtin_bswap32(tex_coord_sets);
#endif
    if(tex_coord_sets > 1){
      printf("WTF it's not a simple U/V, we are fucked\n");
    }
    memp += 4;
    tex_coord_set_size = *((size_t *)&mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
    tex_coord_set_size = __builtin_bswap32(tex_coord_set_size);
#endif
    memp += 4;

    if(tex_coord_set_size > 2){
      printf("texture vertex greater than 2\n");
    }

       
    /* This may be useful, getting the number of vertices */  
    numVertex = (vertexChunkSize - 12)/sizeof(float) / \
      (3 + 3 * (vflags & 1) + 4 *(vflags & 2) + tex_coord_sets * tex_coord_set_size);
    
    
    vertices = SCPMALLOC(vertexChunksize - 12);
    if(!vertices){
      printf("malloc for vertices failed\n");
      // SCPFREE(vertices);
      return -1; 
    }

    memcpy(vertices, &mem[memp], vertexChunkSize - 12);
#ifdef __ORDER_BIG_ENDIAN__
    // vertices is float, is this the right swap?
    vertices = __builtinswap32(vertices);
#endif
    VatNode->numVertex;
    VatNode->flags = vflags;
    VatNode->tex_coord_sets = tex_coord_sets;
    VatNode->tex_coord_set_size = tex_coord_set_size;
    VatNode->vArray = vertices;
    
    // we have completed with the vertex node, then we move to TRIS
    memp += vertexChunkSize - 12;

    // allocate space for index vertices
    VatNode->sizeIndices = 0;
    int indexArray* = SCPMALLOC(meshChunksize - memp); // waste of space but saves time
    if(!indexArray){
      printf("index array failed to allocated\n");
      SCPFREE(vertices);
      return -2; 
    }
    
    // parse the potential multiple triangles, ignoring brush id
    for(size_t i = 0; memp < meshChunksize; i += 3){
      memp += 4; // skip over name
      size_t trisChunksize = *((size_t *) &mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
      trisChunksize = __builtin_swap32(trisChunksize);
#endif
      memp += 8; // skip over brush id
      memcpy(&indexArray[i], &mem[memp], trisChunksize - 4 /* remove brush id*/);
      VatNode->sizeIndices += (trisChunksize - 4) / 4; // 4 is the size of ints
      memp += trisChunksize - 4;
    }

#ifdef __ORDER_BIG_ENDIAN__
    indexArray = __builtin_swap32(indexArray);
#endif
    VatNode->indices = indexArray;
    
    // There is now a possibility for key, animes, or node chunks
    for(size_t i = 0; memp < size;){
      switch(*((size_t *)&mem[memp])){
#ifdef __ORDER_LITTLE_ENDIAN__
      case 0x4b455953: // KEYS
#else
      case 0x5359454b:
#endif
	/* we will skip this for now as it's for animations */
	memp = 4 + memp + *((size_t *)(&mem[memp + 4]));
	break;
#ifdef __ORDER_LITTLE_ENDIAN__
      case 0x4e4f4445: // NODE
#else
      case 0x45444f4e:
#endif
	/* Recursive case */
	memp += 4;
	nodeChunksize = *((size_t *)&mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
	nodeChunksize = __builtin_swap32(nodeChunksize);
#endif
	memp + 4;
	nodecalls += 1;
	nodecalls = NODEparser(&mem[memp], nodeChunksize, VatNode + 1 + i * nodecalls, nodecalls);
	if(nodecalls < 0){
	  SCPFREE(VatNode->indices);
	  SCPFREE(VatNode->vArray);
	  return -10; 
	}
	i = 1; // End of parsing this Node
	memp += nodeChunksize;
	break;

#ifdef __ORDER__LITTLE_ENDIAN__
      case 0x414e494d:// ANIM chunk
#else
      case 0x4d494e41:
#endif
	/* Skipping for now as it's related to animation, we are not there yet. */
	memp += 4;
	animChunksize = *((size_t *) &mem[memp]);
#ifdef __ORDER_BIG_ENDIAN
	animChunksize = __builtin_swap32(animChunksize);
#endif
	memp += 4 + animChunksize;
	break;

      default:
	printf("Error: Mesh Node has an idetified chunk name\n ");
	SCPFREE(VatNode->indices);
	SCPFREE(VatNode->vArray);
	return -3;
       	
      } // end of switch
    }        
  } // end of Mesh Node

#ifdef __ORDER_LITTLE_ENDIAN__
  val = 0x424f4e45; // Bone nodechunk
#else
  val = 0x454e4f42;
#endif
  /* Skipping the Bone chunk for now */
  else if(val == *((size_t *) &mem[memp])){ 
    memp += 4;
    boneChunksize = *((size_t *) &mem[memp]);
#ifdef __ORDER_BIG_ENDIAN__
    boneChunksize = __builtin_swap32(boneChunksize);
#endif
    memp += 4 + boneChunksize;   
  }
  else {
    // type pivot, for now this is an error but doesn't have to be
    printf("nope type is pivot\n");
    return -4;
    break;
  }
    
  return nodecalls;
}



/*
 * parser will send data to gpu as a side effect
 * retval is a struct that needs to be filled out
 * returns status as int, negative if error, 0 for good
 */
int B3dLoader(char* file, struct scpmodel* retval){
  FILE_T fdb3d;
  uint64_t fsize;
  char* finram;
  unsigned val, fp, totalChunkSize, SubChunkSize;
  VRTS* mesh_buff; // contains all meshs from the mesh nodes
  int numNodes;

  fdb3d = scpopen(file);
  fsize = scpgetsize(fd);

  if(fsize < 8){
    printf("file size is too small for type b3d\n");
    CLOSEFILE(fdb3d);
    return -1;
  }

  finram = scpmapfile(fdb3d, fsize);
  CLOSEFILE(fdb3d);

  //parse  B3D Node
  #ifdef __ORDER_LITTLE_ENDIAN__
  val = 0x44334242;
  #else
  val = 0x42423344;
  #endif
  if(*((unsigned*)(finram)) != val){
    // no b3d header
    scpunmapfile(finram, fsize);
    return -2;
  }

  
  totalChunkSize = *((unsigned *) &finram[4]);
  #ifdef __ORDER_BIG_ENDIAN__
  totalChunkSize = __builtin_bswap32(totalChunkSize);
  #endif

  if(totalChunkSize + 8 != fsize){
    printf("file size doesn't match header file size\n");
    scpunmapfile(finram, fsize);
    return -3;
  }

  if(fsize <=  12){
    printf("file only contains b3d chunk\n");
    scpunmapfile(finram, fsize);
    return -4;
  }

  fp = 12

    while (fp < fsize){
      // figure out the type of chunk
      switch(*((unsigned *) &finram[fp])){
#ifdef __ORDER_LITTLE_ENDIAN__
      case 0x53584554: //TEXS CHUNK
#else
      case 0x54455853:  
#endif
      fp += 4; 
      SubChunkSize = *((unsigned *)&finram[fp]); 
      fp += 4;
      //TEXSparser(&finram[fp], SubChunkSize);
      fp += SubChunkSize;
      break;

#ifdef __ORDER_LITTLE_ENDIAN__
      case 0x53555242: // BRUS CHUNCK
#else
      case 0x42525553:
#endif

      fp += 4; 
      SubChunkSize = *((unsigned *)&finram[fp]); 
      fp += 4;
      //  BRUSHparser(&finram[fp], SubChunkSize);
      fp += SubChunkSize;
      break;

#ifdef __ORDER_LITTLE_ENDIAN__
      case 0x45444f4e: //NODE CHUNK
#else
      case 0x4e4f4445:
#endif
      fp += 4;
      SubChunkSize = *((unsigned *)&finram[fp]);
#ifdef __ORDER_BIG_ENDIAN__
      SubChunkSize = __builtin_swap32(SubChunkSize);
#endif
      fp += 4;
      /* This is a waste of memory but it's not the worse*/
      mesh_buff = (VRTS*) SCPMALLOC(SubChunkSize); 
      if(!vertex_buff){
	printf("failed to allocate mem for mesh\n");
	scpunmapfile(finram, fsize);
	return -5;
      }
      numnodes = NODEparser(&finram[fp], SubChunkSize,mesh_buff, 1);
      if(numnodes < 0){
	  printf("Error Occured under Nodeparser\n");
	  SCPFREE(mesh_buff)
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
  
  
}
