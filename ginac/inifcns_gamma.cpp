/** @file inifcns_gamma.cpp
 *
 *  Implementation of Gamma-function, Beta-function, Polygamma-functions, and
 *  some related stuff. */

/*
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

#include "inifcns.h"
#include "ex.h"
#include "constant.h"
#include "series.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "symbol.h"
#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

//////////
// Gamma-function
//////////

static ex gamma_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(gamma(x))
    
    return gamma(ex_to_numeric(x));
}

/** Evaluation of gamma(x). Knows about integer arguments, half-integer
 *  arguments and that's it. Somebody ought to provide some good numerical
 *  evaluation some day...
 *
 *  @exception std::domain_error("gamma_eval(): simple pole") */
static ex gamma_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // trap integer arguments:
        if (x.info(info_flags::integer)) {
            // gamma(n+1) -> n! for postitive n
            if (x.info(info_flags::posint)) {
                return factorial(ex_to_numeric(x).sub(numONE()));
            } else {
                throw (std::domain_error("gamma_eval(): simple pole"));
            }
        }
        // trap half integer arguments:
        if ((x*2).info(info_flags::integer)) {
            // trap positive x==(n+1/2)
            // gamma(n+1/2) -> Pi^(1/2)*(1*3*..*(2*n-1))/(2^n)
            if ((x*2).info(info_flags::posint)) {
                numeric n = ex_to_numeric(x).sub(numHALF());
                numeric coefficient = doublefactorial(n.mul(numTWO()).sub(numONE()));
                coefficient = coefficient.div(numTWO().power(n));
                return coefficient * pow(Pi,numHALF());
            } else {
                // trap negative x==(-n+1/2)
                // gamma(-n+1/2) -> Pi^(1/2)*(-2)^n/(1*3*..*(2*n-1))
                numeric n = abs(ex_to_numeric(x).sub(numHALF()));
                numeric coefficient = numeric(-2).power(n);
                coefficient = coefficient.div(doublefactorial(n.mul(numTWO()).sub(numONE())));;
                return coefficient*sqrt(Pi);
            }
        }
    }
    return gamma(x).hold();
}    

static ex gamma_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return psi(x)*gamma(x);  // diff(log(gamma(x)),x)==psi(x)
}

static ex gamma_series(ex const & x, symbol const & s, ex const & point, int order)
{
    // method:
    // Taylor series where there is no pole falls back to psi functions.
    // On a pole at -n use the identity
    //   series(GAMMA(x),x=-n,order) ==
    //   series(GAMMA(x+n+1)/(x*(x+1)...*(x+n)),x=-n,order+1);
    ex xpoint = x.subs(s==point);
    if (!xpoint.info(info_flags::integer) || xpoint.info(info_flags::positive))
        throw do_taylor();
    // if we got here we have to care for a simple pole at -n:
    numeric n = -ex_to_numeric(xpoint);
    ex ser_numer = gamma(x+n+exONE());
    ex ser_denom = exONE();
    for (numeric p; p<=n; ++p)
        ser_denom *= x+p;
    return (ser_numer/ser_denom).series(s, point, order+1);
}

REGISTER_FUNCTION(gamma, gamma_eval, gamma_evalf, gamma_diff, gamma_series);

//////////
// Beta-function
//////////

static ex beta_evalf(ex const & x, ex const & y)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
        TYPECHECK(y,numeric)
    END_TYPECHECK(beta(x,y))
    
    return gamma(ex_to_numeric(x))*gamma(ex_to_numeric(y))
        / gamma(ex_to_numeric(x+y));
}

static ex beta_eval(ex const & x, ex const & y)
{
    if (x.info(info_flags::numeric) && y.info(info_flags::numeric)) {
        numeric nx(ex_to_numeric(x));
        numeric ny(ex_to_numeric(y));
        // treat all problematic x and y that may not be passed into gamma,
        // because they would throw there although beta(x,y) is well-defined:
        if (nx.is_real() && nx.is_integer() &&
            ny.is_real() && ny.is_integer()) {
            if (nx.is_negative()) {
                if (nx<=-ny)
                    return numMINUSONE().power(ny)*beta(1-x-y, y);
                else
                    throw (std::domain_error("beta_eval(): simple pole"));
            }
            if (ny.is_negative()) {
                if (ny<=-nx)
                    return numMINUSONE().power(nx)*beta(1-y-x, x);
                else
                    throw (std::domain_error("beta_eval(): simple pole"));
            }
            return gamma(x)*gamma(y)/gamma(x+y);
        }
        // no problem in numerator, but denominator has pole:
        if ((nx+ny).is_real() &&
            (nx+ny).is_integer() &&
            !(nx+ny).is_positive())
            return exZERO();
        return gamma(x)*gamma(y)/gamma(x+y);
    }
    return beta(x,y).hold();
}

