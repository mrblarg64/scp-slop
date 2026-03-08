#ifndef B3DLOADER_H
#define B3DLOADER_H

#include "model.h"

#define B3D_FLAG_NORMAL_PRESENT 0b1
#define B3D_FLAG_COLOUR_PRESENT 0b10

#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN
#define B3D_MESH_NODE 0x4853454d
#define B3D_KEYS_NODE 0x4b455953
#define B3D_NODE_NODE 0x4e4f4445
#define B3D_ANIM_NODE 0x414e494d
#define B3D_BONE_NODE 0x424f4e45
#else
#define B3D_MESH_NODE 0x4d455348
#define B3D_KEYS_NODE 0x5359454b
#define B3D_NODE_NODE 0x45444f4e
#define B3D_ANIM_NODE 0x4d494e41
#define B3D_BONE_NODE 0x454e4f42
#endif

int B3DLoader(char* , struct scpmodel*);

#endif
