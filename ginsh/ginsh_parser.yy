/** @file ginsh_parser.yy
 *
 *  Input grammar definition for ginsh.
 *  This file must be processed with yacc/bison.
 *
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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
#include "config.h"

#include <sys/resource.h>

#if HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

#include <map>
#include <string>
#include <stdexcept>

#include "ginsh.h"

// Original readline settings
static int orig_completion_append_character;
static char *orig_basic_word_break_characters;

// Expression stack for ", "" and """
static void push(const ex &e);
static ex exstack[3];

// Start and end time for the time() function
static struct rusage start_time, end_time;

// Table of functions (a multimap, because one function may appear with different
// numbers of parameters)
typedef ex (*fcnp)(const exprseq &e);
typedef ex (*fcnp2)(const exprseq &e, int serial);

struct fcn_desc {
	fcn_desc() : p(NULL), num_params(0) {}
	fcn_desc(fcnp func, int num) : p(func), num_params(num), is_ginac(false) {}
	fcn_desc(fcnp2 func, int num, int ser) : p((fcnp)func), num_params(num), is_ginac(true), serial(ser) {}

	fcnp p;		// Pointer to function
	int num_params;	// Number of parameters (0 = arbitrary)
	bool is_ginac;	// Flag: function is GiNaC function
	int serial;	// GiNaC function serial number (if is_ginac == true)
};

typedef multimap<string, fcn_desc> fcn_tab;
static fcn_tab fcns;

static fcn_tab::const_iterator find_function(const ex &sym, int req_params);

static ex lst2matrix(const ex &l);
%}

/* Tokens (T_LITERAL means a literal value returned by the parser, but not
   of class numeric or symbol (e.g. a constant or the FAIL object)) */
%token T_NUMBER T_SYMBOL T_LITERAL T_DIGITS T_QUOTE T_QUOTE2 T_QUOTE3
%token T_EQUAL T_NOTEQ T_LESSEQ T_GREATEREQ T_MATRIX_BEGIN T_MATRIX_END

%token T_QUIT T_PRINT T_TIME T_XYZZY T_INVENTORY T_LOOK T_SCORE

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
input	: /* empty */
	| input line
	;

line	: ';'
	| exp ';'
		{
			try {
				cout << $1 << endl;
				push($1);
			} catch (exception &e) {
				cerr << e.what() << endl;
				YYERROR;
			}
		}
	| exp ':'
		{
			try {
				push($1);
			} catch (exception &e) {
				cerr << e.what() << endl;
				YYERROR;
			}
		}
	| T_PRINT '(' exp ')' ';'
		{
			try {
				$3.printtree(cout);
			} catch (exception &e) {
				cerr << e.what() << endl;
				YYERROR;
			}
		}
	| T_QUIT		{YYACCEPT;}
	| T_XYZZY		{cout << "Nothing happens.\n";}
	| T_INVENTORY		{cout << "You're not carrying anything.\n";}
	| T_LOOK		{cout << "You're in a twisty little maze of passages, all alike.\n";}
	| T_SCORE
		{
			cout << "If you were to quit now, you would score ";
			cout << (syms.size() > 350 ? 350 : syms.size());
			cout << " out of a possible 350.\n";
		}
	| error ';'		{yyclearin; yyerrok;}
	| error ':'		{yyclearin; yyerrok;}
	;

exp	: T_NUMBER		{$$ = $1;}
	| T_SYMBOL		{$$ = $1.eval();}
	| '\'' T_SYMBOL '\''	{$$ = $2;}
	| T_LITERAL		{$$ = $1;}
	| T_DIGITS		{$$ = $1;}
	| T_QUOTE		{$$ = exstack[0];}
	| T_QUOTE2		{$$ = exstack[1];}
	| T_QUOTE3		{$$ = exstack[2];}
	| T_TIME {getrusage(RUSAGE_SELF, &start_time);} '(' exp ')'
		{
			getrusage(RUSAGE_SELF, &end_time);
			$$ = (end_time.ru_utime.tv_sec - start_time.ru_utime.tv_sec) +
			     (end_time.ru_stime.tv_sec - start_time.ru_stime.tv_sec) +
			     double(end_time.ru_utime.tv_usec - start_time.ru_utime.tv_usec) / 1e6 +
			     double(end_time.ru_stime.tv_usec - start_time.ru_stime.tv_usec) / 1e6;
		}
	| T_SYMBOL '(' exprseq ')'
		{
			fcn_tab::const_iterator i = find_function($1, $3.nops());
			if (i->second.is_ginac) {
				$$ = ((fcnp2)(i->second.p))(static_cast<const exprseq &>(*($3.bp)), i->second.serial);
			} else {
				$$ = (i->second.p)(static_cast<const exprseq &>(*($3.bp)));
			}
		}
	| T_DIGITS '=' T_NUMBER
		{$$ = $3; Digits = ex_to_numeric($3).to_int();}
	| T_SYMBOL '=' exp
		{$$ = $3; const_cast<symbol *>(&ex_to_symbol($1))->assign($3);}
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
	| exp '%' exp		{$$ = $1 % $3;}
	| '-' exp %prec NEG	{$$ = -$2;}
	| '+' exp %prec NEG	{$$ = $2;}
	| exp '^' exp		{$$ = power($1, $3);}
	| exp '!'		{$$ = factorial($1);}
	| '(' exp ')'		{$$ = $2;}
	| '[' list_or_empty ']'	{$$ = $2;}
	| T_MATRIX_BEGIN matrix T_MATRIX_END	{$$ = lst2matrix($2);}
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

