/** @file inifcns_gamma.cpp
 *
 *  Implementation of Gamma-function, Beta-function, Polygamma-functions, and
 *  some related stuff. */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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
#include "pseries.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "symbol.h"
#include "utils.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

//////////
// Logarithm of Gamma function
//////////

static ex lgamma_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(lgamma(x))
    
    return lgamma(ex_to_numeric(x));
}


/** Evaluation of lgamma(x), the natural logarithm of the Gamma function.
 *  Knows about integer arguments and that's it.  Somebody ought to provide
 *  some good numerical evaluation some day...
 *
 *  @exception GiNaC::pole_error("lgamma_eval(): logarithmic pole",0) */
static ex lgamma_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // trap integer arguments:
        if (x.info(info_flags::integer)) {
            // lgamma(n) -> log((n-1)!) for postitive n
            if (x.info(info_flags::posint)) {
                return log(factorial(x.exadd(_ex_1())));
            } else {
                throw (pole_error("lgamma_eval(): logarithmic pole",0));
            }
        }
        //  lgamma_evalf should be called here once it becomes available
    }
    
    return lgamma(x).hold();
}


static ex lgamma_deriv(const ex & x, unsigned deriv_param)
{
    GINAC_ASSERT(deriv_param==0);
    
    // d/dx  lgamma(x) -> psi(x)
    return psi(x);
}


static ex lgamma_series(const ex & arg,
                        const relational & rel,
                        int order,
                        bool branchcut)
{
    // method:
    // Taylor series where there is no pole falls back to psi function
    // evaluation.
    // On a pole at -m we could use the recurrence relation
    //   lgamma(x) == lgamma(x+1)-log(x)
    // from which follows
    //   series(lgamma(x),x==-m,order) ==
    //   series(lgamma(x+m+1)-log(x)...-log(x+m)),x==-m,order);
    // This, however, seems to fail utterly because you run into branch-cut
    // problems.  Somebody ought to implement it some day using an asymptotic
    // series for tgamma:
    const ex arg_pt = arg.subs(rel);
    if (!arg_pt.info(info_flags::integer) || arg_pt.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole of tgamma(-m):
    throw (std::overflow_error("lgamma_series: please implement my at the poles"));
    return _ex0();  // not reached
}


REGISTER_FUNCTION(lgamma, eval_func(lgamma_eval).
                          evalf_func(lgamma_evalf).
                          derivative_func(lgamma_deriv).
                          series_func(lgamma_series));


//////////
// true Gamma function
//////////

static ex tgamma_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(tgamma(x))
    
    return tgamma(ex_to_numeric(x));
}


/** Evaluation of tgamma(x), the true Gamma function.  Knows about integer
 *  arguments, half-integer arguments and that's it. Somebody ought to provide
 *  some good numerical evaluation some day...
 *
 *  @exception pole_error("tgamma_eval(): simple pole",0) */
static ex tgamma_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // trap integer arguments:
        if (x.info(info_flags::integer)) {
            // tgamma(n) -> (n-1)! for postitive n
            if (x.info(info_flags::posint)) {
                return factorial(ex_to_numeric(x).sub(_num1()));
            } else {
                throw (pole_error("tgamma_eval(): simple pole",1));
            }
        }
        // trap half integer arguments:
        if ((x*2).info(info_flags::integer)) {
            // trap positive x==(n+1/2)
            // tgamma(n+1/2) -> Pi^(1/2)*(1*3*..*(2*n-1))/(2^n)
            if ((x*_ex2()).info(info_flags::posint)) {
                numeric n = ex_to_numeric(x).sub(_num1_2());
                numeric coefficient = doublefactorial(n.mul(_num2()).sub(_num1()));
                coefficient = coefficient.div(pow(_num2(),n));
                return coefficient * pow(Pi,_ex1_2());
            } else {
                // trap negative x==(-n+1/2)
                // tgamma(-n+1/2) -> Pi^(1/2)*(-2)^n/(1*3*..*(2*n-1))
                numeric n = abs(ex_to_numeric(x).sub(_num1_2()));
                numeric coefficient = pow(_num_2(), n);
                coefficient = coefficient.div(doublefactorial(n.mul(_num2()).sub(_num1())));;
                return coefficient*power(Pi,_ex1_2());
            }
        }
        //  tgamma_evalf should be called here once it becomes available
    }
    
    return tgamma(x).hold();
}


static ex tgamma_deriv(const ex & x, unsigned deriv_param)
{
    GINAC_ASSERT(deriv_param==0);
    
    // d/dx  tgamma(x) -> psi(x)*tgamma(x)
    return psi(x)*tgamma(x);
}


