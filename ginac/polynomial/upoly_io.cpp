#include <iostream>
#include <string>
#include "upoly.hpp"
#include "upoly_io.hpp"
#include <cln/integer_io.h>

namespace GiNaC
{
using std::ostream;
using std::string;

template<typename T> static void 
print(const T& p, ostream& os, const string& varname = string("x"))
{
	if (p.size() == 0)
		os << '0';

	bool seen_nonzero = false;

	for (std::size_t i = p.size(); i-- != 0;  ) {
		if (zerop(p[i])) {
			if (seen_nonzero)
				continue;
			os << "+ [WARNING: 0]*" << varname << "^" << i << "]";
			continue;
		}
		seen_nonzero = true;
		os << "+ (" << p[i] << ")";
		if (i != 0)
			os << "*" << varname;
		if (i > 1)
			os << '^' << i;
		os << " ";
	}
}

#define DEFINE_OPERATOR_OUT(type)					\
std::ostream& operator<<(std::ostream& os, const type& p)		\
{									\
	print(p, os);							\
	return os;							\
}									\
void dbgprint(const type& p)						\
{									\
	print(p, std::cerr);						\
}

DEFINE_OPERATOR_OUT(upoly);
DEFINE_OPERATOR_OUT(umodpoly);
#undef DEFINE_OPERATOR_OUT

} // namespace GiNaC