matrix	: T_MATRIX_BEGIN row T_MATRIX_END		{$$ = lst($2);}
	| matrix ',' T_MATRIX_BEGIN row	T_MATRIX_END	{lst l(static_cast<lst &>(*($1.bp))); $$ = l.append($4);}
	;

row	: exp			{$$ = lst($1);}
	| row ',' exp		{lst l(static_cast<lst &>(*($1.bp))); $$ = l.append($3);}
	;


/*
 *  Routines
 */

%%
// Error print routine
int yyerror(char *s)
{
	cerr << s << " at " << yytext << endl;
	return 0;
}

// Push expression "e" onto the expression stack (for ", "" and """)
static void push(const ex &e)
{
	exstack[2] = exstack[1];
	exstack[1] = exstack[0];
	exstack[0] = e;
}


/*
 *  Built-in functions
 */

static ex f_beta(const exprseq &e) {return gamma(e[0])*gamma(e[1])/gamma(e[0]+e[1]);}
static ex f_denom(const exprseq &e) {return e[0].denom();}
static ex f_eval1(const exprseq &e) {return e[0].eval();}
static ex f_evalf1(const exprseq &e) {return e[0].evalf();}
static ex f_expand(const exprseq &e) {return e[0].expand();}
static ex f_gcd(const exprseq &e) {return gcd(e[0], e[1]);}
static ex f_lcm(const exprseq &e) {return lcm(e[0], e[1]);}
static ex f_lsolve(const exprseq &e) {return lsolve(e[0], e[1]);}
static ex f_nops(const exprseq &e) {return e[0].nops();}
static ex f_normal1(const exprseq &e) {return e[0].normal();}
static ex f_numer(const exprseq &e) {return e[0].numer();}
static ex f_power(const exprseq &e) {return power(e[0], e[1]);}
static ex f_sqrt(const exprseq &e) {return sqrt(e[0]);}
static ex f_subs2(const exprseq &e) {return e[0].subs(e[1]);}

#define CHECK_ARG(num, type, fcn) if (!is_ex_of_type(e[num], type)) throw(std::invalid_argument("argument " #num " to " #fcn " must be a " #type))

static ex f_charpoly(const exprseq &e)
{
	CHECK_ARG(0, matrix, charpoly);
	CHECK_ARG(1, symbol, charpoly);
	return ex_to_matrix(e[0]).charpoly(ex_to_symbol(e[1]));
}

static ex f_coeff(const exprseq &e)
{
	CHECK_ARG(1, symbol, coeff);
	CHECK_ARG(2, numeric, coeff);
	return e[0].coeff(ex_to_symbol(e[1]), ex_to_numeric(e[2]).to_int());
}

static ex f_collect(const exprseq &e)
{
	CHECK_ARG(1, symbol, collect);
	return e[0].collect(ex_to_symbol(e[1]));
}

static ex f_content(const exprseq &e)
{
	CHECK_ARG(1, symbol, content);
	return e[0].content(ex_to_symbol(e[1]));
}

static ex f_degree(const exprseq &e)
{
	CHECK_ARG(1, symbol, degree);
	return e[0].degree(ex_to_symbol(e[1]));
}

static ex f_determinant(const exprseq &e)
{
	CHECK_ARG(0, matrix, determinant);
	return ex_to_matrix(e[0]).determinant();
}

static ex f_diag(const exprseq &e)
{
	int dim = e.nops();
	matrix &m = *new matrix(dim, dim);
	for (int i=0; i<dim; i++)
		m.set(i, i, e.op(i));
	return m;
}

