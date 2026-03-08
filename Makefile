default: scp

override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -Ofast -flto -fuse-linker-plugin -MMD -I./include
override LDFLAGS += -lm -lGL -lglfw -march=native -O3 -flto -fuse-linker-plugin
#override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -g -Og -MMD -I./include
#override LDFLAGS += -lm -lGL -lglfw -march=native -g -Og

CC=gcc

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
DEPENDS = $(SRCS:.c=.d)

scp : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o scp

test : b3dloader.o test1.o scp-os.o errormsg.o
	$(CC) $^ -o test_triangle 

%.o : src/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

%.o : test/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

# gay
#OBJS: $(SRCS)
#	$(CC) $(CFLAGS) -c -I src -o $@ $(SRCS)


-include $(DEPENDS)

clean:
	$(RM) scp src/*.o 
