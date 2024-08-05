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

TokenNode* scanner(char* source, Arena* a);

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

void scan_token(){}

TokenNode* scanner(char* source, Arena* a){
  int start = 0, current = 0;
  TokenNode* token_list = arena_alloc(a, sizeof(TokenNode));
 
  while(source != '\0'){
    start = current;
    scan_token(&source);
  }

  Token* t = (Token*)arena_alloc(a, sizeof(Token));
  t->type = eof;
  token_list->token = t;

  return token_list;
}

#endif SCANNER_IMPL
