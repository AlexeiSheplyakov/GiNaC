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

static ex gamma_evalf(const ex & x)
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
static ex gamma_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // trap integer arguments:
        if (x.info(info_flags::integer)) {
            // gamma(n+1) -> n! for postitive n
            if (x.info(info_flags::posint)) {
                return factorial(ex_to_numeric(x).sub(_num1()));
            } else {
                throw (std::domain_error("gamma_eval(): simple pole"));
            }
        }
        // trap half integer arguments:
        if ((x*2).info(info_flags::integer)) {
            // trap positive x==(n+1/2)
            // gamma(n+1/2) -> Pi^(1/2)*(1*3*..*(2*n-1))/(2^n)
            if ((x*_ex2()).info(info_flags::posint)) {
                numeric n = ex_to_numeric(x).sub(_num1_2());
                numeric coefficient = doublefactorial(n.mul(_num2()).sub(_num1()));
                coefficient = coefficient.div(pow(_num2(),n));
                return coefficient * pow(Pi,_ex1_2());
            } else {
                // trap negative x==(-n+1/2)
                // gamma(-n+1/2) -> Pi^(1/2)*(-2)^n/(1*3*..*(2*n-1))
                numeric n = abs(ex_to_numeric(x).sub(_num1_2()));
                numeric coefficient = pow(_num_2(), n);
                coefficient = coefficient.div(doublefactorial(n.mul(_num2()).sub(_num1())));;
                return coefficient*power(Pi,_ex1_2());
            }
        }
        //  gamma_evalf should be called here once it becomes available
    }
    
    return gamma(x).hold();
}    

static ex gamma_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx  log(gamma(x)) -> psi(x)
    // d/dx  gamma(x) -> psi(x)*gamma(x)
    return psi(x)*gamma(x);
}

static ex gamma_series(const ex & x, const symbol & s, const ex & point, int order)
{
    // method:
    // Taylor series where there is no pole falls back to psi function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   gamma(x) == gamma(x+1) / x
    // from which follows
    //   series(gamma(x),x,-m,order) ==
    //   series(gamma(x+m+1)/(x*(x+1)...*(x+m)),x,-m,order+1);
    ex xpoint = x.subs(s==point);
    if (!xpoint.info(info_flags::integer) || xpoint.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole at -m:
    numeric m = -ex_to_numeric(xpoint);
    ex ser_numer = gamma(x+m+_ex1());
    ex ser_denom = _ex1();
    for (numeric p; p<=m; ++p)
        ser_denom *= x+p;
    return (ser_numer/ser_denom).series(s, point, order+1);
}

REGISTER_FUNCTION(gamma, gamma_eval, gamma_evalf, gamma_diff, gamma_series);

//////////
// Beta-function
//////////

static ex beta_evalf(const ex & x, const ex & y)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
        TYPECHECK(y,numeric)
    END_TYPECHECK(beta(x,y))
    
    return gamma(ex_to_numeric(x))*gamma(ex_to_numeric(y))
        / gamma(ex_to_numeric(x+y));
}

static ex beta_eval(const ex & x, const ex & y)
{
    if (x.info(info_flags::numeric) && y.info(info_flags::numeric)) {
        numeric nx(ex_to_numeric(x));
        numeric ny(ex_to_numeric(y));
        // treat all problematic x and y that may not be passed into gamma,
        // because they would throw there although beta(x,y) is well-defined
        // using the formula beta(x,y) == (-1)^y * beta(1-x-y, y)
        if (nx.is_real() && nx.is_integer() &&
            ny.is_real() && ny.is_integer()) {
            if (nx.is_negative()) {
                if (nx<=-ny)
                    return pow(_num_1(), ny)*beta(1-x-y, y);
                else
                    throw (std::domain_error("beta_eval(): simple pole"));
            }
            if (ny.is_negative()) {
                if (ny<=-nx)
                    return pow(_num_1(), nx)*beta(1-y-x, x);
                else
                    throw (std::domain_error("beta_eval(): simple pole"));
            }
            return gamma(x)*gamma(y)/gamma(x+y);
        }
        // no problem in numerator, but denominator has pole:
        if ((nx+ny).is_real() &&
            (nx+ny).is_integer() &&
            !(nx+ny).is_positive())
             return _ex0();
        // everything is ok:
        return gamma(x)*gamma(y)/gamma(x+y);
    }
    
    return beta(x,y).hold();
}

