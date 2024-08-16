#ifndef CLOX_H
#define CLOX_H

//
// INTERFACE
//

#include "arena.h"

void error(int line, char* message);
void run_prompt(Arena* a);
void run_file(char* filename, Arena* a);

#endif

//
//IMPLEMENTATION
//

#ifdef CLOX_WIN_IMPL 

#include <stdio.h>

#include "scanner.h"

void run(char* input, int* line, int* start, int* current, Arena* a){
  TokenNode head = {0};
  TokenNode* tl = &head;
  
  scanner(input, line, start, current, &tl, a);
  
  list_for_each(p, head.list.next){
    TokenNode* el = get_node(p, TokenNode);
    printf("%s\n", token_to_string(el->token, a));
  }
}

#define GENERIC_READ 0x80000000
#define OPEN_EXISTING 3
#define FILE_ATTRIBUTE_NORMAL 0x80
#define PAGE_READONLY 0x02
#define FILE_MAP_READ 0x0004

__declspec(dllimport)
void* CreateFileA(char*, unsigned long, unsigned long, void*, unsigned long, unsigned long, void*);
__declspec(dllimport)
void* CreateFileMappingA(void*, void*, unsigned long, unsigned long, unsigned long, char*);
__declspec(dllimport)
void* MapViewOfFile(void*, unsigned long, unsigned long, unsigned long, size_t);
__declspec(dllimport)
void* CloseHandle(void*);
__declspec(dllimport)
void* UnmapViewOfFile(void*);

void run_file(char* filename, Arena* a){ 
  void* h_file, *h_map, *base_ptr;
  int line = 1, start = 0, current = 0;

  h_file = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if(!h_file){ fprintf(stderr, "Error opening file: %s\n", filename); return; }
  
  h_map = CreateFileMappingA(h_file, NULL, PAGE_READONLY, 0, 0, NULL);
  if(!h_map){ fprintf(stderr, "Error creating mem mapping for file: %s\n", filename); return; }

  base_ptr = MapViewOfFile(h_map, FILE_MAP_READ, 0, 0, 0);
  if(!base_ptr){ fprintf(stderr, "Error: map view of file %s failed\n", filename); return; }

  run((char*)base_ptr, &line, &start, &current, a);

  UnmapViewOfFile(base_ptr);
  CloseHandle(h_map);
  CloseHandle(h_file);
}

void run_prompt(Arena *a){
  char input[1024];
  int line = 1, start = 0, current = 0;

  for(;;){
    fprintf(stdout, "> ");
    fscanf(stdin, "%s", &input);
    run(input, &line, &start, &current, a);
    line = 1; start = 0; current = 0;
  }
}

#endif
