/** @file inifcns_trans.cpp
 *
 *  Implementation of transcendental (and trigonometric and hyperbolic)
 *  functions. */

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
#include "numeric.h"
#include "power.h"

namespace GiNaC {

//////////
// exponential function
//////////

static ex exp_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(exp(x))
    
    return exp(ex_to_numeric(x)); // -> numeric exp(numeric)
}

static ex exp_eval(ex const & x)
{
    // exp(0) -> 1
    if (x.is_zero()) {
        return exONE();
    }
    // exp(n*Pi*I/2) -> {+1|+I|-1|-I}
    ex TwoExOverPiI=(2*x)/(Pi*I);
    if (TwoExOverPiI.info(info_flags::integer)) {
        numeric z=mod(ex_to_numeric(TwoExOverPiI),numeric(4));
        if (z.is_equal(numZERO()))
            return exONE();
        if (z.is_equal(numONE()))
            return ex(I);
        if (z.is_equal(numTWO()))
            return exMINUSONE();
        if (z.is_equal(numTHREE()))
            return ex(-I);
    }
    // exp(log(x)) -> x
    if (is_ex_the_function(x, log))
        return x.op(0);
    
    // exp(float)
    if (x.info(info_flags::numeric) && !x.info(info_flags::rational))
        return exp_evalf(x);
    
    return exp(x).hold();
}    

static ex exp_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    return exp(x);
}

REGISTER_FUNCTION(exp, exp_eval, exp_evalf, exp_diff, NULL);

//////////
// natural logarithm
//////////

static ex log_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(log(x))
    
    return log(ex_to_numeric(x)); // -> numeric log(numeric)
}

static ex log_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // log(1) -> 0
        if (x.is_equal(exONE()))
            return exZERO();
        // log(-1) -> I*Pi
        if (x.is_equal(exMINUSONE()))
            return (I*Pi);
        // log(I) -> Pi*I/2
        if (x.is_equal(I))
            return (I*Pi*numeric(1,2));
        // log(-I) -> -Pi*I/2
        if (x.is_equal(-I))
            return (I*Pi*numeric(-1,2));
        // log(0) -> throw singularity
        if (x.is_equal(exZERO()))
            throw(std::domain_error("log_eval(): log(0)"));
        // log(float)
        if (!x.info(info_flags::rational))
            return log_evalf(x);
    }
    
    return log(x).hold();
}    

static ex log_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    return power(x, -1);
}

REGISTER_FUNCTION(log, log_eval, log_evalf, log_diff, NULL);

//////////
// sine (trigonometric function)
//////////

static ex sin_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(sin(x))
    
    return sin(ex_to_numeric(x)); // -> numeric sin(numeric)
}

static ex sin_eval(ex const & x)
{
    // sin(n*Pi) -> 0
    ex xOverPi=x/Pi;
    if (xOverPi.info(info_flags::integer))
        return exZERO();
    
    // sin((2n+1)*Pi/2) -> {+|-}1
    ex xOverPiMinusHalf=xOverPi-exHALF();
    if (xOverPiMinusHalf.info(info_flags::even))
        return exONE();
    else if (xOverPiMinusHalf.info(info_flags::odd))
        return exMINUSONE();
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t=x.op(0);
        // sin(asin(x)) -> x
        if (is_ex_the_function(x, asin))
            return t;
        // sin(acos(x)) -> (1-x^2)^(1/2)
        if (is_ex_the_function(x, acos))
            return power(exONE()-power(t,exTWO()),exHALF());
        // sin(atan(x)) -> x*(1+x^2)^(-1/2)
        if (is_ex_the_function(x, atan))
            return t*power(exONE()+power(t,exTWO()),exMINUSHALF());
    }
    
    // sin(float) -> float
    if (x.info(info_flags::numeric) && !x.info(info_flags::rational))
        return sin_evalf(x);
    
    return sin(x).hold();
}

static ex sin_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return cos(x);
}

REGISTER_FUNCTION(sin, sin_eval, sin_evalf, sin_diff, NULL);

//////////
// cosine (trigonometric function)
//////////

static ex cos_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(cos(x))
    
    return cos(ex_to_numeric(x)); // -> numeric cos(numeric)
}

