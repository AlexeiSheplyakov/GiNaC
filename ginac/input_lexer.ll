/** @file input_lexer.ll
 *
 *  Lexical analyzer definition for reading expressions.
 *  This file must be processed with flex. */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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
#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

#include "input_lexer.h"
#include "ex.h"
#include "constant.h"
#include "fail.h"
#include "numeric.h"
#include "symbol.h"

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

#include "input_parser.h"

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

// Table of all used symbols
typedef map<string, ex> sym_tab;
static sym_tab syms;

// lex input function
static int lexer_input(char *buf, int max_size);
#define YY_INPUT(buf, result, max_size) (result = lexer_input(buf, max_size))
%}

	/* The code output by flex doesn't work well with namespaces, so we're doing it this way */
%option prefix="ginac_yy"

	/* Abbreviations */
D	[0-9]
E	[elEL][-+]?{D}+
A	[a-zA-Z_]
AN	[0-9a-zA-Z_]


/*
 *  Lexical rules
 */

%%
[ \t]+			/* skip whitespace */

			/* special values */
Pi			yylval = Pi; return T_LITERAL;
Euler			yylval = Euler; return T_LITERAL;
Catalan			yylval = Catalan; return T_LITERAL;
FAIL			yylval = *new fail(); return T_LITERAL;
I			yylval = I; return T_NUMBER;
Digits			yylval = (long)Digits; return T_DIGITS;

			/* comparison */
"=="			return T_EQUAL;
"!="			return T_NOTEQ;
"<="			return T_LESSEQ;
">="			return T_GREATEREQ;

			/* matrix delimiters */
\[\[			return T_MATRIX_BEGIN;
\]\]			return T_MATRIX_END;

			/* numbers */
{D}+			|
{D}+"."{D}*({E})?	|
{D}*"."{D}+({E})?	|
{D}+{E}			yylval = numeric(yytext); return T_NUMBER;

			/* symbols */
{A}{AN}*		{
				if (syms.find(yytext) == syms.end())
					syms[yytext] = *(new symbol(yytext));
				yylval = syms[yytext];
				return T_SYMBOL;
			}

			/* everything else */
.			return *yytext;

%%


/*
 *  Routines
 */

// The string from which we will read
static string lexer_string;

// The current position within the string
static int curr_pos = 0;

// Input function that reads from string
static int lexer_input(char *buf, int max_size)
{
	int actual = lexer_string.length() - curr_pos;
	if (actual > max_size)
		actual = max_size;
	if (actual <= 0)
		return YY_NULL;
	lexer_string.copy(buf, actual, curr_pos);
	curr_pos += actual;
	return actual;
}

// EOF encountered, terminate the scanner
int ginac_yywrap()
{
	return 1;
}

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

// Set the input string
void set_lexer_string(const string &s)
{
	lexer_string = s;
	curr_pos = 0;
}

// Set the list of predefined symbols
void set_lexer_symbols(ex l)
{
	syms.clear();
	if (!is_ex_exactly_of_type(l, lst))
		return;
	for (int i=0; i<l.nops(); i++) {
		if (is_ex_exactly_of_type(l.op(i), symbol))
			syms[ex_to_symbol(l.op(i)).getname()] = l.op(i);
	}
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
