/** @file inifcns_trans.cpp
 *
 *  Implementation of transcendental (and trigonometric and hyperbolic)
 *  functions. */

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
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "symbol.h"
#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

//////////
// exponential function
//////////

static ex exp_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(exp(x))
    
    return exp(ex_to_numeric(x)); // -> numeric exp(numeric)
}

static ex exp_eval(const ex & x)
{
    // exp(0) -> 1
    if (x.is_zero()) {
        return _ex1();
    }
    // exp(n*Pi*I/2) -> {+1|+I|-1|-I}
    ex TwoExOverPiI=(_ex2()*x)/(Pi*I);
    if (TwoExOverPiI.info(info_flags::integer)) {
        numeric z=mod(ex_to_numeric(TwoExOverPiI),_num4());
        if (z.is_equal(_num0()))
            return _ex1();
        if (z.is_equal(_num1()))
            return ex(I);
        if (z.is_equal(_num2()))
            return _ex_1();
        if (z.is_equal(_num3()))
            return ex(-I);
    }
    // exp(log(x)) -> x
    if (is_ex_the_function(x, log))
        return x.op(0);
    
    // exp(float)
    if (x.info(info_flags::numeric) && !x.info(info_flags::crational))
        return exp_evalf(x);
    
    return exp(x).hold();
}

static ex exp_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    // d/dx exp(x) -> exp(x)
    return exp(x);
}

REGISTER_FUNCTION(exp, exp_eval, exp_evalf, exp_diff, NULL);

//////////
// natural logarithm
//////////

static ex log_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(log(x))
    
    return log(ex_to_numeric(x)); // -> numeric log(numeric)
}

static ex log_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        if (x.is_equal(_ex1()))  // log(1) -> 0
            return _ex0();
        if (x.is_equal(_ex_1())) // log(-1) -> I*Pi
            return (I*Pi);        
        if (x.is_equal(I))       // log(I) -> Pi*I/2
            return (Pi*I*_num1_2());
        if (x.is_equal(-I))      // log(-I) -> -Pi*I/2
            return (Pi*I*_num_1_2());
        if (x.is_equal(_ex0()))  // log(0) -> infinity
            throw(std::domain_error("log_eval(): log(0)"));
        // log(float)
        if (!x.info(info_flags::crational))
            return log_evalf(x);
    }
    // log(exp(t)) -> t (if -Pi < t.imag() <= Pi):
    if (is_ex_the_function(x, exp)) {
        ex t = x.op(0);
        if (t.info(info_flags::numeric)) {
            numeric nt = ex_to_numeric(t);
            if (nt.is_real())
                return t;
        }
    }
    
    return log(x).hold();
}

static ex log_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    // d/dx log(x) -> 1/x
    return power(x, _ex_1());
}

REGISTER_FUNCTION(log, log_eval, log_evalf, log_diff, NULL);

//////////
// sine (trigonometric function)
//////////

static ex sin_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(sin(x))
    
    return sin(ex_to_numeric(x)); // -> numeric sin(numeric)
}

