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
struct sym_def {
	sym_def() : predefined(false) {}
	sym_def(const ex &s, bool predef) : sym(s), predefined(predef) {}
	~sym_def() {}

	sym_def(const sym_def &other) {sym = other.sym; predefined = other.predefined;}
	const sym_def &operator=(const sym_def &other)
	{
		if (this != &other) {
			sym = other.sym;
			predefined = other.predefined;
		}
		return *this;
	}

	ex sym;
	bool predefined;	// true = user supplied symbol, false = lexer generated symbol
};
typedef map<string, sym_def> sym_tab;
static sym_tab syms;

// lex input function
static int lexer_input(char *buf, int max_size);
#define YY_INPUT(buf, result, max_size) (result = lexer_input(buf, max_size))
%}

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
Pi			ginac_yylval = Pi; return T_LITERAL;
Euler			ginac_yylval = Euler; return T_LITERAL;
Catalan			ginac_yylval = Catalan; return T_LITERAL;
FAIL			ginac_yylval = *new fail(); return T_LITERAL;
I			ginac_yylval = I; return T_NUMBER;
Digits			ginac_yylval = (long)Digits; return T_DIGITS;

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
"#"{D}+"R"{AN}+		|
"#b"([01])+		|
"#o"[0-7]+		|
"#x"[0-9a-fA-F]+	|
{D}+"."{D}*({E})?	|
{D}*"."{D}+({E})?	|
{D}+{E}			ginac_yylval = numeric(yytext); return T_NUMBER;

			/* symbols */
{A}{AN}*		{
				sym_tab::const_iterator i = syms.find(yytext);
				if (i == syms.end()) {
					syms[yytext] = sym_def(ginac_yylval = *(new symbol(yytext)), false);
				} else
					ginac_yylval = i->second.sym;
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
			syms[ex_to_symbol(l.op(i)).getname()] = sym_def(l.op(i), true);
	}
}

// Check whether symbol was predefined
bool is_lexer_symbol_predefined(const ex &s)
{
	sym_tab::const_iterator i = syms.find(ex_to_symbol(s).getname());
	if (i == syms.end())
		return false;
	else
		return i->second.predefined;
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
