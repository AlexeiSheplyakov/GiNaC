/** @file numeric.h
 *
 *  Makes the interface to the underlying bignum package available. */

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

#ifndef __GINAC_NUMERIC_H__
#define __GINAC_NUMERIC_H__

#include <strstream>
#include "basic.h"
#include "ex.h"

class cl_N;     // We want to include cln.h only in numeric.cpp in order to 
                // avoid namespace pollution and keep compile-time low.

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

#define HASHVALUE_NUMERIC 0x80000001U

/** This class is used to instantiate a global object Digits which
 *  behaves just like Maple's Digits.  We need an object rather than a
 *  dumber basic type since as a side-effect we let it change
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
    void print(ostream & os) const;
// member variables
private:
    long digits;
    static bool too_late;
};

/** This class is a wrapper around CLN-numbers within the GiNaC class
 *  hierarchy. Objects of this type may directly be created by the user.*/
class numeric : public basic
{
    GINAC_DECLARE_REGISTERED_CLASS(numeric, basic)

// friends
    friend const numeric exp(const numeric & x);
    friend const numeric log(const numeric & x);
    friend const numeric sin(const numeric & x);
    friend const numeric cos(const numeric & x);
    friend const numeric tan(const numeric & x);
    friend const numeric asin(const numeric & x);
    friend const numeric acos(const numeric & x);
    friend const numeric atan(const numeric & x);
    friend const numeric atan(const numeric & y, const numeric & x);
    friend const numeric sinh(const numeric & x);
    friend const numeric cosh(const numeric & x);
    friend const numeric tanh(const numeric & x);
    friend const numeric asinh(const numeric & x);
    friend const numeric acosh(const numeric & x);
    friend const numeric atanh(const numeric & x);
    friend const numeric zeta(const numeric & x);
    friend const numeric bernoulli(const numeric & n);
    friend const numeric fibonacci(const numeric & n);
    friend numeric abs(const numeric & x);
    friend numeric mod(const numeric & a, const numeric & b);
    friend numeric smod(const numeric & a, const numeric & b);
    friend numeric irem(const numeric & a, const numeric & b);
    friend numeric irem(const numeric & a, const numeric & b, numeric & q);
    friend numeric iquo(const numeric & a, const numeric & b);
    friend numeric iquo(const numeric & a, const numeric & b, numeric & r);
    friend numeric sqrt(const numeric & x);
    friend numeric isqrt(const numeric & x);
    friend numeric gcd(const numeric & a, const numeric & b);
    friend numeric lcm(const numeric & a, const numeric & b);

// member functions