static ex tgamma_series(const ex & arg,
                        const relational & rel,
                        int order,
                        bool branchcut)
{
    // method:
    // Taylor series where there is no pole falls back to psi function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   tgamma(x) == tgamma(x+1) / x
    // from which follows
    //   series(tgamma(x),x==-m,order) ==
    //   series(tgamma(x+m+1)/(x*(x+1)*...*(x+m)),x==-m,order+1);
    const ex arg_pt = arg.subs(rel);
    if (!arg_pt.info(info_flags::integer) || arg_pt.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole at -m:
    numeric m = -ex_to_numeric(arg_pt);
    ex ser_denom = _ex1();
    for (numeric p; p<=m; ++p)
        ser_denom *= arg+p;
    return (tgamma(arg+m+_ex1())/ser_denom).series(rel, order+1);
}


REGISTER_FUNCTION(tgamma, eval_func(tgamma_eval).
                          evalf_func(tgamma_evalf).
                          derivative_func(tgamma_deriv).
                          series_func(tgamma_series));


//////////
// beta-function
//////////

static ex beta_evalf(const ex & x, const ex & y)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
        TYPECHECK(y,numeric)
    END_TYPECHECK(beta(x,y))
    
    return tgamma(ex_to_numeric(x))*tgamma(ex_to_numeric(y))/tgamma(ex_to_numeric(x+y));
}


static ex beta_eval(const ex & x, const ex & y)
{
    if (x.info(info_flags::numeric) && y.info(info_flags::numeric)) {
        // treat all problematic x and y that may not be passed into tgamma,
        // because they would throw there although beta(x,y) is well-defined
        // using the formula beta(x,y) == (-1)^y * beta(1-x-y, y)
        numeric nx(ex_to_numeric(x));
        numeric ny(ex_to_numeric(y));
        if (nx.is_real() && nx.is_integer() &&
            ny.is_real() && ny.is_integer()) {
            if (nx.is_negative()) {
                if (nx<=-ny)
                    return pow(_num_1(), ny)*beta(1-x-y, y);
                else
                    throw (pole_error("beta_eval(): simple pole",1));
            }
            if (ny.is_negative()) {
                if (ny<=-nx)
                    return pow(_num_1(), nx)*beta(1-y-x, x);
                else
                    throw (pole_error("beta_eval(): simple pole",1));
            }
            return tgamma(x)*tgamma(y)/tgamma(x+y);
        }
        // no problem in numerator, but denominator has pole:
        if ((nx+ny).is_real() &&
            (nx+ny).is_integer() &&
            !(nx+ny).is_positive())
             return _ex0();
        // everything is ok:
        return tgamma(x)*tgamma(y)/tgamma(x+y);
    }
    
    return beta(x,y).hold();
}


static ex beta_deriv(const ex & x, const ex & y, unsigned deriv_param)
{
    GINAC_ASSERT(deriv_param<2);
    ex retval;
    
    // d/dx beta(x,y) -> (psi(x)-psi(x+y)) * beta(x,y)
    if (deriv_param==0)
        retval = (psi(x)-psi(x+y))*beta(x,y);
    // d/dy beta(x,y) -> (psi(y)-psi(x+y)) * beta(x,y)
    if (deriv_param==1)
        retval = (psi(y)-psi(x+y))*beta(x,y);
    return retval;
}


static ex beta_series(const ex & arg1,
                      const ex & arg2,
                      const relational & rel,
                      int order,
                      bool branchcut)
{
    // method:
    // Taylor series where there is no pole of one of the tgamma functions
    // falls back to beta function evaluation.  Otherwise, fall back to
    // tgamma series directly.
    const ex arg1_pt = arg1.subs(rel);
    const ex arg2_pt = arg2.subs(rel);
    GINAC_ASSERT(is_ex_exactly_of_type(rel.lhs(),symbol));
    const symbol *s = static_cast<symbol *>(rel.lhs().bp);
    ex arg1_ser, arg2_ser, arg1arg2_ser;
    if ((!arg1_pt.info(info_flags::integer) || arg1_pt.info(info_flags::positive)) &&
        (!arg2_pt.info(info_flags::integer) || arg2_pt.info(info_flags::positive)))
        throw do_taylor();  // caught by function::series()
    // trap the case where arg1 is on a pole:
    if (arg1.info(info_flags::integer) && !arg1.info(info_flags::positive))
        arg1_ser = tgamma(arg1+*s).series(rel,order);
    else
        arg1_ser = tgamma(arg1).series(rel,order);
    // trap the case where arg2 is on a pole:
    if (arg2.info(info_flags::integer) && !arg2.info(info_flags::positive))
        arg2_ser = tgamma(arg2+*s).series(rel,order);
    else
        arg2_ser = tgamma(arg2).series(rel,order);
    // trap the case where arg1+arg2 is on a pole:
    if ((arg1+arg2).info(info_flags::integer) && !(arg1+arg2).info(info_flags::positive))
        arg1arg2_ser = tgamma(arg2+arg1+*s).series(rel,order);
    else
        arg1arg2_ser = tgamma(arg2+arg1).series(rel,order);
    // compose the result (expanding all the terms):
    return (arg1_ser*arg2_ser/arg1arg2_ser).series(rel,order).expand();
}


