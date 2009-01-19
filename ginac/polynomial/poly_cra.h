#ifndef GINAC_POLY_CRA_H
#define GINAC_POLY_CRA_H
#include "ex.h"
#include <cln/integer.h>
#include "smod_helpers.h"

namespace GiNaC
{

/**
 * @brief Chinese reamainder algorithm for polynomials.
 *
 * Given two polynomials \f$e_1 \in Z_{q_1}[x_1, \ldots, x_n]\f$ and
 * \f$e_2 \in Z_{q_2}[x_1, \ldots, x_n]\f$, compute the polynomial
 * \f$r \in Z_{q_1 q_2}[x_1, \ldots, x_n]\f$ such that \f$ r mod q_1 = e_1\f$
 * and \f$ r mod q_2 = e_2 \f$ 
 */
ex chinese_remainder(const ex& e1, const cln::cl_I& q1,
		     const ex& e2, const long q2)
{
	// res = v_1 + v_2 q_1
	// v_1 = e_1 mod q_1
	// v_2 = (e_2 - v_1)/q_1 mod q_2
	const numeric q2n(q2);
	const numeric q1n(q1);
	ex v1 = e1.smod(q1n);
	ex u = v1.smod(q2n);
	ex v2 = (e2.smod(q2n) - v1.smod(q2n)).expand().smod(q2n);
	const numeric q1_1(recip(q1, q2)); // 1/q_1 mod q_2
	v2 = (v2*q1_1).smod(q2n);
	ex ret = (v1 + v2*q1_1).expand();
	return ret;
}

} // namespace GiNaC

#endif /* GINAC_POLY_CRA_H */

