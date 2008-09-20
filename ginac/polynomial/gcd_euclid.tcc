#ifndef GINAC_GCD_EUCLID
#define GINAC_GCD_EUCLID
#include "upoly.hpp"
#include "remainder.tcc"
#include "normalize.tcc"
#include "debug.hpp"
#include "upoly_io.hpp"

namespace GiNaC
{

static bool
gcd_euclid(umodpoly& c, umodpoly /* passed by value */ a, umodpoly b)
{
	if (a.size() == 0) {
		c.clear();
		return true;
	}
	if (b.size() == 0) {
		c.clear();
		return true;
	}
	bug_on(a[0].ring()->modulus != b[0].ring()->modulus,
		"different moduli");

	normalize_in_field(a);
	normalize_in_field(b);
	if (degree(a) < degree(b))
		std::swap(a, b);

	umodpoly r;
	while (b.size() != 0) {
		remainder_in_field(r, a, b); 
		a = b;
		b = r;
	}
	normalize_in_field(a);
	c = a;
	return false;
}

} // namespace GiNaC

#endif // GINAC_GCD_EUCLID

