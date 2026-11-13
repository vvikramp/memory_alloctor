CC     = clang
CFLAGS = -std=c17 -Wall -Wextra -g -MMD -MP

SRCS = $(wildcard *.c)      # every .c file in the folder
OBJS = $(SRCS:.c=.o)        # main.c → main.o, mathx.c → mathx.o
DEPS = $(OBJS:.o=.d)        # files that track which headers each .c uses

hello: $(OBJS)              # link all .o files into the program
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c                    # how to compile any .c into a .o
	$(CC) $(CFLAGS) -c $< -o $@

run: hello
	./hello

clean:
	rm -rf hello hello.dSYM *.o *.ds

-include $(DEPS)

.PHONY: run clean
