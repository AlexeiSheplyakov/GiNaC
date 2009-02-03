#ifndef GINAC_PGCD_EVAL_POINT_FINDER_H
#define GINAC_PGCD_EVAL_POINT_FINDER_H
#include "operators.h"
#include <set>

namespace GiNaC
{

/// Find a `good' evaluation point b \in Z_p for a pair of multivariate
/// polynomials A, B \in Z_p[x_n][x_0, \ldots, x_n]. Here `good' means
/// that b is not a root of GCD of contents of A and B. N.B. content
/// is univariate polynomial \in Z_p[x_n]
struct eval_point_finder
{
	typedef long value_type;
	const value_type p;
	std::set<value_type> points;
	const random_modint modint_generator;
	bool operator()(value_type& b, const ex& g, const ex& x);
	eval_point_finder(const value_type& p_) : p(p_), modint_generator(p)
	{ }
};

bool eval_point_finder::operator()(value_type& b, const ex& lc, const ex& x)
{
	random_modint modint_generator(p);
	// Search for a new element of field
	while (points.size() < p - 1) {
		value_type b_ = modint_generator();
		// check if this evaluation point was already used
		if (points.find(b_) != points.end())
			continue;

		// mark found value as used, even if it's a root of lc
		// (so we don't need to do the check once more)
		points.insert(b_);
		// Now make sure it's NOT the root of GCD's leading coeffient
		if (lc.subs(x == numeric(b_)).smod(numeric(p)).is_zero())
			continue;
		// Nice, it's our next evaluation point
		b = b_;
		return true;
	}
	// All possible evaluation points were used.
	return false;
}

}

#endif /* GINAC_PGCD_EVAL_POINT_FINDER_H */

