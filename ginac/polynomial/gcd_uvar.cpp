#include "upoly.h"
#include "sr_gcd_uvar.h"
#include "heur_gcd_uvar.h"
#include <stdexcept>

namespace GiNaC
{

upoly sr_gcd(const upoly& a, const upoly& b)
{
	upoly g;
	bool found = sr_gcd_priv(g, a, b);
	if (found)
		return g;

	throw std::runtime_error("failed to compute gcd");
}

bool heur_gcd_z(upoly& g, const upoly& a, const upoly& b)
{
	return heur_gcd_z_priv(g, a, b);
}

upoly pseudoremainder(const upoly& a, const upoly& b)
{
	upoly r;
	pseudoremainder(r, a, b);
	return r;

}

}

