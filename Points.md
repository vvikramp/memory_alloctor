- No extension is normal on macOS and Linux. Unlike .exe on Windows, a file is runnable because it has the "executable" permission, not because of its name.

- You can skip address casting completely by aligning the offset. This works because malloc already returns a 16-aligned start.

- _Align for structs and why "align a type by its size" may not be always right.

- There is nothing between C and machine code, they is no interpreter, virtual machine. C complies into machine code. And we dont write C program for a particular architecture. We write it to an abstract machine and the compliers maps this instructions with the actual CPU behaviour. 

- C program -> pre-processing - compliation - > assembly - linking 

- Storage duration - 
    - Automatic: block varibales
    - Static: global variables, strig literals
    - Dynamic: object memory got via malloc

- C standard doesn't say how big an int should be, it only says it should be atleast as big as a short and atleast 16 integer bites in size. Ihis applies for all the types. And the number of bytes for each type and the representation in memmory are implementation-defined.

- In the abstract machine every object has the representation composed of bytes. No matter whether the type is int, float, double or struct. The underlying representation is always a sequence of bytes. In C, a variable is just a few bytes in memory. Its type only tells the compiler how to read those bytes.

- The C abstract machine specifies a module that looks like this:
    - Object live in memory
    - Variables refer to objects
    - Objects have lifetime, alignment and byte represenetations
    - the complier then maps the abstract memory model into the real hardware

- Type punning is where you reinterpret teh stored butes of an object. 


- Why Doesn't malloc() Use mmap() for Everything?
    Using mmap() for small, frequent allocations would hurt performance for two reasons:
    • Kernel Overhead. Every mmap() and munmap() requires a context switch into the kernel, updates to page tables, and TLB (Translation Lookaside Buffer) invalidations.
    • Page Rounding. The kernel only allocates memory in whole pages (typically 4 KB). Calling mmap() for a 16-byte struct would waste 4,080 bytes of address space and physical memory per allocation.

- Every type has its own alignment rule. A type can only sit at addresses divisible by its alignment.

- Use (uintptr_t)ptr instead of (size_t)ptr when you treat a pointer as a number. 
size_t -> Big enough to hold the size of any object
uintptr_t -> Big enough to hold any pointer and convert it back without losing bits

- Crazy pointer work in explict free list helpers. Check those properly.
    header_t *next_free(header_t *h);
    header_t *prev_free(header_t *h);
    void set_next_free(header_t *h, header_t *next);
    void set_prev_free(header_t *h, header_t *prev);