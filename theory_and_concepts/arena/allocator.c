#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define KB(x) ((x) * 1024)

typedef struct {
    char* buffer;
    int bufferSize;
    int offSet;
} Arena;

// void ArenaInit(Arena *arena, int buffer) {
//     *arena = (Arena) {
//         .buffer = malloc(buffer),
//         .bufferSize = buffer, 
//         .offSet = 0
//     };
// }

void ArenaInit(Arena *arena, int buffer) {
    *arena = (Arena) {
        .buffer = (char *) mmap(NULL, buffer, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        .bufferSize = buffer, 
        .offSet = 0
    };
}


void ArenaAlloc1(Arena *arena, int **a, int size) {
    if((*arena).offSet + size <= (*arena).bufferSize) {
        *a = (int *)((*arena).buffer + (*arena).offSet);
        (*arena).offSet += size;
    } else {
        printf("Arena limit reached");
    }
}

void* ArenaAlloc2(Arena *arena, int numOfElements, int elementSize, int alignSize) {
    int alloctmentSize = numOfElements * elementSize;

    // int totalCurrentOffSet = (int)(arena->buffer + arena->offSet);
    // int padding = alignSize - (totalCurrentOffSet % alignSize);
    // totalCurrentOffSet += padding;

    // if(totalCurrentOffSet + alloctmentSize > (int)arena->buffer + (int)arena->bufferSize) {
    //     return NULL;
    // }

    uintptr_t totalCurrentOffSet = (uintptr_t)(arena->buffer + arena->offSet);
    // uintptr_t padding = alignSize - (totalCurrentOffSet % alignSize);
    uintptr_t padding = (~totalCurrentOffSet + 1) & (alignSize - 1);
    totalCurrentOffSet += padding;

    if(totalCurrentOffSet + alloctmentSize > (uintptr_t)arena->buffer + (uintptr_t)arena->bufferSize) {
        return NULL;
    }

    arena->offSet += padding + alloctmentSize;
    memset((void*)totalCurrentOffSet, 0, alloctmentSize);
    return (void*)totalCurrentOffSet;
}

void ArenaReset(Arena *arena) {
    arena->offSet = 0;
    memset(arena->buffer, 0, arena->bufferSize);
}

void ArenaDelete(Arena *arena) {
    free(arena->buffer);
}

int main() {

    Arena arena = {0};
    ArenaInit(&arena, KB(10));

    // int* a = NULL; 
    // ArenaAlloc1(&arena, &a, 4);
    // *a = 10;

    // printf("sizeof a : %zu\n", sizeof(a));
    // printf("value of a : %p\n", a);
    // printf("value of a : %d", *a);

    // int* a = ArenaAlloc2(&arena, 10, sizeof(int), sizeof(int));

    // printf("sizeof a : %zu\n", sizeof(*a));
    // printf("value of a : %p\n", a);

    char* a = ArenaAlloc2(&arena, 100, sizeof(char), sizeof(char));
    strcpy(a, "Hello this is Vikram");

    printf("sizeof a : %zu\n", sizeof(*a));
    printf("value of a : %s\n", a);

    ArenaDelete(&arena);

    return 0;
}