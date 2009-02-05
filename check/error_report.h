#ifndef GINAC_CHECK_ERROR_REPORT_HPP
#define GINAC_CHECK_ERROR_REPORT_HPP
#include <sstream>
#include <stdexcept>

#define bug_on(cond, what)				\
do {							\
if (cond) {						\
	std::ostringstream err_stream;			\
	err_stream << __FILE__ << ':' << __LINE__	\
		   << what;				\
	throw std::logic_error(err_stream.str());	\
}							\
} while (0)

#endif // GINAC_CHECK_ERROR_REPORT_HPP

