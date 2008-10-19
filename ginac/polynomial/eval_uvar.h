#ifndef GINAC_EVAL_UPOLY_TCC
#define GINAC_EVAL_UPOLY_TCC
#include "upoly.hpp"
#include "ring_traits.hpp"

namespace GiNaC
{

/// Evaluate the polynomial using Horner rule.
/// TODO: 
/// - a better algorithm for small polynomials (use SIMD instructions)
/// - a better algorithm for large polynomials (use Karatsuba trick)
/// - a better algorithm for modular polynomials (especially for small
///     moduli and GFN)
template<typename T> static typename T::value_type
eval(const T& p, const typename T::value_type& x)
{
	// p(x) = c_n x^n + c_{n-1} x^{n-1} + \ldots + c_0 =
	// c_0 + x (c_1 + x (c_2 + x ( \ldots (c_{n-1} + c_n x) \ldots )))
	// (AKA Horner rule)
	typedef typename T::value_type ring_t;
	if (p.empty())
		return 0;
	if (degree(p) == 0)
		return p[0];

	ring_t y = lcoeff(p);
	// read the formula above from the right to the left
	for (std::size_t i = p.size() - 1; i-- != 0; )
		y = x*y + p[i];

	return y;
}

} // namespace GiNaC

#endif // GINAC_EVAL_UPOLY_TCC

