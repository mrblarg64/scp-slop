import struct
import glob
from pathlib import Path

class B3DBuilder:
    def __init__(self):
        self.data = bytearray()
        self.chunk_sizes = []
        self.chunkp = []
        self.bytein = 0


        
                  
    def parseFile(self, filename):
        f = open(filename, "r", encoding='ascii')
        content = f.read()
        f.close()
        cl = content.split()

       # print(cl)

        #load bb3d shit into byte array
        self.data += b'BB3D'
        self.data += struct.pack('<i', 1) # size of b3d file
        self.data += struct.pack('<i', int(cl[1]))

        # we need to store the location of the size of the b3d file,
        # which is 5 bytes in
        self.bytein += 12
        self.chunk_sizes.append(4)
        self.chunkp.append(4)

        for elem in cl[2:]:
            try:
                int(elem)
            except ValueError:
                try:
                    float(elem[:-1])
                except ValueError:
                    # Must be a String!
                    match elem:
                        case '[TEXS]':
                            self.data += b'TEXS'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', -1)
                            self.bytein += 4
                            

                        case '[BRUS]':
                            self.data += b'BRUS'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4
                            

                        case '[VRTS]':
                            self.data += b'VRTS'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4
                            

                        case '[TRIS]':
                            self.data += b'TRIS'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4
                            

                        case '[BONE]':
                            self.data += b'BONE'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4
                            

                        case '[KEYS]':
                            self.data += b'KEYS'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4
                            

                        case '[ANIM]':
                            self.data += b'ANIM'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4

                        case '[NODE]':
                            self.data += b'NODE'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4


                        case '[MESH]':
                            self.data += b'MESH'
                            self.bytein += 4
                            self.chunkp.append(self.bytein)
                            self.chunk_sizes.append(0)
                            # store dummy size
                            self.data += struct.pack('<i', 1)
                            self.bytein += 4
                        

                        case 'END':
                            # have a size of chunk, could be sub or not
                            size = self.chunk_sizes.pop()
                            # add the size to the parent chunks
                            self.chunk_sizes = [x + size for x in self.chunk_sizes]
                            location = self.chunkp.pop()
                            self.data[location:location + 4] = struct.pack('<i',size)
                            

                        case string_name:
                            #print(string_name)
                            # name for some chunk
                            string_length = len(string_name) + 1
                            self.chunk_sizes[-1] += string_length
                            self.data += string_name.encode("ascii")
                            self.bytein += string_length
                        
                else:
                    # must be a float
                    self.data += struct.pack('<f', float(elem[:-1]))
                    self.chunk_sizes[-1] += 4
                    self.bytein += 4

            else:
                # must be an int
                self.data += struct.pack('<i', int(elem))
                self.chunk_sizes[-1] += 4
                self.bytein += 4

        #print("checking control flow")
            
                
    
    def BuildB3DFile(self, filename):
        with open(filename, "wb") as fb3d:
            fb3d.write(self.data)
    



"""
this is how the file sturture (b3dtext) will be set up:
[B3DD]
 1
 [TEXS]
   name of texture file (string type)
   1 2 (flags)
   0.0f 0.0f (x y position in float)
   1.0f 1.0f (x y scale float)
   0.0f (rotation)
 END
 [NODE]
  name of the node (string)
   0.0f 0.0f 0.0f
  1.0f 1.0f 1.0f
  0.0f 0.0f 0.0f 0.0f
  [MESH]
    -1
     [VRTS]
       0
       0
       0
       -0.5f  -0.5f  0.0f
       0.5f -0.5f 0.0f
       0.0f 0.5f 0.0f
      END
     [TRIS]
       -1
       0 1 2
     END
   END
END
"""

file_list = glob.glob('*.b3dtxt')

# is the list empty
if not file_list:
    print("there is no b3dtxt files to convert")


for b3d_text_file in file_list:
    # does the .b3d exist or not?
    b3dfile = b3d_text_file[:-7] + '.b3d'
    #print(b3dfile)
    if not Path(b3dfile).exists():
        # time to create a b3dfile
        builder = B3DBuilder()
        builder.parseFile(b3d_text_file)
        builder.BuildB3DFile(b3dfile)

    else:
        print("files already exist")
    
    
