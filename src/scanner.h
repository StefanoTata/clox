#ifndef SCANNER_H
#define SCANNER_H

//
// INTERFACE
//

#include <stdio.h>

#include "arena.h"
#include "list.h"

enum TokenType {
  // Singlecharacter tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

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

void scanner(char* source, int line, TokenNode** token_list_tail, Arena* a);

#endif SCANNER_H

//
// IMPLEMENTATION
//

#ifdef SCANNER_IMPL

#define MAX_LEXEME_SIZE 30

void report(int line, char* where, char* message){
  fprintf(stdout, "[line \"%d\"] Error%s: %s", line, where, message);
}

void error(int line, char* message){
  report(line, "", message);
}

char* token_to_string(Token* t, Arena* a){
  char *str = 0;
  str = arena_alloc(a, (2 + MAX_LEXEME_SIZE)*sizeof(char));
  sprintf(str, "%d %s", t->type, t->lexeme);
  return str;
}

void string(){
}

int match(char** source, char expected){
  if(**source != expected) return 0;
  
  (*source)++;
  return 1;
}

void add_token(int type, int line, TokenNode** tl, Arena* a){
  Token* token = arena_alloc(a, sizeof(Token));
  token->type = type;
  token->line = line;
  
  TokenNode* token_node = arena_alloc(a, sizeof(TokenNode));
  token_node->token = token;
  
  add_node(*tl, token_node);
  *tl = token_node;
}

void scan_token(char** source, int line, TokenNode** tl, Arena* a){
  char c = *((*source)++);

  switch(c){
    case '(': add_token(LEFT_PAREN, line, tl, a); break;
    case ')': add_token(RIGHT_PAREN, line,tl, a); break;
    case '{': add_token(LEFT_BRACE, line, tl, a); break;
    case '}': add_token(RIGHT_BRACE, line, tl, a); break;
    case ',': add_token(COMMA, line, tl, a); break;
    case '.': add_token(DOT, line, tl, a); break;
    case '-': add_token(MINUS, line, tl, a); break;
    case '+': add_token(PLUS, line, tl, a); break;
    case ';': add_token(SEMICOLON, line, tl, a); break;
    case '*': add_token(STAR, line, tl, a); break;
    case '!': add_token(match(source, '=') ? BANG_EQUAL : BANG, line, tl, a); break;
    case '=': add_token(match(source, '=') ? EQUAL_EQUAL : EQUAL, line, tl, a); break;
    case '<': add_token(match(source, '=') ? LESS_EQUAL : LESS, line, tl, a); break;
    case '>': add_token(match(source, '=') ? GREATER_EQUAL : GREATER, line, tl, a); break;
    case '/': 
      if(match(source, '/')) { while(**source != '\n') (*source)++; }
      else add_token(SLASH, line, tl, a);
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n': line++; break;
    case '"': string(); break;
    default: error(line, "Unexpected character.\n"); break;
  }
}

void scanner(char* source, int line, TokenNode** tl, Arena* a){
  while(*source != ';'){ scan_token(&source, line, tl, a); }
}

#endif SCANNER_IMPL
