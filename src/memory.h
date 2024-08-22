#ifndef MEMORY_H
#define MEMORY_H

///
/// INTERFACE
///

#include <stdint.h>

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)
#define GROW_ARRAY(type, pointer, old_count, new_count) (type*)reallocate(pointer, sizeof(type) * (old_count), sizeof(type) * (new_count))  
#define FREE_ARRAY(type, pointer, old_count) reallocate(pointer, sizeof(type) * (old_count), 0)

void* reallocate(void* pointer, size_t old_size, size_t new_size);

#endif

///
/// IMPLEMENTATION
///

#ifdef MEMORY_IMPL

#include <stdlib.h>

void* reallocate(void* pointer, size_t old_size, size_t new_size){
  if(new_size == 0){
    free(pointer);
    return NULL;
  }
  
  void* res = 0;
  if(old_size == 0) res = malloc(new_size);

  res = realloc(pointer, new_size);
  if(!res) exit(1);

  return res;
}

#endif
