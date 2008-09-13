/// @file parser_a_b.cpp Check for some silly bugs in the parser.
#include "ginac.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace GiNaC;

// - a - b was misparsed as -a + b due to a bug in parser::parse_unary_expr()
static int check1(std::ostream& err_str)
{
	const std::string srep("-a-b");
	parser reader;
	ex e = reader(srep);
	ex a = reader.get_syms()["a"];
	ex b = reader.get_syms()["b"];
	ex g = - a - b;
	ex d = (e - g).expand();
	if (!d.is_zero()) {
		err_str << "\"" << srep << "\" was misparsed as \""
			<< e << "\"" << std::endl;
		return 1;
	}
	return 0;
}

/// Parser was rejecting the valid expression '5 - (3*x)/10'.
static int check2(std::ostream& err_str)
{
	const std::string srep("5-(3*x)/10");
	parser reader;
	ex e = reader(srep);
	ex x = reader.get_syms()["x"];
	ex g = 5 - (3*x)/10;
	ex d = (e - g).expand();
	if (!d.is_zero()) {
		err_str << "\"" << srep << "\" was misparsed as \""
			<< e << "\"" << std::endl;
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	std::cout << "checking for parser bugs. " << std::flush;
	std::ostringstream err_str;
	int errors = 0;
	errors += check1(err_str);
	errors += check2(err_str);
	if (errors) {
		std::cout << "Yes, unfortunately:" << std::endl;
		std::cout << err_str.str();
	} else {
		std::cout << "Not found. ";
	}
	return errors;
}

