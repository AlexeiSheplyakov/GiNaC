#ifndef GINAC_PARSER_DEBUG_HPP
#define GINAC_PARSER_DEBUG_HPP
#include <iosfwd>
#include <sstream>
#include <stdexcept>
#include "compiler.h"
#ifndef __GNUC__
#if __STDC_VERSION__ < 199901L
#define __PRETTY_FUNCTION__ "<unknown>"
#else
#define __PRETTY_FUNCTION__ __func__
#endif
#endif

#define bail_out(exception, message) \
do { \
	std::ostringstream err; \
	err << __PRETTY_FUNCTION__ << "(" << __FILE__ << ':' << __LINE__ << ": "; \
	err << message; \
	throw exception(err.str()); \
} while (0)

#define Parse_error_(message) \
do { \
	std::ostringstream err; \
	err << "GiNaC: parse error at line " << scanner->line_num << \
		", column " << scanner->column << ": "; \
	err << message << std::endl; \
	err << '[' << __PRETTY_FUNCTION__ << "(" << __FILE__ << ':' << __LINE__ << ")]" << std::endl; \
	throw parse_error(err.str(), scanner->line_num, scanner->column); \
} while (0)

#define Parse_error(message) \
	Parse_error_(message << ", got: " << scanner->tok2str(token))

#define bug(message) bail_out(std::logic_error, message)

#define dout(condition, message) \
do { \
	if (unlikely(condition)) { \
		std::cerr << __PRETTY_FUNCTION__ \
			<< " (" << __FILE__ << ':' << __LINE__ << "): " \
			<< message << std::endl; \
	} \
} while (0)

#endif // GINAC_PARSER_DEBUG_HPP

