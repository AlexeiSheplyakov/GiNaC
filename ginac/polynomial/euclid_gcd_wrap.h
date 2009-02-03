#ifndef GINAC_PGCD_EUCLID_GCD_H
#define GINAC_PGCD_EUCLID_GCD_H
#include "upoly.hpp"
#include "gcd_euclid.tcc"
#include "smod_helpers.h"
#include "add.h"
#include "ex.h"
#include "operators.h"
#include "power.h"
#include "relational.h"
#include "debug.hpp"

namespace GiNaC
{

static void ex2upoly(umodpoly& u, ex e, const ex& var, const long p)
{
	e = e.expand();
	cln::cl_modint_ring R = cln::find_modint_ring(cln::cl_I(p));
	u.resize(e.degree(var) + 1);
	for (int i = 0; i <= e.degree(var); ++i) {
		ex ce = e.coeff(var, i);
		bug_on(!is_a<numeric>(ce), "i = " << i << ", " <<
			"coefficient is not a number: " << ce);
		const cln::cl_I c = to_cl_I(ce);
		u[i] = R->canonhom(c);
	}
}

static ex umodpoly2ex(const umodpoly& a, const ex& var, const long p)
{
	cln::cl_modint_ring R = cln::find_modint_ring(cln::cl_I(p));
	const numeric pnum(p);
	exvector ev(a.size());
	for (std::size_t i = a.size(); i-- != 0; ) {
		const cln::cl_I c = smod(R->retract(a[i]), p);
		const ex term = numeric(c)*power(var, i);
		ev.push_back(term);
	}
	ex ret = (new add(ev))->setflag(status_flags::dynallocated);
	return ret;
}
	
static ex euclid_gcd(ex A, ex B, const ex& var, const long p)
{
	A = A.expand();
	B = B.expand();

	umodpoly a, b;
	ex2upoly(a, A, var, p);
	ex2upoly(b, B, var, p);
	umodpoly g;
	gcd_euclid(g, a, b);
	ex ge = umodpoly2ex(g, var, p);
	return ge;
}

} // namespace GiNaC

#endif
