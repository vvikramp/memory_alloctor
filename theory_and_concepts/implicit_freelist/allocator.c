#include<stdio.h>
#include <stddef.h>
#include <string.h>;   

#define KB(x) ((x) * 1024)
#define ALIGN 16;

typedef struct {
    size_t size;
    bool isFree;
} block_header;

typedef struct {
    char* buffer;
    int bufferSize;
} FreeList;

void* FreeListInit(FreeList *freeList, size_t size) {
    *freeList = {
        .buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        .bufferSize  = size
    };
}

void* FreeListAlloc(FreeList *freeListInit, size_t size) {
    char *find = freeListInit->buffer;

    do {
        if(find)
    } while();
}



int main() {

    FreeList freeList = {0};
    FreeListInit(&freeListInit, KB(14));

    int *i = FreeListAlloc(&freeListInit, size(int));

    return 0;
}

