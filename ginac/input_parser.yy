/** @file input_parser.yy
 *
 *  Input grammar definition for reading expressions.
 *  This file must be processed with yacc/bison. */

/*
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*
 *  Definitions
 */

%{
#include <stdexcept>

#include "input_lexer.h"
#include "ex.h"
#include "relational.h"
#include "symbol.h"
#include "lst.h"
#include "power.h"
#include "exprseq.h"
#include "matrix.h"
#include "inifcns.h"

namespace GiNaC {

#define YYERROR_VERBOSE 1

// Parsed output expression
ex parsed_ex;

// Last error message returned by parser
static std::string parser_error;
%}

/* Tokens (T_LITERAL means a literal value returned by the parser, but not
   of class numeric or symbol (e.g. a constant or the FAIL object)) */
%token T_NUMBER T_SYMBOL T_LITERAL T_DIGITS T_EQUAL T_NOTEQ T_LESSEQ T_GREATEREQ

/* Operator precedence and associativity */
%right '='
%left T_EQUAL T_NOTEQ
%left '<' '>' T_LESSEQ T_GREATEREQ
%left '+' '-'
%left '*' '/' '%'
%nonassoc NEG
%right '^'
%nonassoc '!'

%start input


/*
 *  Grammar rules
 */

%%
input	: exp {
		try {
			parsed_ex = $1;
			YYACCEPT;
		} catch (std::exception &err) {
			parser_error = err.what();
			YYERROR;
		}
	}
	| error		{yyclearin; yyerrok;}
	;

exp	: T_NUMBER		{$$ = $1;}
	| T_SYMBOL {
		if (is_lexer_symbol_predefined($1))
			$$ = $1.eval();
		else
			throw (std::runtime_error("unknown symbol '" + ex_to_symbol($1).get_name() + "'"));
	}
	| T_LITERAL		{$$ = $1;}
	| T_DIGITS		{$$ = $1;}
	| T_SYMBOL '(' exprseq ')' {
		unsigned i = function::find_function(ex_to_symbol($1).get_name(), $3.nops());
		$$ = function(i, static_cast<const exprseq &>(*($3.bp))).eval(1);
	}
	| exp T_EQUAL exp	{$$ = $1 == $3;}
	| exp T_NOTEQ exp	{$$ = $1 != $3;}
	| exp '<' exp		{$$ = $1 < $3;}
	| exp T_LESSEQ exp	{$$ = $1 <= $3;}
	| exp '>' exp		{$$ = $1 > $3;}
	| exp T_GREATEREQ exp	{$$ = $1 >= $3;}
	| exp '+' exp		{$$ = $1 + $3;}
	| exp '-' exp		{$$ = $1 - $3;}
	| exp '*' exp		{$$ = $1 * $3;}
	| exp '/' exp		{$$ = $1 / $3;}
	| '-' exp %prec NEG	{$$ = -$2;}
	| '+' exp %prec NEG	{$$ = $2;}
	| exp '^' exp		{$$ = pow($1, $3);}
	| exp '!'		{$$ = factorial($1);}
	| '(' exp ')'		{$$ = $2;}
	| '{' list_or_empty '}'	{$$ = $2;}
	| '[' matrix ']'	{$$ = lst_to_matrix(ex_to_lst($2));}
	;

exprseq	: exp			{$$ = exprseq($1);}
	| exprseq ',' exp	{exprseq es(static_cast<exprseq &>(*($1.bp))); $$ = es.append($3);}
	;

list_or_empty: /* empty */	{$$ = *new lst;}
	| list			{$$ = $1;}
	;

list	: exp			{$$ = lst($1);}
	| list ',' exp		{lst l(static_cast<lst &>(*($1.bp))); $$ = l.append($3);}
	;

matrix	: '[' row ']'		{$$ = lst($2);}
	| matrix ',' '[' row ']' {lst l(static_cast<lst &>(*($1.bp))); $$ = l.append($4);}
	;

row	: exp			{$$ = lst($1);}
	| row ',' exp		{lst l(static_cast<lst &>(*($1.bp))); $$ = l.append($3);}
	;


/*
 *  Routines
 */

%%
// Get last error encountered by parser
std::string get_parser_error(void)
{
	return parser_error;
}

} // namespace GiNaC

// Error print routine (store error string in parser_error)
int ginac_yyerror(char *s)
{
	GiNaC::parser_error = std::string(s) + " at " + std::string(ginac_yytext);
	return 0;
}
