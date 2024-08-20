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
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  CEOF
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
void add_token(int type, char* lexeme, int line, TokenNode** tl, Arena* a);
void scanner(char* source, int* line, int* start, int* current, TokenNode** token_list_tail, Arena* a);

#endif SCANNER_H

//
// IMPLEMENTATION
//

#ifdef SCANNER_IMPL

#define MAX_LEXEME_SIZE 30

#include "hashmap.h"

static void scanner_report(int line, char* where, char* message){
  fprintf(stdout, "[line \"%d\"] Error%s: %s", line, where, message);
}

static void scanner_error(int line, char* message){
  scanner_report(line, "", message);
}

char* token_to_string(Token* t, Arena* a){
  char *str = 0;
  str = arena_alloc(a, (2 + MAX_LEXEME_SIZE)*sizeof(char));
  sprintf(str, "%d %s", t->type, t->lexeme);
  return str;
}

static int match(char* source, int* current, char expected){
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

static void string(char* source, int* line, int* start, int* current, TokenNode** tl, Arena* a){
  while(source[*current] != '"' && source[*current] != '\0'){
    if(source[*current] == '\n') *line++;
    (*current)++;
  }

  if(source[*current] == '\0') {
    scanner_error(*line, "Unterminated string.\n");
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

static inline int is_digit(char c){ if(c >= '0' && c <= '9') return 1; else return 0; }
static inline int is_alpha(char c){ if((c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) return 1; else return 0; }
static inline int is_alpha_num(char c){ if((c >= '0' && c <= '9') || (c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) return 1; else return 0; }

static void number(char* source, int line, int* start, int* current, TokenNode** tl, Arena* a){
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

static void identifier(char* source, int line, int* start, int* current, TokenNode** tl, Hashmap* hm, Arena* a){
  while(is_alpha_num(source[*current])) (*current)++;
  
  int lexeme_size = *current - *start;
  char* lexeme = arena_alloc(a, lexeme_size + 1);
  for(int i=0; i < lexeme_size; i++) lexeme[i] = source[(*start)+i];
  lexeme[lexeme_size + 1] = '\0';
  
  int* type = hm_get(hm, lexeme);
  if(!type){
    add_token(IDENTIFIER, lexeme, line, tl, a);
    return;
  }

  add_token(*type, lexeme, line, tl, a);
}

static void scan_token(char* source, int* line, int* start, int* current, TokenNode** tl, Hashmap* hm, Arena* a){
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
      if(is_digit(c)){
        number(source, *line, start, current, tl, a);
      } else if (is_alpha(c)){
        identifier(source, *line, start, current, tl, hm, a);
      } else {
        scanner_error(*line, "Unexpected character.\n");
      }
      break;
  }
}

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

void scanner(char* source, int* line, int* start, int* current, TokenNode** tl, Arena* a){
  Hashmap* hm = hm_create(a);
  set_keywords(hm, a);

  while(source[*current] != '\0'){ 
    *start = *current;
    scan_token(source, line, start, current, tl, hm, a); 
  }

  add_token(CEOF, 0, *line, tl, a);
}

#endif SCANNER_IMPL
