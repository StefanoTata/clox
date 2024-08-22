#pragma warning(disable:4996)

#include <stdio.h>

#define CLOX_WIN_IMPL
#include "clox.h"

#define HASHMAP_IMPL

#define PARSER_IMPL
#include "parser.h"

#define SCANNER_IMPL
#include "scanner.h"

#define ARENA_IMPL
#include "arena.h"

#define ARENA_SIZE 4096

int main(int argc, char** argv){
  Arena a = {0};
  unsigned char b[ARENA_SIZE];
  arena_init(&a, b, ARENA_SIZE);
  
  if(argc > 2){
    fprintf(stdout, "Usage: clox [script]\n");
    return -1;
  }
  else if(argc == 2)
    run_file(argv[1], &a);
  else
    run_prompt(&a);
  
  return 0;
}
