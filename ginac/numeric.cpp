/** @file numeric.cpp
 *
 *  This file contains the interface to the underlying bignum package.
 *  Its most important design principle is to completely hide the inner
 *  working of that other package from the user of GiNaC.  It must either 
 *  provide implementation of arithmetic operators and numerical evaluation
 *  of special functions or implement the interface to the bignum package. */

/*
 *  GiNaC Copyright (C) 1999-2004 Johannes Gutenberg University Mainz, Germany
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
#include <sstream>
#include <limits>

#include "numeric.h"
#include "ex.h"
#include "operators.h"
#include "archive.h"
#include "tostring.h"
#include "utils.h"

// CLN should pollute the global namespace as little as possible.  Hence, we
// include most of it here and include only the part needed for properly
// declaring cln::cl_number in numeric.h.  This can only be safely done in
// namespaced versions of CLN, i.e. version > 1.1.0.  Also, we only need a
// subset of CLN, so we don't include the complete <cln/cln.h> but only the
// essential stuff:
#include <cln/output.h>
#include <cln/integer_io.h>
#include <cln/integer_ring.h>
#include <cln/rational_io.h>
#include <cln/rational_ring.h>
#include <cln/lfloat_class.h>
#include <cln/lfloat_io.h>
#include <cln/real_io.h>
#include <cln/real_ring.h>
#include <cln/complex_io.h>
#include <cln/complex_ring.h>
#include <cln/numtheory.h>

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(numeric, basic,
  print_func<print_context>(&numeric::do_print).
  print_func<print_latex>(&numeric::do_print_latex).
  print_func<print_csrc>(&numeric::do_print_csrc).
  print_func<print_csrc_cl_N>(&numeric::do_print_csrc_cl_N).
  print_func<print_tree>(&numeric::do_print_tree).
  print_func<print_python_repr>(&numeric::do_print_python_repr))

//////////
// default constructor
//////////

/** default ctor. Numerically it initializes to an integer zero. */
numeric::numeric() : basic(TINFO_numeric)
{
	value = cln::cl_I(0);
	setflag(status_flags::evaluated | status_flags::expanded);
}

//////////
// other constructors
//////////

// public

numeric::numeric(int i) : basic(TINFO_numeric)
{
	// Not the whole int-range is available if we don't cast to long
	// first.  This is due to the behaviour of the cl_I-ctor, which
	// emphasizes efficiency.  However, if the integer is small enough
	// we save space and dereferences by using an immediate type.
	// (C.f. <cln/object.h>)
	if (i < (1L << (cl_value_len-1)) && i >= -(1L << (cl_value_len-1)))
		value = cln::cl_I(i);
	else
		value = cln::cl_I(static_cast<long>(i));
	setflag(status_flags::evaluated | status_flags::expanded);
}


numeric::numeric(unsigned int i) : basic(TINFO_numeric)
{
	// Not the whole uint-range is available if we don't cast to ulong
	// first.  This is due to the behaviour of the cl_I-ctor, which
	// emphasizes efficiency.  However, if the integer is small enough
	// we save space and dereferences by using an immediate type.
	// (C.f. <cln/object.h>)
	if (i < (1U << (cl_value_len-1)))
		value = cln::cl_I(i);
	else
		value = cln::cl_I(static_cast<unsigned long>(i));
	setflag(status_flags::evaluated | status_flags::expanded);
}


numeric::numeric(long i) : basic(TINFO_numeric)
{
	value = cln::cl_I(i);
	setflag(status_flags::evaluated | status_flags::expanded);
}


numeric::numeric(unsigned long i) : basic(TINFO_numeric)
{
	value = cln::cl_I(i);
	setflag(status_flags::evaluated | status_flags::expanded);
}


/** Constructor for rational numerics a/b.
 *
 *  @exception overflow_error (division by zero) */
numeric::numeric(long numer, long denom) : basic(TINFO_numeric)
{
	if (!denom)
		throw std::overflow_error("division by zero");
	value = cln::cl_I(numer) / cln::cl_I(denom);
	setflag(status_flags::evaluated | status_flags::expanded);
}


numeric::numeric(double d) : basic(TINFO_numeric)
{
	// We really want to explicitly use the type cl_LF instead of the
	// more general cl_F, since that would give us a cl_DF only which
	// will not be promoted to cl_LF if overflow occurs:
	value = cln::cl_float(d, cln::default_float_format);
	setflag(status_flags::evaluated | status_flags::expanded);
}


/** ctor from C-style string.  It also accepts complex numbers in GiNaC
 *  notation like "2+5*I". */
numeric::numeric(const char *s) : basic(TINFO_numeric)
{
	cln::cl_N ctorval = 0;
	// parse complex numbers (functional but not completely safe, unfortunately
	// std::string does not understand regexpese):
	// ss should represent a simple sum like 2+5*I
	std::string ss = s;
	std::string::size_type delim;

	// make this implementation safe by adding explicit sign
	if (ss.at(0) != '+' && ss.at(0) != '-' && ss.at(0) != '#')
		ss = '+' + ss;

	// We use 'E' as exponent marker in the output, but some people insist on
	// writing 'e' at input, so let's substitute them right at the beginning:
	while ((delim = ss.find("e"))!=std::string::npos)
		ss.replace(delim,1,"E");

	// main parser loop:
	do {
		// chop ss into terms from left to right
		std::string term;
		bool imaginary = false;
		delim = ss.find_first_of(std::string("+-"),1);
		// Do we have an exponent marker like "31.415E-1"?  If so, hop on!
		if (delim!=std::string::npos && ss.at(delim-1)=='E')
			delim = ss.find_first_of(std::string("+-"),delim+1);
		term = ss.substr(0,delim);
		if (delim!=std::string::npos)
			ss = ss.substr(delim);
		// is the term imaginary?
		if (term.find("I")!=std::string::npos) {
			// erase 'I':
			term.erase(term.find("I"),1);
			// erase '*':
			if (term.find("*")!=std::string::npos)
				term.erase(term.find("*"),1);
			// correct for trivial +/-I without explicit factor on I:
			if (term.size()==1)
				term += '1';
			imaginary = true;
		}
		if (term.find('.')!=std::string::npos || term.find('E')!=std::string::npos) {
			// CLN's short type cl_SF is not very useful within the GiNaC
			// framework where we are mainly interested in the arbitrary
			// precision type cl_LF.  Hence we go straight to the construction
			// of generic floats.  In order to create them we have to convert
			// our own floating point notation used for output and construction
			// from char * to CLN's generic notation:
			// 3.14      -->   3.14e0_<Digits>
			// 31.4E-1   -->   31.4e-1_<Digits>
			// and s on.
			// No exponent marker?  Let's add a trivial one.
			if (term.find("E")==std::string::npos)
				term += "E0";
			// E to lower case
			term = term.replace(term.find("E"),1,"e");
			// append _<Digits> to term
			term += "_" + ToString((unsigned)Digits);
			// construct float using cln::cl_F(const char *) ctor.
			if (imaginary)
				ctorval = ctorval + cln::complex(cln::cl_I(0),cln::cl_F(term.c_str()));
			else
				ctorval = ctorval + cln::cl_F(term.c_str());
		} else {
			// this is not a floating point number...
			if (imaginary)
				ctorval = ctorval + cln::complex(cln::cl_I(0),cln::cl_R(term.c_str()));
			else
				ctorval = ctorval + cln::cl_R(term.c_str());
		}
	} while (delim != std::string::npos);
	value = ctorval;
	setflag(status_flags::evaluated | status_flags::expanded);
}


/** Ctor from CLN types.  This is for the initiated user or internal use
 *  only. */
numeric::numeric(const cln::cl_N &z) : basic(TINFO_numeric)
{
	value = z;
	setflag(status_flags::evaluated | status_flags::expanded);
}