static ex cos_eval(ex const & x)
{
    // cos(n*Pi) -> {+|-}1
    ex xOverPi=x/Pi;
    if (xOverPi.info(info_flags::even))
        return exONE();
    else if (xOverPi.info(info_flags::odd))
        return exMINUSONE();
    
    // cos((2n+1)*Pi/2) -> 0
    ex xOverPiMinusHalf=xOverPi-exHALF();
    if (xOverPiMinusHalf.info(info_flags::integer))
        return exZERO();
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t=x.op(0);
        // cos(acos(x)) -> x
        if (is_ex_the_function(x, acos))
            return t;
        // cos(asin(x)) -> (1-x^2)^(1/2)
        if (is_ex_the_function(x, asin))
            return power(exONE()-power(t,exTWO()),exHALF());
        // cos(atan(x)) -> (1+x^2)^(-1/2)
        if (is_ex_the_function(x, atan))
            return power(exONE()+power(t,exTWO()),exMINUSHALF());
    }
    
    // cos(float) -> float
    if (x.info(info_flags::numeric) && !x.info(info_flags::rational))
        return cos_evalf(x);
    
    return cos(x).hold();
}

static ex cos_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    return numMINUSONE()*sin(x);
}

REGISTER_FUNCTION(cos, cos_eval, cos_evalf, cos_diff, NULL);

//////////
// tangent (trigonometric function)
//////////

static ex tan_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(tan(x)) // -> numeric tan(numeric)
    
    return tan(ex_to_numeric(x));
}

static ex tan_eval(ex const & x)
{
    // tan(n*Pi/3) -> {0|3^(1/2)|-(3^(1/2))}
    ex ThreeExOverPi=numTHREE()*x/Pi;
    if (ThreeExOverPi.info(info_flags::integer)) {
        numeric z=mod(ex_to_numeric(ThreeExOverPi),numeric(3));
        if (z.is_equal(numZERO()))
            return exZERO();
        if (z.is_equal(numONE()))
            return power(exTHREE(),exHALF());
        if (z.is_equal(numTWO()))
            return -power(exTHREE(),exHALF());
    }
    
    // tan((2n+1)*Pi/2) -> throw
    ex ExOverPiMinusHalf=x/Pi-exHALF();
    if (ExOverPiMinusHalf.info(info_flags::integer))
        throw (std::domain_error("tan_eval(): infinity"));
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t=x.op(0);
        // tan(atan(x)) -> x
        if (is_ex_the_function(x, atan))
            return t;
        // tan(asin(x)) -> x*(1+x^2)^(-1/2)
        if (is_ex_the_function(x, asin))
            return t*power(exONE()-power(t,exTWO()),exMINUSHALF());
        // tan(acos(x)) -> (1-x^2)^(1/2)/x
        if (is_ex_the_function(x, acos))
            return power(t,exMINUSONE())*power(exONE()-power(t,exTWO()),exHALF());
    }
    
    // tan(float) -> float
    if (x.info(info_flags::numeric) && !x.info(info_flags::rational)) {
        return tan_evalf(x);
    }
    
    return tan(x).hold();
}

static ex tan_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return (1+power(tan(x),exTWO()));
}

REGISTER_FUNCTION(tan, tan_eval, tan_evalf, tan_diff, NULL);

//////////
// inverse sine (arc sine)
//////////

static ex asin_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(asin(x))
    
    return asin(ex_to_numeric(x)); // -> numeric asin(numeric)
}

static ex asin_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // asin(0) -> 0
        if (x.is_zero())
            return x;
        // asin(1/2) -> Pi/6
        if (x.is_equal(exHALF()))
            return numeric(1,6)*Pi;
        // asin(1) -> Pi/2
        if (x.is_equal(exONE()))
            return numeric(1,2)*Pi;
        // asin(-1/2) -> -Pi/6
        if (x.is_equal(exMINUSHALF()))
            return numeric(-1,6)*Pi;
        // asin(-1) -> -Pi/2
        if (x.is_equal(exMINUSONE()))
            return numeric(-1,2)*Pi;
        // asin(float) -> float
        if (!x.info(info_flags::rational))
            return asin_evalf(x);
    }
    
    return asin(x).hold();
}

static ex asin_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return power(1-power(x,exTWO()),exMINUSHALF());
}

REGISTER_FUNCTION(asin, asin_eval, asin_evalf, asin_diff, NULL);

//////////
// inverse cosine (arc cosine)
//////////

static ex acos_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(acos(x))
    
    return acos(ex_to_numeric(x)); // -> numeric acos(numeric)
}

