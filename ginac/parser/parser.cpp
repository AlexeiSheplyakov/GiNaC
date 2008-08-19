#include <stdexcept>
#include <sstream>
#include "parser.hpp"
#include "lexer.hpp"
#include "debug.hpp"
#include "mul.h"
#include "constant.h"

namespace GiNaC 
{

/// identifier_expr:  identifier |  identifier '(' expression* ')'
ex parser::parse_identifier_expr()
{
	std::string name = scanner->str;
	get_next_tok();  // eat identifier.

	if (token != '(') // symbol
		return find_or_insert_symbol(name, syms, strict);

	// function/ctor call.
	get_next_tok();  // eat (
	exvector args;
	if (token != ')') {
		while (true) {
			ex e = parse_expression();
			args.push_back(e);

			if (token == ')')
				break;

			if (token != ',')
				throw std::invalid_argument("Expected ')' or ',' in argument list");

			get_next_tok();
		}
	}
	// Eat the ')'.
	get_next_tok();
	prototype the_prototype = make_pair(name, args.size());
	prototype_table::const_iterator reader = funcs.find(the_prototype);
	if (reader == funcs.end()) {
		bail_out(std::invalid_argument,
			"no function \"" << name << "\" with " << args.size()
			<< " arguments");
	}
	ex ret = reader->second(args);
	return ret;
}

/// paren_expr:  '(' expression ')'
ex parser::parse_paren_expr()
{
	get_next_tok();  // eat (.
	ex e = parse_expression();

	if (token != ')')
		throw std::invalid_argument("parser::parse_paren_expr: expected ')'");
	get_next_tok();  // eat ).
	return e;
}

extern numeric* _num_1_p;

/// unary_expr: [+-] expression
ex parser::parse_unary_expr(const int s)
{
	// consume '-' (or '+')
	get_next_tok();
	ex v = parse_expression();
	switch (s) {
		case '-':
			return (new mul(v, *_num_1_p))->setflag(status_flags::dynallocated);
		case '+':
			return v;
		default:
			throw std::invalid_argument(
					std::string(__func__)
					+ ": invalid unary operator \""
					+ char(s) + "\"");
	}
}

/// primary: identifier_expr | number_expr | paren_expr | unary_expr 
ex parser::parse_primary() 
{
	switch (token) {
		case lexer::token_type::identifier:
			 return parse_identifier_expr();
		case lexer::token_type::number:
			 return parse_number_expr();
		case '(': 
			 return parse_paren_expr();
		case '-':
			 return parse_unary_expr('-');
		case '+':
			 return parse_unary_expr('+');
		case lexer::token_type::literal:
			 return parse_literal_expr();
		case lexer::token_type::eof:
			 bail_out(std::invalid_argument, "got EOF while parsing the expression");
		default:
			 bail_out(std::invalid_argument, "unknown token " <<
				token << " (\"" << 
				(token ? std::string("") + char(token) : "NULL")
				<< "\")");
	}
}

/// expression ::= primary binoprhs
ex parser::parse_expression() 
{
	ex lhs = parse_primary();
	ex res = parse_binop_rhs(0, lhs);
	return res;
}

/// number_expr: number
ex parser::parse_number_expr()
{
	ex n = numeric(scanner->str.c_str());
	get_next_tok(); // consume the number
	return n;
}

/// literal_expr: 'I' | 'Pi' | 'Euler' | 'Catalan'
ex parser::parse_literal_expr()
{
	if (scanner->str == "I")
		return I;
	else if (scanner->str == "Pi")
		return Pi;
	else if (scanner->str == "Euler")
		return Euler;
	else if (scanner->str == "Catalan")
		return Catalan;
	bug("unknown literal: \"" << scanner->str << "\"");
}

ex parser::operator()(std::istream& input)
{
	scanner->switch_input(&input);
	get_next_tok();
	ex ret = parse_expression();
	return ret;
}

ex parser::operator()(const std::string& input)
{
	std::istringstream is(input);
	ex ret = operator()(is);
	return ret;
}

int parser::get_next_tok()
{
	token = scanner->gettok();
	return token;
}

parser::parser(const symtab& syms_, const prototype_table& funcs_,
	       const bool strict_) : strict(strict_), funcs(funcs_),
	syms(syms_)
{
	scanner = new lexer();
}

parser::~parser()
{
	delete scanner;
}

} // namespace GiNaC
