#ifndef GINAC_MOD_GCD_DEBUG_HPP
#define GINAC_MOD_GCD_DEBUG_HPP
#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include "compiler.h"

#define DEBUG_PREFIX __func__ << ':' << __LINE__ << ": "
#define EXCEPTION_PREFIX std::string(__func__) + std::string(": ") +

#define Dout2(stream, msg) \
do {									\
	stream << DEBUG_PREFIX << msg << std::endl << std::flush;	\
} while (0)
#define Dout(msg) Dout2(std::cout, msg)

#define bug3_on(condition, the_exception, msg)				\
do {									\
	if (unlikely(condition)) {					\
		std::ostringstream err_stream;				\
		Dout2(err_stream, "BUG: " << msg);			\
		throw the_exception(err_stream.str());			\
	}								\
} while (0)

#define bug_on(condition, msg) bug3_on(condition, std::logic_error, msg)

#endif // GINAC_MOD_GCD_DEBUG_HPP

