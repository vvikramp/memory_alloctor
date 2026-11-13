#include<stdio.h>
#include <stddef.h>
#include <string.h>

#define POOL_SIZE 1024
static unsigned char memory[POOL_SIZE];
static int offset = 0;

void *bump_allocator(int size) {
    if(offset + size > POOL_SIZE) {
        return NULL;
    }

    void *p = &memory[offset];
    offset += size;
    return p;
}

void bump_reset(void) {
    offset = 0;   
}


int main() {

    char *s = bump_allocator(6);
    int *a = bump_allocator(1);
    int *b = bump_allocator(sizeof(int));
    
    *a = 10; *b = 32;
    strcpy(s, "hi");
    printf("%d %d %s\n", *a, *b, s);
    printf("used = %d\n", offset);

    bump_reset();

    return 0;
}

// If you have time before your interview, add Arena (Bump) Allocation alongside your standard malloc and free. Mentioning that you implemented an arena allocator specifically because LLVM and Clang use them to speed up AST (Abstract Syntax Tree) generation will immediately impress a compiler research panel.
// • Which allocation strategy you are using (free lists, buddy system, or slab allocation)
// • Whether you are targeting single-threaded or multi-threaded safety
