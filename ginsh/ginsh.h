/*
 *  ginsh.h - GiNaC Interactive Shell, global definitions
 */

#ifndef GINSH_H_
#define GINSH_H_

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

// Ersatz functions
#ifndef HAVE_STRDUP
char *strdup(const char *s)
{
	char *n = (char *)malloc(strlen(s) + 1);
	strcpy(n, s);
	return n;
}
#endif

#endif
