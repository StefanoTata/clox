#ifndef CHUNK_H
#define CHUNK_H

///
/// INTERFACE
///

#include <stdint.h>

#include "value.h"

typedef enum{
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN,
} OpCode;

typedef struct{
  int count;
  int capacity;
  uint8_t* code;
  int* lines;
  ValueArray constants;
} Chunk;

void init_chunk(Chunk* chunk);
void free_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8_t bytes, int line);

int add_constant(Chunk* chunk, Value value);

#endif

///
/// IMPLEMENTATION
///

#ifdef CHUNK_IMPL

#include "memory.h"

void init_chunk(Chunk* chunk){
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  init_value_array(&chunk->constants);
}

void free_chunk(Chunk* chunk){
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(uint8_t, chunk->lines, chunk->capacity);
  free_value_array(&chunk->constants);
  init_chunk(chunk);
}

void write_chunk(Chunk* chunk, uint8_t bytes, int line){
  if(chunk->capacity < chunk->count + 1){
    int old_capacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(old_capacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, old_capacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, old_capacity, chunk->capacity);
  }

  chunk->code[chunk->count] = bytes;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

int add_constant(Chunk* chunk, Value value){
  write_value_array(&chunk->constants, value);
  return chunk->constants.count - 1;
}

#endif
