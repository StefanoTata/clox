#ifndef TABLE_H
#define TABLE_H

#include "value.h"

typedef struct{
  ObjString* key;
  Value value;
} Entry;

typedef struct{
  int count;
  int capacity;
  Entry* entries;
} Table;

void init_table(Table* table);
void free_table(Table* table);
int table_get(Table* table, ObjString* key, Value* value);
int table_set(Table* table, ObjString* key, Value value);
int table_delete(Table* table, ObjString* key);
void table_add_all(Table* from, Table* to);
ObjString* table_find_string(Table* table, const char* chars, int length, uint32_t hash);

#endif
