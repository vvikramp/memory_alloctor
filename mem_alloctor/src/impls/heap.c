#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#include "block.h"
#include "heap.h"

// return the block based on the fit
static header_t *find_fit(Heap *heap, size_t need) {
    FIT fit = heap->fit;
    switch(fit) {
        case FIRST: {
            for(header_t *h = (header_t *)heap->free_head; (char *)h < heap->end; h=next_free(h)) {
                size_t b_size = block_size(h);
                bool allocted = is_alloc(h);
                if(b_size < need || allocted) {
                    continue;
                }
                return h;
            }
            break;
        }
        case BEST: {
            header_t *best = NULL;
            size_t size = SIZE_MAX;

            for(header_t *h = (header_t *)heap->free_head; (char *)h < heap->end; h=next_free(h)) {
                size_t b_size = block_size(h);
                bool allocted = is_alloc(h);
                if(b_size < need || allocted) {
                    continue;
                }
                if(b_size == need) {
                    return h;
                }
                if(b_size < size) {
                    size = b_size;
                    best = h;
                }
            }
            if (best != NULL) {
                return best;
            }
            break;
        }
        case NEXT: {
            header_t *h = (header_t *)heap->rover;
            do {                
                size_t b_size = block_size(h);
                bool allocted = is_alloc(h);
                if(b_size < need || allocted) {
                    h = next_block(h);
                    if((char *)h == heap->end) {
                       h = (header_t *)heap->start;
                    }
                    continue;
                }
                heap->rover = (char *)h;
                return h;
            } while((char *)h != heap->rover);
            break;
        }
        default:
            printf("Invalid fit");
    }

    return NULL;
}

static void place_block(Heap *heap, header_t *h, size_t need) {
    size_t b_size = block_size(h);
    list_remove(heap, h); 

    size_t leftover = b_size - need;
    if(leftover >= MIN_BLOCK) {
        set_block(h, need, true); 
        header_t *leftover_block = next_block(h);
        set_block(leftover_block, leftover, false); // split
        list_insert(heap, leftover_block);
    } 
    else {
        set_block(h, b_size, true);
    }
}

static void list_insert(Heap *heap, header_t *h) {
    // Last in - make the insert block as the new free head
    set_next_free(h, heap->free_head);
    set_prev_free(h, NULL);
    set_prev_free(heap->free_head, h);

    heap->free_head = h;
}

static void list_remove(Heap *heap, header_t *h) {
    header_t *h_prev = prev_free(h);
    header_t *h_next = next_free(h);

    // start of the list
    if((char *)h == heap->free_head){
        heap->free_head = h_next;    
        set_prev_free(h, NULL);
    } else {
        set_next_free(h_prev, h_next);
    }

    // end of the list
    if(h_next == NULL) {
        set_next_free(h_prev, NULL);
    } else {
        set_prev_free(h_next, h_prev);
    }
}

bool heap_init(Heap *heap, size_t size, FIT fit) {
    char *base = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(base == MAP_FAILED) {
        return false;
    }
    heap->start = base + HEADER_SIZE;
    heap->end = base + size - HEADER_SIZE;
    heap->fit = fit;
    // heap->rover = heap->start;
    heap->free_head = NULL;
    
    *(header_t *)base = make_header(HEADER_SIZE, true); // fake_footer at start
    *(header_t *)heap->end = make_header(0, true); // fake_header at end

    set_block((header_t *)heap->start, heap->end - heap->start, false);
    list_insert(heap, (header_t *)heap->start); // this sets free to start
    return true;
}

void *heap_alloc(Heap *heap, size_t size) {

    if (size > (size_t)(heap->end - heap->start)) { 
        return NULL;
    }
    size_t need = align_up(size);

    header_t *fit_block = find_fit(heap, need);

    if(fit_block == NULL) {
        printf("Heap full. Cannot find a block for this ");
        return NULL;
    }

    place_block(heap, fit_block, need);    
    return (void *)payload_of(fit_block);

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
    header_t *block = h;

    if((char *)h < heap->start || (char *)h >= heap->end) {
        printf("Free outside heap space is blocked -> FREE FAILED\n");
        return;
    }
    if(!is_alloc(h)){
        printf("Already Freed\n");
        return;
    }

    size_t free_size = block_size(h);

    header_t *next_h = next_block(h);
    header_t *prev_f = prev_footer(h);

    if(!is_alloc(prev_f)) {
        block = prev_block(h);
        list_remove(heap, block);
        free_size += block_size(prev_f);
    }

    if(!is_alloc(next_h)) {
        free_size += block_size(next_h);
        list_remove(heap, next_h);
    }

    set_block(block, free_size, false);
    list_insert(heap, block);
    // if(heap->rover > (char *)block && heap->rover < (char *)block + free_size) {
    //     heap->rover = (char *)block;
    // }

    printf("Dobby is FREE :)\n");
}

bool heap_check(Heap *heap) { 

    header_t *h = (header_t *)heap->start;
    bool check = true;
    do {
        size_t size = block_size(h);
        if(size < MIN_BLOCK) {
            printf("Block size is less then 32. Heap corrupted\n");
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

        if(!check_boundary(h, footer_of(h))) {
            printf("Header and footer is not same\n");
            check = false;
            break;
        }

        h = next_h;
    } while((char *)h < heap->end);

    for(header_t *h = (header_t *)heap->free_head; (char *)h < heap->end; h=next_free(h)) {
        if((char *)h < heap->start || (char *)h >= heap->end) {
            printf("Free list block outside heap space found.\n");
            check = false;
            break;
        }
        if(is_alloc(h)) {
            printf("Allocated block found in free list.\n");
            check = false;
        }

        header_t *next_h = next_free(h);
        if(next_h != NULL && prev_free(next_h) != h) {
            printf("Free list is not properly linked.\n");
            check = false;
            break;
        }
    }

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