static ex sin_eval(const ex & x)
{
    // sin(n/d*Pi) -> { all known non-nested radicals }
    ex SixtyExOverPi = _ex60()*x/Pi;
    ex sign = _ex1();
    if (SixtyExOverPi.info(info_flags::integer)) {
        numeric z = mod(ex_to_numeric(SixtyExOverPi),_num120());
        if (z>=_num60()) {
            // wrap to interval [0, Pi)
            z -= _num60();
            sign = _ex_1();
        }
        if (z>_num30()) {
            // wrap to interval [0, Pi/2)
            z = _num60()-z;
        }
        if (z.is_equal(_num0()))  // sin(0)       -> 0
            return _ex0();
        if (z.is_equal(_num5()))  // sin(Pi/12)   -> sqrt(6)/4*(1-sqrt(3)/3)
            return sign*_ex1_4()*power(_ex6(),_ex1_2())*(_ex1()+_ex_1_3()*power(_ex3(),_ex1_2()));
        if (z.is_equal(_num6()))  // sin(Pi/10)   -> sqrt(5)/4-1/4
            return sign*(_ex1_4()*power(_ex5(),_ex1_2())+_ex_1_4());
        if (z.is_equal(_num10())) // sin(Pi/6)    -> 1/2
            return sign*_ex1_2();
        if (z.is_equal(_num15())) // sin(Pi/4)    -> sqrt(2)/2
            return sign*_ex1_2()*power(_ex2(),_ex1_2());
        if (z.is_equal(_num18())) // sin(3/10*Pi) -> sqrt(5)/4+1/4
            return sign*(_ex1_4()*power(_ex5(),_ex1_2())+_ex1_4());
        if (z.is_equal(_num20())) // sin(Pi/3)    -> sqrt(3)/2
            return sign*_ex1_2()*power(_ex3(),_ex1_2());
        if (z.is_equal(_num25())) // sin(5/12*Pi) -> sqrt(6)/4*(1+sqrt(3)/3)
            return sign*_ex1_4()*power(_ex6(),_ex1_2())*(_ex1()+_ex1_3()*power(_ex3(),_ex1_2()));
        if (z.is_equal(_num30())) // sin(Pi/2)    -> 1
            return sign*_ex1();
    }
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t = x.op(0);
        // sin(asin(x)) -> x
        if (is_ex_the_function(x, asin))
            return t;
        // sin(acos(x)) -> sqrt(1-x^2)
        if (is_ex_the_function(x, acos))
            return power(_ex1()-power(t,_ex2()),_ex1_2());
        // sin(atan(x)) -> x*(1+x^2)^(-1/2)
        if (is_ex_the_function(x, atan))
            return t*power(_ex1()+power(t,_ex2()),_ex_1_2());
    }
    
    // sin(float) -> float
    if (x.info(info_flags::numeric) && !x.info(info_flags::crational))
        return sin_evalf(x);
    
    return sin(x).hold();
}

static ex sin_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx sin(x) -> cos(x)
    return cos(x);
}

REGISTER_FUNCTION(sin, sin_eval, sin_evalf, sin_diff, NULL);

//////////
// cosine (trigonometric function)
//////////

static ex cos_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(cos(x))
    
    return cos(ex_to_numeric(x)); // -> numeric cos(numeric)
}

static ex cos_eval(const ex & x)
{
    // cos(n/d*Pi) -> { all known non-nested radicals }
    ex SixtyExOverPi = _ex60()*x/Pi;
    ex sign = _ex1();
    if (SixtyExOverPi.info(info_flags::integer)) {
        numeric z = mod(ex_to_numeric(SixtyExOverPi),_num120());
        if (z>=_num60()) {
            // wrap to interval [0, Pi)
            z = _num120()-z;
        }
        if (z>=_num30()) {
            // wrap to interval [0, Pi/2)
            z = _num60()-z;
            sign = _ex_1();
        }
        if (z.is_equal(_num0()))  // cos(0)       -> 1
            return sign*_ex1();
        if (z.is_equal(_num5()))  // cos(Pi/12)   -> sqrt(6)/4*(1+sqrt(3)/3)
            return sign*_ex1_4()*power(_ex6(),_ex1_2())*(_ex1()+_ex1_3()*power(_ex3(),_ex1_2()));
        if (z.is_equal(_num10())) // cos(Pi/6)    -> sqrt(3)/2
            return sign*_ex1_2()*power(_ex3(),_ex1_2());
        if (z.is_equal(_num12())) // cos(Pi/5)    -> sqrt(5)/4+1/4
            return sign*(_ex1_4()*power(_ex5(),_ex1_2())+_ex1_4());
        if (z.is_equal(_num15())) // cos(Pi/4)    -> sqrt(2)/2
            return sign*_ex1_2()*power(_ex2(),_ex1_2());
        if (z.is_equal(_num20())) // cos(Pi/3)    -> 1/2
            return sign*_ex1_2();
        if (z.is_equal(_num24())) // cos(2/5*Pi)  -> sqrt(5)/4-1/4x
            return sign*(_ex1_4()*power(_ex5(),_ex1_2())+_ex_1_4());
        if (z.is_equal(_num25())) // cos(5/12*Pi) -> sqrt(6)/4*(1-sqrt(3)/3)
            return sign*_ex1_4()*power(_ex6(),_ex1_2())*(_ex1()+_ex_1_3()*power(_ex3(),_ex1_2()));
        if (z.is_equal(_num30())) // cos(Pi/2)    -> 0
            return sign*_ex0();
    }
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t = x.op(0);
        // cos(acos(x)) -> x
        if (is_ex_the_function(x, acos))
            return t;
        // cos(asin(x)) -> (1-x^2)^(1/2)
        if (is_ex_the_function(x, asin))
            return power(_ex1()-power(t,_ex2()),_ex1_2());
        // cos(atan(x)) -> (1+x^2)^(-1/2)
        if (is_ex_the_function(x, atan))
            return power(_ex1()+power(t,_ex2()),_ex_1_2());
    }
    
    // cos(float) -> float
    if (x.info(info_flags::numeric) && !x.info(info_flags::crational))
        return cos_evalf(x);
    
    return cos(x).hold();
}

