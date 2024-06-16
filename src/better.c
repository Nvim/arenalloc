/* *********************************************
 *   Better implementation that uses alignment
 *   and an 'allocator' interface
 *   https://bytesbeneath.com/p/the-arena-custom-memory-allocators
 * *********************************************/
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  // void * alloc(size_t size, void* context):
  void *(*alloc)(size_t size, void *context);
  // void free(size_t size, void* ptr, void * context):
  void (*free)(size_t size, void *ptr, void *context);
  // The memory allocator implemented, Arenas in this case:
  void *context;
} Allocator;

// calls a's 'alloc' function for a size of 'n', and casts to a T pointer:
#define make(T, n, a) ((T *)((a).alloc(sizeof(T) * n, (a).context)))

// calls a's 'free' function on the pointer p
#define release(s, p, a) ((a).free(s, p, (a).context))

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

typedef struct {
  void *base;       // data buffer
  size_t size;      // max size (size of buffer)
  size_t offset;    // index in buffer (different from commited bc alignment)
  size_t committed; // amount of memory used
} Arena;

#define arena_alloc_init(a)                                                    \
  (Allocator) { arena_alloc, arena_free, a }

#define is_power_of_two(x) ((x != 0) && ((x & (x - 1)) == 0))

uintptr_t align_forward(uintptr_t ptr, size_t alignment) {
  uintptr_t p, a, modulo;
  if (!is_power_of_two(alignment)) {
    return 0;
  }

  p = ptr;
  a = (uintptr_t)alignment;
  modulo = p & (a - 1);

  if (modulo) {
    p += a - modulo;
  }

  return p;
}

void *arena_alloc_aligned(Arena *a, size_t size, size_t alignment) {
  uintptr_t curr_ptr = (uintptr_t)a->base + (uintptr_t)a->offset;
  uintptr_t offset = align_forward(curr_ptr, alignment);
  offset -= (uintptr_t)a->base;

  if (offset + size > a->size) {
    return 0;
  }

  a->committed += size;
  void *ptr = (uint8_t *)a->base + offset;
  a->offset = offset + size;

  return ptr;
}

void *arena_alloc(size_t size, void *context) {
  if (!size) {
    return 0;
  }
  return arena_alloc_aligned((Arena *)context, size, DEFAULT_ALIGNMENT);
}

// Does nothing.
void arena_free(size_t size, void *ptr, void *context) {
  (void)ptr;
  (void)size;
  (void)context;
}

void arena_free_all(void *context) {
  Arena *a = context;
  a->offset = 0;
  a->committed = 0;
}

Arena arena_init(void *buffer, size_t size) {
  return (Arena){.base = buffer, .size = size};
}
