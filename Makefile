default: scp

override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -Ofast -flto -fuse-linker-plugin -MMD -I./include
override LDFLAGS += -lm -lGL -lglfw -march=native -O3 -flto -fuse-linker-plugin
#override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -g -Og -MMD -I./include
#override LDFLAGS += -lm -lGL -lglfw -march=native -g -Og

CC=gcc

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
DEPENDS = $(wildcard objs/*.d)

-include $(DEPENDS) 

scp : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o scp

testparser : objs/b3dloader.o objs/test1.o objs/scp-os.o objs/errormsg.o 
	$(CC) $^ -o testparser

objs/test1.o: test/test1.c
	$(CC) $(CFLAGS) -c $< -o $@

objs/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@	

#objs/%.o : test/%.c
#	$(CC) $(CFLAGS) -c  $< -o $@

# gay
#OBJS: $(SRCS)
#	$(CC) $(CFLAGS) -c -I src -o $@ $(SRCS)



clean:
	$(RM) scp src/*.o 
