CC     = clang
CFLAGS = -std=c17 -Wall -Wextra -g -MMD -MP -fsanitize=address,undefined -I$(SRC_DIR)/headers

# Folders
SRC_DIR = src
BUILD_DIR = build

SRCS = $(shell find ${SRC_DIR} -name '*.c') # every .c file in the directory, wildcard find only in the parent folder
OBJS = $(patsubst ${SRC_DIR}/%.c, ${BUILD_DIR}/%.o, ${SRCS}) # main.c → main.o
DEPS = $(OBJS:.o=.d) # files that track which headers each .c uses

allocator: $(OBJS) # link all .o files into the program
	$(CC) $(CFLAGS) -o $@ $^

${BUILD_DIR}/%.o: ${SRC_DIR}/%.c # compile any .c into a .o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: allocator
	./allocator

clean:
	rm -rf $(BUILD_DIR) allocator allocator.dSYM

-include $(DEPS)

.PHONY: run clean
