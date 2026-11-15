#ifndef HEAP_H
#define HEAP_H

typedef struct {
    char *start;
    char *end;
} Heap;

bool heap_init(Heap *heap, size_t size);
void *heap_alloc(Heap *heap, size_t size);
void heap_free(Heap *heap, void *ptr);
bool heap_check(Heap *heap);

#endif