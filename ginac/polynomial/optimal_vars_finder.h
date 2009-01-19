#ifndef GINAC_CHINREM_GCD_OPTIMAL_SYMBOL_FINDER_H
#define GINAC_CHINREM_GCD_OPTIMAL_SYMBOL_FINDER_H
#include <vector>
#include "ex.h"

namespace GiNaC
{
/**
 * @brief Find the order of variables which is optimal for GCD computation.
 *
 * Collects statistical information about the highest and lowest degrees
 * of all variables that appear in two polynomials. Sorts the variables
 * by minimum degree (lowest to highest). The information gathered by
 * this function is used by GCD routines to find out the main variable
 * for GCD computation.
 */
extern exvector gcd_optimal_variables_order(const ex& A, const ex& B);
}
#endif /* GINAC_CHINREM_GCD_OPTIMAL_SYMBOL_FINDER_H */