static ex f_diff2(const exprseq &e)
{
	CHECK_ARG(1, symbol, diff);
	return e[0].diff(ex_to_symbol(e[1]));
}

static ex f_diff3(const exprseq &e)
{
	CHECK_ARG(1, symbol, diff);
	CHECK_ARG(2, numeric, diff);
	return e[0].diff(ex_to_symbol(e[1]), ex_to_numeric(e[2]).to_int());
}

static ex f_divide(const exprseq &e)
{
	ex q;
	if (divide(e[0], e[1], q))
		return q;
	else
		return *new fail();
}

static ex f_eval2(const exprseq &e)
{
	CHECK_ARG(1, numeric, eval);
	return e[0].eval(ex_to_numeric(e[1]).to_int());
}

static ex f_evalf2(const exprseq &e)
{
	CHECK_ARG(1, numeric, evalf);
	return e[0].evalf(ex_to_numeric(e[1]).to_int());
}

static ex f_has(const exprseq &e)
{
	return e[0].has(e[1]) ? exONE() : exZERO();
}

static ex f_inverse(const exprseq &e)
{
	CHECK_ARG(0, matrix, inverse);
	return ex_to_matrix(e[0]).inverse();
}

static ex f_is(const exprseq &e)
{
	CHECK_ARG(0, relational, is);
	return (bool)ex_to_relational(e[0]) ? exONE() : exZERO();
}

static ex f_lcoeff(const exprseq &e)
{
	CHECK_ARG(1, symbol, lcoeff);
	return e[0].lcoeff(ex_to_symbol(e[1]));
}

static ex f_ldegree(const exprseq &e)
{
	CHECK_ARG(1, symbol, ldegree);
	return e[0].ldegree(ex_to_symbol(e[1]));
}

static ex f_normal2(const exprseq &e)
{
	CHECK_ARG(1, numeric, normal);
	return e[0].normal(ex_to_numeric(e[1]).to_int());
}

static ex f_op(const exprseq &e)
{
	CHECK_ARG(1, numeric, op);
	int n = ex_to_numeric(e[1]).to_int();
	if (n < 0 || n >= e[0].nops())
		throw(std::out_of_range("second argument to op() is out of range"));
	return e[0].op(n);
}

static ex f_prem(const exprseq &e)
{
	CHECK_ARG(2, symbol, prem);
	return prem(e[0], e[1], ex_to_symbol(e[2]));
}

static ex f_primpart(const exprseq &e)
{
	CHECK_ARG(1, symbol, primpart);
	return e[0].primpart(ex_to_symbol(e[1]));
}

static ex f_quo(const exprseq &e)
{
	CHECK_ARG(2, symbol, quo);
	return quo(e[0], e[1], ex_to_symbol(e[2]));
}

static ex f_rem(const exprseq &e)
{
	CHECK_ARG(2, symbol, rem);
	return rem(e[0], e[1], ex_to_symbol(e[2]));
}

static ex f_series2(const exprseq &e)
{
	CHECK_ARG(1, symbol, series);
	return e[0].series(ex_to_symbol(e[1]), exZERO());
}

static ex f_series3(const exprseq &e)
{
	CHECK_ARG(1, symbol, series);
	return e[0].series(ex_to_symbol(e[1]), e[2]);
}

static ex f_series4(const exprseq &e)
{
	CHECK_ARG(1, symbol, series);
	CHECK_ARG(3, numeric, series);
	return e[0].series(ex_to_symbol(e[1]), e[2], ex_to_numeric(e[3]).to_int());
}

static ex f_sqrfree(const exprseq &e)
{
	CHECK_ARG(1, symbol, sqrfree);
	return sqrfree(e[0], ex_to_symbol(e[1]));
}

static ex f_subs3(const exprseq &e)
{
	CHECK_ARG(1, lst, subs);
	CHECK_ARG(2, lst, subs);
	return e[0].subs(ex_to_lst(e[1]), ex_to_lst(e[2]));
}

static ex f_tcoeff(const exprseq &e)
{
	CHECK_ARG(1, symbol, tcoeff);
	return e[0].tcoeff(ex_to_symbol(e[1]));
}

static ex f_trace(const exprseq &e)
{
	CHECK_ARG(0, matrix, trace);
	return ex_to_matrix(e[0]).trace();
}

static ex f_transpose(const exprseq &e)
{
	CHECK_ARG(0, matrix, transpose);
	return ex_to_matrix(e[0]).transpose();
}

static ex f_unassign(const exprseq &e)
{
	CHECK_ARG(0, symbol, unassign);
	(const_cast<symbol *>(&ex_to_symbol(e[0])))->unassign();
	return e[0];
}

