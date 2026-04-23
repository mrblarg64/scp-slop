default: scp
PHONY: clean

#override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -Ofast -flto -fuse-linker-plugin -MMD -I./include
#override LDFLAGS += -lm -lGL -lglfw -march=native -O3 -flto -fuse-linker-plugin
override CFLAGS += -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -pipe -march=native -g -Og -MMD -I./include
override LDFLAGS += -lm -lGL -lglfw -march=native -g -Og

CC=gcc

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))
DEPENDS = $(OBJS:.o=.d)

-include $(DEPENDS) 

scp : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o scp


obj/%.o : src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p $@

clean:
	$(RM) scp obj/*.o obj/*.d 
