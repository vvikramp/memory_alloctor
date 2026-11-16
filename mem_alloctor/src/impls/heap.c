#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#include "block.h"
#include "heap.h"

bool heap_init(Heap *heap, size_t size) {
    char *base = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(base == MAP_FAILED) {
        return false;
    }
    heap->start = base + HEADER_SIZE;
    heap->end = base + size - HEADER_SIZE;
    *(header_t *)heap->start = make_header(heap->end - heap->start, false);
    return true;
}

void *heap_alloc(Heap *heap, size_t size) {

    if (size > (size_t)(heap->end - heap->start)) return NULL;

    size_t need = align_up(size);

    for(header_t *h = (header_t *)heap->start; (char *)h < heap->end; h=next_block(h)) {
        size_t b_size = block_size(h);
        bool allocted = is_alloc(h);
        if(b_size < need || allocted) {
            continue;
        }

        size_t leftover = b_size - need;
        if(leftover >= MIN_BLOCK) {
            header_t *next = (header_t *)((char *)h + need);
            *next = make_header(leftover, false); // split the leftover block
            *h = make_header(need, true);
        } else {
            *h = make_header(b_size, true);
        }
        
        return (void *)(h+1);
    }

    return NULL;
}

void heap_free(Heap *heap, void *ptr) {
    if(ptr == NULL) {
        printf("free NULL ptr -> Doing ntg\n");
        return;
    }
    if(((size_t)ptr & (ALIGNMENT -1)) != 0) {
        printf("ptr is not aligned to 16 -> FREE FAILED\n");
        return;
    }

    header_t *h = header_of(ptr);

    if((char *)h < heap->start || (char *)h >= heap->end) {
        printf("Free outside heap space is blocked -> FREE FAILED\n");
        return;
    }
    if(!is_alloc(h)){
        printf("Already Freed -> FREE FAILED\n");
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

        char *payload = payload_of(h);
        if(((size_t)payload & (ALIGNMENT-1)) !=0) {
            printf("Payload is not aligned\n");
            check = false;
        }

        header_t *next_h = next_block(h);
        if((char *)next_h < heap->end && !is_alloc(next_h) && !is_alloc(h)) {
            printf("Continous free blocks found\n");
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

bool heap_destroy(Heap *heap) {
    int res = munmap(heap->start - HEADER_SIZE, (heap->end - heap->start) + (2 * HEADER_SIZE)); // 0 for success and -1 for failure
    if(res == 0) {
        printf("Heap destroyed passed\n");
        heap->start = NULL;
        heap->end = NULL;
        return true;
    } else {
        // munmap sets the global variable errno to indicate the errors 
        printf("Heap destroyed failed\n");
        return false;
    }

}