static ex beta_diff(ex const & x, ex const & y, unsigned diff_param)
{
    GINAC_ASSERT(diff_param<2);
    ex retval;
    
    if (diff_param==0)  // d/dx beta(x,y)
        retval = (psi(x)-psi(x+y))*beta(x,y);
    if (diff_param==1)  // d/dy beta(x,y)
        retval = (psi(y)-psi(x+y))*beta(x,y);
    return retval;
}

REGISTER_FUNCTION(beta, beta_eval, beta_evalf, beta_diff, NULL);

//////////
// Psi-function (aka polygamma-function)
//////////

static ex psi1_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(psi(x))
    
    return psi(ex_to_numeric(x));
}

/** Evaluation of polygamma-function psi(x). 
 *  Somebody ought to provide some good numerical evaluation some day... */
static ex psi1_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        if (x.info(info_flags::integer) && !x.info(info_flags::positive))
            throw (std::domain_error("psi_eval(): simple pole"));
        if (x.info(info_flags::positive)) {
            // psi(n) -> 1 + 1/2 +...+ 1/(n-1) - EulerGamma
            if (x.info(info_flags::integer)) {
                numeric rat(0);
                for (numeric i(ex_to_numeric(x)-numONE()); i.is_positive(); --i)
                    rat += i.inverse();
                return rat-EulerGamma;
            }
            // psi((2m+1)/2) -> 2/(2m+1) + 2/2m +...+ 2/1 - EulerGamma - 2log(2)
            if ((exTWO()*x).info(info_flags::integer)) {
                numeric rat(0);
                for (numeric i((ex_to_numeric(x)-numONE())*numTWO()); i.is_positive(); i-=numTWO())
                    rat += numTWO()*i.inverse();
                return rat-EulerGamma-exTWO()*log(exTWO());
            }
            if (x.compare(exONE())==1) {
                // should call numeric, since >1
            }
        }
    }
    return psi(x).hold();
}

static ex psi1_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return psi(exONE(), x);
}

const unsigned function_index_psi1 = function::register_new("psi", psi1_eval, psi1_evalf, psi1_diff, NULL);

//////////
// Psi-functions (aka polygamma-functions)  psi(0,x)==psi(x)
//////////

static ex psi2_evalf(ex const & n, ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(n,numeric)
        TYPECHECK(x,numeric)
    END_TYPECHECK(psi(n,x))
    
    return psi(ex_to_numeric(n), ex_to_numeric(x));
}

/** Evaluation of polygamma-function psi(n,x). 
 *  Somebody ought to provide some good numerical evaluation some day... */
static ex psi2_eval(ex const & n, ex const & x)
{
    // psi(0,x) -> psi(x)
    if (n.is_zero())
        return psi(x);
    // psi(-1,x) -> log(gamma(x))
    if (n.is_equal(exMINUSONE()))
        return log(gamma(x));
    if (n.info(info_flags::numeric) && n.info(info_flags::posint) &&
        x.info(info_flags::numeric)) {
        numeric nn = ex_to_numeric(n);
        numeric nx = ex_to_numeric(x);
        if (x.is_equal(exONE()))
            return numMINUSONE().power(nn+numONE())*factorial(nn)*zeta(ex(nn+numONE()));
    }
    return psi(n, x).hold();
}    

static ex psi2_diff(ex const & n, ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param<2);
    
    if (diff_param==0) {
        // d/dn psi(n,x)
        throw(std::logic_error("cannot diff psi(n,x) with respect to n"));
    }
    // d/dx psi(n,x)
    return psi(n+1, x);
}

const unsigned function_index_psi2 = function::register_new("psi", psi2_eval, psi2_evalf, psi2_diff, NULL);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
