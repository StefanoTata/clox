#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "compiler.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "vm.h"

#define DEBUG_TRACE_EXECUTION
#ifdef DEBUG_TRACE_EXECUTION
#include "debug.h"
#endif

VM vm;

static void reset_stack(){
  vm.stack_top = vm.stack;
  vm.frame_count = 0;
}

static void runtime_error(const char* format, ...){
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for(int i = vm.frame_count - 1; i >= 0; i--){
    CallFrame* frame = &vm.frames[i];
    ObjFunction* function = frame->function;
    size_t instruction = frame->ip - frame->function->chunk.code - 1;
    fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
    if(function->name == NULL){
      fprintf(stderr, "script\n");
    } else {
      fprintf(stderr, "%s()\n", function->name->chars);
    }
  }

  reset_stack();
}

void init_vm(){
  reset_stack();
  vm.objects = NULL;
  init_table(&vm.globals);
  init_table(&vm.strings);
}

void free_vm(){
  free_objects();
}

void push(Value value){
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop(){
  vm.stack_top--;
  return *vm.stack_top;
}

static Value value_peek(int distance){
  return vm.stack_top[-1 - distance];
}

static int call(ObjFunction* function, int arg_count){
  if(arg_count != function->arity){
    runtime_error("Expected %d arguments but got %d.", function->arity, arg_count);
    return 0;
  }
  
  if(vm.frame_count == FRAMES_MAX){
    runtime_error("Stack overflow.");
    return 0;
  }

  CallFrame* frame = &vm.frames[vm.frame_count++];
  frame->function = function;
  frame->ip = function->chunk.code;
  frame->slots = vm.stack_top - arg_count - 1;
  return 1;
}

static int call_value(Value callee, int arg_count){
  if(IS_OBJ(callee)){
    switch(OBJ_TYPE(callee)){
      case OBJ_FUNCTION:
        return call(AS_FUNCTION(callee), arg_count);
      default:
        break;
    }
  }
  runtime_error("Can only call functions and classes.");
  return 0;
}

static int is_falsey(Value value){
 return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value)); 
}

static void concatenate(){
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(pop());

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = take_string(chars, length);
  push(OBJ_VAL(result));
}

static InterpretResult run(){
  CallFrame* frame = &vm.frames[vm.frame_count - 1];

#define READ_BYTE() (*frame->ip++)

#define READ_CONSTANT() (frame->function->chunk.constants.values[READ_BYTE()])

#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(value_type, op) \
  do{ \
    if(!IS_NUMBER(value_peek(0)) || !IS_NUMBER(value_peek(1))){ \
      runtime_error("Operands must be numbers."); \
      return INTERPRET_RUNTIME_ERROR; \
    } \
    double b = AS_NUMBER(pop()); \
    double a = AS_NUMBER(pop()); \
    push(value_type(a op b)); \
  } while(0)

  for(;;){
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for(Value* slot = vm.stack; slot < vm.stack_top; slot++){
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembler_instruction(&frame->function->chunk,
        (int)(frame->ip - frame->function->chunk.code));
#endif

    uint8_t instruction;
    switch(instruction = READ_BYTE()){
      case OP_CONSTANT:
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      case OP_NIL: push(NIL_VAL); break;
      case OP_TRUE: push(BOOL_VAL(1)); break;
      case OP_FALSE: push(BOOL_VAL(0)); break;
      case OP_POP: pop(); break;
      case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        push(frame->slots[slot]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        frame->slots[slot] = value_peek(0);
        break;
      }
      case OP_GET_GLOBAL:{
        ObjString* name = READ_STRING();
        Value value;
        if(!table_get(&vm.globals, name, &value)){
          runtime_error("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        push(value);
        break;
      }
      case OP_DEFINE_GLOBAL:{
        ObjString* name = READ_STRING();
        table_set(&vm.globals, name, value_peek(0));
        pop();
        break;
      }
      case OP_SET_GLOBAL:{
        ObjString* name = READ_STRING();
        if(table_set(&vm.globals, name, value_peek(0))){
          table_delete(&vm.globals, name);
          runtime_error("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_EQUAL:{
        Value b = pop();
        Value a = pop();
        push(BOOL_VAL(values_equal(a, b)));
        break;
      }
      case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
      case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
      case OP_ADD:{
        if(IS_STRING(value_peek(0)) && IS_STRING(value_peek(1))){
          concatenate();
        } else if(IS_NUMBER(value_peek(0)) && IS_NUMBER(value_peek(1))){
          double b = AS_NUMBER(pop());
          double a = AS_NUMBER(pop());
          push(NUMBER_VAL(a + b));
        } else {
          runtime_error("Operands must be two numbers or two strings");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
      case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
      case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
      case OP_NOT:
        push(BOOL_VAL(is_falsey(pop())));
        break;
      case OP_NEGATE:  
        if(!IS_NUMBER(value_peek(0))){
          runtime_error("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        push(NUMBER_VAL(-AS_NUMBER(pop())));
        break;
      case OP_PRINT: {
        print_value(pop());
        printf("\n");
        break;
      }
      case OP_JUMP:{
        uint16_t offset = READ_SHORT();
        frame->ip += offset;
        break;
      }
      case OP_JUMP_IF_FALSE:{
        uint8_t offset = READ_SHORT(); 
        if(is_falsey(value_peek(0))) frame->ip += offset;
        break;
      }
      case OP_LOOP:
        uint16_t offset = READ_SHORT();
        frame->ip -= offset;
        break;
      case OP_CALL:{
        int arg_count = READ_BYTE();
        if(!call_value(value_peek(arg_count), arg_count)){
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &vm.frames[vm.frame_count - 1];
        break;
      }
      case OP_RETURN:{
        Value result = pop();
        vm.frame_count--;
        if(vm.frame_count == 0){
          pop();
          return INTERPRET_OK;
        }
        
        vm.stack_top = frame->slots;
        push(result);
        frame = &vm.frames[vm.frame_count - 1];
        break;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_SHORT
#undef READ_STRING
#undef BINARY_OP
}

InterpretResult interpret(const char* source){
  ObjFunction* function = compile(source);
  if(function == NULL) return INTERPRET_COMPILE_ERROR;

  push(OBJ_VAL(function));
  call(function, 0);

  return run();
}