REGISTER_FUNCTION(beta, eval_func(beta_eval).
                        evalf_func(beta_evalf).
                        derivative_func(beta_deriv).
                        series_func(beta_series));


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
                // psi(n) -> 1 + 1/2 +...+ 1/(n-1) - Euler
                numeric rat(0);
                for (numeric i(nx+_num_1()); i.is_positive(); --i)
                    rat += i.inverse();
                return rat-Euler;
            } else {
                // for non-positive integers there is a pole:
                throw (pole_error("psi_eval(): simple pole",1));
            }
        }
        if ((_num2()*nx).is_integer()) {
            // half integer case
            if (nx.is_positive()) {
                // psi((2m+1)/2) -> 2/(2m+1) + 2/2m +...+ 2/1 - Euler - 2log(2)
                numeric rat(0);
                for (numeric i((nx+_num_1())*_num2()); i.is_positive(); i-=_num2())
                                      rat += _num2()*i.inverse();
                                      return rat-Euler-_ex2()*log(_ex2());
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

static ex psi1_deriv(const ex & x, unsigned deriv_param)
{
    GINAC_ASSERT(deriv_param==0);
    
    // d/dx psi(x) -> psi(1,x)
    return psi(_ex1(), x);
}

static ex psi1_series(const ex & arg,
                      const relational & rel,
                      int order,
                      bool branchcut)
{
    // method:
    // Taylor series where there is no pole falls back to polygamma function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   psi(x) == psi(x+1) - 1/z
    // from which follows
    //   series(psi(x),x==-m,order) ==
    //   series(psi(x+m+1) - 1/x - 1/(x+1) - 1/(x+m)),x==-m,order);
    const ex arg_pt = arg.subs(rel);
    if (!arg_pt.info(info_flags::integer) || arg_pt.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole at -m:
    numeric m = -ex_to_numeric(arg_pt);
    ex recur;
    for (numeric p; p<=m; ++p)
        recur += power(arg+p,_ex_1());
    return (psi(arg+m+_ex1())-recur).series(rel, order);
}

const unsigned function_index_psi1 =
    function::register_new(function_options("psi").
                           eval_func(psi1_eval).
                           evalf_func(psi1_evalf).
			   derivative_func(psi1_deriv).
			   series_func(psi1_series).
			   overloaded(2));

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
    // psi(-1,x) -> log(tgamma(x))
    if (n.is_equal(_ex_1()))
        return log(tgamma(x));
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
                throw (pole_error("psi2_eval(): pole",1));
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

static ex psi2_deriv(const ex & n, const ex & x, unsigned deriv_param)
{
    GINAC_ASSERT(deriv_param<2);
    
    if (deriv_param==0) {
        // d/dn psi(n,x)
        throw(std::logic_error("cannot diff psi(n,x) with respect to n"));
    }
    // d/dx psi(n,x) -> psi(n+1,x)
    return psi(n+_ex1(), x);
}

static ex psi2_series(const ex & n,
                      const ex & arg,
                      const relational & rel,
                      int order,
                      bool branchcut)
{
    // method:
    // Taylor series where there is no pole falls back to polygamma function
    // evaluation.
    // On a pole at -m use the recurrence relation
    //   psi(n,x) == psi(n,x+1) - (-)^n * n! / x^(n+1)
    // from which follows
    //   series(psi(x),x==-m,order) == 
    //   series(psi(x+m+1) - (-1)^n * n! * ((x)^(-n-1) + (x+1)^(-n-1) + ...
    //                                      ... + (x+m)^(-n-1))),x==-m,order);
    const ex arg_pt = arg.subs(rel);
    if (!arg_pt.info(info_flags::integer) || arg_pt.info(info_flags::positive))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a pole of order n+1 at -m:
    numeric m = -ex_to_numeric(arg_pt);
    ex recur;
    for (numeric p; p<=m; ++p)
        recur += power(arg+p,-n+_ex_1());
    recur *= factorial(n)*power(_ex_1(),n);
    return (psi(n, arg+m+_ex1())-recur).series(rel, order);
}

const unsigned function_index_psi2 =
    function::register_new(function_options("psi").
                           eval_func(psi2_eval).
                           evalf_func(psi2_evalf).
               derivative_func(psi2_deriv).
			   series_func(psi2_series).
			   overloaded(2));


#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
