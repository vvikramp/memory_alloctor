#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>

#include "block.h"
#include "heap.h"

int main(void) {

    Heap heap = {0};
    
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
