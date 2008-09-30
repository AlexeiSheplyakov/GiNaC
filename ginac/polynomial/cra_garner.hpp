#ifndef CL_INTEGER_CRA
#define CL_INTEGER_CRA
#include <vector>
#include <cln/integer.h>

namespace cln
{
extern cl_I integer_cra(const std::vector<cl_I>& residues,
	                const std::vector<cl_I>& moduli);
}
#endif // CL_INTEGER_CRA

