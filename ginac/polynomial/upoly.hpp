#ifndef GINAC_NEW_UPOLY_HPP
#define GINAC_NEW_UPOLY_HPP
#include <vector>
#include <cstddef>
#include <cln/integer.h>
#include <cln/modinteger.h>
#include <cln/integer_io.h>
#include <stdexcept>
#include <iterator>
#include <limits>
#include "debug.hpp"
#include "compiler.h"

namespace GiNaC
{

typedef std::vector<cln::cl_I> upoly;
typedef std::vector<cln::cl_MI> umodpoly;

template<typename T> static std::size_t degree(const T& p)
{
	return p.size() - 1;
}

template<typename T> static typename T::value_type lcoeff(const T& p)
{
	bug_on(p.empty(), "lcoeff of a zero polynomial is undefined");
	return p[p.size() - 1];
}

template<typename T> static typename T::reference lcoeff(T& p)
{
	bug_on(p.empty(), "lcoeff of a zero polynomial is undefined");
	return p[p.size() - 1];
}

template<typename T> static typename T::value_type max_coeff(const T& p)
{
	bug_on(p.empty(), "max_coeff of a zero polynomial is undefined");
	typename T::value_type curr = p[p.size() - 1];
	for (std::size_t i = p.size(); i-- != 0; ) {
		if (p[i] > curr)
			curr = p[i];
	}
	return curr;
}



// Canonicalize the polynomial, i.e. remove leading zero coefficients
template<typename T> static void
canonicalize(T& p, const typename T::size_type hint =
		   std::numeric_limits<typename T::size_type>::max())
{
	if (p.empty())
		return;

	std::size_t i = p.size() - 1;
	// Be fast if the polynomial is already canonicalized
	if (!zerop(p[i]))
		return;

	if (hint < p.size())
		i = hint;

	bool is_zero = false;
	do {
		if (!zerop(p[i])) {
			++i;
			break;
		}
		if (i == 0) {
			is_zero = true;
			break;
		}
		--i;
	} while (true);

	if (is_zero) {
		p.clear();
		return;
	}

	bug_on(!zerop(p.at(i)), "p[" << i << "] = " << p[i] << " != 0 would be erased.");

	typename T::const_iterator it = p.begin() + i;
	for (std::size_t k = i; it != p.end(); ++it, ++k) {
		bug_on(!zerop(*it), "p[" << k << "] = " << p[k] << " != 0 "
			           "would be erased.");
	}

	p.erase(p.begin() + i, p.end());

	bug_on(!p.empty() && zerop(lcoeff(p)), "oops, lcoeff(p) = 0");
}

// Multiply a univariate polynomial p \in R[x] with a constant c \in R
template<typename T> static T&
operator*=(T& p, const typename T::value_type& c)
{
	if (p.empty())
		return p;
	if (zerop(c)) {
		p.clear();
		return p;
	}
	if (c == the_one(p[0]))
		return p;

	for (std::size_t i = p.size(); i-- != 0; )
		p[i] = p[i]*c;
	canonicalize(p);
	return p;
}

// Convert Z[x] -> Z/p[x]

static void
make_umodpoly(umodpoly& up, const upoly& p, const cln::cl_modint_ring& R)
{
	for (std::size_t i = p.size(); i-- != 0; )
		up[i] = R->canonhom(p[i]);
	canonicalize(up);
}

} // namespace GiNaC

#endif // GINAC_NEW_UPOLY_HPP

