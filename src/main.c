#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arena {
  int size;           // max size
  int offset;         // current offset in the array
  uint8_t *data;      // data object (1 byte chunks)
  struct arena *next; // for if we need to allocate more
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
  fprintf(stderr, "* Allocating %ld bytes...", size);
  // Enough space in arena:
  if (arena->offset + size < arena->size) {
    fprintf(stderr, " ... enough space was found.");
    uint8_t *ptr = &arena->data[arena->offset];
    arena->offset += size;
    fprintf(stderr, "\n");
    return ptr;
  }
  // Enough space in a new Arena:
  if (size < arena->size) {
    // Need to create a new Arena pointer:
    if (arena->next == NULL) {
      Arena new = arena_init(arena->size);
      Arena *next = (Arena *)malloc(sizeof(Arena));
      memcpy(next, &new, sizeof(new));
      arena->next = next;
      printf("Making a new arena to fit %ld bytes\n", size);
      fprintf(stderr, " ... not enough space, used an existing next");
    } else {
      printf("Using existing next arena to fit %ld bytes\n", size);
      fprintf(stderr, " ... not enough space, made a new next");
    }
    fprintf(stderr, "\n");
    return arena_alloc(arena->next, size);
  }
  // No space :
  fprintf(stderr, "arena_alloc: can't allocate %ld bytes, max size is %d", size,
          arena->size);
  return NULL;
}

// Not reusable, data has been freed from heap
void arena_free(Arena *arena) {
  if (arena->next != NULL) {
    arena_free(arena->next);
  }
  free(arena->data);
  // Not freeing these since they are stack allocated:
  arena->size = 0;
  arena->offset = 0;
}

// Reusable, sets offsets to 0
void arena_reset(Arena *arena) {
  arena->offset = 0;
  Arena *current = arena->next;
  while (current != NULL) {
    current->offset = 0;
    current = current->next;
  }
}

void log_arena(const Arena *arena, const char *arena_name) {
  printf("\nLogging Arena %s:\n\tSize: %d\n\tOffset: %d\n", arena_name,
         arena->size, arena->offset);
}

int main(int argc, char *argv[]) {
  Arena arena = arena_init(1024);
  printf("\n\n------ Allocations #1: -------\n\n");
  uint8_t *ptr = arena_alloc(&arena, 224);
  uint8_t *ptr2 = arena_alloc(&arena, 960); // fits a new arena, not this one
  uint8_t *ptr3 = arena_alloc(&arena, 700); // went in the 1st
  uint8_t *ptr4 = arena_alloc(&arena, 40);  // went in the 2nd
  log_arena(&arena, "#1");
  log_arena(arena.next, "#1->next");

  // puts both offsets to 0:
  arena_reset(&arena);
  printf("\n\n------ Reset -------\n\n");
  log_arena(&arena, "#1");
  log_arena(arena.next, "#1->next");

  // Reallocating test:
  printf("\n\n------ Allocations #2: -------\n\n");
  ptr = arena_alloc(&arena, 224);
  ptr2 = arena_alloc(&arena, 960);         // fits a new arena, not this one
  ptr3 = arena_alloc(&arena, 780);         // went in the 1st
  ptr4 = arena_alloc(&arena, 40);          // went in the 2nd
  uint8_t *ptr5 = arena_alloc(&arena, 26); // went in a 3rd

  // Logging...
  Arena current = arena;
  log_arena(&current, "#1");
  int i = 1;
  while (current.next != NULL) {
    current = *(current.next);
    char char_index = '1' + i;
    log_arena(&current, &char_index);
    i++;
  }

  // Free all 3 recursively:
  arena_free(&arena);

  return EXIT_SUCCESS;
}
