#ifndef GINAC_CHINREM_GCD_PGCD_H
#define GINAC_CHINREM_GCD_PGCD_H
#include "ex.h"

namespace GiNaC
{

/// Exception to be thrown when modular GCD algorithm fails
struct pgcd_failed
{
	virtual ~pgcd_failed() { }
};

/**
 * @brief Compute the GCD of two polynomials over a prime field Z_p
 * 
 * @param vars variables
 * @param p designates the coefficient field Z_p
 * @param A polynomial \in Z_p[vars]
 * @param B second polynomial \in Z_p[vars]
 */
extern ex
pgcd(const ex& A, const ex& B, const exvector& vars, const long p);

} // namespace GiNaC

#endif
