/** @file numeric.h
 *
 *  Makes the interface to the underlying bignum package available. */

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

#ifndef __GINAC_NUMERIC_H__
#define __GINAC_NUMERIC_H__

#include <strstream>
#include <ginac/basic.h>
#include <ginac/ex.h>

class cl_N;     // We want to include cln.h only in numeric.cpp in order to 
                // avoid namespace pollution and keep compile-time low.

namespace GiNaC {

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
// friends
    friend numeric exp(numeric const & x);
    friend numeric log(numeric const & x);
    friend numeric sin(numeric const & x);
    friend numeric cos(numeric const & x);
    friend numeric tan(numeric const & x);
    friend numeric asin(numeric const & x);
    friend numeric acos(numeric const & x);
    friend numeric atan(numeric const & x);
    friend numeric atan(numeric const & y, numeric const & x);
    friend numeric sinh(numeric const & x);
    friend numeric cosh(numeric const & x);
    friend numeric tanh(numeric const & x);
    friend numeric asinh(numeric const & x);
    friend numeric acosh(numeric const & x);
    friend numeric atanh(numeric const & x);
    friend numeric bernoulli(numeric const & n);
    friend numeric abs(numeric const & x);
    friend numeric mod(numeric const & a, numeric const & b);
    friend numeric smod(numeric const & a, numeric const & b);
    friend numeric irem(numeric const & a, numeric const & b);
    friend numeric irem(numeric const & a, numeric const & b, numeric & q);
    friend numeric iquo(numeric const & a, numeric const & b);
    friend numeric iquo(numeric const & a, numeric const & b, numeric & r);
    friend numeric sqrt(numeric const & x);
    friend numeric isqrt(numeric const & x);
    friend numeric gcd(numeric const & a, numeric const & b);
    friend numeric lcm(numeric const & a, numeric const & b);
    friend numeric const & numZERO(void);
    friend numeric const & numONE(void);
    friend numeric const & numTWO(void);
    friend numeric const & numTHREE(void);
    friend numeric const & numMINUSONE(void);
    friend numeric const & numHALF(void);

// member functions

    // default constructor, destructor, copy constructor assignment
    // operator and helpers
public:
    numeric();
    ~numeric();
    numeric(numeric const & other);
    numeric const & operator=(numeric const & other);
protected:
    void copy(numeric const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit numeric(int i);
    explicit numeric(unsigned int i);
    explicit numeric(long i);
    explicit numeric(unsigned long i);
    explicit numeric(long numer, long denom);
    explicit numeric(double d);
    explicit numeric(char const *);
    numeric(cl_N const & z);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned precedence=0) const;
    bool info(unsigned inf) const;
    ex evalf(int level=0) const;
    ex diff(symbol const & s) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    numeric integer_content(void) const;
    ex smod(numeric const &xi) const;
    numeric max_coefficient(void) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned calchash(void) const {
        hashvalue=HASHVALUE_NUMERIC;
        return HASHVALUE_NUMERIC;
    }

    // new virtual functions which can be overridden by derived classes
    // (none)

