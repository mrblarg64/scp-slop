#ifndef B3DCREATER_H
#define B3DCREATER_H

/* This file ideally will be used for testing */

// why did I even bother writing this code

#include <stdio.h>
#include <stdint.h>

void write_int(FILE* f, int32_t value){
#ifdef __ORDER_BIG_ENDIAN__
  value = __builtin_bswap32(value);
#endif
  fwrite(&value, 4, 1, f);
  return;
}

// is there a need to garuntee that the float value is 4 bytes
void write_float(FILE* f, __Float32 value){

#ifdef __ORDER_BIG_ENDIAN__
  value = __builtin_bswap32(value);
#endif

  fwrite(&value, 4, 1, f);
  return; 
}

void write_vecf(FILE* f, __Float32* value, size_t len){
#ifdef __ORDER_BIG_ENDIAN__
  for(size_t i = 0; i < len; i++)
    value[i] = __builtin_bswap32(value[i]);
#endif
  fwrite(&value, 4, len, f);
  return; 
}

void write_veci(FILE* f, int32_t* value, size_t len){
#ifdef __ORDER_BIG_ENDIAN__
  for(size_t i = 0; i < len; i++)
    value[i] = __builtin_bswap32(value[i]);
#endif
  fwrite(&value, 4, len, f);
  return; 
}

void write_string(FILE* f, char* s, size_t slen){
#ifdef __ORDER_BIG_ENDIAN__
}


#endif
