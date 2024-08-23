#ifndef DEBUG_H
#define DEBUG_H

#include "chunk.h"

void disassembler_chunk(Chunk* chunk, const char* name);
int disassembler_instruction(Chunk* chunk, int offset);

#endif
