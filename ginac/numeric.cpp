/** @file numeric.cpp
 *
 *  This file contains the interface to the underlying bignum package.
 *  Its most important design principle is to completely hide the inner
 *  working of that other package from the user of GiNaC.  It must either 
 *  provide implementation of arithmetic operators and numerical evaluation
 *  of special functions or implement the interface to the bignum package. */

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

#include "numeric.h"
#include "ex.h"
#include "config.h"
#include "debugmsg.h"

// CLN should not pollute the global namespace, hence we include it here
// instead of in some header file where it would propagate to other parts:
#ifdef HAVE_CLN_CLN_H
#include <CLN/cln.h>
#else
#include <cln.h>
#endif

namespace GiNaC {

// linker has no problems finding text symbols for numerator or denominator
//#define SANE_LINKER

//////////
// default constructor, destructor, copy constructor assignment
// operator and helpers
//////////

// public

/** default ctor. Numerically it initializes to an integer zero. */
numeric::numeric() : basic(TINFO_numeric)
{
    debugmsg("numeric default constructor", LOGLEVEL_CONSTRUCT);
    value = new cl_N;
    *value=cl_I(0);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

numeric::~numeric()
{
    debugmsg("numeric destructor" ,LOGLEVEL_DESTRUCT);
    destroy(0);
}

numeric::numeric(numeric const & other)
{
    debugmsg("numeric copy constructor", LOGLEVEL_CONSTRUCT);
    copy(other);
}

numeric const & numeric::operator=(numeric const & other)
{
    debugmsg("numeric operator=", LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void numeric::copy(numeric const & other)
{
    basic::copy(other);
    value = new cl_N(*other.value);
}

void numeric::destroy(bool call_parent)
{
    delete value;
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

numeric::numeric(int i) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from int",LOGLEVEL_CONSTRUCT);
    // Not the whole int-range is available if we don't cast to long
    // first. This is due to the behaviour of the cl_I-ctor, which
    // emphasizes efficiency:
    value = new cl_I((long) i);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

numeric::numeric(unsigned int i) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from uint",LOGLEVEL_CONSTRUCT);
    // Not the whole uint-range is available if we don't cast to ulong
    // first. This is due to the behaviour of the cl_I-ctor, which
    // emphasizes efficiency:
    value = new cl_I((unsigned long)i);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

numeric::numeric(long i) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from long",LOGLEVEL_CONSTRUCT);
    value = new cl_I(i);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

numeric::numeric(unsigned long i) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from ulong",LOGLEVEL_CONSTRUCT);
    value = new cl_I(i);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

/** Ctor for rational numerics a/b.
 *
 *  @exception overflow_error (division by zero) */
numeric::numeric(long numer, long denom) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from long/long",LOGLEVEL_CONSTRUCT);
    if (!denom)
        throw (std::overflow_error("division by zero"));
    value = new cl_I(numer);
    *value = *value / cl_I(denom);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

numeric::numeric(double d) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from double",LOGLEVEL_CONSTRUCT);
    // We really want to explicitly use the type cl_LF instead of the
    // more general cl_F, since that would give us a cl_DF only which
    // will not be promoted to cl_LF if overflow occurs:
    value = new cl_N;
    *value = cl_float(d, cl_default_float_format);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

numeric::numeric(char const *s) : basic(TINFO_numeric)
{   // MISSING: treatment of complex and ints and rationals.
    debugmsg("numeric constructor from string",LOGLEVEL_CONSTRUCT);
    if (strchr(s, '.'))
        value = new cl_LF(s);
    else
        value = new cl_I(s);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

/** Ctor from CLN types.  This is for the initiated user or internal use
 *  only. */
numeric::numeric(cl_N const & z) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from cl_N", LOGLEVEL_CONSTRUCT);
    value = new cl_N(z);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * numeric::duplicate() const
{
    debugmsg("numeric duplicate", LOGLEVEL_DUPLICATE);
    return new numeric(*this);
}

// The method printraw doesn't do much, it simply uses CLN's operator<<() for
// output, which is ugly but reliable. Examples:
// 2+2i 
void numeric::printraw(ostream & os) const
{
    debugmsg("numeric printraw", LOGLEVEL_PRINT);
    os << "numeric(" << *value << ")";
}

// The method print adds to the output so it blends more consistently together
// with the other routines and produces something compatible to Maple input.
void numeric::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("numeric print", LOGLEVEL_PRINT);
    if (is_real()) {  
        // case 1, real:  x  or  -x
        if ((precedence<=upper_precedence) && (!is_pos_integer())) {
            os << "(" << *value << ")";
        } else {
            os << *value;
        }
    } else {
        // case 2, imaginary:  y*I  or  -y*I
        if (realpart(*value) == 0) {
            if ((precedence<=upper_precedence) && (imagpart(*value) < 0)) {
                if (imagpart(*value) == -1) {
                    os << "(-I)";
                } else {
                    os << "(" << imagpart(*value) << "*I)";
                }
            } else {
                if (imagpart(*value) == 1) {
                    os << "I";
                } else {
                    if (imagpart (*value) == -1) {
                        os << "-I";
                    } else {
                        os << imagpart(*value) << "*I";
                    }
                }
            }
        } else {
            // case 3, complex:  x+y*I  or  x-y*I  or  -x+y*I  or  -x-y*I
            if (precedence <= upper_precedence) os << "(";
            os << realpart(*value);
            if (imagpart(*value) < 0) {
                if (imagpart(*value) == -1) {
                    os << "-I";
                } else {
                    os << imagpart(*value) << "*I";
                }
            } else {
                if (imagpart(*value) == 1) {
                    os << "+I";
                } else {
                    os << "+" << imagpart(*value) << "*I";
                }
            }
            if (precedence <= upper_precedence) os << ")";
        }
    }
}

bool numeric::info(unsigned inf) const
{
    switch (inf) {
    case info_flags::numeric:
    case info_flags::polynomial:
    case info_flags::rational_function:
        return true;
    case info_flags::real:
        return is_real();
    case info_flags::rational:
    case info_flags::rational_polynomial:
        return is_rational();
    case info_flags::integer:
    case info_flags::integer_polynomial:
        return is_integer();
    case info_flags::positive:
        return is_positive();
    case info_flags::negative:
        return is_negative();
    case info_flags::nonnegative:
        return compare(numZERO())>=0;
    case info_flags::posint:
        return is_pos_integer();
    case info_flags::negint:
        return is_integer() && (compare(numZERO())<0);
    case info_flags::nonnegint:
        return is_nonneg_integer();
    case info_flags::even:
        return is_even();
    case info_flags::odd:
        return is_odd();
    case info_flags::prime:
        return is_prime();
    }
    return false;
}

/** Cast numeric into a floating-point object.  For example exact numeric(1) is
 *  returned as a 1.0000000000000000000000 and so on according to how Digits is
 *  currently set.
 *
 *  @param level  ignored, but needed for overriding basic::evalf.
 *  @return an ex-handle to a numeric. */
ex numeric::evalf(int level) const
{
    // level can safely be discarded for numeric objects.
    return numeric(cl_float(1.0, cl_default_float_format) * (*value));  // -> CLN
}

// protected

int numeric::compare_same_type(basic const & other) const
{
    ASSERT(is_exactly_of_type(other, numeric));
    numeric const & o = static_cast<numeric &>(const_cast<basic &>(other));

    if (*value == *o.value) {
        return 0;
    }

    return compare(o);    
}

bool numeric::is_equal_same_type(basic const & other) const
{
    ASSERT(is_exactly_of_type(other,numeric));
    numeric const *o = static_cast<numeric const *>(&other);
    
    return is_equal(*o);
}

/*
unsigned numeric::calchash(void) const
{
    double d=to_double();
    int s=d>0 ? 1 : -1;
    d=fabs(d);
    if (d>0x07FF0000) {
        d=0x07FF0000;
    }
    return 0x88000000U+s*unsigned(d/0x07FF0000);
}
*/


//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// public

/** Numerical addition method.  Adds argument to *this and returns result as
 *  a new numeric object. */
numeric numeric::add(numeric const & other) const
{
    return numeric((*value)+(*other.value));
}

/** Numerical subtraction method.  Subtracts argument from *this and returns
 *  result as a new numeric object. */
numeric numeric::sub(numeric const & other) const
{
    return numeric((*value)-(*other.value));
}

/** Numerical multiplication method.  Multiplies *this and argument and returns
 *  result as a new numeric object. */
numeric numeric::mul(numeric const & other) const
{
    static const numeric * numONEp=&numONE();
    if (this==numONEp) {
        return other;
    } else if (&other==numONEp) {
        return *this;
    }
    return numeric((*value)*(*other.value));
}

/** Numerical division method.  Divides *this by argument and returns result as
 *  a new numeric object.
 *
 *  @exception overflow_error (division by zero) */
numeric numeric::div(numeric const & other) const
{
    if (zerop(*other.value))
        throw (std::overflow_error("division by zero"));
    return numeric((*value)/(*other.value));
}

numeric numeric::power(numeric const & other) const
{
    static const numeric * numONEp=&numONE();
    if (&other==numONEp) {
        return *this;
    }
    if (zerop(*value) && other.is_real() && minusp(realpart(*other.value)))
        throw (std::overflow_error("division by zero"));
    return numeric(expt(*value,*other.value));
}

/** Inverse of a number. */
numeric numeric::inverse(void) const
{
    return numeric(recip(*value));  // -> CLN
}

numeric const & numeric::add_dyn(numeric const & other) const
{
    return static_cast<numeric const &>((new numeric((*value)+(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

numeric const & numeric::sub_dyn(numeric const & other) const
{
    return static_cast<numeric const &>((new numeric((*value)-(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

numeric const & numeric::mul_dyn(numeric const & other) const
{
    static const numeric * numONEp=&numONE();
    if (this==numONEp) {
        return other;
    } else if (&other==numONEp) {
        return *this;
    }
    return static_cast<numeric const &>((new numeric((*value)*(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

numeric const & numeric::div_dyn(numeric const & other) const
{
    if (zerop(*other.value))
        throw (std::overflow_error("division by zero"));
    return static_cast<numeric const &>((new numeric((*value)/(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

numeric const & numeric::power_dyn(numeric const & other) const
{
    static const numeric * numONEp=&numONE();
    if (&other==numONEp) {
        return *this;
    }
    // The ifs are only a workaround for a bug in CLN. It gets stuck otherwise:
    if ( !other.is_integer() &&
         other.is_rational() &&
         (*this).is_nonneg_integer() ) {
        if ( !zerop(*value) ) {
            return static_cast<numeric const &>((new numeric(exp(*other.value * log(*value))))->
                                                setflag(status_flags::dynallocated));
        } else {
            if ( !zerop(*other.value) ) {  // 0^(n/m)
                return static_cast<numeric const &>((new numeric(0))->
                                                    setflag(status_flags::dynallocated));
            } else {                       // raise FPE (0^0 requested)
                return static_cast<numeric const &>((new numeric(1/(*other.value)))->
                                                    setflag(status_flags::dynallocated));
            }
        }
    } else {                               // default -> CLN
        return static_cast<numeric const &>((new numeric(expt(*value,*other.value)))->
                                            setflag(status_flags::dynallocated));
    }
}

numeric const & numeric::operator=(int i)
{
    return operator=(numeric(i));
}

numeric const & numeric::operator=(unsigned int i)
{
    return operator=(numeric(i));
}

numeric const & numeric::operator=(long i)
{
    return operator=(numeric(i));
}

numeric const & numeric::operator=(unsigned long i)
{
    return operator=(numeric(i));
}

numeric const & numeric::operator=(double d)
{
    return operator=(numeric(d));
}

numeric const & numeric::operator=(char const * s)
{
    return operator=(numeric(s));
}

/** This method establishes a canonical order on all numbers.  For complex
 *  numbers this is not possible in a mathematically consistent way but we need
 *  to establish some order and it ought to be fast.  So we simply define it
 *  similar to Maple's csgn. */
int numeric::compare(numeric const & other) const
{
    // Comparing two real numbers?
    if (is_real() && other.is_real())
        // Yes, just compare them
        return cl_compare(The(cl_R)(*value), The(cl_R)(*other.value));    
    else {
        // No, first compare real parts
        cl_signean real_cmp = cl_compare(realpart(*value), realpart(*other.value));
        if (real_cmp)
            return real_cmp;

        return cl_compare(imagpart(*value), imagpart(*other.value));
    }
}

bool numeric::is_equal(numeric const & other) const
{
    return (*value == *other.value);
}

/** True if object is zero. */
bool numeric::is_zero(void) const
{
    return zerop(*value);  // -> CLN
}

/** True if object is not complex and greater than zero. */
bool numeric::is_positive(void) const
{
    if (is_real()) {
        return plusp(The(cl_R)(*value));  // -> CLN
    }
    return false;
}

/** True if object is not complex and less than zero. */
bool numeric::is_negative(void) const
{
    if (is_real()) {
        return minusp(The(cl_R)(*value));  // -> CLN
    }
    return false;
}

/** True if object is a non-complex integer. */
bool numeric::is_integer(void) const
{
    return (bool)instanceof(*value, cl_I_ring);  // -> CLN
}

/** True if object is an exact integer greater than zero. */
bool numeric::is_pos_integer(void) const
{
    return (is_integer() &&
            plusp(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact integer greater or equal zero. */
bool numeric::is_nonneg_integer(void) const
{
    return (is_integer() &&
            !minusp(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact even integer. */
bool numeric::is_even(void) const
{
    return (is_integer() &&
            evenp(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact odd integer. */
bool numeric::is_odd(void) const
{
    return (is_integer() &&
            oddp(The(cl_I)(*value)));  // -> CLN
}

/** Probabilistic primality test.
 *
 *  @return  true if object is exact integer and prime. */
bool numeric::is_prime(void) const
{
    return (is_integer() &&
            isprobprime(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact rational number, may even be complex
 *  (denominator may be unity). */
bool numeric::is_rational(void) const
{
    if (instanceof(*value, cl_RA_ring)) {
        return true;
    } else if (!is_real()) {  // complex case, handle Q(i):
        if ( instanceof(realpart(*value), cl_RA_ring) &&
             instanceof(imagpart(*value), cl_RA_ring) )
            return true;
    }
    return false;
}

/** True if object is a real integer, rational or float (but not complex). */
bool numeric::is_real(void) const
{
    return (bool)instanceof(*value, cl_R_ring);  // -> CLN
}

bool numeric::operator==(numeric const & other) const
{
    return (*value == *other.value);  // -> CLN
}

bool numeric::operator!=(numeric const & other) const
{
    return (*value != *other.value);  // -> CLN
}

/** Numerical comparison: less.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator<(numeric const & other) const
{
    if ( is_real() && other.is_real() ) {
        return (bool)(The(cl_R)(*value) < The(cl_R)(*other.value));  // -> CLN
    }
    throw (std::invalid_argument("numeric::operator<(): complex inequality"));
    return false;  // make compiler shut up
}

/** Numerical comparison: less or equal.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator<=(numeric const & other) const
{
    if ( is_real() && other.is_real() ) {
        return (bool)(The(cl_R)(*value) <= The(cl_R)(*other.value));  // -> CLN
    }
    throw (std::invalid_argument("numeric::operator<=(): complex inequality"));
    return false;  // make compiler shut up
}

/** Numerical comparison: greater.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator>(numeric const & other) const
{
    if ( is_real() && other.is_real() ) {
        return (bool)(The(cl_R)(*value) > The(cl_R)(*other.value));  // -> CLN
    }
    throw (std::invalid_argument("numeric::operator>(): complex inequality"));
    return false;  // make compiler shut up
}

/** Numerical comparison: greater or equal.
 *
 *  @exception invalid_argument (complex inequality) */  
bool numeric::operator>=(numeric const & other) const
{
    if ( is_real() && other.is_real() ) {
        return (bool)(The(cl_R)(*value) >= The(cl_R)(*other.value));  // -> CLN
    }
    throw (std::invalid_argument("numeric::operator>=(): complex inequality"));
    return false;  // make compiler shut up
}

/** Converts numeric types to machine's int. You should check with is_integer()
 *  if the number is really an integer before calling this method. */
int numeric::to_int(void) const
{
    ASSERT(is_integer());
    return cl_I_to_int(The(cl_I)(*value));
}

/** Converts numeric types to machine's double. You should check with is_real()
 *  if the number is really not complex before calling this method. */
double numeric::to_double(void) const
{
    ASSERT(is_real());
    return cl_double_approx(realpart(*value));
}

/** Real part of a number. */
numeric numeric::real(void) const
{
    return numeric(realpart(*value));  // -> CLN
}

/** Imaginary part of a number. */
numeric numeric::imag(void) const
{
    return numeric(imagpart(*value));  // -> CLN
}

#ifndef SANE_LINKER
// Unfortunately, CLN did not provide an official way to access the numerator
// or denominator of a rational number (cl_RA). Doing some excavations in CLN
// one finds how it works internally in src/rational/cl_RA.h:
struct cl_heap_ratio : cl_heap {
    cl_I numerator;
    cl_I denominator;
};

inline cl_heap_ratio* TheRatio (const cl_N& obj)
{ return (cl_heap_ratio*)(obj.pointer); }
#endif // ndef SANE_LINKER

/** Numerator.  Computes the numerator of rational numbers, rationalized
 *  numerator of complex if real and imaginary part are both rational numbers
 *  (i.e numer(4/3+5/6*I) == 8+5*I), the number itself in all other cases. */
numeric numeric::numer(void) const
{
    if (is_integer()) {
        return numeric(*this);
    }
#ifdef SANE_LINKER
    else if (instanceof(*value, cl_RA_ring)) {
        return numeric(numerator(The(cl_RA)(*value)));
    }
    else if (!is_real()) {  // complex case, handle Q(i):
        cl_R r = realpart(*value);
        cl_R i = imagpart(*value);
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_I_ring))
            return numeric(*this);
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_RA_ring))
            return numeric(complex(r*denominator(The(cl_RA)(i)), numerator(The(cl_RA)(i))));
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_I_ring))
            return numeric(complex(numerator(The(cl_RA)(r)), i*denominator(The(cl_RA)(r))));
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_RA_ring)) {
            cl_I s = lcm(denominator(The(cl_RA)(r)), denominator(The(cl_RA)(i)));
            return numeric(complex(numerator(The(cl_RA)(r))*(exquo(s,denominator(The(cl_RA)(r)))),
                                   numerator(The(cl_RA)(i))*(exquo(s,denominator(The(cl_RA)(i))))));
        }
    }
#else
    else if (instanceof(*value, cl_RA_ring)) {
        return numeric(TheRatio(*value)->numerator);
    }
    else if (!is_real()) {  // complex case, handle Q(i):
        cl_R r = realpart(*value);
        cl_R i = imagpart(*value);
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_I_ring))
            return numeric(*this);
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_RA_ring))
            return numeric(complex(r*TheRatio(i)->denominator, TheRatio(i)->numerator));
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_I_ring))
            return numeric(complex(TheRatio(r)->numerator, i*TheRatio(r)->denominator));
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_RA_ring)) {
            cl_I s = lcm(TheRatio(r)->denominator, TheRatio(i)->denominator);
            return numeric(complex(TheRatio(r)->numerator*(exquo(s,TheRatio(r)->denominator)),
                                   TheRatio(i)->numerator*(exquo(s,TheRatio(i)->denominator))));
        }
    }
#endif // def SANE_LINKER
    // at least one float encountered
    return numeric(*this);
}

/** Denominator.  Computes the denominator of rational numbers, common integer
 *  denominator of complex if real and imaginary part are both rational numbers
 *  (i.e denom(4/3+5/6*I) == 6), one in all other cases. */
numeric numeric::denom(void) const
{
    if (is_integer()) {
        return numONE();
    }
#ifdef SANE_LINKER
    if (instanceof(*value, cl_RA_ring)) {
        return numeric(denominator(The(cl_RA)(*value)));
    }
    if (!is_real()) {  // complex case, handle Q(i):
        cl_R r = realpart(*value);
        cl_R i = imagpart(*value);
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_I_ring))
            return numONE();
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_RA_ring))
            return numeric(denominator(The(cl_RA)(i)));
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_I_ring))
            return numeric(denominator(The(cl_RA)(r)));
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_RA_ring))
            return numeric(lcm(denominator(The(cl_RA)(r)), denominator(The(cl_RA)(i))));
    }
#else
    if (instanceof(*value, cl_RA_ring)) {
        return numeric(TheRatio(*value)->denominator);
    }
    if (!is_real()) {  // complex case, handle Q(i):
        cl_R r = realpart(*value);
        cl_R i = imagpart(*value);
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_I_ring))
            return numONE();
        if (instanceof(r, cl_I_ring) && instanceof(i, cl_RA_ring))
            return numeric(TheRatio(i)->denominator);
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_I_ring))
            return numeric(TheRatio(r)->denominator);
        if (instanceof(r, cl_RA_ring) && instanceof(i, cl_RA_ring))
            return numeric(lcm(TheRatio(r)->denominator, TheRatio(i)->denominator));
    }
#endif // def SANE_LINKER
    // at least one float encountered
    return numONE();
}

/** Size in binary notation.  For integers, this is the smallest n >= 0 such
 *  that -2^n <= x < 2^n. If x > 0, this is the unique n > 0 such that
 *  2^(n-1) <= x < 2^n.
 *
 *  @return  number of bits (excluding sign) needed to represent that number
 *  in two's complement if it is an integer, 0 otherwise. */    
int numeric::int_length(void) const
{
    if (is_integer()) {
        return integer_length(The(cl_I)(*value));  // -> CLN
    } else {
        return 0;
    }
}


//////////
// static member variables
//////////

// protected

unsigned numeric::precedence = 30;

//////////
// global constants
//////////

const numeric some_numeric;
type_info const & typeid_numeric=typeid(some_numeric);
/** Imaginary unit.  This is not a constant but a numeric since we are
 *  natively handing complex numbers anyways. */
const numeric I = numeric(complex(cl_I(0),cl_I(1)));

//////////
// global functions
//////////

numeric const & numZERO(void)
{
    const static ex eZERO = ex((new numeric(0))->setflag(status_flags::dynallocated));
    const static numeric * nZERO = static_cast<const numeric *>(eZERO.bp);
    return *nZERO;
}

numeric const & numONE(void)
{
    const static ex eONE = ex((new numeric(1))->setflag(status_flags::dynallocated));
    const static numeric * nONE = static_cast<const numeric *>(eONE.bp);
    return *nONE;
}

numeric const & numTWO(void)
{
    const static ex eTWO = ex((new numeric(2))->setflag(status_flags::dynallocated));
    const static numeric * nTWO = static_cast<const numeric *>(eTWO.bp);
    return *nTWO;
}

numeric const & numTHREE(void)
{
    const static ex eTHREE = ex((new numeric(3))->setflag(status_flags::dynallocated));
    const static numeric * nTHREE = static_cast<const numeric *>(eTHREE.bp);
    return *nTHREE;
}

numeric const & numMINUSONE(void)
{
    const static ex eMINUSONE = ex((new numeric(-1))->setflag(status_flags::dynallocated));
    const static numeric * nMINUSONE = static_cast<const numeric *>(eMINUSONE.bp);
    return *nMINUSONE;
}

numeric const & numHALF(void)
{
    const static ex eHALF = ex((new numeric(1, 2))->setflag(status_flags::dynallocated));
    const static numeric * nHALF = static_cast<const numeric *>(eHALF.bp);
    return *nHALF;
}

/** Exponential function.
 *
 *  @return  arbitrary precision numerical exp(x). */
numeric exp(numeric const & x)
{
    return ::exp(*x.value);  // -> CLN
}

/** Natural logarithm.
 *
 *  @param z complex number
 *  @return  arbitrary precision numerical log(x).
 *  @exception overflow_error (logarithmic singularity) */
numeric log(numeric const & z)
{
    if (z.is_zero())
        throw (std::overflow_error("log(): logarithmic singularity"));
    return ::log(*z.value);  // -> CLN
}

/** Numeric sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical sin(x). */
numeric sin(numeric const & x)
{
    return ::sin(*x.value);  // -> CLN
}

/** Numeric cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical cos(x). */
numeric cos(numeric const & x)
{
    return ::cos(*x.value);  // -> CLN
}
    
/** Numeric tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical tan(x). */
numeric tan(numeric const & x)
{
    return ::tan(*x.value);  // -> CLN
}
    
/** Numeric inverse sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical asin(x). */
numeric asin(numeric const & x)
{
    return ::asin(*x.value);  // -> CLN
}
    
/** Numeric inverse cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical acos(x). */
numeric acos(numeric const & x)
{
    return ::acos(*x.value);  // -> CLN
}
    
/** Arcustangents.
 *
 *  @param z complex number
 *  @return atan(z)
 *  @exception overflow_error (logarithmic singularity) */
numeric atan(numeric const & x)
{
    if (!x.is_real() &&
        x.real().is_zero() &&
        !abs(x.imag()).is_equal(numONE()))
        throw (std::overflow_error("atan(): logarithmic singularity"));
    return ::atan(*x.value);  // -> CLN
}

/** Arcustangents.
 *
 *  @param x real number
 *  @param y real number
 *  @return atan(y/x) */
numeric atan(numeric const & y, numeric const & x)
{
    if (x.is_real() && y.is_real())
        return ::atan(realpart(*x.value), realpart(*y.value));  // -> CLN
    else
        throw (std::invalid_argument("numeric::atan(): complex argument"));        
}

/** Numeric hyperbolic sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical sinh(x). */
numeric sinh(numeric const & x)
{
    return ::sinh(*x.value);  // -> CLN
}

/** Numeric hyperbolic cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical cosh(x). */
numeric cosh(numeric const & x)
{
    return ::cosh(*x.value);  // -> CLN
}
    
/** Numeric hyperbolic tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical tanh(x). */
numeric tanh(numeric const & x)
{
    return ::tanh(*x.value);  // -> CLN
}
    
/** Numeric inverse hyperbolic sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical asinh(x). */
numeric asinh(numeric const & x)
{
    return ::asinh(*x.value);  // -> CLN
}

/** Numeric inverse hyperbolic cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical acosh(x). */
numeric acosh(numeric const & x)
{
    return ::acosh(*x.value);  // -> CLN
}

/** Numeric inverse hyperbolic tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical atanh(x). */
numeric atanh(numeric const & x)
{
    return ::atanh(*x.value);  // -> CLN
}

/** The gamma function.
 *  stub stub stub stub stub stub! */
numeric gamma(numeric const & x)
{
    clog << "gamma(): Nobody expects the Spanish inquisition" << endl;
    return numeric(0);
}

/** Factorial combinatorial function.
 *
 *  @exception range_error (argument must be integer >= 0) */
numeric factorial(numeric const & nn)
{
    if ( !nn.is_nonneg_integer() ) {
        throw (std::range_error("numeric::factorial(): argument must be integer >= 0"));
    }
    
    return numeric(::factorial(nn.to_int()));  // -> CLN
}

/** The double factorial combinatorial function.  (Scarcely used, but still
 *  useful in cases, like for exact results of Gamma(n+1/2) for instance.)
 *
 *  @param n  integer argument >= -1
 *  @return n!! == n * (n-2) * (n-4) * ... * ({1|2}) with 0!! == 1 == (-1)!!
 *  @exception range_error (argument must be integer >= -1) */
numeric doublefactorial(numeric const & nn)
{
    // We store the results separately for even and odd arguments.  This has
    // the advantage that we don't have to compute any even result at all if
    // the function is always called with odd arguments and vice versa.  There
    // is no tradeoff involved in this, it is guaranteed to save time as well
    // as memory.  (If this is not enough justification consider the Gamma
    // function of half integer arguments: it only needs odd doublefactorials.)
    static vector<numeric> evenresults;
    static int highest_evenresult = -1;
    static vector<numeric> oddresults;
    static int highest_oddresult = -1;
    
    if ( nn == numeric(-1) ) {
        return numONE();
    }
    if ( !nn.is_nonneg_integer() ) {
        throw (std::range_error("numeric::doublefactorial(): argument must be integer >= -1"));
    }
    if ( nn.is_even() ) {
        int n = nn.div(numTWO()).to_int();
        if ( n <= highest_evenresult ) {
            return evenresults[n];
        }
        if ( evenresults.capacity() < (unsigned)(n+1) ) {
            evenresults.reserve(n+1);
        }
        if ( highest_evenresult < 0 ) {
            evenresults.push_back(numONE());
            highest_evenresult=0;
        }
        for (int i=highest_evenresult+1; i<=n; i++) {
            evenresults.push_back(numeric(evenresults[i-1].mul(numeric(i*2))));
        }
        highest_evenresult=n;
        return evenresults[n];
    } else {
        int n = nn.sub(numONE()).div(numTWO()).to_int();
        if ( n <= highest_oddresult ) {
            return oddresults[n];
        }
        if ( oddresults.capacity() < (unsigned)n ) {
            oddresults.reserve(n+1);
        }
        if ( highest_oddresult < 0 ) {
            oddresults.push_back(numONE());
            highest_oddresult=0;
        }
        for (int i=highest_oddresult+1; i<=n; i++) {
            oddresults.push_back(numeric(oddresults[i-1].mul(numeric(i*2+1))));
        }
        highest_oddresult=n;
        return oddresults[n];
    }
}

/** The Binomial function. It computes the binomial coefficients. If the
 *  arguments are both nonnegative integers and 0 <= k <= n, then
 *  binomial(n, k) = n!/k!/(n-k)! which is the number of ways of choosing k
 *  objects from n distinct objects. If k > n, then binomial(n,k) returns 0. */
numeric binomial(numeric const & n, numeric const & k)
{
    if (n.is_nonneg_integer() && k.is_nonneg_integer()) {
        return numeric(::binomial(n.to_int(),k.to_int()));  // -> CLN
    } else {
        // should really be gamma(n+1)/(gamma(r+1)/gamma(n-r+1)
        return numeric(0);
    }
    // return factorial(n).div(factorial(k).mul(factorial(n.sub(k))));
}

/** Absolute value. */
numeric abs(numeric const & x)
{
    return ::abs(*x.value);  // -> CLN
}

/** Modulus (in positive representation).
 *  In general, mod(a,b) has the sign of b or is zero, and rem(a,b) has the
 *  sign of a or is zero. This is different from Maple's modp, where the sign
 *  of b is ignored. It is in agreement with Mathematica's Mod.
 *
 *  @return a mod b in the range [0,abs(b)-1] with sign of b if both are
 *  integer, 0 otherwise. */
numeric mod(numeric const & a, numeric const & b)
{
    if (a.is_integer() && b.is_integer()) {
        return ::mod(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    }
    else {
        return numZERO();  // Throw?
    }
}

/** Modulus (in symmetric representation).
 *  Equivalent to Maple's mods.
 *
 *  @return a mod b in the range [-iquo(abs(m)-1,2), iquo(abs(m),2)]. */
numeric smod(numeric const & a, numeric const & b)
{
    if (a.is_integer() && b.is_integer()) {
        cl_I b2 = The(cl_I)(ceiling1(The(cl_I)(*b.value) / 2)) - 1;
        return ::mod(The(cl_I)(*a.value) + b2, The(cl_I)(*b.value)) - b2;
    } else {
        return numZERO();  // Throw?
    }
}

/** Numeric integer remainder.
 *  Equivalent to Maple's irem(a,b) as far as sign conventions are concerned.
 *  In general, mod(a,b) has the sign of b or is zero, and irem(a,b) has the
 *  sign of a or is zero.
 *
 *  @return remainder of a/b if both are integer, 0 otherwise. */
numeric irem(numeric const & a, numeric const & b)
{
    if (a.is_integer() && b.is_integer()) {
        return ::rem(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    }
    else {
        return numZERO();  // Throw?
    }
}

/** Numeric integer remainder.
 *  Equivalent to Maple's irem(a,b,'q') it obeyes the relation
 *  irem(a,b,q) == a - q*b.  In general, mod(a,b) has the sign of b or is zero,
 *  and irem(a,b) has the sign of a or is zero.  
 *
 *  @return remainder of a/b and quotient stored in q if both are integer,
 *  0 otherwise. */
numeric irem(numeric const & a, numeric const & b, numeric & q)
{
    if (a.is_integer() && b.is_integer()) {  // -> CLN
        cl_I_div_t rem_quo = truncate2(The(cl_I)(*a.value), The(cl_I)(*b.value));
        q = rem_quo.quotient;
        return rem_quo.remainder;
    }
    else {
        q = numZERO();
        return numZERO();  // Throw?
    }
}

/** Numeric integer quotient.
 *  Equivalent to Maple's iquo as far as sign conventions are concerned.
 *  
 *  @return truncated quotient of a/b if both are integer, 0 otherwise. */
numeric iquo(numeric const & a, numeric const & b)
{
    if (a.is_integer() && b.is_integer()) {
        return truncate1(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    } else {
        return numZERO();  // Throw?
    }
}

/** Numeric integer quotient.
 *  Equivalent to Maple's iquo(a,b,'r') it obeyes the relation
 *  r == a - iquo(a,b,r)*b.
 *
 *  @return truncated quotient of a/b and remainder stored in r if both are
 *  integer, 0 otherwise. */
numeric iquo(numeric const & a, numeric const & b, numeric & r)
{
    if (a.is_integer() && b.is_integer()) {  // -> CLN
        cl_I_div_t rem_quo = truncate2(The(cl_I)(*a.value), The(cl_I)(*b.value));
        r = rem_quo.remainder;
        return rem_quo.quotient;
    } else {
        r = numZERO();
        return numZERO();  // Throw?
    }
}

/** Numeric square root.
 *  If possible, sqrt(z) should respect squares of exact numbers, i.e. sqrt(4)
 *  should return integer 2.
 *
 *  @param z numeric argument
 *  @return square root of z. Branch cut along negative real axis, the negative
 *  real axis itself where imag(z)==0 and real(z)<0 belongs to the upper part
 *  where imag(z)>0. */
numeric sqrt(numeric const & z)
{
    return ::sqrt(*z.value);  // -> CLN
}

/** Integer numeric square root. */
numeric isqrt(numeric const & x)
{
	if (x.is_integer()) {
		cl_I root;
		::isqrt(The(cl_I)(*x.value), &root);	// -> CLN
		return root;
	} else
		return numZERO();  // Throw?
}

/** Greatest Common Divisor.
 *   
 *  @return  The GCD of two numbers if both are integer, a numerical 1
 *  if they are not. */
numeric gcd(numeric const & a, numeric const & b)
{
    if (a.is_integer() && b.is_integer())
        return ::gcd(The(cl_I)(*a.value), The(cl_I)(*b.value));	// -> CLN
    else
        return numONE();
}

/** Least Common Multiple.
 *   
 *  @return  The LCM of two numbers if both are integer, the product of those
 *  two numbers if they are not. */
numeric lcm(numeric const & a, numeric const & b)
{
    if (a.is_integer() && b.is_integer())
        return ::lcm(The(cl_I)(*a.value), The(cl_I)(*b.value));	// -> CLN
    else
        return *a.value * *b.value;
}

ex PiEvalf(void)
{ 
    return numeric(cl_pi(cl_default_float_format));  // -> CLN
}

ex EulerGammaEvalf(void)
{ 
    return numeric(cl_eulerconst(cl_default_float_format));  // -> CLN
}

ex CatalanEvalf(void)
{
    return numeric(cl_catalanconst(cl_default_float_format));  // -> CLN
}

// It initializes to 17 digits, because in CLN cl_float_format(17) turns out to
// be 61 (<64) while cl_float_format(18)=65.  We want to have a cl_LF instead 
// of cl_SF, cl_FF or cl_DF but everything else is basically arbitrary.
_numeric_digits::_numeric_digits()
    : digits(17)
{
    assert(!too_late);
    too_late = true;
    cl_default_float_format = cl_float_format(17); 
}

_numeric_digits& _numeric_digits::operator=(long prec)
{
    digits=prec;
    cl_default_float_format = cl_float_format(prec); 
    return *this;
}

_numeric_digits::operator long()
{
    return (long)digits;
}

void _numeric_digits::print(ostream & os) const
{
    debugmsg("_numeric_digits print", LOGLEVEL_PRINT);
    os << digits;
}

ostream& operator<<(ostream& os, _numeric_digits const & e)
{
    e.print(os);
    return os;
}

//////////
// static member variables
//////////

// private

bool _numeric_digits::too_late = false;

/** Accuracy in decimal digits.  Only object of this type!  Can be set using
 *  assignment from C++ unsigned ints and evaluated like any built-in type. */
_numeric_digits Digits;

} // namespace GiNaC