static ex f_unit(const exprseq &e)
{
	CHECK_ARG(1, symbol, unit);
	return e[0].unit(ex_to_symbol(e[1]));
}

static ex f_dummy(const exprseq &e)
{
	throw(std::logic_error("dummy function called (shouldn't happen)"));
}


/*
 *  Add all registered GiNaC functions to ginsh
 */

static ex f_ginac_function(const exprseq &es, int serial)
{
	return function(serial, es).eval(1);
}

void GiNaC::ginsh_get_ginac_functions(void)
{
	vector<registered_function_info>::const_iterator i = function::registered_functions().begin(), end = function::registered_functions().end();
	unsigned serial = 0;
	while (i != end) {
		fcns.insert(make_pair(i->name, fcn_desc(f_ginac_function, i->nparams, serial)));
		i++;
		serial++;
	}
}


/*
 *  Find a function given a name and number of parameters. Throw exceptions on error.
 */

static fcn_tab::const_iterator find_function(const ex &sym, int req_params)
{
	const string &name = ex_to_symbol(sym).getname();
	typedef fcn_tab::const_iterator I;
	pair<I, I> b = fcns.equal_range(name);
	if (b.first == b.second)
		throw(std::logic_error("unknown function '" + name + "'"));
	else {
		for (I i=b.first; i!=b.second; i++)
			if ((i->second.num_params == 0) || (i->second.num_params == req_params))
				return i;
	}
	throw(std::logic_error("invalid number of arguments to " + name + "()"));
}


/*
 *  Convert list of lists to matrix
 */

static ex lst2matrix(const ex &l)
{
	if (!is_ex_of_type(l, lst))
		throw(std::logic_error("internal error: argument to lst2matrix() is not a list"));

	// Find number of rows and columns
	int rows = l.nops(), cols = 0, i, j;
	for (i=0; i<rows; i++)
		if (l.op(i).nops() > cols)
			cols = l.op(i).nops();

	// Allocate and fill matrix
	matrix &m = *new matrix(rows, cols);
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			if (l.op(i).nops() > j)
				m.set(i, j, l.op(i).op(j));
			else
				m.set(i, j, exZERO());
	return m;
}


/*
 *  Function name completion functions for readline
 */

static char *fcn_generator(char *text, int state)
{
	static int len;				// Length of word to complete
	static fcn_tab::const_iterator index;	// Iterator to function being currently considered

	// If this is a new word to complete, initialize now
	if (state == 0) {
		index = fcns.begin();
		len = strlen(text);
	}

	// Return the next function which partially matches
	while (index != fcns.end()) {
		const char *fcn_name = index->first.c_str();
		index++;
		if (strncmp(fcn_name, text, len) == 0)
			return strdup(fcn_name);
	}
	return NULL;
}

static char **fcn_completion(char *text, int start, int end)
{
	if (rl_line_buffer[0] == '!') {
		// For shell commands, revert back to filename completion
		rl_completion_append_character = orig_completion_append_character;
		rl_basic_word_break_characters = orig_basic_word_break_characters;
		return completion_matches(text, filename_completion_function);
	} else {
		// Otherwise, complete function names
		rl_completion_append_character = '(';
		rl_basic_word_break_characters = " \t\n\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~";
		return completion_matches(text, fcn_generator);
	}
}


/*
 *  Main program
 */

