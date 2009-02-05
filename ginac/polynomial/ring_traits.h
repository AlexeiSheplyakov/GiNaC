#ifndef GINAC_RING_TRAITS_HPP
#define GINAC_RING_TRAITS_HPP
#include <cln/integer.h>
#include <cln/modinteger.h>

namespace cln
{
static inline cln::cl_I div(const cln::cl_I& x, const cln::cl_I& y)
{
	return cln::exquo(x, y);
}

/// Exact integer division.
/// Check if y divides x, if yes put the quotient into q, otherwise don't
/// touch q. Returns true if y divides x and false if not.
static inline bool div(cln::cl_I& q, const cln::cl_I& x, const cln::cl_I& y)
{
	const cln::cl_I_div_t qr = cln::truncate2(x, y);
	if (zerop(qr.remainder)) {
		q = qr.quotient;
		return true;
	}
	return false;
}

static inline cln::cl_I get_ring_elt(const cln::cl_I& sample, const int val)
{
	return cln::cl_I(val);
}

static inline cln::cl_MI get_ring_elt(const cln::cl_MI& sample, const int val)
{
	return sample.ring()->canonhom(val);
}

template<typename T>
static inline T the_one(const T& sample)
{
	return get_ring_elt(sample, 1);
}

} // namespace cln

#endif // GINAC_RING_TRAITS_HPP

