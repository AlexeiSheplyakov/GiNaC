/*
 *  ginsh.h - GiNaC Interactive Shell, global definitions
 */

#ifndef GINSH_H
#define GINSH_H

// yacc semantic type
#define YYSTYPE ex

// lex functions/variables
extern int yyerror(char *s);
extern int yylex(void);
#if YYTEXT_POINTER
extern char *yytext;
#else
extern char yytext[];
#endif

// Table of all used symbols
typedef map<string, symbol> sym_tab;
extern sym_tab syms;

// Prototypes for missing functions
#ifndef HAVE_STRDUP
extern char *strdup(const char *s);
#endif

#endif
