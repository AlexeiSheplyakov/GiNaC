#ifndef GINAC_CHINREM_GCD_DIVIDE_IN_Z_P_H
#define GINAC_CHINREM_GCD_DIVIDE_IN_Z_P_H

#include <ginac/ginac.h>
namespace GiNaC
{

/** 
 * Exact polynomial division of a, b \in Z_p[x_0, \ldots, x_n]
 * It doesn't check whether the inputs are proper polynomials, so be careful
 * of what you pass in.
 *  
 * @param a  first multivariate polynomial (dividend)
 * @param b  second multivariate polynomial (divisor)
 * @param q  quotient (returned)
 * @param var variables X iterator to first element of vector of symbols
 *
 * @return "true" when exact division succeeds (the quotient is returned in
 *          q), "false" otherwise.
 */
extern bool
divide_in_z_p(const ex &a, const ex &b, ex &q, const exvector& vars, const long p);

} // namespace GiNaC

#endif /* GINAC_CHINREM_GCD_DIVIDE_IN_Z_P_H */

