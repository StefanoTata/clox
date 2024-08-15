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

void scanner(char* source, int* line, int* start, int* current, TokenNode** token_list_tail, Arena* a);

#endif SCANNER_H

//
// IMPLEMENTATION
//

#ifdef SCANNER_IMPL

#define MAX_LEXEME_SIZE 30

#include <ctype.h>

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

int match(char* source, int* current, char expected){
  if(source[*current] == '\0') return 0;
  if(source[*current] != expected) return 0;
  
  (*current)++;
  return 1;
}

void add_token(int type, char* lexeme, int line, TokenNode** tl, Arena* a){
  Token* token = arena_alloc(a, sizeof(Token));
  token->type = type;
  token->lexeme = lexeme;
  token->line = line;

  TokenNode* token_node = arena_alloc(a, sizeof(TokenNode));
  token_node->token = token;
  
  add_node(*tl, token_node);
  *tl = token_node;
}

void string(char* source, int* line, int* start, int* current, TokenNode** tl, Arena* a){
  while(source[*current] != '"' && source[*current] != '\0'){
    if(source[*current] == '\n') *line++;
    (*current)++;
  }

  if(source[*current] == '\0') {
    error(*line, "Unterminated string.\n");
    return;
  }
  
  // consuming closing "
  (*current)++;

  int lexeme_size = *current - *start - 2;
  char* lexeme = arena_alloc(a, lexeme_size + 1);
  for(int i=0; i < lexeme_size; i++) lexeme[i] = source[(*start)+i+1];
  lexeme[lexeme_size + 1] = '\0';

  add_token(STRING, lexeme, *line, tl, a);
}

int is_digit(char c){ if(c >= '0' && c <= '9') return 1; else return 0; }
int is_alpha(char c){ if((c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) return 1; else return 0; }
int is_alpha_num(char c){ if((c >= '0' && c <= '9') || (c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) return 1; else return 0; }

void number(char* source, int line, int* start, int* current, TokenNode** tl, Arena* a){
  while(is_digit(source[*current])) (*current)++;

  if(source[*current] == '.' && is_digit(source[*current + 1])){
    (*current)++;
    while(is_digit(source[*current])) (*current)++;
  }

  int lexeme_size = *current - *start;
  char* lexeme = arena_alloc(a, lexeme_size + 1);
  for(int i=0; i < lexeme_size; i++) lexeme[i] = source[(*start)+i];
  lexeme[lexeme_size + 1] = '\0';
  
  add_token(NUMBER, lexeme, line, tl, a);
}

void identifier(char* source, int line, int* start, int* current, TokenNode** tl, Arena* a){
  while(is_alpha_num(source[*current])) (*current)++;
  
  int lexeme_size = *current - *start;
  char* lexeme = arena_alloc(a, lexeme_size + 1);
  for(int i=0; i < lexeme_size; i++) lexeme[i] = source[(*start)+i];
  lexeme[lexeme_size + 1] = '\0';
  
  add_token(IDENTIFIER, lexeme, line, tl, a);
}

void scan_token(char* source, int* line, int* start, int* current, TokenNode** tl, Arena* a){
  char c = source[(*current)++];

  switch(c){
    case '(': add_token(LEFT_PAREN, 0, *line, tl, a); break;
    case ')': add_token(RIGHT_PAREN, 0, *line,tl, a); break;
    case '{': add_token(LEFT_BRACE, 0, *line, tl, a); break;
    case '}': add_token(RIGHT_BRACE, 0, *line, tl, a); break;
    case ',': add_token(COMMA, 0, *line, tl, a); break;
    case '.': add_token(DOT, 0, *line, tl, a); break;
    case '-': add_token(MINUS, 0, *line, tl, a); break;
    case '+': add_token(PLUS, 0, *line, tl, a); break;
    case ';': add_token(SEMICOLON, 0, *line, tl, a); break;
    case '*': add_token(STAR, 0, *line, tl, a); break;
    case '!': add_token(match(source, current, '=') ? BANG_EQUAL : BANG, 0, *line, tl, a); break;
    case '=': add_token(match(source, current, '=') ? EQUAL_EQUAL : EQUAL, 0, *line, tl, a); break;
    case '<': add_token(match(source, current, '=') ? LESS_EQUAL : LESS, 0, *line, tl, a); break;
    case '>': add_token(match(source, current, '=') ? GREATER_EQUAL : GREATER, 0, *line, tl, a); break;
    case '/': 
      if(match(source, current, '/')) { while(source[*current] != '\n' && source[*current] != '\0') (*current)++; }
      else add_token(SLASH, 0, *line, tl, a);
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n': *line++; break;
    case '"': string(source, line, start, current, tl, a); break;
    default: 
      if(is_digit(source[*current])){
        number(source, *line, start, current, tl, a);
      } else if (is_alpha(c)){
        identifier(source, *line, start, current, tl, a);
      } else {
        error(*line, "Unexpected character.\n");
      }
      break;
  }
}

void scanner(char* source, int* line, int* start, int* current, TokenNode** tl, Arena* a){
  while(source[*current] != '\0'){ scan_token(source, line, start, current, tl, a); }
}

#endif SCANNER_IMPL
