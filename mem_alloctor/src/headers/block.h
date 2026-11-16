#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h> 

#define ALIGNMENT 16
#define HEADER_SIZE sizeof(size_t)
#define MIN_BLOCK 16

typedef size_t header_t;

header_t make_header(size_t size, bool a);
header_t *next_block(header_t *h);
size_t block_size(header_t *h);
bool is_alloc(header_t *h);
size_t align_up(size_t n);
header_t *header_of(void *ptr);
char *payload_of(header_t *h);

#endif