//////////
// archiving
//////////

numeric::numeric(const archive_node &n, lst &sym_lst) : inherited(n, sym_lst)
{
	cln::cl_N ctorval = 0;

	// Read number as string
	std::string str;
	if (n.find_string("number", str)) {
		std::istringstream s(str);
		cln::cl_idecoded_float re, im;
		char c;
		s.get(c);
		switch (c) {
			case 'R':    // Integer-decoded real number
				s >> re.sign >> re.mantissa >> re.exponent;
				ctorval = re.sign * re.mantissa * cln::expt(cln::cl_float(2.0, cln::default_float_format), re.exponent);
				break;
			case 'C':    // Integer-decoded complex number
				s >> re.sign >> re.mantissa >> re.exponent;
				s >> im.sign >> im.mantissa >> im.exponent;
				ctorval = cln::complex(re.sign * re.mantissa * cln::expt(cln::cl_float(2.0, cln::default_float_format), re.exponent),
				                       im.sign * im.mantissa * cln::expt(cln::cl_float(2.0, cln::default_float_format), im.exponent));
				break;
			default:    // Ordinary number
				s.putback(c);
				s >> ctorval;
				break;
		}
	}
	value = ctorval;
	setflag(status_flags::evaluated | status_flags::expanded);
}

void numeric::archive(archive_node &n) const
{
	inherited::archive(n);

	// Write number as string
	std::ostringstream s;
	if (this->is_crational())
		s << value;
	else {
		// Non-rational numbers are written in an integer-decoded format
		// to preserve the precision
		if (this->is_real()) {
			cln::cl_idecoded_float re = cln::integer_decode_float(cln::the<cln::cl_F>(value));
			s << "R";
			s << re.sign << " " << re.mantissa << " " << re.exponent;
		} else {
			cln::cl_idecoded_float re = cln::integer_decode_float(cln::the<cln::cl_F>(cln::realpart(cln::the<cln::cl_N>(value))));
			cln::cl_idecoded_float im = cln::integer_decode_float(cln::the<cln::cl_F>(cln::imagpart(cln::the<cln::cl_N>(value))));
			s << "C";
			s << re.sign << " " << re.mantissa << " " << re.exponent << " ";
			s << im.sign << " " << im.mantissa << " " << im.exponent;
		}
	}
	n.add_string("number", s.str());
}

DEFAULT_UNARCHIVE(numeric)

//////////
// functions overriding virtual functions from base classes
//////////

/** Helper function to print a real number in a nicer way than is CLN's
 *  default.  Instead of printing 42.0L0 this just prints 42.0 to ostream os
 *  and instead of 3.99168L7 it prints 3.99168E7.  This is fine in GiNaC as
 *  long as it only uses cl_LF and no other floating point types that we might
 *  want to visibly distinguish from cl_LF.
 *
 *  @see numeric::print() */
static void print_real_number(const print_context & c, const cln::cl_R & x)
{
	cln::cl_print_flags ourflags;
	if (cln::instanceof(x, cln::cl_RA_ring)) {
		// case 1: integer or rational
		if (cln::instanceof(x, cln::cl_I_ring) ||
		    !is_a<print_latex>(c)) {
			cln::print_real(c.s, ourflags, x);
		} else {  // rational output in LaTeX context
			if (x < 0)
				c.s << "-";
			c.s << "\\frac{";
			cln::print_real(c.s, ourflags, cln::abs(cln::numerator(cln::the<cln::cl_RA>(x))));
			c.s << "}{";
			cln::print_real(c.s, ourflags, cln::denominator(cln::the<cln::cl_RA>(x)));
			c.s << '}';
		}
	} else {
		// case 2: float
		// make CLN believe this number has default_float_format, so it prints
		// 'E' as exponent marker instead of 'L':
		ourflags.default_float_format = cln::float_format(cln::the<cln::cl_F>(x));
		cln::print_real(c.s, ourflags, x);
	}
}

/** Helper function to print integer number in C++ source format.
 *
 *  @see numeric::print() */
static void print_integer_csrc(const print_context & c, const cln::cl_I & x)
{
	// Print small numbers in compact float format, but larger numbers in
	// scientific format
	const int max_cln_int = 536870911; // 2^29-1
	if (x >= cln::cl_I(-max_cln_int) && x <= cln::cl_I(max_cln_int))
		c.s << cln::cl_I_to_int(x) << ".0";
	else
		c.s << cln::double_approx(x);
}

/** Helper function to print real number in C++ source format.
 *
 *  @see numeric::print() */
static void print_real_csrc(const print_context & c, const cln::cl_R & x)
{
	if (cln::instanceof(x, cln::cl_I_ring)) {

		// Integer number
		print_integer_csrc(c, cln::the<cln::cl_I>(x));

	} else if (cln::instanceof(x, cln::cl_RA_ring)) {

		// Rational number
		const cln::cl_I numer = cln::numerator(cln::the<cln::cl_RA>(x));
		const cln::cl_I denom = cln::denominator(cln::the<cln::cl_RA>(x));
		if (cln::plusp(x) > 0) {
			c.s << "(";
			print_integer_csrc(c, numer);
		} else {
			c.s << "-(";
			print_integer_csrc(c, -numer);
		}
		c.s << "/";
		print_integer_csrc(c, denom);
		c.s << ")";

	} else {

		// Anything else
		c.s << cln::double_approx(x);
	}
}

/** Helper function to print real number in C++ source format using cl_N types.
 *
 *  @see numeric::print() */
static void print_real_cl_N(const print_context & c, const cln::cl_R & x)
{
	if (cln::instanceof(x, cln::cl_I_ring)) {

		// Integer number
		c.s << "cln::cl_I(\"";
		print_real_number(c, x);
		c.s << "\")";

	} else if (cln::instanceof(x, cln::cl_RA_ring)) {

		// Rational number
		cln::cl_print_flags ourflags;
		c.s << "cln::cl_RA(\"";
		cln::print_rational(c.s, ourflags, cln::the<cln::cl_RA>(x));
		c.s << "\")";

	} else {

		// Anything else
		c.s << "cln::cl_F(\"";
		print_real_number(c, cln::cl_float(1.0, cln::default_float_format) * x);
		c.s << "_" << Digits << "\")";
	}
}

void numeric::print_numeric(const print_context & c, const char *par_open, const char *par_close, const char *imag_sym, const char *mul_sym, unsigned level) const
{
	const cln::cl_R r = cln::realpart(value);
	const cln::cl_R i = cln::imagpart(value);

	if (cln::zerop(i)) {

		// case 1, real:  x  or  -x
		if ((precedence() <= level) && (!this->is_nonneg_integer())) {
			c.s << par_open;
			print_real_number(c, r);
			c.s << par_close;
		} else {
			print_real_number(c, r);
		}

	} else {
		if (cln::zerop(r)) {

			// case 2, imaginary:  y*I  or  -y*I
			if (i == 1)
				c.s << imag_sym;
			else {
				if (precedence()<=level)
					c.s << par_open;
				if (i == -1)
					c.s << "-" << imag_sym;
				else {
					print_real_number(c, i);
					c.s << mul_sym << imag_sym;
				}
				if (precedence()<=level)
					c.s << par_close;
			}

		} else {

			// case 3, complex:  x+y*I  or  x-y*I  or  -x+y*I  or  -x-y*I
			if (precedence() <= level)
				c.s << par_open;
			print_real_number(c, r);
			if (i < 0) {
				if (i == -1) {
					c.s << "-" << imag_sym;
				} else {
					print_real_number(c, i);
					c.s << mul_sym << imag_sym;
				}
			} else {
				if (i == 1) {
					c.s << "+" << imag_sym;
				} else {
					c.s << "+";
					print_real_number(c, i);
					c.s << mul_sym << imag_sym;
				}
			}
			if (precedence() <= level)
				c.s << par_close;
		}
	}
}