static ex acos_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // acos(1) -> 0
        if (x.is_equal(exONE()))
            return exZERO();
        // acos(1/2) -> Pi/3
        if (x.is_equal(exHALF()))
            return numeric(1,3)*Pi;
        // acos(0) -> Pi/2
        if (x.is_zero())
            return numeric(1,2)*Pi;
        // acos(-1/2) -> 2/3*Pi
        if (x.is_equal(exMINUSHALF()))
            return numeric(2,3)*Pi;
        // acos(-1) -> Pi
        if (x.is_equal(exMINUSONE()))
            return Pi;
        // acos(float) -> float
        if (!x.info(info_flags::rational))
            return acos_evalf(x);
    }
    
    return acos(x).hold();
}

static ex acos_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return numMINUSONE()*power(1-power(x,exTWO()),exMINUSHALF());
}

REGISTER_FUNCTION(acos, acos_eval, acos_evalf, acos_diff, NULL);

//////////
// inverse tangent (arc tangent)
//////////

static ex atan_evalf(ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(atan(x))
    
    return atan(ex_to_numeric(x)); // -> numeric atan(numeric)
}

static ex atan_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // atan(0) -> 0
        if (x.is_equal(exZERO()))
            return exZERO();
        // atan(float) -> float
        if (!x.info(info_flags::rational))
            return atan_evalf(x);
    }
    
    return atan(x).hold();
}    

static ex atan_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    return power(1+x*x, -1);
}

REGISTER_FUNCTION(atan, atan_eval, atan_evalf, atan_diff, NULL);

//////////
// inverse tangent (atan2(y,x))
//////////

static ex atan2_evalf(ex const & y, ex const & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(y,numeric)
        TYPECHECK(x,numeric)
    END_TYPECHECK(atan2(y,x))
    
    return atan(ex_to_numeric(y),ex_to_numeric(x)); // -> numeric atan(numeric)
}

static ex atan2_eval(ex const & y, ex const & x)
{
    if (y.info(info_flags::numeric) && !y.info(info_flags::rational) &&
        x.info(info_flags::numeric) && !x.info(info_flags::rational)) {
        return atan2_evalf(y,x);
    }
    
    return atan2(y,x).hold();
}    

static ex atan2_diff(ex const & y, ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param<2);
    
    if (diff_param==0) {
        // d/dy atan(y,x)
        return x*pow(pow(x,2)+pow(y,2),-1);
    }
    // d/dx atan(y,x)
    return -y*pow(pow(x,2)+pow(y,2),-1);
}

REGISTER_FUNCTION(atan2, atan2_eval, atan2_evalf, atan2_diff, NULL);

//////////
// hyperbolic sine (trigonometric function)
//////////

static ex sinh_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(sinh(x))
    
    return sinh(ex_to_numeric(x)); // -> numeric sinh(numeric)
}

static ex sinh_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // sinh(0) -> 0
        if (x.is_zero())
            return exZERO();
        // sinh(float) -> float
        if (!x.info(info_flags::rational))
            return sinh_evalf(x);
    }
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t=x.op(0);
        // sinh(asinh(x)) -> x
        if (is_ex_the_function(x, asinh))
            return t;
        // sinh(acosh(x)) -> (x-1)^(1/2) * (x+1)^(1/2)
        if (is_ex_the_function(x, acosh))
            return power(t-exONE(),exHALF())*power(t+exONE(),exHALF());
        // sinh(atanh(x)) -> x*(1-x^2)^(-1/2)
        if (is_ex_the_function(x, atanh))
            return t*power(exONE()-power(t,exTWO()),exMINUSHALF());
    }
    
    return sinh(x).hold();
}

static ex sinh_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return cosh(x);
}

REGISTER_FUNCTION(sinh, sinh_eval, sinh_evalf, sinh_diff, NULL);

//////////
// hyperbolic cosine (trigonometric function)
//////////

static ex cosh_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(cosh(x))
    
    return cosh(ex_to_numeric(x)); // -> numeric cosh(numeric)
}