static ex cos_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    // d/dx cos(x) -> -sin(x)
    return _ex_1()*sin(x);
}

REGISTER_FUNCTION(cos, cos_eval, cos_evalf, cos_diff, NULL);

//////////
// tangent (trigonometric function)
//////////

static ex tan_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(tan(x)) // -> numeric tan(numeric)
    
    return tan(ex_to_numeric(x));
}

static ex tan_eval(const ex & x)
{
    // tan(n/d*Pi) -> { all known non-nested radicals }
    ex SixtyExOverPi = _ex60()*x/Pi;
    ex sign = _ex1();
    if (SixtyExOverPi.info(info_flags::integer)) {
        numeric z = mod(ex_to_numeric(SixtyExOverPi),_num60());
        if (z>=_num60()) {
            // wrap to interval [0, Pi)
            z -= _num60();
        }
        if (z>=_num30()) {
            // wrap to interval [0, Pi/2)
            z = _num60()-z;
            sign = _ex_1();
        }
        if (z.is_equal(_num0()))  // tan(0)       -> 0
            return _ex0();
        if (z.is_equal(_num5()))  // tan(Pi/12)   -> 2-sqrt(3)
            return sign*(_ex2()-power(_ex3(),_ex1_2()));
        if (z.is_equal(_num10())) // tan(Pi/6)    -> sqrt(3)/3
            return sign*_ex1_3()*power(_ex3(),_ex1_2());
        if (z.is_equal(_num15())) // tan(Pi/4)    -> 1
            return sign*_ex1();
        if (z.is_equal(_num20())) // tan(Pi/3)    -> sqrt(3)
            return sign*power(_ex3(),_ex1_2());
        if (z.is_equal(_num25())) // tan(5/12*Pi) -> 2+sqrt(3)
            return sign*(power(_ex3(),_ex1_2())+_ex2());
        if (z.is_equal(_num30())) // tan(Pi/2)    -> infinity
            throw (std::domain_error("tan_eval(): infinity"));
    }
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t = x.op(0);
        // tan(atan(x)) -> x
        if (is_ex_the_function(x, atan))
            return t;
        // tan(asin(x)) -> x*(1+x^2)^(-1/2)
        if (is_ex_the_function(x, asin))
            return t*power(_ex1()-power(t,_ex2()),_ex_1_2());
        // tan(acos(x)) -> (1-x^2)^(1/2)/x
        if (is_ex_the_function(x, acos))
            return power(t,_ex_1())*power(_ex1()-power(t,_ex2()),_ex1_2());
    }
    
    // tan(float) -> float
    if (x.info(info_flags::numeric) && !x.info(info_flags::crational)) {
        return tan_evalf(x);
    }
    
    return tan(x).hold();
}

static ex tan_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx tan(x) -> 1+tan(x)^2;
    return (_ex1()+power(tan(x),_ex2()));
}

static ex tan_series(const ex & x, const symbol & s, const ex & pt, int order)
{
    // method:
    // Taylor series where there is no pole falls back to tan_diff.
    // On a pole simply expand sin(x)/cos(x).
    const ex x_pt = x.subs(s==pt);
    if (!(2*x_pt/Pi).info(info_flags::odd))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole
    return (sin(x)/cos(x)).series(s, pt, order+2);
}