    // default constructor, destructor, copy constructor assignment
    // operator and helpers
public:
    numeric();
    ~numeric();
    numeric(const numeric & other);
    const numeric & operator=(const numeric & other);
protected:
    void copy(const numeric & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit numeric(int i);
    explicit numeric(unsigned int i);
    explicit numeric(long i);
    explicit numeric(unsigned long i);
    explicit numeric(long numer, long denom);
    explicit numeric(double d);
    explicit numeric(const char *);
    numeric(const cl_N & z);
    
    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void print(ostream & os, unsigned precedence=0) const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void printcsrc(ostream & os, unsigned type, unsigned precedence=0) const;
    bool info(unsigned inf) const;
    bool has(const ex & other) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    numeric integer_content(void) const;
    ex smod(const numeric &xi) const;
    numeric max_coefficient(void) const;
protected:
    ex derivative(const symbol & s) const;
    int compare_same_type(const basic & other) const;
    bool is_equal_same_type(const basic & other) const;
    unsigned calchash(void) const;

    // new virtual functions which can be overridden by derived classes
    // (none)

    // non-virtual functions in this class
public:
    numeric add(const numeric & other) const;
    numeric sub(const numeric & other) const;
    numeric mul(const numeric & other) const;
    numeric div(const numeric & other) const;
    numeric power(const numeric & other) const;
    const numeric & add_dyn(const numeric & other) const;
    const numeric & sub_dyn(const numeric & other) const;
    const numeric & mul_dyn(const numeric & other) const;
    const numeric & div_dyn(const numeric & other) const;
    const numeric & power_dyn(const numeric & other) const;
    const numeric & operator=(int i);
    const numeric & operator=(unsigned int i);
    const numeric & operator=(long i);
    const numeric & operator=(unsigned long i);
    const numeric & operator=(double d);
    const numeric & operator=(const char * s);
    numeric inverse(void) const;
    int csgn(void) const;
    int compare(const numeric & other) const;
    bool is_equal(const numeric & other) const;
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
    bool operator==(const numeric & other) const;
    bool operator!=(const numeric & other) const;
    bool operator<(const numeric & other) const;
    bool operator<=(const numeric & other) const;
    bool operator>(const numeric & other) const;
    bool operator>=(const numeric & other) const;
    int to_int(void) const;
    long to_long(void) const;
    double to_double(void) const;
    numeric real(void) const;
    numeric imag(void) const;
    numeric numer(void) const;
    numeric denom(void) const;
    int int_length(void) const;

// member variables

protected:
    static unsigned precedence;
    cl_N *value;
};

// global constants

extern const numeric some_numeric;
extern const numeric I;
extern const type_info & typeid_numeric;
extern _numeric_digits Digits;

//#define is_a_numeric_hash(x) ((x)==HASHVALUE_NUMERIC)
// may have to be changed to ((x)>=0x80000000U)

// has been changed
//#define is_a_numeric_hash(x) ((x)&0x80000000U)

// global functions

const numeric exp(const numeric & x);
const numeric log(const numeric & x);
const numeric sin(const numeric & x);
const numeric cos(const numeric & x);
const numeric tan(const numeric & x);
const numeric asin(const numeric & x);
const numeric acos(const numeric & x);
const numeric atan(const numeric & x);
const numeric atan(const numeric & y, const numeric & x);
const numeric sinh(const numeric & x);
const numeric cosh(const numeric & x);
const numeric tanh(const numeric & x);
const numeric asinh(const numeric & x);
const numeric acosh(const numeric & x);
const numeric atanh(const numeric & x);
const numeric zeta(const numeric & x);
const numeric gamma(const numeric & x);
const numeric psi(const numeric & x);
const numeric psi(const numeric & n, const numeric & x);
const numeric factorial(const numeric & n);
const numeric doublefactorial(const numeric & n);
const numeric binomial(const numeric & n, const numeric & k);
const numeric bernoulli(const numeric & n);
const numeric fibonacci(const numeric & n);

numeric abs(const numeric & x);
numeric mod(const numeric & a, const numeric & b);
numeric smod(const numeric & a, const numeric & b);
numeric irem(const numeric & a, const numeric & b);
numeric irem(const numeric & a, const numeric & b, numeric & q);
numeric iquo(const numeric & a, const numeric & b);
numeric iquo(const numeric & a, const numeric & b, numeric & r);
numeric sqrt(const numeric & x);
numeric isqrt(const numeric & x);

numeric gcd(const numeric & a, const numeric & b);
numeric lcm(const numeric & a, const numeric & b);

/** Exception thrown by numeric members to signal failure */
struct numeric_fail
{
    int failval;
    numeric_fail(int n) { failval = n; }
};

// wrapper functions around member functions
inline numeric pow(const numeric & x, const numeric & y)
{ return x.power(y); }

inline numeric inverse(const numeric & x)
{ return x.inverse(); }

inline bool csgn(const numeric & x)
{ return x.csgn(); }

inline bool is_zero(const numeric & x)
{ return x.is_zero(); }

inline bool is_positive(const numeric & x)
{ return x.is_positive(); }

inline bool is_integer(const numeric & x)
{ return x.is_integer(); }

inline bool is_pos_integer(const numeric & x)
{ return x.is_pos_integer(); }

inline bool is_nonneg_integer(const numeric & x)
{ return x.is_nonneg_integer(); }

inline bool is_even(const numeric & x)
{ return x.is_even(); }

inline bool is_odd(const numeric & x)
{ return x.is_odd(); }

inline bool is_prime(const numeric & x)
{ return x.is_prime(); }

inline bool is_rational(const numeric & x)
{ return x.is_rational(); }

inline bool is_real(const numeric & x)
{ return x.is_real(); }

inline bool is_cinteger(const numeric & x)
{ return x.is_cinteger(); }

inline bool is_crational(const numeric & x)
{ return x.is_crational(); }

inline numeric real(const numeric & x)
{ return x.real(); }

inline numeric imag(const numeric & x)
{ return x.imag(); }

inline numeric numer(const numeric & x)
{ return x.numer(); }

inline numeric denom(const numeric & x)
{ return x.denom(); }

// numeric evaluation functions for class constant objects:

ex PiEvalf(void);
ex EulerGammaEvalf(void);
ex CatalanEvalf(void);


// utility functions
inline const numeric &ex_to_numeric(const ex &e)
{
    return static_cast<const numeric &>(*e.bp);
}


#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_NUMERIC_H__
