#ifndef SCANNER_H
#define SCANNER_H

//
// INTERFACE
//

#include <stdio.h>

#include "arena.h"
#include "containers.h"

enum TokenType {
  // Singlecharacter tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two char token
  BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,

  // Literals
  IDENTIFIER, STRING, NUMBER,

  // Keywords
  AND, CLASS, ELSE, FALSE, FUN, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR,

  eof
};

typedef struct Token {
  int type;
  char* lexeme;
  int line;
} Token;

typedef struct TokenNode {
  Token* token;
  List list;
} TokenNode;

char* token_to_string(Token* t, Arena* a);

//void scanner(char* source, TokenNode* token_list_head, Arena* a);

#endif SCANNER_H

//
// IMPLEMENTATION
//

#ifdef SCANNER_IMPL

#define MAX_LEXEME_SIZE 30

char* token_to_string(Token* t, Arena* a){
  char *str = 0;
  str = arena_alloc(a, (2 + MAX_LEXEME_SIZE)*sizeof(char));
  sprintf(str, "%d %s", t->type, t->lexeme);
  return str;
}

void add_token(int type, TokenNode* last_node, Arena* a){
  Token* token = arena_alloc(a, sizeof(Token));
  token->type = type;
  
  TokenNode* token_node = arena_alloc(a, sizeof(TokenNode));
  token_node->token = token;
  
  add_node(last_node, token_node);

  last_node = token_node;
}

void scan_token(char** source, TokenNode* last_node, Arena* a){
  char c = *((*source)++);

  switch(c){
    case '(': add_token(LEFT_PAREN, last_node, a); break;
    case ')': add_token(RIGHT_PAREN, last_node, a); break;
  }
}


void scanner(char* source, TokenNode* token_list_head, Arena* a){
  TokenNode* last_token = token_list_head;

  while(*source != ';'){ scan_token(&source, last_token, a); }
  
}

#endif SCANNER_IMPL