void numeric::do_print(const print_context & c, unsigned level) const
{
	print_numeric(c, "(", ")", "I", "*", level);
}

void numeric::do_print_latex(const print_latex & c, unsigned level) const
{
	print_numeric(c, "{(", ")}", "i", " ", level);
}

void numeric::do_print_csrc(const print_csrc & c, unsigned level) const
{
	std::ios::fmtflags oldflags = c.s.flags();
	c.s.setf(std::ios::scientific);
	int oldprec = c.s.precision();

	// Set precision
	if (is_a<print_csrc_double>(c))
		c.s.precision(std::numeric_limits<double>::digits10 + 1);
	else
		c.s.precision(std::numeric_limits<float>::digits10 + 1);

	if (this->is_real()) {

		// Real number
		print_real_csrc(c, cln::the<cln::cl_R>(value));

	} else {

		// Complex number
		c.s << "std::complex<";
		if (is_a<print_csrc_double>(c))
			c.s << "double>(";
		else
			c.s << "float>(";

		print_real_csrc(c, cln::realpart(value));
		c.s << ",";
		print_real_csrc(c, cln::imagpart(value));
		c.s << ")";
	}

	c.s.flags(oldflags);
	c.s.precision(oldprec);
}

void numeric::do_print_csrc_cl_N(const print_csrc_cl_N & c, unsigned level) const
{
	if (this->is_real()) {

		// Real number
		print_real_cl_N(c, cln::the<cln::cl_R>(value));

	} else {

		// Complex number
		c.s << "cln::complex(";
		print_real_cl_N(c, cln::realpart(value));
		c.s << ",";
		print_real_cl_N(c, cln::imagpart(value));
		c.s << ")";
	}
}

void numeric::do_print_tree(const print_tree & c, unsigned level) const
{
	c.s << std::string(level, ' ') << value
	    << " (" << class_name() << ")" << " @" << this
	    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
	    << std::endl;
}

void numeric::do_print_python_repr(const print_python_repr & c, unsigned level) const
{
	c.s << class_name() << "('";
	print_numeric(c, "(", ")", "I", "*", level);
	c.s << "')";
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
		case info_flags::algebraic:
			return !is_real();
	}
	return false;
}

int numeric::degree(const ex & s) const
{
	return 0;
}

int numeric::ldegree(const ex & s) const
{
	return 0;
}

ex numeric::coeff(const ex & s, int n) const
{
	return n==0 ? *this : _ex0;
}

/** Disassemble real part and imaginary part to scan for the occurrence of a
 *  single number.  Also handles the imaginary unit.  It ignores the sign on
 *  both this and the argument, which may lead to what might appear as funny
 *  results:  (2+I).has(-2) -> true.  But this is consistent, since we also
 *  would like to have (-2+I).has(2) -> true and we want to think about the
 *  sign as a multiplicative factor. */
