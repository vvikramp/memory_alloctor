#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "block.h"

header_t make_header(size_t size, bool a) {
    return size | a;
}

// next block header
header_t *next_block(header_t *h) { 
    return (header_t *)((char *)h + block_size(h)); 
}

header_t *prev_block(header_t *h) { 
    header_t *prev_foot = prev_footer(h);
    return (header_t *)((char *)h - block_size(prev_foot)); 
}

header_t *prev_footer(header_t *head) {
    return (header_t *)((char *)head - HEADER_SIZE);
}

header_t *footer_of(header_t *head) {
    return (header_t *)((char *)head + block_size(head) - HEADER_SIZE);
}

void set_block(header_t *start, size_t size, bool a) {
    *(header_t *)start = make_header(size, a); // header
    *(header_t *)((char*)start + size - HEADER_SIZE) = make_header(size, a); // footer
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
    // return (n + HEADER_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT -1); // round up and down down (The low 4 bits are its remainder when divided by 16) - cool
    return (n + (2 * HEADER_SIZE) + (ALIGNMENT - 1)) & ~(ALIGNMENT -1); // including footer during align up
}

header_t *header_of(void *ptr) {
    return (header_t *)((char *)ptr - HEADER_SIZE);
}

char *payload_of(header_t *h) {
    return (char *)h + HEADER_SIZE;
}

bool check_boundary(header_t *head, header_t *foot) {
    if((is_alloc(head) == is_alloc(foot)) && (block_size(head) == block_size(foot))) {
        return true;
    }
    return false;
}

header_t *next_free(header_t *h) {
    header_t **next = (header_t **)((char *)h + HEADER_SIZE);
    return *next;
}

header_t *prev_free(header_t *h) {
    header_t **prev = (header_t **)((char *)h + (2 * HEADER_SIZE));
    return *prev;
}

void set_next_free(header_t *h, header_t *next) {
    header_t **next_free = (header_t **)((char *)h + HEADER_SIZE);
    *next_free = next;
}

void set_prev_free(header_t *h, header_t *prev) {
    header_t **prev_free = (header_t **)((char *)h + (2 * HEADER_SIZE));
    *prev_free = prev;
}