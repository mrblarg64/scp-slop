#include <stdio.h>
#include <string.h>
#include <glad/glad.h>
#include "scp-os.h"
#include "model.h"
#include "errormsg.h"

typedef struct {
  unsigned int sizevArray;
  unsigned int sizeIndices;
  uint32_t flags; // same as b3d
  uint32_t tex_coord_sets;
  uint32_t tex_coord_set_size;
  float* vArray;
  int32_t* indices;
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
int NODEparser(char* mem, unsigned int size, VRTS* VatNode, int nodecalls){
  uint32_t val, vflags, tex_coord_sets, tex_coord_set_size;
  uint32_t vertexChunksize, meshChunksize, nodeChunksize, animChunksize, boneChunksize; 
  uint32_t memp, TRISp;
  float* vertices;
  uint32_t numVertex;
  int32_t* indexArray;
  
  
  // skip over info until we hit type of node
  memp = strlen(mem) + 1 + 40;
  printf("%s\n", mem);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  val = 0x4853454d;
#else
  val = 0x4d455348;
#endif

  // printf("%u", *((uint32_t*)&mem[memp]));
  
  if(val == *((uint32_t*)&mem[memp])){ // MESH Node
    memp += 4;
    meshChunksize = *((uint32_t*)&mem[memp]);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
    meshChunksize = __builtin_bswap32(meshChunksize);
#endif
    // skip brush id
    memp += 8; // now at a vertix chunk, we can skip the name
    vertexChunksize = *((uint32_t *) &mem[memp]);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
    vertexChunksize = __builtin_bswap32(vertexChunksize);
#endif
    memp += 4;
    vflags = *((uint32_t *) &mem[memp]);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    vflags = __builtin_bswap32(vflags);
#endif
    memp += 4;
    tex_coord_sets = *((uint32_t *) &mem[memp]);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
    tex_coord_sets = __builtin_bswap32(tex_coord_sets);
#endif
    if(tex_coord_sets > 1){
      printf("WTF it's not a simple U/V, we are fucked\n");
    }
    memp += 4;
    tex_coord_set_size = *((uint32_t *)&mem[memp]);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
    tex_coord_set_size = __builtin_bswap32(tex_coord_set_size);
#endif
    memp += 4;

    if(tex_coord_set_size > 2){
      printf("texture vertex greater than 2\n");
    }

       
    /* This may be useful, getting the number of vertices */  
    numVertex = (vertexChunksize - 12)/sizeof(float) / \
      (3 + 3 * (vflags & 1) + 4 *(vflags & 2) + tex_coord_sets * tex_coord_set_size);
    
    
    vertices = SCPMALLOC(vertexChunksize - 12);
    if(!vertices){
      printf("malloc for vertices failed\n");
      // SCPFREE(vertices);
      return -1; 
    }

    memcpy(vertices, &mem[memp], vertexChunksize - 12);
#if __ORDER_BYTE__ ==  __ORDER_BIG_ENDIAN__
    // vertices is float, is this the right swap?
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

    // allocate space for index vertices
    VatNode->sizeIndices = 0;
    indexArray = SCPMALLOC(meshChunksize - memp); // waste of space but saves time
    if(!indexArray){
      errormsg("index array failed to allocated\n");
      SCPFREE(vertices);
      return -2; 
    }
    
    // parse the potential multiple triangles, ignoring brush id
    for(int i = 0; memp < meshChunksize; i += 3){
      memp += 4; // skip over name
      uint32_t trisChunksize = *((uint32_t *) &mem[memp]);
#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
      trisChunksize = __builtin_bswap32(trisChunksize);
#endif
      memp += 8; // skip over brush id
      memcpy(&indexArray[i], &mem[memp], trisChunksize - 4 /* remove brush id*/);
      VatNode->sizeIndices += (trisChunksize - 4) / 4; // 4 is the size of ints
      memp += trisChunksize - 4;
    }

#if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
    for(int i = 0; i < VatNode->sizeIndices; i++){
      indexArray[i] = __builtin_bswap32(indexArray[i]);
    }
    
#endif
    VatNode->indices = indexArray;
    
    // There is now a possibility for key, animes, or node chunks
    for(int i = 0; memp < size;){
      switch(*((uint32_t *)&mem[memp])){
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      case 0x4b455953: // KEYS
#else
      case 0x5359454b:
#endif
	/* we will skip this for now as it's for animations */
	memp = 4 + memp + *((uint32_t *)(&mem[memp + 4]));
	break;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      case 0x4e4f4445: // NODE
#else
      case 0x45444f4e:
#endif
	/* Recursive case */
	memp += 4;
	nodeChunksize = *((uint32_t*)&mem[memp]);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	nodeChunksize = __builtin_bswap32(nodeChunksize);
#endif
	memp += 4;
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

#if __BYTE_ORDER__ ==  __ORDER__LITTLE_ENDIAN__
      case 0x414e494d:// ANIM chunk
#else
      case 0x4d494e41:
#endif
	/* Skipping for now as it's related to animation, we are not there yet. */
	memp += 4;
	animChunksize = *((uint32_t *) &mem[memp]);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN
	animChunksize = __builtin_bswap32(animChunksize);
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
#if __BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__
    val = 0x424f4e45; // Bone nodechunk
#else
    val = 0x454e4f42;
#endif

  } else if(val == *((uint32_t *) &mem[memp])){ 
    memp += 4;
    boneChunksize = *((uint32_t *) &mem[memp]);
#if __BYTE_ORDERR__ ==  __ORDER_BIG_ENDIAN__
    boneChunksize = __builtin_bswap32(boneChunksize);
#endif
    memp += 4 + boneChunksize;   
  }
  else {
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
  unsigned int fsize, fp;
  char* finram;
  uint32_t totalChunkSize, SubChunkSize;
  VRTS* mesh_buff; // contains all meshs from the mesh nodes
  int numNodes;

  fdb3d = scpopen(file);
  fsize = scpgetfsize(fdb3d);

  printf("%u\n", fsize);

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

  // printf("%u\n", *((uint32_t*)(finram)));
  
  if(*((uint32_t*)(finram)) != val){
    // no b3d header
    errormsg("no b3d header");
    scpunmapfile(finram, fsize);
    return -2;
  }

  
  totalChunkSize = *((uint32_t *) &finram[4]);
  #if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
  printf("here\n");
  totalChunkSize = __builtin_bswap32(totalChunkSize);
  #endif

  printf("%u\n", totalChunkSize);

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
	fp += 4; 
	SubChunkSize = *((uint32_t *)&finram[fp]);

	printf("%u\n", SubChunkSize);
	
	fp += 4;
	//TEXSparser(&finram[fp], SubChunkSize);
	fp += SubChunkSize;
	break;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      case 0x53555242: // BRUS CHUNCK
#else
      case 0x42525553:
#endif

      fp += 4; 
      SubChunkSize = *((uint32_t *)&finram[fp]); 
      fp += 4;
      //  BRUSHparser(&finram[fp], SubChunkSize);
      fp += SubChunkSize;
      break;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      case 0x45444f4e: //NODE CHUNK
#else
      case 0x4e4f4445:
#endif
      fp += 4;
      printf("here the value of fp is %u\n", fp);
      SubChunkSize = *((uint32_t *)&finram[fp]);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      SubChunkSize = __builtin_bswap32(SubChunkSize);
#endif

      printf("%u", SubChunkSize);
      
      fp += 4;
      /* This is a waste of memory but it's not the worse*/
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
    unsigned int  size_of_array = (mesh_buff[i]).sizevArray;
    for(unsigned int j = 0; j < size_of_array; j++){
      printf("%f\n", (mesh_buff[i]).vArray[j]);
    }
    SCPFREE((mesh_buff[i]).vArray);
    unsigned int size_of_indices = (mesh_buff[i]).sizeIndices;
    for(unsigned int j = 0; j < size_of_indices; j++){
      printf("%u", (mesh_buff[i]).indices[j]);
    }
    SCPFREE((mesh_buff[i]).indices);
  }

  /* testing code segment is done */
  
  return 0;
}
