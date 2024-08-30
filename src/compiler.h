#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "scanner.h"

#define UINT8_COUNT (UINT8_MAX + 1)

typedef struct {
  Token current;
  Token previous;
  int had_error;
  int panic_mode;
} Parser;

typedef enum{
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,
  PREC_COMPARISON,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_CALL,
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(int can_assign);

typedef struct{
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

typedef struct{
  Token name;
  int depth;
} Local;

typedef struct {
  Local locals[UINT8_COUNT];
  int local_count;
  int scope_depth;
} Compiler;

int compile(const char* source, Chunk* chunk); 

#endif
