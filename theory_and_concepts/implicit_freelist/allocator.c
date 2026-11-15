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

    heap_free(&heap, p);
    heap_check(&heap);
}

// main funcs impl
bool heap_init(Heap *heap, size_t size) {
    char *base = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(base == MAP_FAILED) {
        return false;
    }
    heap->start = base + 8;
    heap->end = base + size - 8;
    *(header_t *)heap->start = make_header(heap->end - heap->start, false);
    return true;
}

void *heap_alloc(Heap *heap, size_t size) {
    // size_t padding = (size + HEADER_SIZE) % ALIGNMENT; -> this is the remainder not the allignemnt for 32 byte block
    size_t need = ((size + HEADER_SIZE) + ALIGNMENT - 1) & (size_t)~(ALIGNMENT - 1); // round up then round down (The low 4 bits are its remainder when divided by 16) - cool

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

void heap_free(Heap *heap, void *ptr) {
    if(ptr == NULL) {
        printf("Cannot free NULL ptr -> FREE FAILED\n");
        return;
    }
    if(((size_t)ptr & (ALIGNMENT -1)) != 0) {
        printf("ptr is not aligned to 16 -> FREE FAILED");
        return;
    }

    header_t *h = (header_t *)((char *)ptr - 8);

    if((char *)h < heap->start || (char *)h >= heap->end) {
        printf("Free outside heap space is blocked -> FREE FAILED");
        return;
    }
    if(!is_alloc(h)){
        printf("Already Freed -> FREE FAILED");
        return;
    }
    
    *h = *h & ~1; // mark as free
    printf("Dobby is FREE :)\n");
    header_t *next_h = next_block(h);

    if((char *)next_h < heap->end && !is_alloc(next_h)) {
        // next block is free
        *h = make_header(block_size(h) + block_size(next_h), false);
    }
}

bool heap_check(Heap *heap) { 

    header_t *h = (header_t *)heap->start;
    bool check = true;
    do {
        size_t size = block_size(h);
        if(size < MIN_BLOCK) {
            printf("Block size is less then 16. Heap corrupted\n");
            check = false;
            break;
        }
        if(((size_t)size & (ALIGNMENT-1)) !=0) {
            printf("Block size is not Aligned\n");
            check = false;
        }

        char *payload = (char *)h + ALIGNMENT;
        if(((size_t)payload & (ALIGNMENT-1)) !=0) {
            printf("Payload is not aligned\n");
            check = false;
        }

        header_t *next_h = next_block(h);
        if((char *)next_h < heap->end && !is_alloc(next_h) && !is_alloc(h)) {
            printf("Continous free blocks found"\n);
            check = false;
        }
        h = next_h;
    } while((char *)h < heap->end);

    if((char *)h > heap->end) {
        printf("Header overshooting.Heap corrupted.\n");
        check = false;
    }

    if(check) {
        printf("Heap check passed\n");
    } else {
        printf("Heap check faield\n");
    }

    return check;
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