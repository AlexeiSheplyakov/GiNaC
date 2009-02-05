#ifndef GINAC_POLYNOMIAL_PREM_TCC
#define GINAC_POLYNOMIAL_PREM_TCC
#include "upoly.h"
#include "debug.h"
#include "remainder.h"

namespace GiNaC
{

/// Compute the pseudo-remainder of univariate polynomials @a a and @a b
/// Pseudo remainder \f$r(x)\f$ is defined as 
/// \f$\beta^l a(x) = b(x) q(x) + r(x) \f$, where \f$\beta\f$ is leading
/// coefficient of \f$b(x)\f$ and \f$l = degree(a) - degree(b) + 1\f$
/// FIXME: this implementation is extremely dumb.
template<typename T> bool pseudoremainder(T& r, const T& a, const T& b)
{
	typedef typename T::value_type ring_t;
	bug_on(b.size() == 0, "division by zero");
	if (a.size() == 1 && b.size() == 1) {
		r.clear();
		return true;
	}
	if (a.size() == 1) {
		r = b;
		return false;
	}
	if (degree(b) > degree(a)) {
		r = b;
		return false;
	}

	const ring_t one = get_ring_elt(b[0], 1);
	const std::size_t l = degree(a) - degree(b) + 1;
	const ring_t blcoeff = lcoeff(b);
	const ring_t b_lth = expt_pos(blcoeff, l);
	if (b_lth == one)
		return remainder_in_ring(r, a, b);

	T a_(a);
	a_ *= b_lth;
	return remainder_in_ring(r, a_, b);
}

}

#endif // GINAC_POLYNOMIAL_PREM_TCC

