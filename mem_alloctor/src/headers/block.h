#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h> 

#define ALIGNMENT 16 // each payload block is 16 bytes
#define HEADER_SIZE sizeof(size_t)
#define MIN_BLOCK 32 // each blokc with header and footer in 32 bytes 

typedef size_t header_t;

header_t make_header(size_t size, bool a);
header_t *next_block(header_t *h);
size_t block_size(header_t *h);
bool is_alloc(header_t *h);
size_t align_up(size_t n);
header_t *header_of(void *ptr);
char *payload_of(header_t *h);
header_t *prev_block(header_t *h);
header_t *prev_footer(header_t *head);
header_t *footer_of(header_t *head);
void set_block(char *start, size_t size, bool a);
bool check_boundary(header_t *head, header_t *foot);

#endif