static ex cosh_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // cosh(0) -> 1
        if (x.is_zero())
            return exONE();
        // cosh(float) -> float
        if (!x.info(info_flags::rational))
            return cosh_evalf(x);
    }
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t=x.op(0);
        // cosh(acosh(x)) -> x
        if (is_ex_the_function(x, acosh))
            return t;
        // cosh(asinh(x)) -> (1+x^2)^(1/2)
        if (is_ex_the_function(x, asinh))
            return power(exONE()+power(t,exTWO()),exHALF());
        // cosh(atanh(x)) -> (1-x^2)^(-1/2)
        if (is_ex_the_function(x, atanh))
            return power(exONE()-power(t,exTWO()),exMINUSHALF());
    }
    
    return cosh(x).hold();
}

static ex cosh_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return sinh(x);
}

REGISTER_FUNCTION(cosh, cosh_eval, cosh_evalf, cosh_diff, NULL);

//////////
// hyperbolic tangent (trigonometric function)
//////////

static ex tanh_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(tanh(x))
    
    return tanh(ex_to_numeric(x)); // -> numeric tanh(numeric)
}

static ex tanh_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // tanh(0) -> 0
        if (x.is_zero())
            return exZERO();
        // tanh(float) -> float
        if (!x.info(info_flags::rational))
            return tanh_evalf(x);
    }
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t=x.op(0);
        // tanh(atanh(x)) -> x
        if (is_ex_the_function(x, atanh))
            return t;
        // tanh(asinh(x)) -> x*(1+x^2)^(-1/2)
        if (is_ex_the_function(x, asinh))
            return t*power(exONE()+power(t,exTWO()),exMINUSHALF());
        // tanh(acosh(x)) -> (x-1)^(1/2)*(x+1)^(1/2)/x
        if (is_ex_the_function(x, acosh))
            return power(t-exONE(),exHALF())*power(t+exONE(),exHALF())*power(t,exMINUSONE());
    }
    
    return tanh(x).hold();
}

static ex tanh_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return exONE()-power(tanh(x),exTWO());
}

REGISTER_FUNCTION(tanh, tanh_eval, tanh_evalf, tanh_diff, NULL);

//////////
// inverse hyperbolic sine (trigonometric function)
//////////

static ex asinh_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(asinh(x))
    
    return asinh(ex_to_numeric(x)); // -> numeric asinh(numeric)
}

static ex asinh_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // asinh(0) -> 0
        if (x.is_zero())
            return exZERO();
        // asinh(float) -> float
        if (!x.info(info_flags::rational))
            return asinh_evalf(x);
    }
    
    return asinh(x).hold();
}

static ex asinh_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return power(1+power(x,exTWO()),exMINUSHALF());
}

REGISTER_FUNCTION(asinh, asinh_eval, asinh_evalf, asinh_diff, NULL);

//////////
// inverse hyperbolic cosine (trigonometric function)
//////////

static ex acosh_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(acosh(x))
    
    return acosh(ex_to_numeric(x)); // -> numeric acosh(numeric)
}

static ex acosh_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // acosh(0) -> Pi*I/2
        if (x.is_zero())
            return Pi*I*numeric(1,2);
        // acosh(1) -> 0
        if (x.is_equal(exONE()))
            return exZERO();
        // acosh(-1) -> Pi*I
        if (x.is_equal(exMINUSONE()))
            return Pi*I;
        // acosh(float) -> float
        if (!x.info(info_flags::rational))
            return acosh_evalf(x);
    }
    
    return acosh(x).hold();
}

static ex acosh_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return power(x-1,exMINUSHALF())*power(x+1,exMINUSHALF());
}

REGISTER_FUNCTION(acosh, acosh_eval, acosh_evalf, acosh_diff, NULL);

//////////
// inverse hyperbolic tangent (trigonometric function)
//////////

static ex atanh_evalf(ex const & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(atanh(x))
    
    return atanh(ex_to_numeric(x)); // -> numeric atanh(numeric)
}

static ex atanh_eval(ex const & x)
{
    if (x.info(info_flags::numeric)) {
        // atanh(0) -> 0
        if (x.is_zero())
            return exZERO();
        // atanh({+|-}1) -> throw
        if (x.is_equal(exONE()) || x.is_equal(exONE()))
            throw (std::domain_error("atanh_eval(): infinity"));
        // atanh(float) -> float
        if (!x.info(info_flags::rational))
            return atanh_evalf(x);
    }
    
    return atanh(x).hold();
}

static ex atanh_diff(ex const & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    return power(exONE()-power(x,exTWO()),exMINUSONE());
}

REGISTER_FUNCTION(atanh, atanh_eval, atanh_evalf, atanh_diff, NULL);

} // namespace GiNaC
