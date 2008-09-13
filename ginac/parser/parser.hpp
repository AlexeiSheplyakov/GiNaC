#ifndef GINAC_PARSER_HPP_
#define GINAC_PARSER_HPP_

#include "parse_context.hpp"
#include <stdexcept>
#include "ex.h"

namespace GiNaC
{

class lexer;

class parse_error : public std::invalid_argument
{
public:
	const std::size_t line;
	const std::size_t column;
	parse_error(const std::string& what_,
		    const std::size_t line_ = 0,
		    const std::size_t column_ = 0) throw () :
		std::invalid_argument(what_), line(line_), column(column_)
	{ }
};

/**
 * Recursive descent parser for GiNaC expressions.
 */
class parser
{
	// The actual parser rules (in EBNF-alike notation):

	/// expression: primary binoprhs
	ex parse_expression();

	/// primary: indentifier_expr | number_expr | paren_expr | unary_expr
	ex parse_primary();

	/// binoprhs: ([+*/^-] primary)*
	ex parse_binop_rhs(int, ex&);

	/// identifier_expr: identifier |
	///                  identifier '(' expression (',' expression)* ')'
	ex parse_identifier_expr();

	/// paren_expr: '(' expression ')'
	ex parse_paren_expr();

	/// number_expr: number
	ex parse_number_expr();

	/// unary_expr: [+-] expression
	ex parse_unary_expr(const int c);

	/// literal_expr: 'I' | 'Pi' | 'Euler' | 'Catalan'
	ex parse_literal_expr();

public:
	/**
	 * @param syms_ symbol table.
	 * @param funcs_ function/ctors table.
	 * @param strict_ if true, throw an exception if unknown
	 *        symbol is encountered.
	 */
	parser(const symtab& syms_ = symtab(),
		const prototype_table& funcs_ = get_default_reader(),
		const bool strict_ = false);
	~parser();

	/// parse the stream @a input
	ex operator()(std::istream& input);
	/// parse the string @a input
	ex operator()(const std::string& input);

	/// report the symbol table used by parser
	symtab get_syms() const 
	{ 
		return syms; 
	}

private:
	/// If true, throw an exception if an unknown symbol is encountered.
	const bool strict;
	/**
	 * Function/ctor table, maps a prototype (which is a name and number
	 * arguments) to a C++ function. Used for parsing identifier_expr's
	 * (see parse_identifier_expr). If expression contains unknown
	 * prototype, an exception will be thrown.
	 */
	const prototype_table funcs;
	/**
	 * Symbol (variable) table. Used for parsing identifier_expr's
	 * (see parse_identifier_expr). If parser is strict, exception is
	 * thrown if an unknown symbol is encountered. Non-strict parser
	 * appends unknown symbols to the symbol table.
	 */
	symtab syms;
	/// token scanner
	lexer* scanner;
	/// current token the parser is looking at
	int token;
	/// read the next token from the scanner
	int get_next_tok();
};

} // namespace GiNaC

#endif // GINAC_PARSER_HPP_