REGISTER_FUNCTION(tan, tan_eval, tan_evalf, tan_diff, tan_series);

//////////
// inverse sine (arc sine)
//////////

static ex asin_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(asin(x))
    
    return asin(ex_to_numeric(x)); // -> numeric asin(numeric)
}

static ex asin_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // asin(0) -> 0
        if (x.is_zero())
            return x;
        // asin(1/2) -> Pi/6
        if (x.is_equal(_ex1_2()))
            return numeric(1,6)*Pi;
        // asin(1) -> Pi/2
        if (x.is_equal(_ex1()))
            return _num1_2()*Pi;
        // asin(-1/2) -> -Pi/6
        if (x.is_equal(_ex_1_2()))
            return numeric(-1,6)*Pi;
        // asin(-1) -> -Pi/2
        if (x.is_equal(_ex_1()))
            return _num_1_2()*Pi;
        // asin(float) -> float
        if (!x.info(info_flags::crational))
            return asin_evalf(x);
    }
    
    return asin(x).hold();
}

static ex asin_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx asin(x) -> 1/sqrt(1-x^2)
    return power(1-power(x,_ex2()),_ex_1_2());
}

REGISTER_FUNCTION(asin, asin_eval, asin_evalf, asin_diff, NULL);

//////////
// inverse cosine (arc cosine)
//////////

static ex acos_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(acos(x))
    
    return acos(ex_to_numeric(x)); // -> numeric acos(numeric)
}

static ex acos_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // acos(1) -> 0
        if (x.is_equal(_ex1()))
            return _ex0();
        // acos(1/2) -> Pi/3
        if (x.is_equal(_ex1_2()))
            return _ex1_3()*Pi;
        // acos(0) -> Pi/2
        if (x.is_zero())
            return _ex1_2()*Pi;
        // acos(-1/2) -> 2/3*Pi
        if (x.is_equal(_ex_1_2()))
            return numeric(2,3)*Pi;
        // acos(-1) -> Pi
        if (x.is_equal(_ex_1()))
            return Pi;
        // acos(float) -> float
        if (!x.info(info_flags::crational))
            return acos_evalf(x);
    }
    
    return acos(x).hold();
}

static ex acos_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx acos(x) -> -1/sqrt(1-x^2)
    return _ex_1()*power(1-power(x,_ex2()),_ex_1_2());
}

REGISTER_FUNCTION(acos, acos_eval, acos_evalf, acos_diff, NULL);

//////////
// inverse tangent (arc tangent)
//////////

static ex atan_evalf(const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(x,numeric)
    END_TYPECHECK(atan(x))
    
    return atan(ex_to_numeric(x)); // -> numeric atan(numeric)
}

static ex atan_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // atan(0) -> 0
        if (x.is_equal(_ex0()))
            return _ex0();
        // atan(float) -> float
        if (!x.info(info_flags::crational))
            return atan_evalf(x);
    }
    
    return atan(x).hold();
}    

static ex atan_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);

    // d/dx atan(x) -> 1/(1+x^2)
    return power(_ex1()+power(x,_ex2()), _ex_1());
}

REGISTER_FUNCTION(atan, atan_eval, atan_evalf, atan_diff, NULL);

//////////
// inverse tangent (atan2(y,x))
//////////

static ex atan2_evalf(const ex & y, const ex & x)
{
    BEGIN_TYPECHECK
        TYPECHECK(y,numeric)
        TYPECHECK(x,numeric)
    END_TYPECHECK(atan2(y,x))
    
    return atan(ex_to_numeric(y),ex_to_numeric(x)); // -> numeric atan(numeric)
}

static ex atan2_eval(const ex & y, const ex & x)
{
    if (y.info(info_flags::numeric) && !y.info(info_flags::crational) &&
        x.info(info_flags::numeric) && !x.info(info_flags::crational)) {
        return atan2_evalf(y,x);
    }
    
    return atan2(y,x).hold();
}    