static ex beta_diff(const ex & x, const ex & y, unsigned diff_param)
{
    GINAC_ASSERT(diff_param<2);
    ex retval;
    
    // d/dx beta(x,y) -> (psi(x)-psi(x+y)) * beta(x,y)
    if (diff_param==0)
        retval = (psi(x)-psi(x+y))*beta(x,y);
    // d/dy beta(x,y) -> (psi(y)-psi(x+y)) * beta(x,y)
    if (diff_param==1)
        retval = (psi(y)-psi(x+y))*beta(x,y);
    return retval;
}

static ex beta_series(const ex & x, const ex & y, const symbol & s, const ex & point, int order)
{
    // method:
    // Taylor series where there is no pole falls back to beta function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   gamma(x) == gamma(x+1) / x
    // from which follows
    //   series(gamma(x),x,-m,order) ==
    //   series(gamma(x+m+1)/(x*(x+1)...*(x+m)),x,-m,order+1);
    ex xpoint = x.subs(s==point);
    ex ypoint = y.subs(s==point);
    if ((!xpoint.info(info_flags::integer) || xpoint.info(info_flags::positive)) &&
        (!ypoint.info(info_flags::integer) || ypoint.info(info_flags::positive)))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole at -m:
    throw (std::domain_error("beta_series(): Mama, please code me!"));
}

REGISTER_FUNCTION(beta, beta_eval, beta_evalf, beta_diff, beta_series);

//////////
// Psi-function (aka digamma-function)
//////////

static ex psi1_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(psi(x))
    
    return psi(ex_to_numeric(x));
}

/** Evaluation of digamma-function psi(x).
 *  Somebody ought to provide some good numerical evaluation some day... */
static ex psi1_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        numeric nx = ex_to_numeric(x);
        if (nx.is_integer()) {
            // integer case 
            if (nx.is_positive()) {
                // psi(n) -> 1 + 1/2 +...+ 1/(n-1) - EulerGamma
                numeric rat(0);
                for (numeric i(nx+_num_1()); i.is_positive(); --i)
                    rat += i.inverse();
                return rat-EulerGamma;
            } else {
                // for non-positive integers there is a pole:
                throw (std::domain_error("psi_eval(): simple pole"));
            }
        }
        if ((_num2()*nx).is_integer()) {
            // half integer case
            if (nx.is_positive()) {
                // psi((2m+1)/2) -> 2/(2m+1) + 2/2m +...+ 2/1 - EulerGamma - 2log(2)
                numeric rat(0);
                for (numeric i((nx+_num_1())*_num2()); i.is_positive(); i-=_num2())
                                      rat += _num2()*i.inverse();
                                      return rat-EulerGamma-_ex2()*log(_ex2());
            } else {
                // use the recurrence relation
                //   psi(-m-1/2) == psi(-m-1/2+1) - 1 / (-m-1/2)
                // to relate psi(-m-1/2) to psi(1/2):
                //   psi(-m-1/2) == psi(1/2) + r
                // where r == ((-1/2)^(-1) + ... + (-m-1/2)^(-1))
                numeric recur(0);
                for (numeric p(nx); p<0; ++p)
                    recur -= pow(p, _num_1());
                return recur+psi(_ex1_2());
            }
        }
        //  psi1_evalf should be called here once it becomes available
    }
    
    return psi(x).hold();
}

static ex psi1_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx psi(x) -> psi(1,x)
    return psi(_ex1(), x);
}

