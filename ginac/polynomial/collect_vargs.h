#ifndef GINAC_COLLECT_VARGS_HPP
#define GINAC_COLLECT_VARGS_HPP
#include "ex.h"
#include <cln/integer.h>
#include <vector>
#include <utility> // std::pair

namespace GiNaC
{

typedef std::vector<int> exp_vector_t;
typedef std::vector<std::pair<exp_vector_t, ex> > ex_collect_t;

extern void
collect_vargs(ex_collect_t& ec, const ex& e, const exvector& x);
extern ex
ex_collect_to_ex(const ex_collect_t& ec, const exvector& x);

/**
 * Leading coefficient of a multivariate polynomial e, considering it
 * as a multivariate polynomial in x_0, \ldots x_{n-1} with coefficients
 * being univariate polynomials in R[x_n] (where R is some ring)
 */
extern ex lcoeff_wrt(ex e, const exvector& x);

/**
 * Leading coefficient c \in R (where R = Z or Z_p) of a multivariate
 * polynomial e \in R[x_0, \ldots, x_n]
 */
extern cln::cl_I integer_lcoeff(const ex& e, const exvector& vars);

} // namespace GiNaC

#endif /* GINAC_COLLECT_VARGS_HPP */
