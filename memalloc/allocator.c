#include<stdio.h>
#include<sys/mman.h>

void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);

// TODO: msync, munmap(addr, length)



void* malloc(size_t size) {

}

int main() {
}

// notes:
// -> custom malloc:
    // The Threshold Check - brk() vs. mmap()
    // Anonymous Mapping
    // Metadata Header
    // Demand Paging - page faults
    // Immediate Release on free()