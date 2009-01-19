#ifndef GINAC_CHINREM_GCD_H
#define GINAC_CHINREM_GCD_H
#include "ex.h"

namespace GiNaC
{

extern ex chinrem_gcd(const ex& A_, const ex& B_, const exvector& vars);
extern ex chinrem_gcd(const ex& A, const ex& B);

struct chinrem_gcd_failed
{
	virtual ~chinrem_gcd_failed() { }
};

}

#endif /* GINAC_CHINREM_GCD_H */

