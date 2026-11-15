#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#define KB(x) ((x) * 1024)
#define ALIGNMENT 16
#define HEADER_SIZE sizeof(size_t)
#define MIN_BLOCK 16

typedef size_t header_t;

typedef struct {
    char *start;
    char *end;
} Heap;

// main funcs
bool heap_init(Heap *heap, size_t size);
void *heap_alloc(Heap *heap, size_t size);   // TODO: first fit + split
void heap_free(Heap *heap, void *ptr);      // TODO: clear bit + merge forward
bool heap_check(Heap *heap);                // TODO: see below

// helpers
header_t make_header(size_t size, bool a);
header_t *next_block(header_t *h);
size_t block_size(header_t *h);
bool is_alloc(header_t *h);

void heap_dump(Heap *heap);


int main() {

    Heap heap = {};
    
    if(heap_init(&heap, KB(10))) {
        printf("Heap initiation successfully.\n");
        // printf("sizeof alocated: %ld", (heap.end - heap.start));
    } else {
        printf("Heap initiation failed");
        return 0;
    }

    int *p = heap_alloc(&heap, sizeof(int));
    *p = 4;
    printf("%d\n", *p);
}

// main funcs impl
bool heap_init(Heap *heap, size_t size) {
    char *base = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(base == MAP_FAILED) {
        return false;
    }
    heap->start = base + 8;
    heap->end = base + size - 8;
    *(header_t *)heap->start = make_header(size, false);
    return true;
}

void *heap_alloc(Heap *heap, size_t size) {
    // size_t padding = (size + HEADER_SIZE) % ALIGNMENT; -> this is the remainder not the allignemnt for 32 byte block
    size_t need = ((size + HEADER_SIZE) + ALIGNMENT - 1) & (size_t)~(ALIGNMENT - 1);

    for(header_t *h = (header_t *)heap->start; (char *)h < heap->end; h=next_block(h)) {
        size_t b_size = block_size(h);
        bool allocted = is_alloc(h);
        if(b_size < need || allocted) {
            continue;
        }

        size_t leftover = b_size - need;
        if(leftover >= ALIGNMENT) {
            header_t *next = (header_t *)((char *)h + need);
            *next = make_header(leftover, false);
        }
        *h = make_header(need, true);
        return (void *)(h+1);
    }

    return NULL;
}

// helpers impl
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

// heap walk log
// void heap_dump(Heap *heap) {
//     for (header_t *h = (header_t *)heap->start; (char *)h < heap->end; h = next_block(h))
//         printf("%p  size=%5zu  %s\n", (void *)h, block_size(h), is_alloc(h) ? "ALLOC" : "free");
//     printf("----\n");
// }