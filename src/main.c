#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int size;      // max size
  int offset;    // current offset in the array
  uint8_t *data; // data object (1 byte chunks)
} Arena;

Arena arena_init(size_t size) {
  void *data = malloc(size * sizeof(uint8_t));
  Arena new = (Arena){
      .size = size,
      .offset = 0,
      .data = data,
  };
  return new;
}

// Pointer to chunk of 'size' bytes
void *arena_alloc(Arena *arena, size_t size) {
  if (arena->offset + size >= arena->size) {
    fprintf(stderr, "Alloc error: not enough space in arena");
    return NULL;
  }
  uint8_t *ptr = &arena->data[arena->offset];
  arena->offset += size;
  return ptr;
}

// Reusable
void arena_reset(Arena *arena) { arena->offset = 0; }

// Not reusable, data has been freed from heap
void arena_free(Arena *arena) {
  free(arena->data);
  arena->size = 0;
  arena->offset = 0;
}

void log_arena(const Arena *arena) {
  printf("\nLogging Arena:\n\tSize: %d\n\tOffset: %d\n", arena->size,
         arena->offset);
}

int main(int argc, char *argv[]) {
  printf("Hello World!");
  Arena arena = arena_init(1024);
  uint8_t *ptr = arena_alloc(&arena, 26);

  for (int i = 0; i < 26; i++) {
    ptr[i] = 'A' + i;
    printf("%c ", ptr[i]);
  }
  ptr[26] = '8';
  ptr[27] = '9';
  printf("%c %c", ptr[26], ptr[27]);
  log_arena(&arena);
  return EXIT_SUCCESS;
}