    // non-virtual functions in this class
public:
    numeric add(numeric const & other) const;
    numeric sub(numeric const & other) const;
    numeric mul(numeric const & other) const;
    numeric div(numeric const & other) const;
    numeric power(numeric const & other) const;
    numeric const & add_dyn(numeric const & other) const;
    numeric const & sub_dyn(numeric const & other) const;
    numeric const & mul_dyn(numeric const & other) const;
    numeric const & div_dyn(numeric const & other) const;
    numeric const & power_dyn(numeric const & other) const;
    numeric const & operator=(int i);
    numeric const & operator=(unsigned int i);
    numeric const & operator=(long i);
    numeric const & operator=(unsigned long i);
    numeric const & operator=(double d);
    numeric const & operator=(char const * s);
    /*
    numeric add_dyn(numeric const & other) const   { return add(other);   }
    numeric sub_dyn(numeric const & other) const   { return sub(other);   }
    numeric mul_dyn(numeric const & other) const   { return mul(other);   }
    numeric div_dyn(numeric const & other) const   { return div(other);   }
    numeric power_dyn(numeric const & other) const { return power(other); }
    */
    numeric inverse(void) const;
    int csgn(void) const;
    int compare(numeric const & other) const;
    bool is_equal(numeric const & other) const;
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
    bool operator==(numeric const & other) const;
    bool operator!=(numeric const & other) const;
    bool operator<(numeric const & other) const;
    bool operator<=(numeric const & other) const;
    bool operator>(numeric const & other) const;
    bool operator>=(numeric const & other) const;
    int to_int(void) const;
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
extern type_info const & typeid_numeric;
extern _numeric_digits Digits;

#define is_a_numeric_hash(x) ((x)==HASHVALUE_NUMERIC)
// may have to be changed to ((x)>=0x80000000U)

// global functions

numeric const & numZERO(void);
numeric const & numONE(void);
numeric const & numTWO(void);
numeric const & numMINUSONE(void);
numeric const & numHALF(void);

numeric exp(numeric const & x);
numeric log(numeric const & x);
numeric sin(numeric const & x);
numeric cos(numeric const & x);
numeric tan(numeric const & x);
numeric asin(numeric const & x);
numeric acos(numeric const & x);
numeric atan(numeric const & x);
numeric atan(numeric const & y, numeric const & x);
numeric sinh(numeric const & x);
numeric cosh(numeric const & x);
numeric tanh(numeric const & x);
numeric asinh(numeric const & x);
numeric acosh(numeric const & x);
numeric atanh(numeric const & x);
numeric zeta(numeric const & x);
numeric gamma(numeric const & x);
numeric psi(numeric const & x);
numeric psi(numeric const & n, numeric const & x);
numeric factorial(numeric const & n);
numeric doublefactorial(numeric const & n);
numeric binomial(numeric const & n, numeric const & k);
numeric bernoulli(numeric const & n);

numeric abs(numeric const & x);
numeric mod(numeric const & a, numeric const & b);
numeric smod(numeric const & a, numeric const & b);
numeric irem(numeric const & a, numeric const & b);
numeric irem(numeric const & a, numeric const & b, numeric & q);
numeric iquo(numeric const & a, numeric const & b);
numeric iquo(numeric const & a, numeric const & b, numeric & r);
numeric sqrt(numeric const & x);
numeric isqrt(numeric const & x);

numeric gcd(numeric const & a, numeric const & b);
numeric lcm(numeric const & a, numeric const & b);

/** Exception thrown by numeric members to signal failure */
struct numeric_fail
{
    int failval;
    numeric_fail(int n) { failval = n; }
};

// wrapper functions around member functions
inline numeric inverse(numeric const & x)
{ return x.inverse(); }

inline bool csgn(numeric const & x)
{ return x.csgn(); }

inline bool is_zero(numeric const & x)
{ return x.is_zero(); }

inline bool is_positive(numeric const & x)
{ return x.is_positive(); }

inline bool is_integer(numeric const & x)
{ return x.is_integer(); }

inline bool is_pos_integer(numeric const & x)
{ return x.is_pos_integer(); }

inline bool is_nonneg_integer(numeric const & x)
{ return x.is_nonneg_integer(); }

inline bool is_even(numeric const & x)
{ return x.is_even(); }

inline bool is_odd(numeric const & x)
{ return x.is_odd(); }

inline bool is_prime(numeric const & x)
{ return x.is_prime(); }

inline bool is_rational(numeric const & x)
{ return x.is_rational(); }

inline bool is_real(numeric const & x)
{ return x.is_real(); }

inline numeric real(numeric const & x)
{ return x.real(); }

inline numeric imag(numeric const & x)
{ return x.imag(); }

inline numeric numer(numeric const & x)
{ return x.numer(); }

inline numeric denom(numeric const & x)
{ return x.denom(); }

ex IEvalf(void);
ex PiEvalf(void);
ex EulerGammaEvalf(void);
ex CatalanEvalf(void);

// utility functions
inline const numeric &ex_to_numeric(const ex &e)
{
	return static_cast<const numeric &>(*e.bp);
}

} // namespace GiNaC

#endif // ndef __GINAC_NUMERIC_H__
