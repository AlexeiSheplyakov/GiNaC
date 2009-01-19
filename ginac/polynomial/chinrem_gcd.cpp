#include "chinrem_gcd.h"
#include "optimal_vars_finder.h"

namespace GiNaC
{

ex chinrem_gcd(const ex& A, const ex& B)
{
	const exvector vars = gcd_optimal_variables_order(A, B);
	ex g = chinrem_gcd(A, B, vars);
	return g;
}
} // namespace GiNaC

