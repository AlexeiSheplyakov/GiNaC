/** @file inifcns_trans.cpp
 *
 *  Implementation of transcendental (and trigonometric and hyperbolic)
 *  functions. */

/*
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
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
#include "pseries.h"
#include "utils.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

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

static ex exp_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);

	// d/dx exp(x) -> exp(x)
	return exp(x);
}

REGISTER_FUNCTION(exp, eval_func(exp_eval).
                       evalf_func(exp_evalf).
                       derivative_func(exp_deriv));

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
		if (x.is_equal(_ex0()))  // log(0) -> infinity
			throw(pole_error("log_eval(): log(0)",0));
		if (x.info(info_flags::real) && x.info(info_flags::negative))
			return (log(-x)+I*Pi);
		if (x.is_equal(_ex1()))  // log(1) -> 0
			return _ex0();
		if (x.is_equal(I))       // log(I) -> Pi*I/2
			return (Pi*I*_num1_2());
		if (x.is_equal(-I))      // log(-I) -> -Pi*I/2
			return (Pi*I*_num_1_2());
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

static ex log_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx log(x) -> 1/x
	return power(x, _ex_1());
}

static ex log_series(const ex &arg,
                     const relational &rel,
                     int order,
                     unsigned options)
{
	GINAC_ASSERT(is_ex_exactly_of_type(rel.lhs(),symbol));
	ex arg_pt;
	bool must_expand_arg = false;
	// maybe substitution of rel into arg fails because of a pole
	try {
		arg_pt = arg.subs(rel);
	} catch (pole_error) {
		must_expand_arg = true;
	}
	// or we are at the branch cut anyways
	if (arg_pt.is_zero())
		must_expand_arg = true;
	
	if (must_expand_arg) {
		// method:
		// This is the branch point: Series expand the argument first, then
		// trivially factorize it to isolate that part which has constant
		// leading coefficient in this fashion:
		//   x^n + Order(x^(n+m))  ->  x^n * (1 + Order(x^m)).
		// Return a plain n*log(x) for the x^n part and series expand the
		// other part.  Add them together and reexpand again in order to have
		// one unnested pseries object.  All this also works for negative n.
		const pseries argser = ex_to_pseries(arg.series(rel, order, options));
		const symbol *s = static_cast<symbol *>(rel.lhs().bp);
		const ex point = rel.rhs();
		const int n = argser.ldegree(*s);
		epvector seq;
		seq.push_back(expair(n*log(*s-point), _ex0()));
		if (!argser.is_terminating() || argser.nops()!=1) {
			// in this case n more terms are needed
			ex newarg = ex_to_pseries(arg.series(rel, order+n, options)).shift_exponents(-n).convert_to_poly(true);
			return pseries(rel, seq).add_series(ex_to_pseries(log(newarg).series(rel, order, options)));
		} else  // it was a monomial
			return pseries(rel, seq);
	}
	if (!(options & series_options::suppress_branchcut) &&
		 arg_pt.info(info_flags::negative)) {
		// method:
		// This is the branch cut: assemble the primitive series manually and
		// then add the corresponding complex step function.
		const symbol *s = static_cast<symbol *>(rel.lhs().bp);
		const ex point = rel.rhs();
		const symbol foo;
		ex replarg = series(log(arg), *s==foo, order, false).subs(foo==point);
		epvector seq;
		seq.push_back(expair(-I*csgn(arg*I)*Pi, _ex0()));
		seq.push_back(expair(Order(_ex1()), order));
		return series(replarg - I*Pi + pseries(rel, seq), rel, order);
	}
	throw do_taylor();  // caught by function::series()
}

REGISTER_FUNCTION(log, eval_func(log_eval).
                       evalf_func(log_evalf).
                       derivative_func(log_deriv).
                       series_func(log_series));

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

static ex sin_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx sin(x) -> cos(x)
	return cos(x);
}

REGISTER_FUNCTION(sin, eval_func(sin_eval).
                       evalf_func(sin_evalf).
                       derivative_func(sin_deriv));

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

static ex cos_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);

	// d/dx cos(x) -> -sin(x)
	return _ex_1()*sin(x);
}

REGISTER_FUNCTION(cos, eval_func(cos_eval).
                       evalf_func(cos_evalf).
                       derivative_func(cos_deriv));

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
			throw (pole_error("tan_eval(): simple pole",1));
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

static ex tan_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx tan(x) -> 1+tan(x)^2;
	return (_ex1()+power(tan(x),_ex2()));
}

static ex tan_series(const ex &x,
                     const relational &rel,
                     int order,
                     unsigned options)
{
	GINAC_ASSERT(is_ex_exactly_of_type(rel.lhs(),symbol));
	// method:
	// Taylor series where there is no pole falls back to tan_deriv.
	// On a pole simply expand sin(x)/cos(x).
	const ex x_pt = x.subs(rel);
	if (!(2*x_pt/Pi).info(info_flags::odd))
		throw do_taylor();  // caught by function::series()
	// if we got here we have to care for a simple pole
	return (sin(x)/cos(x)).series(rel, order+2, options);
}

REGISTER_FUNCTION(tan, eval_func(tan_eval).
                       evalf_func(tan_evalf).
                       derivative_func(tan_deriv).
                       series_func(tan_series));

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

static ex asin_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx asin(x) -> 1/sqrt(1-x^2)
	return power(1-power(x,_ex2()),_ex_1_2());
}

REGISTER_FUNCTION(asin, eval_func(asin_eval).
                        evalf_func(asin_evalf).
                        derivative_func(asin_deriv));

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

static ex acos_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx acos(x) -> -1/sqrt(1-x^2)
	return _ex_1()*power(1-power(x,_ex2()),_ex_1_2());
}

REGISTER_FUNCTION(acos, eval_func(acos_eval).
                        evalf_func(acos_evalf).
                        derivative_func(acos_deriv));

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
		// atan(1) -> Pi/4
		if (x.is_equal(_ex1()))
			return _ex1_4()*Pi;
		// atan(-1) -> -Pi/4
		if (x.is_equal(_ex_1()))
			return _ex_1_4()*Pi;
		if (x.is_equal(I) || x.is_equal(-I))
			throw (pole_error("atan_eval(): logarithmic pole",0));
		// atan(float) -> float
		if (!x.info(info_flags::crational))
			return atan_evalf(x);
	}
	
	return atan(x).hold();
}

static ex atan_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);

	// d/dx atan(x) -> 1/(1+x^2)
	return power(_ex1()+power(x,_ex2()), _ex_1());
}

static ex atan_series(const ex &x,
                      const relational &rel,
                      int order,
                      unsigned options)
{
	GINAC_ASSERT(is_ex_exactly_of_type(rel.lhs(),symbol));
	// method:
	// Taylor series where there is no pole or cut falls back to atan_deriv.
	// There are two branch cuts, one runnig from I up the imaginary axis and
	// one running from -I down the imaginary axis.  The points I and -I are
	// poles.
	// On the branch cuts and the poles series expand
	//     log((1+I*x)/(1-I*x))/(2*I)
	// instead.
	// (The constant term on the cut itself could be made simpler.)
	const ex x_pt = x.subs(rel);
	if (!(I*x_pt).info(info_flags::real))
		throw do_taylor();     // Re(x) != 0
	if ((I*x_pt).info(info_flags::real) && abs(I*x_pt)<_ex1())
		throw do_taylor();     // Re(x) == 0, but abs(x)<1
	// if we got here we have to care for cuts and poles
	return (log((1+I*x)/(1-I*x))/(2*I)).series(rel, order, options);
}

REGISTER_FUNCTION(atan, eval_func(atan_eval).
                        evalf_func(atan_evalf).
                        derivative_func(atan_deriv).
                        series_func(atan_series));

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

static ex atan2_deriv(const ex & y, const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param<2);
	
	if (deriv_param==0) {
		// d/dy atan(y,x)
		return x*power(power(x,_ex2())+power(y,_ex2()),_ex_1());
	}
	// d/dx atan(y,x)
	return -y*power(power(x,_ex2())+power(y,_ex2()),_ex_1());
}

REGISTER_FUNCTION(atan2, eval_func(atan2_eval).
                         evalf_func(atan2_evalf).
                         derivative_func(atan2_deriv));

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

static ex sinh_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx sinh(x) -> cosh(x)
	return cosh(x);
}

REGISTER_FUNCTION(sinh, eval_func(sinh_eval).
                        evalf_func(sinh_evalf).
                        derivative_func(sinh_deriv));

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

static ex cosh_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx cosh(x) -> sinh(x)
	return sinh(x);
}

REGISTER_FUNCTION(cosh, eval_func(cosh_eval).
                        evalf_func(cosh_evalf).
                        derivative_func(cosh_deriv));


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

static ex tanh_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx tanh(x) -> 1-tanh(x)^2
	return _ex1()-power(tanh(x),_ex2());
}

static ex tanh_series(const ex &x,
                      const relational &rel,
                      int order,
                      unsigned options)
{
	GINAC_ASSERT(is_ex_exactly_of_type(rel.lhs(),symbol));
	// method:
	// Taylor series where there is no pole falls back to tanh_deriv.
	// On a pole simply expand sinh(x)/cosh(x).
	const ex x_pt = x.subs(rel);
	if (!(2*I*x_pt/Pi).info(info_flags::odd))
		throw do_taylor();  // caught by function::series()
	// if we got here we have to care for a simple pole
	return (sinh(x)/cosh(x)).series(rel, order+2, options);
}

REGISTER_FUNCTION(tanh, eval_func(tanh_eval).
                        evalf_func(tanh_evalf).
                        derivative_func(tanh_deriv).
                        series_func(tanh_series));

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

static ex asinh_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx asinh(x) -> 1/sqrt(1+x^2)
	return power(_ex1()+power(x,_ex2()),_ex_1_2());
}

REGISTER_FUNCTION(asinh, eval_func(asinh_eval).
                         evalf_func(asinh_evalf).
                         derivative_func(asinh_deriv));

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

static ex acosh_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx acosh(x) -> 1/(sqrt(x-1)*sqrt(x+1))
	return power(x+_ex_1(),_ex_1_2())*power(x+_ex1(),_ex_1_2());
}

REGISTER_FUNCTION(acosh, eval_func(acosh_eval).
                         evalf_func(acosh_evalf).
                         derivative_func(acosh_deriv));

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
		if (x.is_equal(_ex1()) || x.is_equal(_ex_1()))
			throw (pole_error("atanh_eval(): logarithmic pole",0));
		// atanh(float) -> float
		if (!x.info(info_flags::crational))
			return atanh_evalf(x);
	}
	
	return atanh(x).hold();
}

static ex atanh_deriv(const ex & x, unsigned deriv_param)
{
	GINAC_ASSERT(deriv_param==0);
	
	// d/dx atanh(x) -> 1/(1-x^2)
	return power(_ex1()-power(x,_ex2()),_ex_1());
}

static ex atanh_series(const ex &x,
                       const relational &rel,
                       int order,
                       unsigned options)
{
	GINAC_ASSERT(is_ex_exactly_of_type(rel.lhs(),symbol));
	// method:
	// Taylor series where there is no pole or cut falls back to atan_deriv.
	// There are two branch cuts, one runnig from 1 up the real axis and one
	// one running from -1 down the real axis.  The points 1 and -1 are poles
	// On the branch cuts and the poles series expand
	//     log((1+x)/(1-x))/(2*I)
	// instead.
	// (The constant term on the cut itself could be made simpler.)
	const ex x_pt = x.subs(rel);
	if (!(x_pt).info(info_flags::real))
		throw do_taylor();     // Im(x) != 0
	if ((x_pt).info(info_flags::real) && abs(x_pt)<_ex1())
		throw do_taylor();     // Im(x) == 0, but abs(x)<1
	// if we got here we have to care for cuts and poles
	return (log((1+x)/(1-x))/2).series(rel, order, options);
}

REGISTER_FUNCTION(atanh, eval_func(atanh_eval).
                         evalf_func(atanh_evalf).
                         derivative_func(atanh_deriv).
                         series_func(atanh_series));


#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
