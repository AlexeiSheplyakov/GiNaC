/** @file numeric.cpp
 *
 *  This file contains the interface to the underlying bignum package.
 *  Its most important design principle is to completely hide the inner
 *  working of that other package from the user of GiNaC.  It must either 
 *  provide implementation of arithmetic operators and numerical evaluation
 *  of special functions or implement the interface to the bignum package. */

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

#include "config.h"

#include <vector>
#include <stdexcept>
#include <string>

#if defined(HAVE_SSTREAM)
#include <sstream>
#elif defined(HAVE_STRSTREAM)
#include <strstream>
#else
#error Need either sstream or strstream
#endif

#include "numeric.h"
#include "ex.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

// CLN should not pollute the global namespace, hence we include it here
// instead of in some header file where it would propagate to other parts.
// Also, we only need a subset of CLN, so we don't include the complete cln.h:
#ifdef HAVE_CLN_CLN_H
#include <cln/cl_output.h>
#include <cln/cl_integer_io.h>
#include <cln/cl_integer_ring.h>
#include <cln/cl_rational_io.h>
#include <cln/cl_rational_ring.h>
#include <cln/cl_lfloat_class.h>
#include <cln/cl_lfloat_io.h>
#include <cln/cl_real_io.h>
#include <cln/cl_real_ring.h>
#include <cln/cl_complex_io.h>
#include <cln/cl_complex_ring.h>
#include <cln/cl_numtheory.h>
#else  // def HAVE_CLN_CLN_H
#include <cl_output.h>
#include <cl_integer_io.h>
#include <cl_integer_ring.h>
#include <cl_rational_io.h>
#include <cl_rational_ring.h>
#include <cl_lfloat_class.h>
#include <cl_lfloat_io.h>
#include <cl_real_io.h>
#include <cl_real_ring.h>
#include <cl_complex_io.h>
#include <cl_complex_ring.h>
#include <cl_numtheory.h>
#endif  // def HAVE_CLN_CLN_H

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif  // ndef NO_NAMESPACE_GINAC

// linker has no problems finding text symbols for numerator or denominator
//#define SANE_LINKER

GINAC_IMPLEMENT_REGISTERED_CLASS(numeric, basic)

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
    *value = cl_I(0);
    calchash();
    setflag(status_flags::evaluated |
            status_flags::expanded |
            status_flags::hash_calculated);
}

numeric::~numeric()
{
    debugmsg("numeric destructor" ,LOGLEVEL_DESTRUCT);
    destroy(0);
}

numeric::numeric(const numeric & other)
{
    debugmsg("numeric copy constructor", LOGLEVEL_CONSTRUCT);
    copy(other);
}

