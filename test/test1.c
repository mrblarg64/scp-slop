#include <stdio.h>
#include "loadb3d.h"
#include "model.h"
#include "errormsg.h"

int main() {
  struct scpmodel testObject;
  char* file1 = "test/triangle.b3d";
  char* file2 = "resource/models/096/scp096.b3d";
  char* file3 = "test/header.b3d";

  if(B3DLoader(file3, &testObject) != 0){
    errormsg("an error has occured! \n");
    return -1;
  }
  
  return 0;
}
