// Setup

// Put it in tests/stress.c, with its own main(). It includes only heap.h (plus block.h if you want to look at headers).
// Read the seed from argv, call srand(seed), and print the seed. When it fails, you can rerun the exact same sequence.
// Keep a slot table: void *ptr[100] and size_t size[100].
// Loop (around 100,000 times): pick a random slot i.

// Empty slot → allocate a random size (mostly 1 to 300, sometimes larger).
// If you get NULL, the heap is full. That's fine; skip it.
// If you get a pointer, check that it's 16-aligned, lies between start and end, and that its block is at least size + 8 bytes.
// Fill the payload with memset(p, i & 0xFF, size) and save p and size in slot i.
// Full slot → check, then free.
// Every byte must still equal i & 0xFF. If one doesn't, two blocks overlap or a header got written into a payload.
// Free it and clear the slot.
// Run heap_check after every step. If it fails, print the step number and the seed, then call abort().