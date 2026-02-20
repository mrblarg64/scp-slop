default: scp

override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -Ofast -flto -fuse-linker-plugin -MMD -I./include
override LDFLAGS += -lm -lGL -lglfw -march=native -O3 -flto -fuse-linker-plugin
#override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -g -Og
#override LDFLAGS += -lGL -lglfw -march=native -g -Og

CC=gcc

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

scp: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o scp
OBJS: $(SRCS)
	$(CC) $(CFLAGS) -c -I src -o $@ $(SRCS)
clean:
	$(RM) scp src/*.o
