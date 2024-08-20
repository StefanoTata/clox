#include "scanner.h"

#define HASHMAP_IMPL
#include "hashmap.h"

#define ARENA_IMPL
#include "arena.h"

#include <assert.h>
#include <stdio.h>

#define ARENA_SIZE 2048

void set_keywords(Hashmap* hm, Arena* a){
  int* tk_and = arena_alloc(a, sizeof(int));
  *tk_and = AND;
  hm_set(hm, "and", tk_and, a);
  
  int* tk_class = arena_alloc(a, sizeof(int));
  *tk_class = CLASS;
  hm_set(hm, "class", tk_class, a);
  
  int* tk_else = arena_alloc(a, sizeof(int));
  *tk_else = ELSE;
  hm_set(hm, "else", tk_else, a);
  
  int* tk_false = arena_alloc(a, sizeof(int));
  *tk_false = FALSE;
  hm_set(hm, "false", tk_false, a);
  
  int* tk_for = arena_alloc(a, sizeof(int));
  *tk_for = FOR;
  hm_set(hm, "for", tk_for, a);
  
  int* tk_fun = arena_alloc(a, sizeof(int));
  *tk_fun = FUN;
  hm_set(hm, "fun", tk_fun, a);
  
  int* tk_if = arena_alloc(a, sizeof(int));
  *tk_if =IF;
  hm_set(hm, "if", tk_if, a);
  
  int* tk_nil = arena_alloc(a, sizeof(int));
  *tk_nil = NIL;
  hm_set(hm, "nil", tk_nil, a);
  
  int* tk_print = arena_alloc(a, sizeof(int));
  *tk_print = PRINT;
  hm_set(hm, "print", tk_print, a);
  
  int* tk_return = arena_alloc(a, sizeof(int));
  *tk_return = RETURN;
  hm_set(hm, "return", tk_return, a);
  
  int* tk_super = arena_alloc(a, sizeof(int));
  *tk_super = SUPER;
  hm_set(hm, "super", tk_super, a);
  
  int* tk_this = arena_alloc(a, sizeof(int));
  *tk_this = THIS;
  hm_set(hm, "this", tk_this, a);
  
  int* tk_true = arena_alloc(a, sizeof(int));
  *tk_true = TRUE;
  hm_set(hm, "true", tk_true, a);
  
  int* tk_var = arena_alloc(a, sizeof(int));
  *tk_var = VAR;
  hm_set(hm, "var", tk_var, a);
  
  int* tk_while = arena_alloc(a, sizeof(int));
  *tk_while = WHILE;
  hm_set(hm, "while", tk_while, a);
}

int main(){
  Arena a = {0};
  unsigned char b[ARENA_SIZE];
  arena_init(&a, b, ARENA_SIZE);
  
  Hashmap* hm = hm_create(&a);
  set_keywords(hm, &a);

  int* res = hm_get(hm, "and");
  assert(*res == AND);
  
  res = hm_get(hm, "class");
  assert(*res == CLASS);
  
  res = hm_get(hm, "else");
  assert(*res == ELSE);
  
  res = hm_get(hm, "for");
  assert(*res == FOR);
  
  res = hm_get(hm, "fun");
  assert(*res == FUN);
  
  res = hm_get(hm, "if");
  assert(*res == IF);

  res = hm_get(hm, "nil");
  assert(*res == NIL);
  
  res = hm_get(hm, "print");
  assert(*res == PRINT);
  
  res = hm_get(hm, "return");
  assert(*res == RETURN);
  
  res = hm_get(hm, "super");
  assert(*res == SUPER);
  
  res = hm_get(hm, "this");
  assert(*res == THIS);
  
  res = hm_get(hm, "true");
  assert(*res == TRUE);
  
  res = hm_get(hm, "while");
  assert(*res == WHILE);
  
  res = hm_get(hm, "var");
  assert(*res == VAR);

  return 0;
}
