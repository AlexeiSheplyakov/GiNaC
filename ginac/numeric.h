/** @file numeric.h
 *
 *  Makes the interface to the underlying bignum package available. */

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

#ifndef __GINAC_NUMERIC_H__
#define __GINAC_NUMERIC_H__

#include "basic.h"
#include "ex.h"

#include <cln/number.h>
// forward decln of cln::cl_N, since cln/complex_class.h is not included:
namespace cln { class cl_N; }

#if defined(G__CINTVERSION) && !defined(__MAKECINT__)
// Cint @$#$! doesn't like forward declaring classes used for casting operators
// so we have to include the definition of cln::cl_N here, but it is enough to
// do so for the compiler, hence the !defined(__MAKECINT__).
  #include <cln/complex_class.h>
#endif

namespace GiNaC {

/** This class is used to instantiate a global singleton object Digits
 *  which behaves just like Maple's Digits.  We need an object rather 
 *  than a dumber basic type since as a side-effect we let it change
 *  cl_default_float_format when it gets changed.  The only other
 *  meaningful thing to do with it is converting it to an unsigned,
 *  for temprary storing its value e.g.  The user must not create an
 *  own working object of this class!  Since C++ forces us to make the
 *  class definition visible in order to use an object we put in a
 *  flag which prevents other objects of that class to be created. */
class _numeric_digits
{
// member functions
public:
	_numeric_digits();
	_numeric_digits& operator=(long prec);
	operator long();
	void print(std::ostream &os) const;
// member variables
private:
	long digits;                        ///< Number of decimal digits
	static bool too_late;               ///< Already one object present
};

/** This class is a wrapper around CLN-numbers within the GiNaC class
 *  hierarchy. Objects of this type may directly be created by the user.*/
class numeric : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(numeric, basic)
	
// member functions
	
	// other ctors
public:
	numeric(int i);
	numeric(unsigned int i);
	numeric(long i);
	numeric(unsigned long i);
	numeric(long numer, long denom);
	numeric(double d);
	numeric(const char *);
	
	// functions overriding virtual functions from bases classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned precedence(void) const {return 30;}
	bool info(unsigned inf) const;
	bool has(const ex &other) const;
	ex eval(int level = 0) const;
	ex evalf(int level = 0) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	ex to_rational(lst &repl_lst) const;
	numeric integer_content(void) const;
	ex smod(const numeric &xi) const;
	numeric max_coefficient(void) const;
protected:
	/** Implementation of ex::diff for a numeric always returns 0.
	 *  @see ex::diff */
	ex derivative(const symbol &s) const { return _ex0(); }
	bool is_equal_same_type(const basic &other) const;
	unsigned calchash(void) const;
	
	// new virtual functions which can be overridden by derived classes
	// (none)
	
	// non-virtual functions in this class
public:
	const numeric add(const numeric &other) const;
	const numeric sub(const numeric &other) const;
	const numeric mul(const numeric &other) const;
	const numeric div(const numeric &other) const;
	const numeric power(const numeric &other) const;
	const numeric & add_dyn(const numeric &other) const;
	const numeric & sub_dyn(const numeric &other) const;
	const numeric & mul_dyn(const numeric &other) const;
	const numeric & div_dyn(const numeric &other) const;
	const numeric & power_dyn(const numeric &other) const;
	const numeric & operator=(int i);
	const numeric & operator=(unsigned int i);
	const numeric & operator=(long i);
	const numeric & operator=(unsigned long i);
	const numeric & operator=(double d);
	const numeric & operator=(const char *s);
	const numeric inverse(void) const;
	int csgn(void) const;
	int compare(const numeric &other) const;
	bool is_equal(const numeric &other) const;
	bool is_zero(void) const;
	bool is_positive(void) const;
	bool is_negative(void) const;
	bool is_integer(void) const;
	bool is_pos_integer(void) const;
	bool is_nonneg_integer(void) const;
	bool is_even(void) const;
	bool is_odd(void) const;
	bool is_prime(void) const;
	bool is_rational(void) const;
	bool is_real(void) const;
	bool is_cinteger(void) const;
	bool is_crational(void) const;
	bool operator==(const numeric &other) const;
	bool operator!=(const numeric &other) const;
	bool operator<(const numeric &other) const;
	bool operator<=(const numeric &other) const;
	bool operator>(const numeric &other) const;
	bool operator>=(const numeric &other) const;
	int to_int(void) const;
	long to_long(void) const;
	double to_double(void) const;
	cln::cl_N to_cl_N(void) const;
	const numeric real(void) const;
	const numeric imag(void) const;
	const numeric numer(void) const;
	const numeric denom(void) const;
	int int_length(void) const;
	// converting routines for interfacing with CLN:
	numeric(const cln::cl_N &z);

// member variables

protected:
	cln::cl_number value;
};

