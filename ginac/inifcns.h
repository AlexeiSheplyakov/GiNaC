/** @file inifcns.h
 *
 *  Interface to GiNaC's initially known functions. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_INIFCNS_H__
#define __GINAC_INIFCNS_H__

#include "function.h"
#include "ex.h"

namespace GiNaC {

/** Absolute value. */
DECLARE_FUNCTION_1P(abs)
	
/** Complex sign. */
DECLARE_FUNCTION_1P(csgn)

/** Eta function: log(a*b) == log(a) + log(b) + eta(a, b). */
DECLARE_FUNCTION_2P(eta)

/** Sine. */
DECLARE_FUNCTION_1P(sin)

/** Cosine. */
DECLARE_FUNCTION_1P(cos)

/** Tangent. */
DECLARE_FUNCTION_1P(tan)

/** Exponential function. */
DECLARE_FUNCTION_1P(exp)

/** Natural logarithm. */
DECLARE_FUNCTION_1P(log)

/** Inverse sine (arc sine). */
DECLARE_FUNCTION_1P(asin)

/** Inverse cosine (arc cosine). */
DECLARE_FUNCTION_1P(acos)

/** Inverse tangent (arc tangent). */
DECLARE_FUNCTION_1P(atan)

/** Inverse tangent with two arguments. */
DECLARE_FUNCTION_2P(atan2)

/** Hyperbolic Sine. */
DECLARE_FUNCTION_1P(sinh)

/** Hyperbolic Cosine. */
DECLARE_FUNCTION_1P(cosh)

/** Hyperbolic Tangent. */
DECLARE_FUNCTION_1P(tanh)

/** Inverse hyperbolic Sine (area hyperbolic sine). */
DECLARE_FUNCTION_1P(asinh)

/** Inverse hyperbolic Cosine (area hyperbolic cosine). */
DECLARE_FUNCTION_1P(acosh)

/** Inverse hyperbolic Tangent (area hyperbolic tangent). */
DECLARE_FUNCTION_1P(atanh)

/** Dilogarithm. */
DECLARE_FUNCTION_1P(Li2)

/** Trilogarithm. */
DECLARE_FUNCTION_1P(Li3)

// overloading at work: we cannot use the macros here
/** Riemann's Zeta-function. */
class zeta1_SERIAL { public: static unsigned serial; };
template<typename T1>
inline function zeta(const T1 & p1) {
	return function(zeta1_SERIAL::serial, ex(p1));
}
/** Derivatives of Riemann's Zeta-function. */
class zeta2_SERIAL { public: static unsigned serial; };
template<typename T1, typename T2>
inline function zeta(const T1 & p1, const T2 & p2) {
	return function(zeta2_SERIAL::serial, ex(p1), ex(p2));
}
class zeta_SERIAL;
template<> inline bool is_the_function<class zeta_SERIAL>(const ex & x)
{
	return is_the_function<zeta1_SERIAL>(x) || is_the_function<zeta2_SERIAL>(x);
}

/** Gamma-function. */
DECLARE_FUNCTION_1P(lgamma)
DECLARE_FUNCTION_1P(tgamma)

/** Beta-function. */
DECLARE_FUNCTION_2P(beta)

// overloading at work: we cannot use the macros here
/** Psi-function (aka digamma-function). */
class psi1_SERIAL { public: static unsigned serial; };
template<typename T1>
inline function psi(const T1 & p1) {
	return function(psi1_SERIAL::serial, ex(p1));
}
/** Derivatives of Psi-function (aka polygamma-functions). */
class psi2_SERIAL { public: static unsigned serial; };
template<typename T1, typename T2>
inline function psi(const T1 & p1, const T2 & p2) {
	return function(psi2_SERIAL::serial, ex(p1), ex(p2));
}
class psi_SERIAL;
template<> inline bool is_the_function<class psi_SERIAL>(const ex & x)
{
	return is_the_function<psi1_SERIAL>(x) || is_the_function<psi2_SERIAL>(x);
}
	
/** Factorial function. */
DECLARE_FUNCTION_1P(factorial)

/** Binomial function. */
DECLARE_FUNCTION_2P(binomial)

/** Order term function (for truncated power series). */
DECLARE_FUNCTION_1P(Order)

/** Polylogarithm and multiple polylogarithm. */
DECLARE_FUNCTION_2P(Li)

/** Nielsen's generalized polylogarithm. */
DECLARE_FUNCTION_3P(S)

/** Harmonic polylogarithm. */
DECLARE_FUNCTION_2P(H)

/** Multiple zeta value. */
DECLARE_FUNCTION_1P(mZeta)
 
ex lsolve(const ex &eqns, const ex &symbols, unsigned options = solve_algo::automatic);

/** Check whether a function is the Order (O(n)) function. */
inline bool is_order_function(const ex & e)
{
	return is_ex_the_function(e, Order);
}

} // namespace GiNaC

#endif // ndef __GINAC_INIFCNS_H__
