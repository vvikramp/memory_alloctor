CC     = clang
CFLAGS = -std=c17 -Wall -Wextra -g -MMD -MP -fsanitize=address,undefined

SRCS = $(wildcard *.c)      # every .c file in the folder
OBJS = $(SRCS:.c=.o)        # main.c → main.o, mathx.c → mathx.o
DEPS = $(OBJS:.o=.d)        # files that track which headers each .c uses
ASMS = $(SRCS:.c=.s)

allocator: $(OBJS)              # link all .o files into the program
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c                    # how to compile any .c into a .o
	$(CC) $(CFLAGS) -c $< -o $@

asm: $(ASMS)

%.s: %.c
	$(CC) $(CFLAGS) -S $< -o $@

run: allocator
	./allocator

clean:
	rm -rf allocator allocator.dSYM *.o *.d

-include $(DEPS)

.PHONY: run clean