// global constants

extern const numeric I;
extern _numeric_digits Digits;

// deprecated macro, for internal use only
#define is_a_numeric_hash(x) ((x)&0x80000000U)

// global functions

const numeric exp(const numeric &x);
const numeric log(const numeric &x);
const numeric sin(const numeric &x);
const numeric cos(const numeric &x);
const numeric tan(const numeric &x);
const numeric asin(const numeric &x);
const numeric acos(const numeric &x);
const numeric atan(const numeric &x);
const numeric atan(const numeric &y, const numeric &x);
const numeric sinh(const numeric &x);
const numeric cosh(const numeric &x);
const numeric tanh(const numeric &x);
const numeric asinh(const numeric &x);
const numeric acosh(const numeric &x);
const numeric atanh(const numeric &x);
const numeric Li2(const numeric &x);
const numeric zeta(const numeric &x);
const numeric lgamma(const numeric &x);
const numeric tgamma(const numeric &x);
const numeric psi(const numeric &x);
const numeric psi(const numeric &n, const numeric &x);
const numeric factorial(const numeric &n);
const numeric doublefactorial(const numeric &n);
const numeric binomial(const numeric &n, const numeric &k);
const numeric bernoulli(const numeric &n);
const numeric fibonacci(const numeric &n);
const numeric isqrt(const numeric &x);
const numeric sqrt(const numeric &x);
const numeric abs(const numeric &x);
const numeric mod(const numeric &a, const numeric &b);
const numeric smod(const numeric &a, const numeric &b);
const numeric irem(const numeric &a, const numeric &b);
const numeric irem(const numeric &a, const numeric &b, numeric &q);
const numeric iquo(const numeric &a, const numeric &b);
const numeric iquo(const numeric &a, const numeric &b, numeric &r);
const numeric gcd(const numeric &a, const numeric &b);
const numeric lcm(const numeric &a, const numeric &b);

// wrapper functions around member functions
inline const numeric pow(const numeric &x, const numeric &y)
{ return x.power(y); }

inline const numeric inverse(const numeric &x)
{ return x.inverse(); }

inline int csgn(const numeric &x)
{ return x.csgn(); }

inline bool is_zero(const numeric &x)
{ return x.is_zero(); }

inline bool is_positive(const numeric &x)
{ return x.is_positive(); }

inline bool is_integer(const numeric &x)
{ return x.is_integer(); }

inline bool is_pos_integer(const numeric &x)
{ return x.is_pos_integer(); }

inline bool is_nonneg_integer(const numeric &x)
{ return x.is_nonneg_integer(); }

inline bool is_even(const numeric &x)
{ return x.is_even(); }

inline bool is_odd(const numeric &x)
{ return x.is_odd(); }

inline bool is_prime(const numeric &x)
{ return x.is_prime(); }

inline bool is_rational(const numeric &x)
{ return x.is_rational(); }

inline bool is_real(const numeric &x)
{ return x.is_real(); }

inline bool is_cinteger(const numeric &x)
{ return x.is_cinteger(); }

inline bool is_crational(const numeric &x)
{ return x.is_crational(); }

inline int to_int(const numeric &x)
{ return x.to_int(); }

inline long to_long(const numeric &x)
{ return x.to_long(); }

inline double to_double(const numeric &x)
{ return x.to_double(); }

inline const numeric real(const numeric &x)
{ return x.real(); }

inline const numeric imag(const numeric &x)
{ return x.imag(); }

inline const numeric numer(const numeric &x)
{ return x.numer(); }

inline const numeric denom(const numeric &x)
{ return x.denom(); }

// numeric evaluation functions for class constant objects:

ex PiEvalf(void);
ex EulerEvalf(void);
ex CatalanEvalf(void);


// utility functions

/** Return the numeric object handled by an ex.  Deprecated: use ex_to<numeric>().
 *  This is unsafe: you need to check the type first. */
inline const numeric &ex_to_numeric(const ex &e)
{
	return static_cast<const numeric &>(*e.bp);
}

/** Specialization of is_exactly_a<numeric>(obj) for numeric objects. */
template<> inline bool is_exactly_a<numeric>(const basic & obj)
{
	return obj.tinfo()==TINFO_numeric;
}

} // namespace GiNaC

#ifdef __MAKECINT__
#pragma link off defined_in cln/number.h;
#pragma link off defined_in cln/complex_class.h;
#endif

#endif // ndef __GINAC_NUMERIC_H__
