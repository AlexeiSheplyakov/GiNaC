/** @file inifcns_gamma.cpp
 *
 *  Implementation of Gamma function and some related stuff.
 *
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <vector>
#include <stdexcept>

#include "ginac.h"

//////////
// gamma function
//////////

/** Evaluation of gamma(x). Knows about integer arguments, half-integer
 *  arguments and that's it. Somebody ought to provide some good numerical
 *  evaluation some day...
 *
 *  @exception fail_numeric("complex_infinity") or something similar... */
ex gamma_eval(ex const & x)
{
    if ( x.info(info_flags::numeric) ) {

        // trap integer arguments:
        if ( x.info(info_flags::integer) ) {
            // gamma(n+1) -> n! for postitive n
            if ( x.info(info_flags::posint) ) {
                return factorial(ex_to_numeric(x).sub(numONE()));
            } else {
                return numZERO();  // Infinity. Throw? What?
            }
        }
        // trap half integer arguments:
        if ( (x*2).info(info_flags::integer) ) {
            // trap positive x=(n+1/2)
            // gamma(n+1/2) -> Pi^(1/2)*(1*3*..*(2*n-1))/(2^n)
            if ( (x*2).info(info_flags::posint) ) {
                numeric n = ex_to_numeric(x).sub(numHALF());
                numeric coefficient = doublefactorial(n.mul(numTWO()).sub(numONE()));
                coefficient = coefficient.div(numTWO().power(n));
                return mul(coefficient,power(Pi,numHALF()));
            } else {
                // trap negative x=(-n+1/2)
                // gamma(-n+1/2) -> Pi^(1/2)*(-2)^n/(1*3*..*(2*n-1))
                numeric n = abs(ex_to_numeric(x).sub(numHALF()));
                numeric coefficient = numeric(-2).power(n);
                coefficient = coefficient.div(doublefactorial(n.mul(numTWO()).sub(numONE())));;
                return mul(coefficient,power(Pi,numHALF()));
            }
        }
    }
    return gamma(x).hold();
}    
    
ex gamma_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(gamma(x))
    
    return gamma(ex_to_numeric(x));
}

ex gamma_diff(ex const & x, unsigned diff_param)
{
    ASSERT(diff_param==0);

    return power(x, -1);	//!!
}

ex gamma_series(ex const & x, symbol const & s, ex const & point, int order)
{
	//!! Only handle one special case for now...
	if (x.is_equal(s) && point.is_zero()) {
		ex e = 1 / s - EulerGamma + s * (power(Pi, 2) / 12 + power(EulerGamma, 2) / 2) + Order(power(s, 2));
		return e.series(s, point, order);
	} else
		throw(std::logic_error("don't know the series expansion of this particular gamma function"));
}

REGISTER_FUNCTION(gamma, gamma_eval, gamma_evalf, gamma_diff, gamma_series);
