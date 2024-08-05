#include <assert.h>
#include <stdlib.h>

#define ARENA_IMPL
#include "arena.h"

int main(){
  assert(is_power_of_two(2));
  assert(!is_power_of_two(3));
  
  assert(align_forward(3, 2) == 4);
  assert(align_forward(3, 2) != 5);
  assert(align_forward(129, 64) == 192);

  unsigned char b[1024];
  Arena a = {0};

  arena_init(&a, b, 1024);

  int *i = (int*) arena_alloc(&a, sizeof(int));
  *i = 22;
  
  char *str = 0;
  str = arena_alloc(&a, 2*sizeof(char));
  memmove(str, "Hi", 2);

  assert(strcmp(str, "Hi") == 0);
  assert(*i == 22);
  
  arena_resize(&a, str, 2, 8);
  memmove(str+2, " world", 6);
  assert(strcmp(str, "Hi world") == 0);

  arena_free_all(&a);
  assert(a.buf_len == 0);
  
  return 0;
}
