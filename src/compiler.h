#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "scanner.h"

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

typedef void (*ParseFn)();

typedef struct{
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

Parser parser;
Chunk* compiling_chunk;

int compile(const char* source, Chunk* chunk); 

#endif
