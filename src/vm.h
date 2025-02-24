#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "compiler.h"
#include "chunk.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct{
  ObjFunction* function;
  uint8_t* ip;
  Value* slots;
} CallFrame;

typedef struct{
  CallFrame frames[FRAMES_MAX];
  int frame_count;

  Value stack[STACK_MAX];
  Value* stack_top;
  Table globals;
  Table strings;
  Obj* objects;
} VM;

typedef enum{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;

void init_vm();
void free_vm();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif
