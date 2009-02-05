#ifndef GINAC_CHINREM_GCD_PRIMES_FACTORY_H
#define GINAC_CHINREM_GCD_PRIMES_FACTORY_H
#include <cln/integer.h>
#include <cln/numtheory.h>
#include <limits>
#include "smod_helpers.h"
#include "debug.h"

namespace GiNaC
{

/**
 * Find a `big' prime p such that lc mod p != 0. Helper class used by modular
 * GCD algorithm.
 */
class primes_factory
{
private:
	// These primes need to be large enough, so that the number of images
	// we need to reconstruct the GCD (in Z) is reasonable. On the other
	// hand, they should be as small as possible, so that operations on
	// coefficients are efficient. Practically this means we coefficients
	// should be native integers. (N.B.: as of now chinrem_gcd uses cl_I
	// or even numeric. Eventually this will be fixed).
	cln::cl_I last;
	// This ensures coefficients are immediate.
	static const int immediate_bits = 8*sizeof(void *) - __alignof__(void *);
	static const long opt_hint = (1L << (immediate_bits >> 1)) - 1;
public:
	primes_factory()
	{
		last = cln::nextprobprime(cln::cl_I(opt_hint));
	}

	bool operator()(long& p, const cln::cl_I& lc)
	{
		static const cln::cl_I maxval(std::numeric_limits<long>::max());
		while (last < maxval) {
			long p_ = cln::cl_I_to_long(last);
			last = cln::nextprobprime(last + 1);

			if (!zerop(smod(lc, p_))) {
				p = p_;
				return true;
			}
		}
		return false;
	}

	bool has_primes() const
	{
		static const cln::cl_I maxval(std::numeric_limits<long>::max());
		return last < maxval;
	}
};

} // namespace GiNaC

#endif /* GINAC_CHINREM_GCD_PRIMES_FACTORY_H */