static ex psi1_series(const ex & x, const symbol & s, const ex & point, int order)
{
    // method:
    // Taylor series where there is no pole falls back to polygamma function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   psi(x) == psi(x+1) - 1/z
    // from which follows
    //   series(psi(x),x,-m,order) ==
    //   series(psi(x+m+1) - 1/x - 1/(x+1) - 1/(x+m)),x,-m,order);
    ex xpoint = x.subs(s==point);
    if (!xpoint.info(info_flags::integer) || xpoint.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole at -m:
    numeric m = -ex_to_numeric(xpoint);
    ex recur;
    for (numeric p; p<=m; ++p)
        recur += power(x+p,_ex_1());
    return (psi(x+m+_ex1())-recur).series(s, point, order);
}

const unsigned function_index_psi1 = function::register_new("psi", psi1_eval, psi1_evalf, psi1_diff, psi1_series);

//////////
// Psi-functions (aka polygamma-functions)  psi(0,x)==psi(x)
//////////

static ex psi2_evalf(const ex & n, const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(n,numeric)
        TYPECHECK(x,numeric)
    END_TYPECHECK(psi(n,x))
    
    return psi(ex_to_numeric(n), ex_to_numeric(x));
}

/** Evaluation of polygamma-function psi(n,x). 
 *  Somebody ought to provide some good numerical evaluation some day... */
static ex psi2_eval(const ex & n, const ex & x)
{
    // psi(0,x) -> psi(x)
    if (n.is_zero())
        return psi(x);
    // psi(-1,x) -> log(gamma(x))
    if (n.is_equal(_ex_1()))
        return log(gamma(x));
    if (n.info(info_flags::numeric) && n.info(info_flags::posint) &&
        x.info(info_flags::numeric)) {
        numeric nn = ex_to_numeric(n);
        numeric nx = ex_to_numeric(x);
        if (nx.is_integer()) {
            // integer case 
            if (nx.is_equal(_num1()))
                // use psi(n,1) == (-)^(n+1) * n! * zeta(n+1)
                return pow(_num_1(),nn+_num1())*factorial(nn)*zeta(ex(nn+_num1()));
            if (nx.is_positive()) {
                // use the recurrence relation
                //   psi(n,m) == psi(n,m+1) - (-)^n * n! / m^(n+1)
                // to relate psi(n,m) to psi(n,1):
                //   psi(n,m) == psi(n,1) + r
                // where r == (-)^n * n! * (1^(-n-1) + ... + (m-1)^(-n-1))
                numeric recur(0);
                for (numeric p(1); p<nx; ++p)
                    recur += pow(p, -nn+_num_1());
                recur *= factorial(nn)*pow(_num_1(), nn);
                return recur+psi(n,_ex1());
            } else {
                // for non-positive integers there is a pole:
                throw (std::domain_error("psi2_eval(): pole"));
            }
        }
        if ((_num2()*nx).is_integer()) {
            // half integer case
            if (nx.is_equal(_num1_2()))
                // use psi(n,1/2) == (-)^(n+1) * n! * (2^(n+1)-1) * zeta(n+1)
                return pow(_num_1(),nn+_num1())*factorial(nn)*(pow(_num2(),nn+_num1()) + _num_1())*zeta(ex(nn+_num1()));
            if (nx.is_positive()) {
                numeric m = nx - _num1_2();
                // use the multiplication formula
                //   psi(n,2*m) == (psi(n,m) + psi(n,m+1/2)) / 2^(n+1)
                // to revert to positive integer case
                return psi(n,_num2()*m)*pow(_num2(),nn+_num1())-psi(n,m);
            } else {
                // use the recurrence relation
                //   psi(n,-m-1/2) == psi(n,-m-1/2+1) - (-)^n * n! / (-m-1/2)^(n+1)
                // to relate psi(n,-m-1/2) to psi(n,1/2):
                //   psi(n,-m-1/2) == psi(n,1/2) + r
                // where r == (-)^(n+1) * n! * ((-1/2)^(-n-1) + ... + (-m-1/2)^(-n-1))
                numeric recur(0);
                for (numeric p(nx); p<0; ++p)
                    recur += pow(p, -nn+_num_1());
                recur *= factorial(nn)*pow(_num_1(), nn+_num_1());
                return recur+psi(n,_ex1_2());
            }
        }
        //  psi2_evalf should be called here once it becomes available
    }
    
    return psi(n, x).hold();
}    

static ex psi2_diff(const ex & n, const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param<2);
    
    if (diff_param==0) {
        // d/dn psi(n,x)
        throw(std::logic_error("cannot diff psi(n,x) with respect to n"));
    }
    // d/dx psi(n,x) -> psi(n+1,x)
    return psi(n+_ex1(), x);
}

static ex psi2_series(const ex & n, const ex & x, const symbol & s, const ex & point, int order)
{
    // method:
    // Taylor series where there is no pole falls back to polygamma function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   psi(n,x) == psi(n,x+1) - (-)^n * n! / x^(n+1)
    // from which follows
    //   series(psi(x),x,-m,order) == 
    //   series(psi(x+m+1) - (-1)^n * n! * ((x)^(-n-1) + (x+1)^(-n-1) + ...
    //                                      ... + (x+m)^(-n-1))),x,-m,order);
    ex xpoint = x.subs(s==point);
    if (!xpoint.info(info_flags::integer) || xpoint.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a pole of order n+1 at -m:
    numeric m = -ex_to_numeric(xpoint);
    ex recur;
    for (numeric p; p<=m; ++p)
        recur += power(x+p,-n+_ex_1());
    recur *= factorial(n)*power(_ex_1(),n);
    return (psi(n, x+m+_ex1())-recur).series(s, point, order);
}

const unsigned function_index_psi2 = function::register_new("psi", psi2_eval, psi2_evalf, psi2_diff, psi2_series);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
