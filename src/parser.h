#ifndef PARSER_H
#define PARSER_H

///
/// INTERFACE
///

#include "scanner.h"

typedef union Expr{
  struct Unary* unary;
  struct Binary* binary;
  struct Grouping* grouping;
  struct Literal* literal;
} Expr;

typedef struct Unary{
  Token* operator;
  Expr* right;
}Unary;

typedef struct Binary{
  Expr* left, *right;
  Token* operator;
}Binary;

typedef struct Grouping{
  Expr* expression;
}Grouping;

typedef struct Literal{
  Token* literal;
}Literal;

void parser(TokenNode** tl, Arena* a);

#endif

///
/// IMPLEMENTATION
///

#ifdef PARSER_IMPL

#include <stdarg.h>
#include <stdio.h>

#include "list.h"

Expr* expression(TokenNode** tl, Arena* a);

static int token_match(TokenNode** current, int size, ...){
  va_list args;
  va_start(args, size);
  
  for(int i=0; i<size; ++i){
    if((*current)->token->type == va_arg(args, int)){
      *current = get_node((*current)->list.next, TokenNode);
      return 1;
    }
  }

  va_end(args);
  return 0;
}

void parse_error(Token* t, const char* msg){
  if(t->type == CEOF){
    fprintf(stdout, "[line \"%d\"] Error at end: %s", t->line, msg);
  } else {
    fprintf(stdout, "[line \"%d\"] Error at '%s': %s", t->line, t->lexeme, msg);
  }
}

void token_consume(TokenNode** current, int type, const char* msg){
  if((*current)->token->type == type && (*current)->token->type != CEOF){
    *current = get_node((*current)->list.next, TokenNode);
    return;
  }
  parse_error((*current)->token, msg);
}

Expr* primary(TokenNode** tl, Arena* a){ 
  Expr* e = arena_alloc(a, sizeof(Expr));
  
  if(token_match(tl, 5, FALSE, TRUE, NIL, NUMBER, STRING)){
    TokenNode* prev = get_node((*tl)->list.prev, TokenNode); 
    e->literal = arena_alloc(a, sizeof(Literal));
    e->literal->literal = prev->token;
  }

  if(token_match(tl, 1, LEFT_PAREN)){
    Expr* expr = expression(tl, a);
    token_consume(tl, RIGHT_PAREN, "Expect ')' after expression.\n");
    e->grouping = arena_alloc(a, sizeof(Grouping)); 
    e->grouping->expression = expr;
  }
  
  return e;
} 

Expr* unary(TokenNode** tl, Arena* a){ 
  if(token_match(tl, 2, BANG, MINUS)){
    Expr* e = arena_alloc(a, sizeof(Expr));
    e->unary = arena_alloc(a, sizeof(Unary));
    
    TokenNode* prev = get_node((*tl)->list.prev, TokenNode); 
    Expr* right = unary(tl, a);
    e->unary->operator = prev->token;
    e->unary->right = right;
    return e;
  }

  return primary(tl, a);
}

Expr* factor(TokenNode** tl, Arena* a){ 
  Expr* e = unary(tl, a);

  while(token_match(tl, 2, SLASH, STAR)){
    e->binary = arena_alloc(a, sizeof(Binary));
    
    TokenNode* prev = get_node((*tl)->list.prev, TokenNode); 
    Expr* right = unary(tl, a);
    e->binary->left = e;
    e->binary->right = right;
    e->binary->operator = prev->token;
  }

  return e;
}

Expr* term(TokenNode** tl, Arena* a){
  Expr* e = factor(tl, a);

  while(token_match(tl, 2, MINUS, PLUS)){
    e->binary = arena_alloc(a, sizeof(Binary));
    
    TokenNode* prev = get_node((*tl)->list.prev, TokenNode); 
    Expr* right = factor(tl, a);
    e->binary->left = e;
    e->binary->right = right;
    e->binary->operator = prev->token;
  }

  return e;
}

Expr* comparison(TokenNode** tl, Arena* a){ 
  Expr* e = term(tl, a);
  
  while(token_match(tl, 4, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)){
    e->binary = arena_alloc(a, sizeof(Binary));
    
    TokenNode* prev = get_node((*tl)->list.prev, TokenNode); 
    Expr* right = term(tl, a);
    e->binary->left = e;
    e->binary->right = right;
    e->binary->operator = prev->token;
  }

  return e;
}

Expr* equality(TokenNode** tl, Arena* a){ 
  Expr* e = comparison(tl, a);

  while(token_match(tl, 2, BANG_EQUAL, EQUAL_EQUAL)){
    e->binary = arena_alloc(a, sizeof(Binary));
    
    TokenNode* prev = get_node((*tl)->list.prev, TokenNode); 
    Expr* right = comparison(tl, a);
    e->binary->left = e;
    e->binary->right = right;
    e->binary->operator = prev->token;
  }
  
  return e;
}

Expr* expression(TokenNode** tl, Arena* a){ return equality(tl, a); }

void parser(TokenNode** tl, Arena* a){ expression(tl, a); }

#endif