static ex atan2_diff(const ex & y, const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param<2);
    
    if (diff_param==0) {
        // d/dy atan(y,x)
        return x*power(power(x,_ex2())+power(y,_ex2()),_ex_1());
    }
    // d/dx atan(y,x)
    return -y*power(power(x,_ex2())+power(y,_ex2()),_ex_1());
}

REGISTER_FUNCTION(atan2, atan2_eval, atan2_evalf, atan2_diff, NULL);

//////////
// hyperbolic sine (trigonometric function)
//////////

static ex sinh_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(sinh(x))
    
    return sinh(ex_to_numeric(x)); // -> numeric sinh(numeric)
}

static ex sinh_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        if (x.is_zero())  // sinh(0) -> 0
            return _ex0();        
        if (!x.info(info_flags::crational))  // sinh(float) -> float
            return sinh_evalf(x);
    }
    
    if ((x/Pi).info(info_flags::numeric) &&
        ex_to_numeric(x/Pi).real().is_zero())  // sinh(I*x) -> I*sin(x)
        return I*sin(x/I);
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t = x.op(0);
        // sinh(asinh(x)) -> x
        if (is_ex_the_function(x, asinh))
            return t;
        // sinh(acosh(x)) -> (x-1)^(1/2) * (x+1)^(1/2)
        if (is_ex_the_function(x, acosh))
            return power(t-_ex1(),_ex1_2())*power(t+_ex1(),_ex1_2());
        // sinh(atanh(x)) -> x*(1-x^2)^(-1/2)
        if (is_ex_the_function(x, atanh))
            return t*power(_ex1()-power(t,_ex2()),_ex_1_2());
    }
    
    return sinh(x).hold();
}

static ex sinh_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx sinh(x) -> cosh(x)
    return cosh(x);
}

REGISTER_FUNCTION(sinh, sinh_eval, sinh_evalf, sinh_diff, NULL);

//////////
// hyperbolic cosine (trigonometric function)
//////////

static ex cosh_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(cosh(x))
    
    return cosh(ex_to_numeric(x)); // -> numeric cosh(numeric)
}

static ex cosh_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        if (x.is_zero())  // cosh(0) -> 1
            return _ex1();
        if (!x.info(info_flags::crational))  // cosh(float) -> float
            return cosh_evalf(x);
    }
    
    if ((x/Pi).info(info_flags::numeric) &&
        ex_to_numeric(x/Pi).real().is_zero())  // cosh(I*x) -> cos(x)
        return cos(x/I);
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t = x.op(0);
        // cosh(acosh(x)) -> x
        if (is_ex_the_function(x, acosh))
            return t;
        // cosh(asinh(x)) -> (1+x^2)^(1/2)
        if (is_ex_the_function(x, asinh))
            return power(_ex1()+power(t,_ex2()),_ex1_2());
        // cosh(atanh(x)) -> (1-x^2)^(-1/2)
        if (is_ex_the_function(x, atanh))
            return power(_ex1()-power(t,_ex2()),_ex_1_2());
    }
    
    return cosh(x).hold();
}

static ex cosh_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx cosh(x) -> sinh(x)
    return sinh(x);
}

REGISTER_FUNCTION(cosh, cosh_eval, cosh_evalf, cosh_diff, NULL);

//////////
// hyperbolic tangent (trigonometric function)
//////////

static ex tanh_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(tanh(x))
    
    return tanh(ex_to_numeric(x)); // -> numeric tanh(numeric)
}

static ex tanh_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        if (x.is_zero())  // tanh(0) -> 0
            return _ex0();
        if (!x.info(info_flags::crational))  // tanh(float) -> float
            return tanh_evalf(x);
    }
    
    if ((x/Pi).info(info_flags::numeric) &&
        ex_to_numeric(x/Pi).real().is_zero())  // tanh(I*x) -> I*tan(x);
        return I*tan(x/I);
    
    if (is_ex_exactly_of_type(x, function)) {
        ex t = x.op(0);
        // tanh(atanh(x)) -> x
        if (is_ex_the_function(x, atanh))
            return t;
        // tanh(asinh(x)) -> x*(1+x^2)^(-1/2)
        if (is_ex_the_function(x, asinh))
            return t*power(_ex1()+power(t,_ex2()),_ex_1_2());
        // tanh(acosh(x)) -> (x-1)^(1/2)*(x+1)^(1/2)/x
        if (is_ex_the_function(x, acosh))
            return power(t-_ex1(),_ex1_2())*power(t+_ex1(),_ex1_2())*power(t,_ex_1());
    }
    
    return tanh(x).hold();
}

