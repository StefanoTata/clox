#ifndef ARENA_H
#define ARENA_H

// 
// INTERFACE
//

#include <stddef.h>
#include <stdint.h>

typedef struct Arena{
  unsigned char* buf;
  size_t buf_len;
  size_t prv_offset;
  size_t cur_offset;
} Arena;

void arena_init(Arena *a, void* backing_buffer, size_t backing_buffer_lenght);

void *arena_alloc(Arena *a, size_t size);

void *arena_alloc_align(Arena *a, size_t size, size_t align);

void *arena_resize(Arena *a, void* old_buf, size_t old_size, size_t new_size);

void *arena_resize_align(Arena *a, void* old_buf, size_t old_size, size_t new_size, size_t align);

void arena_free_all(Arena *a);

#endif

// 
// IMPLEMENTATION
//

#ifdef ARENA_IMPL

#include <assert.h>
#include <string.h>

inline int is_power_of_two(uintptr_t x){
  return (x & (x-1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align){
  assert(is_power_of_two(align));
  
  uintptr_t p = ptr, mod = 1;
  mod = p & ((uintptr_t)(align) - 1);
  if(mod != 0) p += align - mod;

  return p;
}

void *arena_alloc_align(Arena *a, size_t size, size_t align){
  uintptr_t cur_ptr = (uintptr_t)a->buf + (uintptr_t)a->cur_offset;
  uintptr_t offset =  align_forward(cur_ptr, align);
  offset -= (uintptr_t)a->buf;
  
  if(offset + size <= a->buf_len){
    void* p = &a->buf[offset];
    a->prv_offset = offset;
    a->cur_offset = offset+size;

    memset(p, 0, size);
    return p;
  }
  else{
    assert(0 && "Arena out of memory"); 
    return NULL;
  }
}

#ifndef DEFAULT_ALIGN
#define DEFAULT_ALIGN (2*sizeof(void*))
#endif
void *arena_alloc(Arena *a, size_t size){
  return arena_alloc_align(a, size, DEFAULT_ALIGN);
}

void arena_init(Arena *a, void* backing_buffer, size_t backing_buffer_lenght){
  a->buf = (unsigned char*)backing_buffer;
  a->buf_len = backing_buffer_lenght;
  a->cur_offset = 0;
  a->prv_offset = 0;
}

void *arena_resize_align(Arena *a, void* old_buf, size_t old_size, size_t new_size, size_t align){
  assert(is_power_of_two(align));
  
  if(old_buf == 0 || old_size == 0)
    return arena_alloc_align(a, new_size, align);
  else if(a->buf <= (unsigned char*)old_buf && (unsigned char*)old_buf < (a->buf + a->buf_len)){
    if(a->buf + a->prv_offset == old_buf){
      a->cur_offset = a->prv_offset + new_size;
      if(new_size > old_size)
        memset(&a->buf[a->cur_offset], 0, new_size - old_size);
      return old_buf;
    }
    else{
      void* new_buf = arena_alloc_align(a, new_size, align);
      size_t copy_size = old_size < new_size ? old_size : new_size;
      memmove(new_buf, old_buf, copy_size);
      return new_buf;
    }
  }
  else{
    assert(0 && "Out of memory");
    return NULL;
  }
  
}

void *arena_resize(Arena *a, void* old_buf, size_t old_size, size_t new_size){
  return arena_resize_align(a, old_buf, old_size, new_size, DEFAULT_ALIGN);
}

void arena_free_all(Arena* a){
  a->buf = 0;
  a->buf_len = 0;
  a->prv_offset = 0;
  a->cur_offset = 0;
}

#endif
