#ifndef HEAP_H
#define HEAP_H

#define KB(x) ((x) * 1024)

typedef enum {
    FIRST,
    NEXT, 
    BEST
} FIT;

typedef struct {
    char *start;
    char *end;
    char *rover;
    char *free_head;
    FIT fit;
} Heap;

bool heap_init(Heap *heap, size_t size, FIT fit);
void *heap_alloc(Heap *heap, size_t size);
void heap_free(Heap *heap, void *ptr);
bool heap_check(Heap *heap);
bool heap_destroy(Heap *heap);

#endif