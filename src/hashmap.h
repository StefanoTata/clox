#ifndef HASMAP_H
#define HASHMAP_h

///
/// INTERFACE
///

#define INITIAL_CAPACITY 32
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#include "arena.h"

typedef struct{
  const char* key;
  void* value;
} Hm_entry;

typedef struct{
  Hm_entry* entries;
  size_t capacity;
  size_t lenght;
} Hashmap;

Hashmap* hm_create(Arena* a);

void* hm_get(Hashmap* hm, const char* key);

const char* hm_set (Hashmap* hm, const char* key, void* value, Arena* a);

#endif

///
/// IMPLEMENTATION
///

#ifdef HASHMAP_IMPL

#include <assert.h>
#include <string.h>

Hashmap* hm_create(Arena* a){
  Hashmap* hm = arena_alloc(a, sizeof(Hashmap));
  hm->lenght = 0;
  hm->capacity = INITIAL_CAPACITY;
  hm->entries = arena_alloc(a, INITIAL_CAPACITY*(sizeof(Hm_entry)));

  return hm;
}

static uint64_t hash_key(const char* key){
  uint64_t hash = FNV_OFFSET;
  for (const char* p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

void* hm_get(Hashmap* hm, const char* key){
  uint64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (uint64_t)(hm->capacity - 1));
  
  while(hm->entries[index].key != NULL){
    if(strcmp(key, hm->entries[index].key) == 0){
      return hm->entries[index].value;
    }
    index++;
    if(index >= hm->capacity){
      index = 0;
    }
  }

  return NULL;
}

const char* hm_set_entry(Hm_entry* entries, size_t capacity,
  const char* key, void* value, size_t* plenght){
  
  uint64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (uint64_t)(capacity - 1));
  
  while(entries[index].key != NULL){
    if(strcmp(key, entries[index].key) == 0){
      entries[index].value = value;
      return entries[index].key;
    }
    index++;
    if(index >= capacity){
      index = 0;
    }
  }

  if(plenght != NULL) {
    //key = _strdup(key);
    (*plenght)++;
  }
  entries[index].key = key;
  entries[index].value = value;
  return key;
}

static int hm_expand(Hashmap* hm, Arena* a){
  size_t new_capacity = hm->capacity * 2;
  if(new_capacity < hm->capacity) return 0;

  Hm_entry* new_entries = arena_alloc(a, new_capacity * sizeof(Hm_entry));
  for(size_t i=0; i<hm->capacity; ++i){
    Hm_entry entry = hm->entries[i];
    if(entry.key != NULL) hm_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
  }

  hm->entries = new_entries;
  hm->capacity = new_capacity;
  return 1;
}

const char* hm_set (Hashmap* hm, const char* key, void* value, Arena* a){
  assert(value != NULL);

  if(hm->lenght >= hm->capacity / 2){
    if(!hm_expand(hm, a)) return NULL;
  }

  return hm_set_entry(hm->entries, hm->capacity, key, value, &hm->lenght);
}

#endif
