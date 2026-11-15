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

bool heap_init(Heap *heap, size_t size);
header_t pack(size_t size, bool a);


int main() {

    Heap heap = {};
    
    if(heap_init(&heap, KB(10))) {
        printf("Heap initiation successfully.\n");
        printf("sizeof heap %zu, start ptr addr %p, end ptr s=addr %p", sizeof(heap), (void *)heap.start, (void *)heap.end);
    } else {
        printf("Heap initiation failed");
        return 0;
    }
}

bool heap_init(Heap *heap, size_t size) {
    char *base = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(base == MAP_FAILED) {
        return false;
    }
    heap->start = base + 8;
    heap->end = base + size -8;
    *(header_t *)heap->start = pack(size, false);
    return true;
}

header_t pack(size_t size, bool a) {
    return size | a;
}