bool numeric::has(const ex &other) const
{
	if (!is_exactly_a<numeric>(other))
		return false;
	const numeric &o = ex_to<numeric>(other);
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
 *  currently set.  In case the object already was a floating point number the
 *  precision is trimmed to match the currently set default.
 *
 *  @param level  ignored, only needed for overriding basic::evalf.
 *  @return  an ex-handle to a numeric. */
ex numeric::evalf(int level) const
{
	// level can safely be discarded for numeric objects.
	return numeric(cln::cl_float(1.0, cln::default_float_format) * value);
}

ex numeric::conjugate() const
{
	if (is_real()) {
		return *this;
	}
	return numeric(cln::conjugate(this->value));
}

// protected

int numeric::compare_same_type(const basic &other) const
{
	GINAC_ASSERT(is_exactly_a<numeric>(other));
	const numeric &o = static_cast<const numeric &>(other);
	
	return this->compare(o);
}


bool numeric::is_equal_same_type(const basic &other) const
{
	GINAC_ASSERT(is_exactly_a<numeric>(other));
	const numeric &o = static_cast<const numeric &>(other);
	
	return this->is_equal(o);
}


unsigned numeric::calchash() const
{
	// Base computation of hashvalue on CLN's hashcode.  Note: That depends
	// only on the number's value, not its type or precision (i.e. a true
	// equivalence relation on numbers).  As a consequence, 3 and 3.0 share
	// the same hashvalue.  That shouldn't really matter, though.
	setflag(status_flags::hash_calculated);
	hashvalue = golden_ratio_hash(cln::equal_hashcode(value));
	return hashvalue;
}


//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// public

/** Numerical addition method.  Adds argument to *this and returns result as
 *  a numeric object. */
const numeric numeric::add(const numeric &other) const
{
	return numeric(value + other.value);
}


/** Numerical subtraction method.  Subtracts argument from *this and returns
 *  result as a numeric object. */
const numeric numeric::sub(const numeric &other) const
{
	return numeric(value - other.value);
}


/** Numerical multiplication method.  Multiplies *this and argument and returns
 *  result as a numeric object. */
const numeric numeric::mul(const numeric &other) const
{
	return numeric(value * other.value);
}


/** Numerical division method.  Divides *this by argument and returns result as
 *  a numeric object.
 *
 *  @exception overflow_error (division by zero) */
const numeric numeric::div(const numeric &other) const
{
	if (cln::zerop(other.value))
		throw std::overflow_error("numeric::div(): division by zero");
	return numeric(value / other.value);
}


/** Numerical exponentiation.  Raises *this to the power given as argument and
 *  returns result as a numeric object. */
const numeric numeric::power(const numeric &other) const
{
	// Shortcut for efficiency and numeric stability (as in 1.0 exponent):
	// trap the neutral exponent.
	if (&other==_num1_p || cln::equal(other.value,_num1.value))
		return *this;
	
	if (cln::zerop(value)) {
		if (cln::zerop(other.value))
			throw std::domain_error("numeric::eval(): pow(0,0) is undefined");
		else if (cln::zerop(cln::realpart(other.value)))
			throw std::domain_error("numeric::eval(): pow(0,I) is undefined");
		else if (cln::minusp(cln::realpart(other.value)))
			throw std::overflow_error("numeric::eval(): division by zero");
		else
			return _num0;
	}
	return numeric(cln::expt(value, other.value));
}



/** Numerical addition method.  Adds argument to *this and returns result as
 *  a numeric object on the heap.  Use internally only for direct wrapping into
 *  an ex object, where the result would end up on the heap anyways. */
const numeric &numeric::add_dyn(const numeric &other) const
{
	// Efficiency shortcut: trap the neutral element by pointer.  This hack
	// is supposed to keep the number of distinct numeric objects low.
	if (this==_num0_p)
		return other;
	else if (&other==_num0_p)
		return *this;
	
	return static_cast<const numeric &>((new numeric(value + other.value))->
	                                    setflag(status_flags::dynallocated));
}


/** Numerical subtraction method.  Subtracts argument from *this and returns
 *  result as a numeric object on the heap.  Use internally only for direct
 *  wrapping into an ex object, where the result would end up on the heap
 *  anyways. */
const numeric &numeric::sub_dyn(const numeric &other) const
{
	// Efficiency shortcut: trap the neutral exponent (first by pointer).  This
	// hack is supposed to keep the number of distinct numeric objects low.
	if (&other==_num0_p || cln::zerop(other.value))
		return *this;
	
	return static_cast<const numeric &>((new numeric(value - other.value))->
	                                    setflag(status_flags::dynallocated));
}


/** Numerical multiplication method.  Multiplies *this and argument and returns
 *  result as a numeric object on the heap.  Use internally only for direct
 *  wrapping into an ex object, where the result would end up on the heap
 *  anyways. */
const numeric &numeric::mul_dyn(const numeric &other) const
{
	// Efficiency shortcut: trap the neutral element by pointer.  This hack
	// is supposed to keep the number of distinct numeric objects low.
	if (this==_num1_p)
		return other;
	else if (&other==_num1_p)
		return *this;
	
	return static_cast<const numeric &>((new numeric(value * other.value))->
	                                    setflag(status_flags::dynallocated));
}


/** Numerical division method.  Divides *this by argument and returns result as
 *  a numeric object on the heap.  Use internally only for direct wrapping
 *  into an ex object, where the result would end up on the heap
 *  anyways.
 *
 *  @exception overflow_error (division by zero) */
const numeric &numeric::div_dyn(const numeric &other) const
{
	// Efficiency shortcut: trap the neutral element by pointer.  This hack
	// is supposed to keep the number of distinct numeric objects low.
	if (&other==_num1_p)
		return *this;
	if (cln::zerop(cln::the<cln::cl_N>(other.value)))
		throw std::overflow_error("division by zero");
	return static_cast<const numeric &>((new numeric(value / other.value))->
	                                    setflag(status_flags::dynallocated));
}


/** Numerical exponentiation.  Raises *this to the power given as argument and
 *  returns result as a numeric object on the heap.  Use internally only for
 *  direct wrapping into an ex object, where the result would end up on the
 *  heap anyways. */
const numeric &numeric::power_dyn(const numeric &other) const
{
	// Efficiency shortcut: trap the neutral exponent (first try by pointer, then
	// try harder, since calls to cln::expt() below may return amazing results for
	// floating point exponent 1.0).
	if (&other==_num1_p || cln::equal(other.value, _num1.value))
		return *this;
	
	if (cln::zerop(value)) {
		if (cln::zerop(other.value))
			throw std::domain_error("numeric::eval(): pow(0,0) is undefined");
		else if (cln::zerop(cln::realpart(other.value)))
			throw std::domain_error("numeric::eval(): pow(0,I) is undefined");
		else if (cln::minusp(cln::realpart(other.value)))
			throw std::overflow_error("numeric::eval(): division by zero");
		else
			return _num0;
	}
	return static_cast<const numeric &>((new numeric(cln::expt(value, other.value)))->
	                                     setflag(status_flags::dynallocated));
}


const numeric &numeric::operator=(int i)
{
	return operator=(numeric(i));
}


const numeric &numeric::operator=(unsigned int i)
{
	return operator=(numeric(i));
}


const numeric &numeric::operator=(long i)
{
	return operator=(numeric(i));
}


const numeric &numeric::operator=(unsigned long i)
{
	return operator=(numeric(i));
}


const numeric &numeric::operator=(double d)
{
	return operator=(numeric(d));
}


const numeric &numeric::operator=(const char * s)
{
	return operator=(numeric(s));
}


/** Inverse of a number. */
const numeric numeric::inverse() const
{
	if (cln::zerop(value))
		throw std::overflow_error("numeric::inverse(): division by zero");
	return numeric(cln::recip(value));
}


/** Return the complex half-plane (left or right) in which the number lies.
 *  csgn(x)==0 for x==0, csgn(x)==1 for Re(x)>0 or Re(x)=0 and Im(x)>0,
 *  csgn(x)==-1 for Re(x)<0 or Re(x)=0 and Im(x)<0.
 *
 *  @see numeric::compare(const numeric &other) */
int numeric::csgn() const
{
	if (cln::zerop(value))
		return 0;
	cln::cl_R r = cln::realpart(value);
	if (!cln::zerop(r)) {
		if (cln::plusp(r))
			return 1;
		else
			return -1;
	} else {
		if (cln::plusp(cln::imagpart(value)))
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
 *  @see numeric::csgn() */
int numeric::compare(const numeric &other) const
{
	// Comparing two real numbers?
	if (cln::instanceof(value, cln::cl_R_ring) &&
		cln::instanceof(other.value, cln::cl_R_ring))
		// Yes, so just cln::compare them
		return cln::compare(cln::the<cln::cl_R>(value), cln::the<cln::cl_R>(other.value));
	else {
		// No, first cln::compare real parts...
		cl_signean real_cmp = cln::compare(cln::realpart(value), cln::realpart(other.value));
		if (real_cmp)
			return real_cmp;
		// ...and then the imaginary parts.
		return cln::compare(cln::imagpart(value), cln::imagpart(other.value));
	}
}


bool numeric::is_equal(const numeric &other) const
{
	return cln::equal(value, other.value);
}


/** True if object is zero. */
bool numeric::is_zero() const
{
	return cln::zerop(value);
}


/** True if object is not complex and greater than zero. */
bool numeric::is_positive() const
{
	if (cln::instanceof(value, cln::cl_R_ring))  // real?
		return cln::plusp(cln::the<cln::cl_R>(value));
	return false;
}


/** True if object is not complex and less than zero. */
bool numeric::is_negative() const
{
	if (cln::instanceof(value, cln::cl_R_ring))  // real?
		return cln::minusp(cln::the<cln::cl_R>(value));
	return false;
}


/** True if object is a non-complex integer. */
bool numeric::is_integer() const
{
	return cln::instanceof(value, cln::cl_I_ring);
}


/** True if object is an exact integer greater than zero. */
bool numeric::is_pos_integer() const
{
	return (cln::instanceof(value, cln::cl_I_ring) && cln::plusp(cln::the<cln::cl_I>(value)));
}


/** True if object is an exact integer greater or equal zero. */
bool numeric::is_nonneg_integer() const
{
	return (cln::instanceof(value, cln::cl_I_ring) && !cln::minusp(cln::the<cln::cl_I>(value)));
}


/** True if object is an exact even integer. */
bool numeric::is_even() const
{
	return (cln::instanceof(value, cln::cl_I_ring) && cln::evenp(cln::the<cln::cl_I>(value)));
}


/** True if object is an exact odd integer. */
bool numeric::is_odd() const
{
	return (cln::instanceof(value, cln::cl_I_ring) && cln::oddp(cln::the<cln::cl_I>(value)));
}


/** Probabilistic primality test.
 *
 *  @return  true if object is exact integer and prime. */
bool numeric::is_prime() const
{
	return (cln::instanceof(value, cln::cl_I_ring)  // integer?
	     && cln::plusp(cln::the<cln::cl_I>(value))  // positive?
	     && cln::isprobprime(cln::the<cln::cl_I>(value)));
}


/** True if object is an exact rational number, may even be complex
 *  (denominator may be unity). */
bool numeric::is_rational() const
{
	return cln::instanceof(value, cln::cl_RA_ring);
}


/** True if object is a real integer, rational or float (but not complex). */
bool numeric::is_real() const
{
	return cln::instanceof(value, cln::cl_R_ring);
}


bool numeric::operator==(const numeric &other) const
{
	return cln::equal(value, other.value);
}


bool numeric::operator!=(const numeric &other) const
{
	return !cln::equal(value, other.value);
}


/** True if object is element of the domain of integers extended by I, i.e. is
 *  of the form a+b*I, where a and b are integers. */
bool numeric::is_cinteger() const
{
	if (cln::instanceof(value, cln::cl_I_ring))
		return true;
	else if (!this->is_real()) {  // complex case, handle n+m*I
		if (cln::instanceof(cln::realpart(value), cln::cl_I_ring) &&
		    cln::instanceof(cln::imagpart(value), cln::cl_I_ring))
			return true;
	}
	return false;
}


/** True if object is an exact rational number, may even be complex
 *  (denominator may be unity). */
bool numeric::is_crational() const
{
	if (cln::instanceof(value, cln::cl_RA_ring))
		return true;
	else if (!this->is_real()) {  // complex case, handle Q(i):
		if (cln::instanceof(cln::realpart(value), cln::cl_RA_ring) &&
		    cln::instanceof(cln::imagpart(value), cln::cl_RA_ring))
			return true;
	}
	return false;
}


/** Numerical comparison: less.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator<(const numeric &other) const
{
	if (this->is_real() && other.is_real())
		return (cln::the<cln::cl_R>(value) < cln::the<cln::cl_R>(other.value));
	throw std::invalid_argument("numeric::operator<(): complex inequality");
}


/** Numerical comparison: less or equal.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator<=(const numeric &other) const
{
	if (this->is_real() && other.is_real())
		return (cln::the<cln::cl_R>(value) <= cln::the<cln::cl_R>(other.value));
	throw std::invalid_argument("numeric::operator<=(): complex inequality");
}


/** Numerical comparison: greater.
 *
 *  @exception invalid_argument (complex inequality) */ 
bool numeric::operator>(const numeric &other) const
{
	if (this->is_real() && other.is_real())
		return (cln::the<cln::cl_R>(value) > cln::the<cln::cl_R>(other.value));
	throw std::invalid_argument("numeric::operator>(): complex inequality");
}


/** Numerical comparison: greater or equal.
 *
 *  @exception invalid_argument (complex inequality) */  
bool numeric::operator>=(const numeric &other) const
{
	if (this->is_real() && other.is_real())
		return (cln::the<cln::cl_R>(value) >= cln::the<cln::cl_R>(other.value));
	throw std::invalid_argument("numeric::operator>=(): complex inequality");
}


/** Converts numeric types to machine's int.  You should check with
 *  is_integer() if the number is really an integer before calling this method.
 *  You may also consider checking the range first. */
int numeric::to_int() const
{
	GINAC_ASSERT(this->is_integer());
	return cln::cl_I_to_int(cln::the<cln::cl_I>(value));
}


/** Converts numeric types to machine's long.  You should check with
 *  is_integer() if the number is really an integer before calling this method.
 *  You may also consider checking the range first. */
long numeric::to_long() const
{
	GINAC_ASSERT(this->is_integer());
	return cln::cl_I_to_long(cln::the<cln::cl_I>(value));
}


/** Converts numeric types to machine's double. You should check with is_real()
 *  if the number is really not complex before calling this method. */
double numeric::to_double() const
{
	GINAC_ASSERT(this->is_real());
	return cln::double_approx(cln::realpart(value));
}


/** Returns a new CLN object of type cl_N, representing the value of *this.
 *  This method may be used when mixing GiNaC and CLN in one project.
 */
cln::cl_N numeric::to_cl_N() const
{
	return value;
}


/** Real part of a number. */
const numeric numeric::real() const
{
	return numeric(cln::realpart(value));
}


/** Imaginary part of a number. */
const numeric numeric::imag() const
{
	return numeric(cln::imagpart(value));
}


/** Numerator.  Computes the numerator of rational numbers, rationalized
 *  numerator of complex if real and imaginary part are both rational numbers
 *  (i.e numer(4/3+5/6*I) == 8+5*I), the number carrying the sign in all other
 *  cases. */
const numeric numeric::numer() const
{
	if (cln::instanceof(value, cln::cl_I_ring))
		return numeric(*this);  // integer case
	
	else if (cln::instanceof(value, cln::cl_RA_ring))
		return numeric(cln::numerator(cln::the<cln::cl_RA>(value)));
	
	else if (!this->is_real()) {  // complex case, handle Q(i):
		const cln::cl_RA r = cln::the<cln::cl_RA>(cln::realpart(value));
		const cln::cl_RA i = cln::the<cln::cl_RA>(cln::imagpart(value));
		if (cln::instanceof(r, cln::cl_I_ring) && cln::instanceof(i, cln::cl_I_ring))
			return numeric(*this);
		if (cln::instanceof(r, cln::cl_I_ring) && cln::instanceof(i, cln::cl_RA_ring))
			return numeric(cln::complex(r*cln::denominator(i), cln::numerator(i)));
		if (cln::instanceof(r, cln::cl_RA_ring) && cln::instanceof(i, cln::cl_I_ring))
			return numeric(cln::complex(cln::numerator(r), i*cln::denominator(r)));
		if (cln::instanceof(r, cln::cl_RA_ring) && cln::instanceof(i, cln::cl_RA_ring)) {
			const cln::cl_I s = cln::lcm(cln::denominator(r), cln::denominator(i));
			return numeric(cln::complex(cln::numerator(r)*(cln::exquo(s,cln::denominator(r))),
			   	   	                    cln::numerator(i)*(cln::exquo(s,cln::denominator(i)))));
		}
	}
	// at least one float encountered
	return numeric(*this);
}


/** Denominator.  Computes the denominator of rational numbers, common integer
 *  denominator of complex if real and imaginary part are both rational numbers
 *  (i.e denom(4/3+5/6*I) == 6), one in all other cases. */
const numeric numeric::denom() const
{
	if (cln::instanceof(value, cln::cl_I_ring))
		return _num1;  // integer case
	
	if (cln::instanceof(value, cln::cl_RA_ring))
		return numeric(cln::denominator(cln::the<cln::cl_RA>(value)));
	
	if (!this->is_real()) {  // complex case, handle Q(i):
		const cln::cl_RA r = cln::the<cln::cl_RA>(cln::realpart(value));
		const cln::cl_RA i = cln::the<cln::cl_RA>(cln::imagpart(value));
		if (cln::instanceof(r, cln::cl_I_ring) && cln::instanceof(i, cln::cl_I_ring))
			return _num1;
		if (cln::instanceof(r, cln::cl_I_ring) && cln::instanceof(i, cln::cl_RA_ring))
			return numeric(cln::denominator(i));
		if (cln::instanceof(r, cln::cl_RA_ring) && cln::instanceof(i, cln::cl_I_ring))
			return numeric(cln::denominator(r));
		if (cln::instanceof(r, cln::cl_RA_ring) && cln::instanceof(i, cln::cl_RA_ring))
			return numeric(cln::lcm(cln::denominator(r), cln::denominator(i)));
	}
	// at least one float encountered
	return _num1;
}


/** Size in binary notation.  For integers, this is the smallest n >= 0 such
 *  that -2^n <= x < 2^n. If x > 0, this is the unique n > 0 such that
 *  2^(n-1) <= x < 2^n.
 *
 *  @return  number of bits (excluding sign) needed to represent that number
 *  in two's complement if it is an integer, 0 otherwise. */    
int numeric::int_length() const
{
	if (cln::instanceof(value, cln::cl_I_ring))
		return cln::integer_length(cln::the<cln::cl_I>(value));
	else
		return 0;
}

//////////
// global constants
//////////

/** Imaginary unit.  This is not a constant but a numeric since we are
 *  natively handing complex numbers anyways, so in each expression containing
 *  an I it is automatically eval'ed away anyhow. */
const numeric I = numeric(cln::complex(cln::cl_I(0),cln::cl_I(1)));


/** Exponential function.
 *
 *  @return  arbitrary precision numerical exp(x). */
const numeric exp(const numeric &x)
{
	return cln::exp(x.to_cl_N());
}


/** Natural logarithm.
 *
 *  @param x complex number
 *  @return  arbitrary precision numerical log(x).
 *  @exception pole_error("log(): logarithmic pole",0) */
const numeric log(const numeric &x)
{
	if (x.is_zero())
		throw pole_error("log(): logarithmic pole",0);
	return cln::log(x.to_cl_N());
}


/** Numeric sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical sin(x). */
const numeric sin(const numeric &x)
{
	return cln::sin(x.to_cl_N());
}


/** Numeric cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical cos(x). */
const numeric cos(const numeric &x)
{
	return cln::cos(x.to_cl_N());
}


/** Numeric tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical tan(x). */
const numeric tan(const numeric &x)
{
	return cln::tan(x.to_cl_N());
}
	

/** Numeric inverse sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical asin(x). */
const numeric asin(const numeric &x)
{
	return cln::asin(x.to_cl_N());
}


/** Numeric inverse cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical acos(x). */
const numeric acos(const numeric &x)
{
	return cln::acos(x.to_cl_N());
}
	

/** Arcustangent.
 *
 *  @param x complex number
 *  @return atan(x)
 *  @exception pole_error("atan(): logarithmic pole",0) */
const numeric atan(const numeric &x)
{
	if (!x.is_real() &&
	    x.real().is_zero() &&
	    abs(x.imag()).is_equal(_num1))
		throw pole_error("atan(): logarithmic pole",0);
	return cln::atan(x.to_cl_N());
}


/** Arcustangent.
 *
 *  @param x real number
 *  @param y real number
 *  @return atan(y/x) */
const numeric atan(const numeric &y, const numeric &x)
{
	if (x.is_real() && y.is_real())
		return cln::atan(cln::the<cln::cl_R>(x.to_cl_N()),
		                 cln::the<cln::cl_R>(y.to_cl_N()));
	else
		throw std::invalid_argument("atan(): complex argument");        
}


/** Numeric hyperbolic sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical sinh(x). */
const numeric sinh(const numeric &x)
{
	return cln::sinh(x.to_cl_N());
}


/** Numeric hyperbolic cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical cosh(x). */
const numeric cosh(const numeric &x)
{
	return cln::cosh(x.to_cl_N());
}


/** Numeric hyperbolic tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical tanh(x). */
const numeric tanh(const numeric &x)
{
	return cln::tanh(x.to_cl_N());
}
	

/** Numeric inverse hyperbolic sine (trigonometric function).
 *
 *  @return  arbitrary precision numerical asinh(x). */
const numeric asinh(const numeric &x)
{
	return cln::asinh(x.to_cl_N());
}


/** Numeric inverse hyperbolic cosine (trigonometric function).
 *
 *  @return  arbitrary precision numerical acosh(x). */
const numeric acosh(const numeric &x)
{
	return cln::acosh(x.to_cl_N());
}


/** Numeric inverse hyperbolic tangent (trigonometric function).
 *
 *  @return  arbitrary precision numerical atanh(x). */
const numeric atanh(const numeric &x)
{
	return cln::atanh(x.to_cl_N());
}


/*static cln::cl_N Li2_series(const ::cl_N &x,
                            const ::float_format_t &prec)
{
	// Note: argument must be in the unit circle
	// This is very inefficient unless we have fast floating point Bernoulli
	// numbers implemented!
	cln::cl_N c1 = -cln::log(1-x);
	cln::cl_N c2 = c1;
	// hard-wire the first two Bernoulli numbers
	cln::cl_N acc = c1 - cln::square(c1)/4;
	cln::cl_N aug;
	cln::cl_F pisq = cln::square(cln::cl_pi(prec));  // pi^2
	cln::cl_F piac = cln::cl_float(1, prec);  // accumulator: pi^(2*i)
	unsigned i = 1;
	c1 = cln::square(c1);
	do {
		c2 = c1 * c2;
		piac = piac * pisq;
		aug = c2 * (*(bernoulli(numeric(2*i)).clnptr())) / cln::factorial(2*i+1);
		// aug = c2 * cln::cl_I(i%2 ? 1 : -1) / cln::cl_I(2*i+1) * cln::cl_zeta(2*i, prec) / piac / (cln::cl_I(1)<<(2*i-1));
		acc = acc + aug;
		++i;
	} while (acc != acc+aug);
	return acc;
}*/

/** Numeric evaluation of Dilogarithm within circle of convergence (unit
 *  circle) using a power series. */
static cln::cl_N Li2_series(const cln::cl_N &x,
                            const cln::float_format_t &prec)
{
	// Note: argument must be in the unit circle
	cln::cl_N aug, acc;
	cln::cl_N num = cln::complex(cln::cl_float(1, prec), 0);
	cln::cl_I den = 0;
	unsigned i = 1;
	do {
		num = num * x;
		den = den + i;  // 1, 4, 9, 16, ...
		i += 2;
		aug = num / den;
		acc = acc + aug;
	} while (acc != acc+aug);
	return acc;
}

/** Folds Li2's argument inside a small rectangle to enhance convergence. */
static cln::cl_N Li2_projection(const cln::cl_N &x,
                                const cln::float_format_t &prec)
{
	const cln::cl_R re = cln::realpart(x);
	const cln::cl_R im = cln::imagpart(x);
	if (re > cln::cl_F(".5"))
		// zeta(2) - Li2(1-x) - log(x)*log(1-x)
		return(cln::zeta(2)
		       - Li2_series(1-x, prec)
		       - cln::log(x)*cln::log(1-x));
	if ((re <= 0 && cln::abs(im) > cln::cl_F(".75")) || (re < cln::cl_F("-.5")))
		// -log(1-x)^2 / 2 - Li2(x/(x-1))
		return(- cln::square(cln::log(1-x))/2
		       - Li2_series(x/(x-1), prec));
	if (re > 0 && cln::abs(im) > cln::cl_LF(".75"))
		// Li2(x^2)/2 - Li2(-x)
		return(Li2_projection(cln::square(x), prec)/2
		       - Li2_projection(-x, prec));
	return Li2_series(x, prec);
}

/** Numeric evaluation of Dilogarithm.  The domain is the entire complex plane,
 *  the branch cut lies along the positive real axis, starting at 1 and
 *  continuous with quadrant IV.
 *
 *  @return  arbitrary precision numerical Li2(x). */
const numeric Li2(const numeric &x)
{
	if (x.is_zero())
		return _num0;
	
	// what is the desired float format?
	// first guess: default format
	cln::float_format_t prec = cln::default_float_format;
	const cln::cl_N value = x.to_cl_N();
	// second guess: the argument's format
	if (!x.real().is_rational())
		prec = cln::float_format(cln::the<cln::cl_F>(cln::realpart(value)));
	else if (!x.imag().is_rational())
		prec = cln::float_format(cln::the<cln::cl_F>(cln::imagpart(value)));
	
	if (value==1)  // may cause trouble with log(1-x)
		return cln::zeta(2, prec);
	
	if (cln::abs(value) > 1)
		// -log(-x)^2 / 2 - zeta(2) - Li2(1/x)
		return(- cln::square(cln::log(-value))/2
		       - cln::zeta(2, prec)
		       - Li2_projection(cln::recip(value), prec));
	else
		return Li2_projection(x.to_cl_N(), prec);
}


/** Numeric evaluation of Riemann's Zeta function.  Currently works only for
 *  integer arguments. */
const numeric zeta(const numeric &x)
{
	// A dirty hack to allow for things like zeta(3.0), since CLN currently
	// only knows about integer arguments and zeta(3).evalf() automatically
	// cascades down to zeta(3.0).evalf().  The trick is to rely on 3.0-3
	// being an exact zero for CLN, which can be tested and then we can just
	// pass the number casted to an int:
	if (x.is_real()) {
		const int aux = (int)(cln::double_approx(cln::the<cln::cl_R>(x.to_cl_N())));
		if (cln::zerop(x.to_cl_N()-aux))
			return cln::zeta(aux);
	}
	throw dunno();
}


/** The Gamma function.
 *  This is only a stub! */
const numeric lgamma(const numeric &x)
{
	throw dunno();
}
const numeric tgamma(const numeric &x)
{
	throw dunno();
}


/** The psi function (aka polygamma function).
 *  This is only a stub! */
const numeric psi(const numeric &x)
{
	throw dunno();
}


/** The psi functions (aka polygamma functions).
 *  This is only a stub! */
const numeric psi(const numeric &n, const numeric &x)
{
	throw dunno();
}


/** Factorial combinatorial function.
 *
 *  @param n  integer argument >= 0
 *  @exception range_error (argument must be integer >= 0) */
const numeric factorial(const numeric &n)
{
	if (!n.is_nonneg_integer())
		throw std::range_error("numeric::factorial(): argument must be integer >= 0");
	return numeric(cln::factorial(n.to_int()));
}


/** The double factorial combinatorial function.  (Scarcely used, but still
 *  useful in cases, like for exact results of tgamma(n+1/2) for instance.)
 *
 *  @param n  integer argument >= -1
 *  @return n!! == n * (n-2) * (n-4) * ... * ({1|2}) with 0!! == (-1)!! == 1
 *  @exception range_error (argument must be integer >= -1) */
const numeric doublefactorial(const numeric &n)
{
	if (n.is_equal(_num_1))
		return _num1;
	
	if (!n.is_nonneg_integer())
		throw std::range_error("numeric::doublefactorial(): argument must be integer >= -1");
	
	return numeric(cln::doublefactorial(n.to_int()));
}


/** The Binomial coefficients.  It computes the binomial coefficients.  For
 *  integer n and k and positive n this is the number of ways of choosing k
 *  objects from n distinct objects.  If n is negative, the formula
 *  binomial(n,k) == (-1)^k*binomial(k-n-1,k) is used to compute the result. */
const numeric binomial(const numeric &n, const numeric &k)
{
	if (n.is_integer() && k.is_integer()) {
		if (n.is_nonneg_integer()) {
			if (k.compare(n)!=1 && k.compare(_num0)!=-1)
				return numeric(cln::binomial(n.to_int(),k.to_int()));
			else
				return _num0;
		} else {
			return _num_1.power(k)*binomial(k-n-_num1,k);
		}
	}
	
	// should really be gamma(n+1)/gamma(r+1)/gamma(n-r+1) or a suitable limit
	throw std::range_error("numeric::binomial(): don´t know how to evaluate that.");
}


/** Bernoulli number.  The nth Bernoulli number is the coefficient of x^n/n!
 *  in the expansion of the function x/(e^x-1).
 *
 *  @return the nth Bernoulli number (a rational number).
 *  @exception range_error (argument must be integer >= 0) */
const numeric bernoulli(const numeric &nn)
{
	if (!nn.is_integer() || nn.is_negative())
		throw std::range_error("numeric::bernoulli(): argument must be integer >= 0");

	// Method:
	//
	// The Bernoulli numbers are rational numbers that may be computed using
	// the relation
	//
	//     B_n = - 1/(n+1) * sum_{k=0}^{n-1}(binomial(n+1,k)*B_k)
	//
	// with B(0) = 1.  Since the n'th Bernoulli number depends on all the
	// previous ones, the computation is necessarily very expensive.  There are
	// several other ways of computing them, a particularly good one being
	// cl_I s = 1;
	// cl_I c = n+1;
	// cl_RA Bern = 0;
	// for (unsigned i=0; i<n; i++) {
	//     c = exquo(c*(i-n),(i+2));
	//     Bern = Bern + c*s/(i+2);
	//     s = s + expt_pos(cl_I(i+2),n);
	// }
	// return Bern;
	// 
	// But if somebody works with the n'th Bernoulli number she is likely to
	// also need all previous Bernoulli numbers. So we need a complete remember
	// table and above divide and conquer algorithm is not suited to build one
	// up.  The formula below accomplishes this.  It is a modification of the
	// defining formula above but the computation of the binomial coefficients
	// is carried along in an inline fashion.  It also honors the fact that
	// B_n is zero when n is odd and greater than 1.
	// 
	// (There is an interesting relation with the tangent polynomials described
	// in `Concrete Mathematics', which leads to a program a little faster as
	// our implementation below, but it requires storing one such polynomial in
	// addition to the remember table.  This doubles the memory footprint so
	// we don't use it.)

	const unsigned n = nn.to_int();

	// the special cases not covered by the algorithm below
	if (n & 1)
		return (n==1) ? _num_1_2 : _num0;
	if (!n)
		 return _num1;

	// store nonvanishing Bernoulli numbers here
	static std::vector< cln::cl_RA > results;
	static unsigned next_r = 0;

	// algorithm not applicable to B(2), so just store it
	if (!next_r) {
		results.push_back(cln::recip(cln::cl_RA(6)));
		next_r = 4;
	}
	if (n<next_r)
		return results[n/2-1];

	results.reserve(n/2);
	for (unsigned p=next_r; p<=n;  p+=2) {
		cln::cl_I  c = 1;  // seed for binonmial coefficients
		cln::cl_RA b = cln::cl_RA(1-p)/2;
		const unsigned p3 = p+3;
		const unsigned pm = p-2;
		unsigned i, k, p_2;
		// test if intermediate unsigned int can be represented by immediate
		// objects by CLN (i.e. < 2^29 for 32 Bit machines, see <cln/object.h>)
		if (p < (1UL<<cl_value_len/2)) {
			for (i=2, k=1, p_2=p/2; i<=pm; i+=2, ++k, --p_2) {
				c = cln::exquo(c * ((p3-i) * p_2), (i-1)*k);
				b = b + c*results[k-1];
			}
		} else {
			for (i=2, k=1, p_2=p/2; i<=pm; i+=2, ++k, --p_2) {
				c = cln::exquo((c * (p3-i)) * p_2, cln::cl_I(i-1)*k);
				b = b + c*results[k-1];
			}
 		}
		results.push_back(-b/(p+1));
	}
	next_r = n+2;
	return results[n/2-1];
}


/** Fibonacci number.  The nth Fibonacci number F(n) is defined by the
 *  recurrence formula F(n)==F(n-1)+F(n-2) with F(0)==0 and F(1)==1.
 *
 *  @param n an integer
 *  @return the nth Fibonacci number F(n) (an integer number)
 *  @exception range_error (argument must be an integer) */
const numeric fibonacci(const numeric &n)
{
	if (!n.is_integer())
		throw std::range_error("numeric::fibonacci(): argument must be integer");
	// Method:
	//
	// The following addition formula holds:
	//
	//      F(n+m)   = F(m-1)*F(n) + F(m)*F(n+1)  for m >= 1, n >= 0.
	//
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
		return _num0;
	if (n.is_negative())
		if (n.is_even())
			return -fibonacci(-n);
		else
			return fibonacci(-n);
	
	cln::cl_I u(0);
	cln::cl_I v(1);
	cln::cl_I m = cln::the<cln::cl_I>(n.to_cl_N()) >> 1L;  // floor(n/2);
	for (uintL bit=cln::integer_length(m); bit>0; --bit) {
		// Since a squaring is cheaper than a multiplication, better use
		// three squarings instead of one multiplication and two squarings.
		cln::cl_I u2 = cln::square(u);
		cln::cl_I v2 = cln::square(v);
		if (cln::logbitp(bit-1, m)) {
			v = cln::square(u + v) - u2;
			u = u2 + v2;
		} else {
			u = v2 - cln::square(v - u);
			v = u2 + v2;
		}
	}
	if (n.is_even())
		// Here we don't use the squaring formula because one multiplication
		// is cheaper than two squarings.
		return u * ((v << 1) - u);
	else
		return cln::square(u) + cln::square(v);    
}


/** Absolute value. */
const numeric abs(const numeric& x)
{
	return cln::abs(x.to_cl_N());
}


/** Modulus (in positive representation).
 *  In general, mod(a,b) has the sign of b or is zero, and rem(a,b) has the
 *  sign of a or is zero. This is different from Maple's modp, where the sign
 *  of b is ignored. It is in agreement with Mathematica's Mod.
 *
 *  @return a mod b in the range [0,abs(b)-1] with sign of b if both are
 *  integer, 0 otherwise. */
const numeric mod(const numeric &a, const numeric &b)
{
	if (a.is_integer() && b.is_integer())
		return cln::mod(cln::the<cln::cl_I>(a.to_cl_N()),
		                cln::the<cln::cl_I>(b.to_cl_N()));
	else
		return _num0;
}


/** Modulus (in symmetric representation).
 *  Equivalent to Maple's mods.
 *
 *  @return a mod b in the range [-iquo(abs(b)-1,2), iquo(abs(b),2)]. */
const numeric smod(const numeric &a, const numeric &b)
{
	if (a.is_integer() && b.is_integer()) {
		const cln::cl_I b2 = cln::ceiling1(cln::the<cln::cl_I>(b.to_cl_N()) >> 1) - 1;
		return cln::mod(cln::the<cln::cl_I>(a.to_cl_N()) + b2,
		                cln::the<cln::cl_I>(b.to_cl_N())) - b2;
	} else
		return _num0;
}


/** Numeric integer remainder.
 *  Equivalent to Maple's irem(a,b) as far as sign conventions are concerned.
 *  In general, mod(a,b) has the sign of b or is zero, and irem(a,b) has the
 *  sign of a or is zero.
 *
 *  @return remainder of a/b if both are integer, 0 otherwise.
 *  @exception overflow_error (division by zero) if b is zero. */
const numeric irem(const numeric &a, const numeric &b)
{
	if (b.is_zero())
		throw std::overflow_error("numeric::irem(): division by zero");
	if (a.is_integer() && b.is_integer())
		return cln::rem(cln::the<cln::cl_I>(a.to_cl_N()),
		                cln::the<cln::cl_I>(b.to_cl_N()));
	else
		return _num0;
}


/** Numeric integer remainder.
 *  Equivalent to Maple's irem(a,b,'q') it obeyes the relation
 *  irem(a,b,q) == a - q*b.  In general, mod(a,b) has the sign of b or is zero,
 *  and irem(a,b) has the sign of a or is zero.
 *
 *  @return remainder of a/b and quotient stored in q if both are integer,
 *  0 otherwise.
 *  @exception overflow_error (division by zero) if b is zero. */
const numeric irem(const numeric &a, const numeric &b, numeric &q)
{
	if (b.is_zero())
		throw std::overflow_error("numeric::irem(): division by zero");
	if (a.is_integer() && b.is_integer()) {
		const cln::cl_I_div_t rem_quo = cln::truncate2(cln::the<cln::cl_I>(a.to_cl_N()),
		                                               cln::the<cln::cl_I>(b.to_cl_N()));
		q = rem_quo.quotient;
		return rem_quo.remainder;
	} else {
		q = _num0;
		return _num0;
	}
}


/** Numeric integer quotient.
 *  Equivalent to Maple's iquo as far as sign conventions are concerned.
 *  
 *  @return truncated quotient of a/b if both are integer, 0 otherwise.
 *  @exception overflow_error (division by zero) if b is zero. */
const numeric iquo(const numeric &a, const numeric &b)
{
	if (b.is_zero())
		throw std::overflow_error("numeric::iquo(): division by zero");
	if (a.is_integer() && b.is_integer())
		return cln::truncate1(cln::the<cln::cl_I>(a.to_cl_N()),
	                          cln::the<cln::cl_I>(b.to_cl_N()));
	else
		return _num0;
}


/** Numeric integer quotient.
 *  Equivalent to Maple's iquo(a,b,'r') it obeyes the relation
 *  r == a - iquo(a,b,r)*b.
 *
 *  @return truncated quotient of a/b and remainder stored in r if both are
 *  integer, 0 otherwise.
 *  @exception overflow_error (division by zero) if b is zero. */
const numeric iquo(const numeric &a, const numeric &b, numeric &r)
{
	if (b.is_zero())
		throw std::overflow_error("numeric::iquo(): division by zero");
	if (a.is_integer() && b.is_integer()) {
		const cln::cl_I_div_t rem_quo = cln::truncate2(cln::the<cln::cl_I>(a.to_cl_N()),
		                                               cln::the<cln::cl_I>(b.to_cl_N()));
		r = rem_quo.remainder;
		return rem_quo.quotient;
	} else {
		r = _num0;
		return _num0;
	}
}


/** Greatest Common Divisor.
 *   
 *  @return  The GCD of two numbers if both are integer, a numerical 1
 *  if they are not. */
const numeric gcd(const numeric &a, const numeric &b)
{
	if (a.is_integer() && b.is_integer())
		return cln::gcd(cln::the<cln::cl_I>(a.to_cl_N()),
		                cln::the<cln::cl_I>(b.to_cl_N()));
	else
		return _num1;
}


/** Least Common Multiple.
 *   
 *  @return  The LCM of two numbers if both are integer, the product of those
 *  two numbers if they are not. */
const numeric lcm(const numeric &a, const numeric &b)
{
	if (a.is_integer() && b.is_integer())
		return cln::lcm(cln::the<cln::cl_I>(a.to_cl_N()),
		                cln::the<cln::cl_I>(b.to_cl_N()));
	else
		return a.mul(b);
}


/** Numeric square root.
 *  If possible, sqrt(x) should respect squares of exact numbers, i.e. sqrt(4)
 *  should return integer 2.
 *
 *  @param x numeric argument
 *  @return square root of x. Branch cut along negative real axis, the negative
 *  real axis itself where imag(x)==0 and real(x)<0 belongs to the upper part
 *  where imag(x)>0. */
const numeric sqrt(const numeric &x)
{
	return cln::sqrt(x.to_cl_N());
}


/** Integer numeric square root. */
const numeric isqrt(const numeric &x)
{
	if (x.is_integer()) {
		cln::cl_I root;
		cln::isqrt(cln::the<cln::cl_I>(x.to_cl_N()), &root);
		return root;
	} else
		return _num0;
}


/** Floating point evaluation of Archimedes' constant Pi. */
ex PiEvalf()
{ 
	return numeric(cln::pi(cln::default_float_format));
}


/** Floating point evaluation of Euler's constant gamma. */
ex EulerEvalf()
{ 
	return numeric(cln::eulerconst(cln::default_float_format));
}


/** Floating point evaluation of Catalan's constant. */
ex CatalanEvalf()
{
	return numeric(cln::catalanconst(cln::default_float_format));
}


/** _numeric_digits default ctor, checking for singleton invariance. */
_numeric_digits::_numeric_digits()
  : digits(17)
{
	// It initializes to 17 digits, because in CLN float_format(17) turns out
	// to be 61 (<64) while float_format(18)=65.  The reason is we want to
	// have a cl_LF instead of cl_SF, cl_FF or cl_DF.
	if (too_late)
		throw(std::runtime_error("I told you not to do instantiate me!"));
	too_late = true;
	cln::default_float_format = cln::float_format(17);
}


/** Assign a native long to global Digits object. */
_numeric_digits& _numeric_digits::operator=(long prec)
{
	digits = prec;
	cln::default_float_format = cln::float_format(prec); 
	return *this;
}


/** Convert global Digits object to native type long. */
_numeric_digits::operator long()
{
	// BTW, this is approx. unsigned(cln::default_float_format*0.301)-1
	return (long)digits;
}


/** Append global Digits object to ostream. */
void _numeric_digits::print(std::ostream &os) const
{
	os << digits;
}


std::ostream& operator<<(std::ostream &os, const _numeric_digits &e)
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