int main(int argc, char **argv)
{
	// Print banner in interactive mode
	if (isatty(0)) {
		cout << "ginsh - GiNaC Interactive Shell (" << PACKAGE << " " << VERSION << ")\n";
		cout << "Copyright (C) 1999 Johannes Gutenberg Universitaet Mainz, Germany\n";
		cout << "This is free software, and you are welcome to redistribute it\n";
		cout << "under certain conditions; see the file COPYING for details.\n"; 
	}

	// Init table of built-in functions
	fcns.insert(make_pair(string("beta"), fcn_desc(f_beta, 2)));
	fcns.insert(make_pair(string("charpoly"), fcn_desc(f_charpoly, 2)));
	fcns.insert(make_pair(string("coeff"), fcn_desc(f_coeff, 3)));
	fcns.insert(make_pair(string("collect"), fcn_desc(f_collect, 2)));
	fcns.insert(make_pair(string("content"), fcn_desc(f_content, 2)));
	fcns.insert(make_pair(string("degree"), fcn_desc(f_degree, 2)));
	fcns.insert(make_pair(string("denom"), fcn_desc(f_denom, 1)));
	fcns.insert(make_pair(string("determinant"), fcn_desc(f_determinant, 1)));
	fcns.insert(make_pair(string("diag"), fcn_desc(f_diag, 0)));
	fcns.insert(make_pair(string("diff"), fcn_desc(f_diff2, 2)));
	fcns.insert(make_pair(string("diff"), fcn_desc(f_diff3, 3)));
	fcns.insert(make_pair(string("divide"), fcn_desc(f_divide, 2)));
	fcns.insert(make_pair(string("eval"), fcn_desc(f_eval1, 1)));
	fcns.insert(make_pair(string("eval"), fcn_desc(f_eval2, 2)));
	fcns.insert(make_pair(string("evalf"), fcn_desc(f_evalf1, 1)));
	fcns.insert(make_pair(string("evalf"), fcn_desc(f_evalf2, 2)));
	fcns.insert(make_pair(string("expand"), fcn_desc(f_expand, 1)));
	fcns.insert(make_pair(string("gcd"), fcn_desc(f_gcd, 2)));
	fcns.insert(make_pair(string("has"), fcn_desc(f_has, 2)));
	fcns.insert(make_pair(string("inverse"), fcn_desc(f_inverse, 1)));
	fcns.insert(make_pair(string("is"), fcn_desc(f_is, 1)));
	fcns.insert(make_pair(string("lcm"), fcn_desc(f_lcm, 2)));
	fcns.insert(make_pair(string("lcoeff"), fcn_desc(f_lcoeff, 2)));
	fcns.insert(make_pair(string("ldegree"), fcn_desc(f_ldegree, 2)));
	fcns.insert(make_pair(string("lsolve"), fcn_desc(f_lsolve, 2)));
	fcns.insert(make_pair(string("nops"), fcn_desc(f_nops, 1)));
	fcns.insert(make_pair(string("normal"), fcn_desc(f_normal1, 1)));
	fcns.insert(make_pair(string("normal"), fcn_desc(f_normal2, 2)));
	fcns.insert(make_pair(string("numer"), fcn_desc(f_numer, 1)));
	fcns.insert(make_pair(string("op"), fcn_desc(f_op, 2)));
	fcns.insert(make_pair(string("power"), fcn_desc(f_power, 2)));
	fcns.insert(make_pair(string("prem"), fcn_desc(f_prem, 3)));
	fcns.insert(make_pair(string("primpart"), fcn_desc(f_primpart, 2)));
	fcns.insert(make_pair(string("quo"), fcn_desc(f_quo, 3)));
	fcns.insert(make_pair(string("rem"), fcn_desc(f_rem, 3)));
	fcns.insert(make_pair(string("series"), fcn_desc(f_series2, 2)));
	fcns.insert(make_pair(string("series"), fcn_desc(f_series3, 3)));
	fcns.insert(make_pair(string("series"), fcn_desc(f_series4, 4)));
	fcns.insert(make_pair(string("sqrfree"), fcn_desc(f_sqrfree, 2)));
	fcns.insert(make_pair(string("sqrt"), fcn_desc(f_sqrt, 1)));
	fcns.insert(make_pair(string("subs"), fcn_desc(f_subs2, 2)));
	fcns.insert(make_pair(string("subs"), fcn_desc(f_subs3, 3)));
	fcns.insert(make_pair(string("tcoeff"), fcn_desc(f_tcoeff, 2)));
	fcns.insert(make_pair(string("time"), fcn_desc(f_dummy, 0)));
	fcns.insert(make_pair(string("trace"), fcn_desc(f_trace, 1)));
	fcns.insert(make_pair(string("transpose"), fcn_desc(f_transpose, 1)));
	fcns.insert(make_pair(string("unassign"), fcn_desc(f_unassign, 1)));
	fcns.insert(make_pair(string("unit"), fcn_desc(f_unit, 2)));
	ginsh_get_ginac_functions();

	// Init readline completer
	rl_readline_name = argv[0];
	rl_attempted_completion_function = (CPPFunction *)fcn_completion;
	orig_completion_append_character = rl_completion_append_character;
	orig_basic_word_break_characters = rl_basic_word_break_characters;

	// Init input file list, open first file
	num_files = argc - 1;
	file_list = argv + 1;
	if (num_files) {
		yyin = fopen(*file_list, "r");
		if (yyin == NULL) {
			cerr << "Can't open " << *file_list << endl;
			exit(1);
		}
		num_files--;
		file_list++;
	}

	// Parse input, catch all remaining exceptions
	int result;
again:	try {
		result = yyparse();
	} catch (exception &e) {
		cerr << e.what() << endl;
		goto again;
	}
	return result;
}
