#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "block.h"

header_t make_header(size_t size, bool a) {
    return size | a;
}

header_t *next_block(header_t *h) { 
    return (header_t *)((char *)h + block_size(h)); 
}

// removing the flag bit
size_t block_size(header_t *h) { 
    return (*h & ~(ALIGNMENT - 1));
}

bool is_alloc(header_t *h) {
    return (*h & 1);
}

size_t align_up(size_t n) {
    // size_t padding = (size + HEADER_SIZE) % ALIGNMENT; -> this is the remainder not the allignemnt for 32 byte block
    return (n + HEADER_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT -1); // round up and down down (The low 4 bits are its remainder when divided by 16) - cool
}

header_t *header_of(void *ptr) {
    return (header_t *)((char *)ptr - HEADER_SIZE);
}

char *payload_of(header_t *h) {
    return (char *)h + HEADER_SIZE;
}