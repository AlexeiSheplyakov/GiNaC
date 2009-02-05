#ifndef GINAC_UPOLY_INTERPOLATE_PADIC_TCC
#define GINAC_UPOLY_INTERPOLATE_PADIC_TCC
#include "ring_traits.h"

namespace cln
{
static inline cln::cl_I smod(const cln::cl_I& x, const cln::cl_I y)
{
	cln::cl_I r = mod(x, y);
	const cln::cl_I y_2 = y >> 1;
	if (r > y_2)
		r = r - y;
	return r;
}

} // namespace cln

namespace GiNaC
{

template<typename T> static void
interpolate(T& g, const typename T::value_type& gamma,
	    const typename T::value_type& modulus,
	    const std::size_t degree_hint = 1)
{
	typedef typename T::value_type ring_t;
	g.clear();
	g.reserve(degree_hint);
	ring_t e = gamma;
	while (!zerop(e)) {
		const ring_t gi = smod(e, modulus);
		g.push_back(gi);
		e = exquo(e - gi, modulus);
	}
}

} // namespace GiNaC

#endif // GINAC_UPOLY_INTERPOLATE_PADIC_TCC