const numeric & numeric::operator=(const numeric & other)
{
    debugmsg("numeric operator=", LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void numeric::copy(const numeric & other)
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
    // first.  This is due to the behaviour of the cl_I-ctor, which
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
    // first.  This is due to the behaviour of the cl_I-ctor, which
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


numeric::numeric(const char *s) : basic(TINFO_numeric)
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
numeric::numeric(const cl_N & z) : basic(TINFO_numeric)
{
    debugmsg("numeric constructor from cl_N", LOGLEVEL_CONSTRUCT);
    value = new cl_N(z);
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
numeric::numeric(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("numeric constructor from archive_node", LOGLEVEL_CONSTRUCT);
    value = new cl_N;
#ifdef HAVE_SSTREAM
    // Read number as string
    string str;
    if (n.find_string("number", str)) {
        istringstream s(str);
        cl_idecoded_float re, im;
        char c;
        s.get(c);
        switch (c) {
            case 'N':    // Ordinary number
            case 'R':    // Integer-decoded real number
                s >> re.sign >> re.mantissa >> re.exponent;
                *value = re.sign * re.mantissa * ::expt(cl_float(2.0, cl_default_float_format), re.exponent);
                break;
            case 'C':    // Integer-decoded complex number
                s >> re.sign >> re.mantissa >> re.exponent;
                s >> im.sign >> im.mantissa >> im.exponent;
                *value = ::complex(re.sign * re.mantissa * ::expt(cl_float(2.0, cl_default_float_format), re.exponent),
                                 im.sign * im.mantissa * ::expt(cl_float(2.0, cl_default_float_format), im.exponent));
                break;
            default:	// Ordinary number
				s.putback(c);
                s >> *value;
                break;
        }
    }
#else
    // Read number as string
    string str;
    if (n.find_string("number", str)) {
        istrstream f(str.c_str(), str.size() + 1);
        cl_idecoded_float re, im;
        char c;
        f.get(c);
        switch (c) {
            case 'R':    // Integer-decoded real number
                f >> re.sign >> re.mantissa >> re.exponent;
                *value = re.sign * re.mantissa * ::expt(cl_float(2.0, cl_default_float_format), re.exponent);
                break;
            case 'C':    // Integer-decoded complex number
                f >> re.sign >> re.mantissa >> re.exponent;
                f >> im.sign >> im.mantissa >> im.exponent;
                *value = ::complex(re.sign * re.mantissa * ::expt(cl_float(2.0, cl_default_float_format), re.exponent),
                                 im.sign * im.mantissa * ::expt(cl_float(2.0, cl_default_float_format), im.exponent));
                break;
            default:	// Ordinary number
				f.putback(c);
                f >> *value;
				break;
        }
    }
#endif
    calchash();
    setflag(status_flags::evaluated|
            status_flags::hash_calculated);
}

/** Unarchive the object. */
ex numeric::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new numeric(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void numeric::archive(archive_node &n) const
{
    inherited::archive(n);
#ifdef HAVE_SSTREAM
    // Write number as string
    ostringstream s;
    if (this->is_crational())
        s << *value;
    else {
        // Non-rational numbers are written in an integer-decoded format
        // to preserve the precision
        if (this->is_real()) {
            cl_idecoded_float re = integer_decode_float(The(cl_F)(*value));
            s << "R";
            s << re.sign << " " << re.mantissa << " " << re.exponent;
        } else {
            cl_idecoded_float re = integer_decode_float(The(cl_F)(::realpart(*value)));
            cl_idecoded_float im = integer_decode_float(The(cl_F)(::imagpart(*value)));
            s << "C";
            s << re.sign << " " << re.mantissa << " " << re.exponent << " ";
            s << im.sign << " " << im.mantissa << " " << im.exponent;
        }
    }
    n.add_string("number", s.str());
#else
    // Write number as string
    char buf[1024];
    ostrstream f(buf, 1024);
    if (this->is_crational())
        f << *value << ends;
    else {
        // Non-rational numbers are written in an integer-decoded format
        // to preserve the precision
        if (this->is_real()) {
            cl_idecoded_float re = integer_decode_float(The(cl_F)(*value));
            f << "R";
            f << re.sign << " " << re.mantissa << " " << re.exponent << ends;
        } else {
            cl_idecoded_float re = integer_decode_float(The(cl_F)(::realpart(*value)));
            cl_idecoded_float im = integer_decode_float(The(cl_F)(::imagpart(*value)));
            f << "C";
            f << re.sign << " " << re.mantissa << " " << re.exponent << " ";
            f << im.sign << " " << im.mantissa << " " << im.exponent << ends;
        }
    }
    string str(buf);
    n.add_string("number", str);
#endif
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


/** Helper function to print a real number in a nicer way than is CLN's
 *  default.  Instead of printing 42.0L0 this just prints 42.0 to ostream os
 *  and instead of 3.99168L7 it prints 3.99168E7.  This is fine in GiNaC as
 *  long as it only uses cl_LF and no other floating point types.
 *
 *  @see numeric::print() */
void print_real_number(ostream & os, const cl_R & num)
{
    cl_print_flags ourflags;
    if (::instanceof(num, ::cl_RA_ring)) {
        // case 1: integer or rational, nothing special to do:
        ::print_real(os, ourflags, num);
    } else {
        // case 2: float
        // make CLN believe this number has default_float_format, so it prints
        // 'E' as exponent marker instead of 'L':
        ourflags.default_float_format = ::cl_float_format(The(cl_F)(num));
        ::print_real(os, ourflags, num);
    }
    return;
}

/** This method adds to the output so it blends more consistently together
 *  with the other routines and produces something compatible to ginsh input.
 *  
 *  @see print_real_number() */
void numeric::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("numeric print", LOGLEVEL_PRINT);
    if (this->is_real()) {
        // case 1, real:  x  or  -x
        if ((precedence<=upper_precedence) && (!this->is_nonneg_integer())) {
            os << "(";
            print_real_number(os, The(cl_R)(*value));
            os << ")";
        } else {
            print_real_number(os, The(cl_R)(*value));
        }
    } else {
        // case 2, imaginary:  y*I  or  -y*I
        if (::realpart(*value) == 0) {
            if ((precedence<=upper_precedence) && (::imagpart(*value) < 0)) {
                if (::imagpart(*value) == -1) {
                    os << "(-I)";
                } else {
                    os << "(";
                    print_real_number(os, The(cl_R)(::imagpart(*value)));
                    os << "*I)";
                }
            } else {
                if (::imagpart(*value) == 1) {
                    os << "I";
                } else {
                    if (::imagpart (*value) == -1) {
                        os << "-I";
                    } else {
                        print_real_number(os, The(cl_R)(::imagpart(*value)));
                        os << "*I";
                    }
                }
            }
        } else {
            // case 3, complex:  x+y*I  or  x-y*I  or  -x+y*I  or  -x-y*I
            if (precedence <= upper_precedence)
                os << "(";
            print_real_number(os, The(cl_R)(::realpart(*value)));
            if (::imagpart(*value) < 0) {
                if (::imagpart(*value) == -1) {
                    os << "-I";
                } else {
                    print_real_number(os, The(cl_R)(::imagpart(*value)));
                    os << "*I";
                }
            } else {
                if (::imagpart(*value) == 1) {
                    os << "+I";
                } else {
                    os << "+";
                    print_real_number(os, The(cl_R)(::imagpart(*value)));
                    os << "*I";
                }
            }
            if (precedence <= upper_precedence)
                os << ")";
        }
    }
}


void numeric::printraw(ostream & os) const
{
    // The method printraw doesn't do much, it simply uses CLN's operator<<()
    // for output, which is ugly but reliable. e.g: 2+2i
    debugmsg("numeric printraw", LOGLEVEL_PRINT);
    os << "numeric(" << *value << ")";
}


void numeric::printtree(ostream & os, unsigned indent) const
{
    debugmsg("numeric printtree", LOGLEVEL_PRINT);
    os << string(indent,' ') << *value
       << " (numeric): "
       << "hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}


void numeric::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("numeric print csrc", LOGLEVEL_PRINT);
    ios::fmtflags oldflags = os.flags();
    os.setf(ios::scientific);
    if (this->is_rational() && !this->is_integer()) {
        if (compare(_num0()) > 0) {
            os << "(";
            if (type == csrc_types::ctype_cl_N)
                os << "cl_F(\"" << numer().evalf() << "\")";
            else
                os << numer().to_double();
        } else {
            os << "-(";
            if (type == csrc_types::ctype_cl_N)
                os << "cl_F(\"" << -numer().evalf() << "\")";
            else
                os << -numer().to_double();
        }
        os << "/";
        if (type == csrc_types::ctype_cl_N)
            os << "cl_F(\"" << denom().evalf() << "\")";
        else
            os << denom().to_double();
        os << ")";
    } else {
        if (type == csrc_types::ctype_cl_N)
            os << "cl_F(\"" << evalf() << "\")";
        else
            os << to_double();
    }
    os.flags(oldflags);
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
    case info_flags::crational:
    case info_flags::crational_polynomial:
        return is_crational();
    case info_flags::integer:
    case info_flags::integer_polynomial:
        return is_integer();
    case info_flags::cinteger:
    case info_flags::cinteger_polynomial:
        return is_cinteger();
    case info_flags::positive:
        return is_positive();
    case info_flags::negative:
        return is_negative();
    case info_flags::nonnegative:
        return !is_negative();
    case info_flags::posint:
        return is_pos_integer();
    case info_flags::negint:
        return is_integer() && is_negative();
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

/** Disassemble real part and imaginary part to scan for the occurrence of a
 *  single number.  Also handles the imaginary unit.  It ignores the sign on
 *  both this and the argument, which may lead to what might appear as funny
 *  results:  (2+I).has(-2) -> true.  But this is consistent, since we also
 *  would like to have (-2+I).has(2) -> true and we want to think about the
 *  sign as a multiplicative factor. */
bool numeric::has(const ex & other) const
{
    if (!is_exactly_of_type(*other.bp, numeric))
        return false;
    const numeric & o = static_cast<numeric &>(const_cast<basic &>(*other.bp));
    if (this->is_equal(o) || this->is_equal(-o))
        return true;
    if (o.imag().is_zero())  // e.g. scan for 3 in -3*I
        return (this->real().is_equal(o) || this->imag().is_equal(o) ||
                this->real().is_equal(-o) || this->imag().is_equal(-o));
    else {
        if (o.is_equal(I))  // e.g scan for I in 42*I
            return !this->is_real();
        if (o.real().is_zero())  // e.g. scan for 2*I in 2*I+1
            return (this->real().has(o*I) || this->imag().has(o*I) ||
                    this->real().has(-o*I) || this->imag().has(-o*I));
    }
    return false;
}


/** Evaluation of numbers doesn't do anything at all. */
ex numeric::eval(int level) const
{
    // Warning: if this is ever gonna do something, the ex ctors from all kinds
    // of numbers should be checking for status_flags::evaluated.
    return this->hold();
}


/** Cast numeric into a floating-point object.  For example exact numeric(1) is
 *  returned as a 1.0000000000000000000000 and so on according to how Digits is
 *  currently set.
 *
 *  @param level  ignored, but needed for overriding basic::evalf.
 *  @return  an ex-handle to a numeric. */
ex numeric::evalf(int level) const
{
    // level can safely be discarded for numeric objects.
    return numeric(::cl_float(1.0, ::cl_default_float_format) * (*value));  // -> CLN
}

// protected

/** Implementation of ex::diff() for a numeric. It always returns 0.
 *
 *  @see ex::diff */
ex numeric::derivative(const symbol & s) const
{
    return _ex0();
}


int numeric::compare_same_type(const basic & other) const
{
    GINAC_ASSERT(is_exactly_of_type(other, numeric));
    const numeric & o = static_cast<numeric &>(const_cast<basic &>(other));

    if (*value == *o.value) {
        return 0;
    }

    return compare(o);    
}


bool numeric::is_equal_same_type(const basic & other) const
{
    GINAC_ASSERT(is_exactly_of_type(other,numeric));
    const numeric *o = static_cast<const numeric *>(&other);
    
    return this->is_equal(*o);
}

unsigned numeric::calchash(void) const
{
    return (hashvalue=cl_equal_hashcode(*value) | 0x80000000U);
    /*
    cout << *value << "->" << hashvalue << endl;
    hashvalue=HASHVALUE_NUMERIC+1000U;
    return HASHVALUE_NUMERIC+1000U;
    */
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
numeric numeric::add(const numeric & other) const
{
    return numeric((*value)+(*other.value));
}

/** Numerical subtraction method.  Subtracts argument from *this and returns
 *  result as a new numeric object. */
numeric numeric::sub(const numeric & other) const
{
    return numeric((*value)-(*other.value));
}

/** Numerical multiplication method.  Multiplies *this and argument and returns
 *  result as a new numeric object. */
numeric numeric::mul(const numeric & other) const
{
    static const numeric * _num1p=&_num1();
    if (this==_num1p) {
        return other;
    } else if (&other==_num1p) {
        return *this;
    }
    return numeric((*value)*(*other.value));
}

/** Numerical division method.  Divides *this by argument and returns result as
 *  a new numeric object.
 *
 *  @exception overflow_error (division by zero) */
numeric numeric::div(const numeric & other) const
{
    if (::zerop(*other.value))
        throw (std::overflow_error("division by zero"));
    return numeric((*value)/(*other.value));
}

numeric numeric::power(const numeric & other) const
{
    static const numeric * _num1p = &_num1();
    if (&other==_num1p)
        return *this;
    if (::zerop(*value)) {
        if (::zerop(*other.value))
            throw (std::domain_error("numeric::eval(): pow(0,0) is undefined"));
        else if (::zerop(::realpart(*other.value)))
            throw (std::domain_error("numeric::eval(): pow(0,I) is undefined"));
        else if (::minusp(::realpart(*other.value)))
            throw (std::overflow_error("numeric::eval(): division by zero"));
        else
            return _num0();
    }
    return numeric(::expt(*value,*other.value));
}

/** Inverse of a number. */
numeric numeric::inverse(void) const
{
    return numeric(::recip(*value));  // -> CLN
}

const numeric & numeric::add_dyn(const numeric & other) const
{
    return static_cast<const numeric &>((new numeric((*value)+(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

const numeric & numeric::sub_dyn(const numeric & other) const
{
    return static_cast<const numeric &>((new numeric((*value)-(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

const numeric & numeric::mul_dyn(const numeric & other) const
{
    static const numeric * _num1p=&_num1();
    if (this==_num1p) {
        return other;
    } else if (&other==_num1p) {
        return *this;
    }
    return static_cast<const numeric &>((new numeric((*value)*(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

const numeric & numeric::div_dyn(const numeric & other) const
{
    if (::zerop(*other.value))
        throw (std::overflow_error("division by zero"));
    return static_cast<const numeric &>((new numeric((*value)/(*other.value)))->
                                        setflag(status_flags::dynallocated));
}

const numeric & numeric::power_dyn(const numeric & other) const
{
    static const numeric * _num1p=&_num1();
    if (&other==_num1p)
        return *this;
    if (::zerop(*value)) {
        if (::zerop(*other.value))
            throw (std::domain_error("numeric::eval(): pow(0,0) is undefined"));
        else if (::zerop(::realpart(*other.value)))
            throw (std::domain_error("numeric::eval(): pow(0,I) is undefined"));
        else if (::minusp(::realpart(*other.value)))
            throw (std::overflow_error("numeric::eval(): division by zero"));
        else
            return _num0();
    }
    return static_cast<const numeric &>((new numeric(::expt(*value,*other.value)))->
                                        setflag(status_flags::dynallocated));
}

const numeric & numeric::operator=(int i)
{
    return operator=(numeric(i));
}

const numeric & numeric::operator=(unsigned int i)
{
    return operator=(numeric(i));
}

const numeric & numeric::operator=(long i)
{
    return operator=(numeric(i));
}

const numeric & numeric::operator=(unsigned long i)
{
    return operator=(numeric(i));
}

const numeric & numeric::operator=(double d)
{
    return operator=(numeric(d));
}

const numeric & numeric::operator=(const char * s)
{
    return operator=(numeric(s));
}

/** Return the complex half-plane (left or right) in which the number lies.
 *  csgn(x)==0 for x==0, csgn(x)==1 for Re(x)>0 or Re(x)=0 and Im(x)>0,
 *  csgn(x)==-1 for Re(x)<0 or Re(x)=0 and Im(x)<0.
 *
 *  @see numeric::compare(const numeric & other) */
int numeric::csgn(void) const
{
    if (this->is_zero())
        return 0;
    if (!::zerop(::realpart(*value))) {
        if (::plusp(::realpart(*value)))
            return 1;
        else
            return -1;
    } else {
        if (::plusp(::imagpart(*value)))
            return 1;
        else
            return -1;
    }
}

/** This method establishes a canonical order on all numbers.  For complex
 *  numbers this is not possible in a mathematically consistent way but we need
 *  to establish some order and it ought to be fast.  So we simply define it
 *  to be compatible with our method csgn.
 *
 *  @return csgn(*this-other)
 *  @see numeric::csgn(void) */
int numeric::compare(const numeric & other) const
{
    // Comparing two real numbers?
    if (this->is_real() && other.is_real())
        // Yes, just compare them
        return ::cl_compare(The(cl_R)(*value), The(cl_R)(*other.value));    
    else {
        // No, first compare real parts
        cl_signean real_cmp = ::cl_compare(::realpart(*value), ::realpart(*other.value));
        if (real_cmp)
            return real_cmp;

        return ::cl_compare(::imagpart(*value), ::imagpart(*other.value));
    }
}

bool numeric::is_equal(const numeric & other) const
{
    return (*value == *other.value);
}

/** True if object is zero. */
bool numeric::is_zero(void) const
{
    return ::zerop(*value);  // -> CLN
}

/** True if object is not complex and greater than zero. */
bool numeric::is_positive(void) const
{
    if (this->is_real())
        return ::plusp(The(cl_R)(*value));  // -> CLN
    return false;
}

/** True if object is not complex and less than zero. */
bool numeric::is_negative(void) const
{
    if (this->is_real())
        return ::minusp(The(cl_R)(*value));  // -> CLN
    return false;
}

/** True if object is a non-complex integer. */
bool numeric::is_integer(void) const
{
    return ::instanceof(*value, ::cl_I_ring);  // -> CLN
}

/** True if object is an exact integer greater than zero. */
bool numeric::is_pos_integer(void) const
{
    return (this->is_integer() && ::plusp(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact integer greater or equal zero. */
bool numeric::is_nonneg_integer(void) const
{
    return (this->is_integer() && !::minusp(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact even integer. */
bool numeric::is_even(void) const
{
    return (this->is_integer() && ::evenp(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact odd integer. */
bool numeric::is_odd(void) const
{
    return (this->is_integer() && ::oddp(The(cl_I)(*value)));  // -> CLN
}

/** Probabilistic primality test.
 *
 *  @return  true if object is exact integer and prime. */
bool numeric::is_prime(void) const
{
    return (this->is_integer() && ::isprobprime(The(cl_I)(*value)));  // -> CLN
}

/** True if object is an exact rational number, may even be complex
 *  (denominator may be unity). */
bool numeric::is_rational(void) const
{
    return ::instanceof(*value, ::cl_RA_ring);  // -> CLN
}

/** True if object is a real integer, rational or float (but not complex). */
bool numeric::is_real(void) const
{
    return ::instanceof(*value, ::cl_R_ring);  // -> CLN
}

bool numeric::operator==(const numeric & other) const
{
    return (*value == *other.value);  // -> CLN
}

bool numeric::operator!=(const numeric & other) const
{
    return (*value != *other.value);  // -> CLN
}

/** True if object is element of the domain of integers extended by I, i.e. is
 *  of the form a+b*I, where a and b are integers. */
bool numeric::is_cinteger(void) const
{
    if (::instanceof(*value, ::cl_I_ring))
        return true;
    else if (!this->is_real()) {  // complex case, handle n+m*I
        if (::instanceof(::realpart(*value), ::cl_I_ring) &&
            ::instanceof(::imagpart(*value), ::cl_I_ring))
            return true;
    }
    return false;
}

/** True if object is an exact rational number, may even be complex
 *  (denominator may be unity). */
bool numeric::is_crational(void) const
{
    if (::instanceof(*value, ::cl_RA_ring))
        return true;
    else if (!this->is_real()) {  // complex case, handle Q(i):
        if (::instanceof(::realpart(*value), ::cl_RA_ring) &&
            ::instanceof(::imagpart(*value), ::cl_RA_ring))
            return true;
    }
    return false;
}

/** Numerical comparison: less.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator<(const numeric & other) const
{
    if (this->is_real() && other.is_real())
        return (The(cl_R)(*value) < The(cl_R)(*other.value));  // -> CLN
    throw (std::invalid_argument("numeric::operator<(): complex inequality"));
    return false;  // make compiler shut up
}

/** Numerical comparison: less or equal.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator<=(const numeric & other) const
{
    if (this->is_real() && other.is_real())
        return (The(cl_R)(*value) <= The(cl_R)(*other.value));  // -> CLN
    throw (std::invalid_argument("numeric::operator<=(): complex inequality"));
    return false;  // make compiler shut up
}

/** Numerical comparison: greater.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator>(const numeric & other) const
{
    if (this->is_real() && other.is_real())
        return (The(cl_R)(*value) > The(cl_R)(*other.value));  // -> CLN
    throw (std::invalid_argument("numeric::operator>(): complex inequality"));
    return false;  // make compiler shut up
}

/** Numerical comparison: greater or equal.
 *
 *  @exception invalid_argument (complex inequality) */  
bool numeric::operator>=(const numeric & other) const
{
    if (this->is_real() && other.is_real())
        return (The(cl_R)(*value) >= The(cl_R)(*other.value));  // -> CLN
    throw (std::invalid_argument("numeric::operator>=(): complex inequality"));
    return false;  // make compiler shut up
}

/** Converts numeric types to machine's int.  You should check with
 *  is_integer() if the number is really an integer before calling this method.
 *  You may also consider checking the range first. */
int numeric::to_int(void) const
{
    GINAC_ASSERT(this->is_integer());
    return ::cl_I_to_int(The(cl_I)(*value));  // -> CLN
}

/** Converts numeric types to machine's long.  You should check with
 *  is_integer() if the number is really an integer before calling this method.
 *  You may also consider checking the range first. */
long numeric::to_long(void) const
{
    GINAC_ASSERT(this->is_integer());
    return ::cl_I_to_long(The(cl_I)(*value));  // -> CLN
}

/** Converts numeric types to machine's double. You should check with is_real()
 *  if the number is really not complex before calling this method. */
double numeric::to_double(void) const
{
    GINAC_ASSERT(this->is_real());
    return ::cl_double_approx(::realpart(*value));  // -> CLN
}

/** Real part of a number. */
const numeric numeric::real(void) const
{
    return numeric(::realpart(*value));  // -> CLN
}

/** Imaginary part of a number. */
const numeric numeric::imag(void) const
{
    return numeric(::imagpart(*value));  // -> CLN
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
 *  (i.e numer(4/3+5/6*I) == 8+5*I), the number carrying the sign in all other
 *  cases. */
const numeric numeric::numer(void) const
{
    if (this->is_integer()) {
        return numeric(*this);
    }
#ifdef SANE_LINKER
    else if (::instanceof(*value, ::cl_RA_ring)) {
        return numeric(::numerator(The(cl_RA)(*value)));
    }
    else if (!this->is_real()) {  // complex case, handle Q(i):
        cl_R r = ::realpart(*value);
        cl_R i = ::imagpart(*value);
        if (::instanceof(r, ::cl_I_ring) && ::instanceof(i, ::cl_I_ring))
            return numeric(*this);
        if (::instanceof(r, ::cl_I_ring) && ::instanceof(i, ::cl_RA_ring))
            return numeric(::complex(r*::denominator(The(cl_RA)(i)), ::numerator(The(cl_RA)(i))));
        if (::instanceof(r, ::cl_RA_ring) && ::instanceof(i, ::cl_I_ring))
            return numeric(::complex(::numerator(The(cl_RA)(r)), i*::denominator(The(cl_RA)(r))));
        if (::instanceof(r, ::cl_RA_ring) && ::instanceof(i, ::cl_RA_ring)) {
            cl_I s = ::lcm(::denominator(The(cl_RA)(r)), ::denominator(The(cl_RA)(i)));
            return numeric(::complex(::numerator(The(cl_RA)(r))*(exquo(s,::denominator(The(cl_RA)(r)))),
                                   ::numerator(The(cl_RA)(i))*(exquo(s,::denominator(The(cl_RA)(i))))));
        }
    }
#else
    else if (instanceof(*value, ::cl_RA_ring)) {
        return numeric(TheRatio(*value)->numerator);
    }
    else if (!this->is_real()) {  // complex case, handle Q(i):
        cl_R r = ::realpart(*value);
        cl_R i = ::imagpart(*value);
        if (instanceof(r, ::cl_I_ring) && instanceof(i, ::cl_I_ring))
            return numeric(*this);
        if (instanceof(r, ::cl_I_ring) && instanceof(i, ::cl_RA_ring))
            return numeric(::complex(r*TheRatio(i)->denominator, TheRatio(i)->numerator));
        if (instanceof(r, ::cl_RA_ring) && instanceof(i, ::cl_I_ring))
            return numeric(::complex(TheRatio(r)->numerator, i*TheRatio(r)->denominator));
        if (instanceof(r, ::cl_RA_ring) && instanceof(i, ::cl_RA_ring)) {
            cl_I s = ::lcm(TheRatio(r)->denominator, TheRatio(i)->denominator);
            return numeric(::complex(TheRatio(r)->numerator*(exquo(s,TheRatio(r)->denominator)),
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
const numeric numeric::denom(void) const
{
    if (this->is_integer()) {
        return _num1();
    }
#ifdef SANE_LINKER
    if (instanceof(*value, ::cl_RA_ring)) {
        return numeric(::denominator(The(cl_RA)(*value)));
    }
    if (!this->is_real()) {  // complex case, handle Q(i):
        cl_R r = ::realpart(*value);
        cl_R i = ::imagpart(*value);
        if (::instanceof(r, ::cl_I_ring) && ::instanceof(i, ::cl_I_ring))
            return _num1();
        if (::instanceof(r, ::cl_I_ring) && ::instanceof(i, ::cl_RA_ring))
            return numeric(::denominator(The(cl_RA)(i)));
        if (::instanceof(r, ::cl_RA_ring) && ::instanceof(i, ::cl_I_ring))
            return numeric(::denominator(The(cl_RA)(r)));
        if (::instanceof(r, ::cl_RA_ring) && ::instanceof(i, ::cl_RA_ring))
            return numeric(::lcm(::denominator(The(cl_RA)(r)), ::denominator(The(cl_RA)(i))));
    }
#else
    if (instanceof(*value, ::cl_RA_ring)) {
        return numeric(TheRatio(*value)->denominator);
    }
    if (!this->is_real()) {  // complex case, handle Q(i):
        cl_R r = ::realpart(*value);
        cl_R i = ::imagpart(*value);
        if (instanceof(r, ::cl_I_ring) && instanceof(i, ::cl_I_ring))
            return _num1();
        if (instanceof(r, ::cl_I_ring) && instanceof(i, ::cl_RA_ring))
            return numeric(TheRatio(i)->denominator);
        if (instanceof(r, ::cl_RA_ring) && instanceof(i, ::cl_I_ring))
            return numeric(TheRatio(r)->denominator);
        if (instanceof(r, ::cl_RA_ring) && instanceof(i, ::cl_RA_ring))
            return numeric(::lcm(TheRatio(r)->denominator, TheRatio(i)->denominator));
    }
#endif // def SANE_LINKER
    // at least one float encountered
    return _num1();
}

/** Size in binary notation.  For integers, this is the smallest n >= 0 such
 *  that -2^n <= x < 2^n. If x > 0, this is the unique n > 0 such that
 *  2^(n-1) <= x < 2^n.
 *
 *  @return  number of bits (excluding sign) needed to represent that number
 *  in two's complement if it is an integer, 0 otherwise. */    
int numeric::int_length(void) const
{
    if (this->is_integer())
        return ::integer_length(The(cl_I)(*value));  // -> CLN
    else
        return 0;
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
const type_info & typeid_numeric=typeid(some_numeric);
/** Imaginary unit.  This is not a constant but a numeric since we are
 *  natively handing complex numbers anyways. */
const numeric I = numeric(::complex(cl_I(0),cl_I(1)));


/** Exponential function.
 *
 *  @return  arbitrary precision numerical exp(x). */
const numeric exp(const numeric & x)
{
    return ::exp(*x.value);  // -> CLN
}


/** Natural logarithm.
 *
 *  @param z complex number
 *  @return  arbitrary precision numerical log(x).
 *  @exception overflow_error (logarithmic singularity) */
const numeric log(const numeric & z)
{
    if (z.is_zero())
        throw (std::overflow_error("log(): logarithmic singularity"));
    return ::log(*z.value);  // -> CLN
}


/** Numeric sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical sin(x). */
const numeric sin(const numeric & x)
{
    return ::sin(*x.value);  // -> CLN
}


/** Numeric cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical cos(x). */
const numeric cos(const numeric & x)
{
    return ::cos(*x.value);  // -> CLN
}


/** Numeric tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical tan(x). */
const numeric tan(const numeric & x)
{
    return ::tan(*x.value);  // -> CLN
}
    

/** Numeric inverse sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical asin(x). */
const numeric asin(const numeric & x)
{
    return ::asin(*x.value);  // -> CLN
}


/** Numeric inverse cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical acos(x). */
const numeric acos(const numeric & x)
{
    return ::acos(*x.value);  // -> CLN
}
    

/** Arcustangent.
 *
 *  @param z complex number
 *  @return atan(z)
 *  @exception overflow_error (logarithmic singularity) */
const numeric atan(const numeric & x)
{
    if (!x.is_real() &&
        x.real().is_zero() &&
        !abs(x.imag()).is_equal(_num1()))
        throw (std::overflow_error("atan(): logarithmic singularity"));
    return ::atan(*x.value);  // -> CLN
}


/** Arcustangent.
 *
 *  @param x real number
 *  @param y real number
 *  @return atan(y/x) */
const numeric atan(const numeric & y, const numeric & x)
{
    if (x.is_real() && y.is_real())
        return ::atan(::realpart(*x.value), ::realpart(*y.value));  // -> CLN
    else
        throw (std::invalid_argument("numeric::atan(): complex argument"));        
}


/** Numeric hyperbolic sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical sinh(x). */
const numeric sinh(const numeric & x)
{
    return ::sinh(*x.value);  // -> CLN
}


/** Numeric hyperbolic cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical cosh(x). */
const numeric cosh(const numeric & x)
{
    return ::cosh(*x.value);  // -> CLN
}


/** Numeric hyperbolic tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical tanh(x). */
const numeric tanh(const numeric & x)
{
    return ::tanh(*x.value);  // -> CLN
}
    

/** Numeric inverse hyperbolic sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical asinh(x). */
const numeric asinh(const numeric & x)
{
    return ::asinh(*x.value);  // -> CLN
}


/** Numeric inverse hyperbolic cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical acosh(x). */
const numeric acosh(const numeric & x)
{
    return ::acosh(*x.value);  // -> CLN
}


/** Numeric inverse hyperbolic tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical atanh(x). */
const numeric atanh(const numeric & x)
{
    return ::atanh(*x.value);  // -> CLN
}


/** Numeric evaluation of Riemann's Zeta function.  Currently works only for
 *  integer arguments. */
const numeric zeta(const numeric & x)
{
    // A dirty hack to allow for things like zeta(3.0), since CLN currently
    // only knows about integer arguments and zeta(3).evalf() automatically
    // cascades down to zeta(3.0).evalf().  The trick is to rely on 3.0-3
    // being an exact zero for CLN, which can be tested and then we can just
    // pass the number casted to an int:
    if (x.is_real()) {
        int aux = (int)(::cl_double_approx(::realpart(*x.value)));
        if (zerop(*x.value-aux))
            return ::cl_zeta(aux);  // -> CLN
    }
    clog << "zeta(" << x
         << "): Does anybody know good way to calculate this numerically?"
         << endl;
    return numeric(0);
}


/** The gamma function.
 *  This is only a stub! */
const numeric gamma(const numeric & x)
{
    clog << "gamma(" << x
         << "): Does anybody know good way to calculate this numerically?"
         << endl;
    return numeric(0);
}


/** The psi function (aka polygamma function).
 *  This is only a stub! */
const numeric psi(const numeric & x)
{
    clog << "psi(" << x
         << "): Does anybody know good way to calculate this numerically?"
         << endl;
    return numeric(0);
}


/** The psi functions (aka polygamma functions).
 *  This is only a stub! */
const numeric psi(const numeric & n, const numeric & x)
{
    clog << "psi(" << n << "," << x
         << "): Does anybody know good way to calculate this numerically?"
         << endl;
    return numeric(0);
}


/** Factorial combinatorial function.
 *
 *  @param n  integer argument >= 0
 *  @exception range_error (argument must be integer >= 0) */
const numeric factorial(const numeric & n)
{
    if (!n.is_nonneg_integer())
        throw (std::range_error("numeric::factorial(): argument must be integer >= 0"));
    return numeric(::factorial(n.to_int()));  // -> CLN
}


/** The double factorial combinatorial function.  (Scarcely used, but still
 *  useful in cases, like for exact results of Gamma(n+1/2) for instance.)
 *
 *  @param n  integer argument >= -1
 *  @return n!! == n * (n-2) * (n-4) * ... * ({1|2}) with 0!! == (-1)!! == 1
 *  @exception range_error (argument must be integer >= -1) */
const numeric doublefactorial(const numeric & n)
{
    if (n == numeric(-1)) {
        return _num1();
    }
    if (!n.is_nonneg_integer()) {
        throw (std::range_error("numeric::doublefactorial(): argument must be integer >= -1"));
    }
    return numeric(::doublefactorial(n.to_int()));  // -> CLN
}


/** The Binomial coefficients.  It computes the binomial coefficients.  For
 *  integer n and k and positive n this is the number of ways of choosing k
 *  objects from n distinct objects.  If n is negative, the formula
 *  binomial(n,k) == (-1)^k*binomial(k-n-1,k) is used to compute the result. */
const numeric binomial(const numeric & n, const numeric & k)
{
    if (n.is_integer() && k.is_integer()) {
        if (n.is_nonneg_integer()) {
            if (k.compare(n)!=1 && k.compare(_num0())!=-1)
                return numeric(::binomial(n.to_int(),k.to_int()));  // -> CLN
            else
                return _num0();
        } else {
            return _num_1().power(k)*binomial(k-n-_num1(),k);
        }
    }
    
    // should really be gamma(n+1)/(gamma(r+1)/gamma(n-r+1) or a suitable limit
    throw (std::range_error("numeric::binomial(): don´t know how to evaluate that."));
}


/** Bernoulli number.  The nth Bernoulli number is the coefficient of x^n/n!
 *  in the expansion of the function x/(e^x-1).
 *
 *  @return the nth Bernoulli number (a rational number).
 *  @exception range_error (argument must be integer >= 0) */
const numeric bernoulli(const numeric & nn)
{
    if (!nn.is_integer() || nn.is_negative())
        throw (std::range_error("numeric::bernoulli(): argument must be integer >= 0"));
    if (nn.is_zero())
        return _num1();
    if (!nn.compare(_num1()))
        return numeric(-1,2);
    if (nn.is_odd())
        return _num0();
    // Until somebody has the Blues and comes up with a much better idea and
    // codes it (preferably in CLN) we make this a remembering function which
    // computes its results using the defining formula
    // B(nn) == - 1/(nn+1) * sum_{k=0}^{nn-1}(binomial(nn+1,k)*B(k))
    // whith B(0) == 1.
    // Be warned, though: the Bernoulli numbers are probably computationally 
    // very expensive anyhow and you shouldn't expect miracles to happen.
    static vector<numeric> results;
    static int highest_result = -1;
    int n = nn.sub(_num2()).div(_num2()).to_int();
    if (n <= highest_result)
        return results[n];
    if (results.capacity() < (unsigned)(n+1))
        results.reserve(n+1);
    
    numeric tmp;  // used to store the sum
    for (int i=highest_result+1; i<=n; ++i) {
        // the first two elements:
        tmp = numeric(-2*i-1,2);
        // accumulate the remaining elements:
        for (int j=0; j<i; ++j)
            tmp += binomial(numeric(2*i+3),numeric(j*2+2))*results[j];
        // divide by -(nn+1) and store result:
        results.push_back(-tmp/numeric(2*i+3));
    }
    highest_result=n;
    return results[n];
}


/** Fibonacci number.  The nth Fibonacci number F(n) is defined by the
 *  recurrence formula F(n)==F(n-1)+F(n-2) with F(0)==0 and F(1)==1.
 *
 *  @param n an integer
 *  @return the nth Fibonacci number F(n) (an integer number)
 *  @exception range_error (argument must be an integer) */
const numeric fibonacci(const numeric & n)
{
    if (!n.is_integer())
        throw (std::range_error("numeric::fibonacci(): argument must be integer"));
    // The following addition formula holds:
    //      F(n+m)   = F(m-1)*F(n) + F(m)*F(n+1)  for m >= 1, n >= 0.
    // (Proof: For fixed m, the LHS and the RHS satisfy the same recurrence
    // w.r.t. n, and the initial values (n=0, n=1) agree. Hence all values
    // agree.)
    // Replace m by m+1:
    //      F(n+m+1) = F(m)*F(n) + F(m+1)*F(n+1)      for m >= 0, n >= 0
    // Now put in m = n, to get
    //      F(2n) = (F(n+1)-F(n))*F(n) + F(n)*F(n+1) = F(n)*(2*F(n+1) - F(n))
    //      F(2n+1) = F(n)^2 + F(n+1)^2
    // hence
    //      F(2n+2) = F(n+1)*(2*F(n) + F(n+1))
    if (n.is_zero())
        return _num0();
    if (n.is_negative())
        if (n.is_even())
            return -fibonacci(-n);
        else
            return fibonacci(-n);
    
    cl_I u(0);
    cl_I v(1);
    cl_I m = The(cl_I)(*n.value) >> 1L;  // floor(n/2);
    for (uintL bit=::integer_length(m); bit>0; --bit) {
        // Since a squaring is cheaper than a multiplication, better use
        // three squarings instead of one multiplication and two squarings.
        cl_I u2 = ::square(u);
        cl_I v2 = ::square(v);
        if (::logbitp(bit-1, m)) {
            v = ::square(u + v) - u2;
            u = u2 + v2;
        } else {
            u = v2 - ::square(v - u);
            v = u2 + v2;
        }
    }
    if (n.is_even())
        // Here we don't use the squaring formula because one multiplication
        // is cheaper than two squarings.
        return u * ((v << 1) - u);
    else
        return ::square(u) + ::square(v);    
}


/** Absolute value. */
numeric abs(const numeric & x)
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
numeric mod(const numeric & a, const numeric & b)
{
    if (a.is_integer() && b.is_integer())
        return ::mod(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    else
        return _num0();  // Throw?
}


/** Modulus (in symmetric representation).
 *  Equivalent to Maple's mods.
 *
 *  @return a mod b in the range [-iquo(abs(m)-1,2), iquo(abs(m),2)]. */
numeric smod(const numeric & a, const numeric & b)
{
    if (a.is_integer() && b.is_integer()) {
        cl_I b2 = The(cl_I)(ceiling1(The(cl_I)(*b.value) / 2)) - 1;
        return ::mod(The(cl_I)(*a.value) + b2, The(cl_I)(*b.value)) - b2;
    } else
        return _num0();  // Throw?
}


/** Numeric integer remainder.
 *  Equivalent to Maple's irem(a,b) as far as sign conventions are concerned.
 *  In general, mod(a,b) has the sign of b or is zero, and irem(a,b) has the
 *  sign of a or is zero.
 *
 *  @return remainder of a/b if both are integer, 0 otherwise. */
numeric irem(const numeric & a, const numeric & b)
{
    if (a.is_integer() && b.is_integer())
        return ::rem(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    else
        return _num0();  // Throw?
}


/** Numeric integer remainder.
 *  Equivalent to Maple's irem(a,b,'q') it obeyes the relation
 *  irem(a,b,q) == a - q*b.  In general, mod(a,b) has the sign of b or is zero,
 *  and irem(a,b) has the sign of a or is zero.  
 *
 *  @return remainder of a/b and quotient stored in q if both are integer,
 *  0 otherwise. */
numeric irem(const numeric & a, const numeric & b, numeric & q)
{
    if (a.is_integer() && b.is_integer()) {  // -> CLN
        cl_I_div_t rem_quo = truncate2(The(cl_I)(*a.value), The(cl_I)(*b.value));
        q = rem_quo.quotient;
        return rem_quo.remainder;
    }
    else {
        q = _num0();
        return _num0();  // Throw?
    }
}


/** Numeric integer quotient.
 *  Equivalent to Maple's iquo as far as sign conventions are concerned.
 *  
 *  @return truncated quotient of a/b if both are integer, 0 otherwise. */
numeric iquo(const numeric & a, const numeric & b)
{
    if (a.is_integer() && b.is_integer())
        return truncate1(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    else
        return _num0();  // Throw?
}


/** Numeric integer quotient.
 *  Equivalent to Maple's iquo(a,b,'r') it obeyes the relation
 *  r == a - iquo(a,b,r)*b.
 *
 *  @return truncated quotient of a/b and remainder stored in r if both are
 *  integer, 0 otherwise. */
numeric iquo(const numeric & a, const numeric & b, numeric & r)
{
    if (a.is_integer() && b.is_integer()) {  // -> CLN
        cl_I_div_t rem_quo = truncate2(The(cl_I)(*a.value), The(cl_I)(*b.value));
        r = rem_quo.remainder;
        return rem_quo.quotient;
    } else {
        r = _num0();
        return _num0();  // Throw?
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
numeric sqrt(const numeric & z)
{
    return ::sqrt(*z.value);  // -> CLN
}


/** Integer numeric square root. */
numeric isqrt(const numeric & x)
{
    if (x.is_integer()) {
        cl_I root;
        ::isqrt(The(cl_I)(*x.value), &root);  // -> CLN
        return root;
    } else
        return _num0();  // Throw?
}


/** Greatest Common Divisor.
 *   
 *  @return  The GCD of two numbers if both are integer, a numerical 1
 *  if they are not. */
numeric gcd(const numeric & a, const numeric & b)
{
    if (a.is_integer() && b.is_integer())
        return ::gcd(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    else
        return _num1();
}


/** Least Common Multiple.
 *   
 *  @return  The LCM of two numbers if both are integer, the product of those
 *  two numbers if they are not. */
numeric lcm(const numeric & a, const numeric & b)
{
    if (a.is_integer() && b.is_integer())
        return ::lcm(The(cl_I)(*a.value), The(cl_I)(*b.value));  // -> CLN
    else
        return *a.value * *b.value;
}


/** Floating point evaluation of Archimedes' constant Pi. */
ex PiEvalf(void)
{ 
    return numeric(::cl_pi(cl_default_float_format));  // -> CLN
}


/** Floating point evaluation of Euler's constant Gamma. */
ex EulerGammaEvalf(void)
{ 
    return numeric(::cl_eulerconst(cl_default_float_format));  // -> CLN
}


/** Floating point evaluation of Catalan's constant. */
ex CatalanEvalf(void)
{
    return numeric(::cl_catalanconst(cl_default_float_format));  // -> CLN
}


// It initializes to 17 digits, because in CLN cl_float_format(17) turns out to
// be 61 (<64) while cl_float_format(18)=65.  We want to have a cl_LF instead 
// of cl_SF, cl_FF or cl_DF but everything else is basically arbitrary.
_numeric_digits::_numeric_digits()
    : digits(17)
{
    assert(!too_late);
    too_late = true;
    cl_default_float_format = ::cl_float_format(17);
}


_numeric_digits& _numeric_digits::operator=(long prec)
{
    digits=prec;
    cl_default_float_format = ::cl_float_format(prec); 
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


ostream& operator<<(ostream& os, const _numeric_digits & e)
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

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
