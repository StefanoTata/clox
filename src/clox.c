#include <stdio.h>

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

#define VM_IMPL
#include "vm.h"

#define CHUNK_IMPL
#include "chunk.h"

#define SCANNER_IMPL
#include "scanner.h"

#define COMPILER_IMPL
#include "compiler.h"

#define DEBUG_IMPL
#include "debug.h"

#define VALUE_IMPL
#include "value.h"

#define MEMORY_IMPL
#include "memory.h"

static void repl(){
  char line[1024];
  for(;;){
    printf("> ");
    if(!fgets(line, sizeof(line), stdin)){
      printf("\n");
      break;
    }

    interpret(line);
  }
}

static char* read_file(const char* path){
  FILE* file = fopen(path, "rb");
  if(file == NULL){
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(file_size + 1);
  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if(bytes_read < file_size){
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytes_read] = '\0';

  fclose(file);
  return buffer;
}

static void run_file(const char* path){
  char* source = read_file(path);
  InterpretResult res = interpret(source);
  free(source);

  if(res == INTERPRET_COMPILE_ERROR) exit(65);
  if(res == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char** argv){
  init_vm();

  if(argc > 2){
    fprintf(stdout, "Usage: clox [path]\n");
    exit(64);
  }
  else if(argc == 2)
    run_file(argv[1]);
  else
    repl();

  free_vm();
  return 0;
}
