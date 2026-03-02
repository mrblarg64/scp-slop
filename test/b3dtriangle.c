#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Program will create a b3d file for testing, this
 Program assumes the machine is little endian as the
file needs to be little endian */

int main() {
  unsigned char* data[160];
  int32_t tempi;
  char name[9] = "root_node";
  __Float32 nodeData = {
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f
  };
  int32_t vrts_flags[3] = {0,0,0};
  __Float32 vrts_data[9] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
  };

  int32_t indices = {0 , 1 , 2};
  
  data[0] = 'B';
  data[1] = 'B';
  data[2] = '3';
  data[3] = 'D';

  // copy the size of the file
  tempi = 150;
  memcpy(&data[4],&tempi,4);
  tempi = 1;
  memcpy(&data[8],&tempi,4);

  // Node Chunk
  data[12] = 'N';
  data[13] = 'O';
  data[14] = 'D';
  data[15] = 'E';

  tempi = 142; // size of node chunk
  memcpy(&data[16], &tempi, 4);

  
  memcpy(&data[20], &name, 10);
  memcpy(&data[30], nodeData, 40);

  // Mesh chunk
  data[70] = 'M';
  data[71] = 'E';
  data[72] = 'S';
  data[73] = 'H';

  tempi = 84; // size of mesh node
  memcpy(&data[74], &tempi, 4);
  
  tempi = -1;
  memcpy(&data[78],  &tempi, 4);

  
  // VRTS chunk
  data[82] = 'V';
  data[83] = 'R';
  data[84] = 'T';
  data[85] = 'S';

  tempi = 72; // size of VRTS chunk 68 0r 72
  memcpy(&data[86], &tempi, 4);

  memcpy(&data[90], vrts_flags, 12);
  memcpy(&data[102], vrts_data, 36);

  data[138] = 'T';
  data[139] = 'R';
  data[140] = 'I';
  data[141] = 'S';

  // size of TRIS chunk
  tempi = 16;

  memcpy(&data[142], &tempi, 4);

  tempi = -1;
  memcpy(&data[142], &tempi, 4);

  memcpy(&data[146], &indices, 12); // Therefore total data is 158

  // Done filling in the data

  FILE* tf = fopen("test1triangle.b3d", "wb");

  if(tf == NULL){
    pritnf("failed to open file\n");
    return -1;
  }

  if(fwrite(data, 158, 1, tf) == 1){
    printf("file test1triangle.b3d created successfully\n");
  }
  
  fclose(tf);
  return 0;
}