static ex tanh_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx tanh(x) -> 1-tanh(x)^2
    return _ex1()-power(tanh(x),_ex2());
}

static ex tanh_series(const ex & x, const symbol & s, const ex & pt, int order)
{
    // method:
    // Taylor series where there is no pole falls back to tanh_diff.
    // On a pole simply expand sinh(x)/cosh(x).
    const ex x_pt = x.subs(s==pt);
    if (!(2*I*x_pt/Pi).info(info_flags::odd))
        throw do_taylor();  // caught by function::series()
    // if we got here we have to care for a simple pole
    return (sinh(x)/cosh(x)).series(s, pt, order+2);
}

REGISTER_FUNCTION(tanh, tanh_eval, tanh_evalf, tanh_diff, tanh_series);

//////////
// inverse hyperbolic sine (trigonometric function)
//////////

static ex asinh_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(asinh(x))
    
    return asinh(ex_to_numeric(x)); // -> numeric asinh(numeric)
}

static ex asinh_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // asinh(0) -> 0
        if (x.is_zero())
            return _ex0();
        // asinh(float) -> float
        if (!x.info(info_flags::crational))
            return asinh_evalf(x);
    }
    
    return asinh(x).hold();
}

static ex asinh_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx asinh(x) -> 1/sqrt(1+x^2)
    return power(_ex1()+power(x,_ex2()),_ex_1_2());
}

REGISTER_FUNCTION(asinh, asinh_eval, asinh_evalf, asinh_diff, NULL);

//////////
// inverse hyperbolic cosine (trigonometric function)
//////////

static ex acosh_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(acosh(x))
    
    return acosh(ex_to_numeric(x)); // -> numeric acosh(numeric)
}

static ex acosh_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // acosh(0) -> Pi*I/2
        if (x.is_zero())
            return Pi*I*numeric(1,2);
        // acosh(1) -> 0
        if (x.is_equal(_ex1()))
            return _ex0();
        // acosh(-1) -> Pi*I
        if (x.is_equal(_ex_1()))
            return Pi*I;
        // acosh(float) -> float
        if (!x.info(info_flags::crational))
            return acosh_evalf(x);
    }
    
    return acosh(x).hold();
}

static ex acosh_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx acosh(x) -> 1/(sqrt(x-1)*sqrt(x+1))
    return power(x+_ex_1(),_ex_1_2())*power(x+_ex1(),_ex_1_2());
}

REGISTER_FUNCTION(acosh, acosh_eval, acosh_evalf, acosh_diff, NULL);

//////////
// inverse hyperbolic tangent (trigonometric function)
//////////

static ex atanh_evalf(const ex & x)
{
    BEGIN_TYPECHECK
       TYPECHECK(x,numeric)
    END_TYPECHECK(atanh(x))
    
    return atanh(ex_to_numeric(x)); // -> numeric atanh(numeric)
}

static ex atanh_eval(const ex & x)
{
    if (x.info(info_flags::numeric)) {
        // atanh(0) -> 0
        if (x.is_zero())
            return _ex0();
        // atanh({+|-}1) -> throw
        if (x.is_equal(_ex1()) || x.is_equal(_ex1()))
            throw (std::domain_error("atanh_eval(): infinity"));
        // atanh(float) -> float
        if (!x.info(info_flags::crational))
            return atanh_evalf(x);
    }
    
    return atanh(x).hold();
}

static ex atanh_diff(const ex & x, unsigned diff_param)
{
    GINAC_ASSERT(diff_param==0);
    
    // d/dx atanh(x) -> 1/(1-x^2)
    return power(_ex1()-power(x,_ex2()),_ex_1());
}

REGISTER_FUNCTION(atanh, atanh_eval, atanh_evalf, atanh_diff, NULL);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
