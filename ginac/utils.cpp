/** @file utils.cpp
 *
 *  Implementation of several small and furry utilities needed within GiNaC
 *  but not of any interest to the user of the library. */

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

#include "ex.h"
#include "numeric.h"
#include "utils.h"
#include "version.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/* Version information buried into the library */
const int version_major = GINACLIB_MAJOR_VERSION;
const int version_minor = GINACLIB_MINOR_VERSION;
const int version_micro = GINACLIB_MICRO_VERSION;


/** ctor for pole_error exception class. */
pole_error::pole_error(const std::string& what_arg, int degree)
  : domain_error(what_arg)
{
	deg = degree;
}

/** Return the degree of the pole_error exception class. */
int pole_error::degree(void) const
{
	return deg;
}

// some compilers (e.g. cygwin) define a macro log2, causing confusion
#ifndef log2
/** Integer binary logarithm */
unsigned log2(unsigned n)
{
	unsigned k;
	for (k = 0; n > 1; n >>= 1) ++k;
	return k;
}
#endif

/** Compare two pointers (just to establish some sort of canonical order).
 *  @return -1, 0, or 1 */
int compare_pointers(const void * a, const void * b)
{
	if (a<b) {
		return -1;
	} else if (a>b) {
		return 1;
	}
	return 0;
}

//////////
// `construct on first use' chest of numbers
//////////

// numeric -120
const numeric & _num_120(void)
{
	const static ex e = ex(numeric(-120));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_120(void)
{
	static ex * e = new ex(_num_120());
	return *e;
}

// numeric -60
const numeric & _num_60(void)
{
	const static ex e = ex(numeric(-60));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_60(void)
{
	static ex * e = new ex(_num_60());
	return *e;
}

// numeric -48
const numeric & _num_48(void)
{
	const static ex e = ex(numeric(-48));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_48(void)
{
	static ex * e = new ex(_num_48());
	return *e;
}

// numeric -30
const numeric & _num_30(void)
{
	const static ex e = ex(numeric(-30));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_30(void)
{
	static ex * e = new ex(_num_30());
	return *e;
}

// numeric -25
const numeric & _num_25(void)
{
	const static ex e = ex(numeric(-25));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_25(void)
{
	static ex * e = new ex(_num_25());
	return *e;
}

// numeric -24
const numeric & _num_24(void)
{
	const static ex e = ex(numeric(-24));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_24(void)
{
	static ex * e = new ex(_num_24());
	return *e;
}

// numeric -20
const numeric & _num_20(void)
{
	const static ex e = ex(numeric(-20));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_20(void)
{
	static ex * e = new ex(_num_20());
	return *e;
}

// numeric -18
const numeric & _num_18(void)
{
	const static ex e = ex(numeric(-18));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_18(void)
{
	static ex * e = new ex(_num_18());
	return *e;
}

// numeric -15
const numeric & _num_15(void)
{
	const static ex e = ex(numeric(-15));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_15(void)
{
	static ex * e = new ex(_num_15());
	return *e;
}

// numeric -12
const numeric & _num_12(void)
{
	const static ex e = ex(numeric(-12));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_12(void)
{
	static ex * e = new ex(_num_12());
	return *e;
}

// numeric -11
const numeric & _num_11(void)
{
	const static ex e = ex(numeric(-11));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_11(void)
{
	static ex * e = new ex(_num_11());
	return *e;
}

// numeric -10
const numeric & _num_10(void)
{
	const static ex e = ex(numeric(-10));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_10(void)
{
	static ex * e = new ex(_num_10());
	return *e;
}

// numeric -9
const numeric & _num_9(void)
{
	const static ex e = ex(numeric(-9));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_9(void)
{
	static ex * e = new ex(_num_9());
	return *e;
}

// numeric -8
const numeric & _num_8(void)
{
	const static ex e = ex(numeric(-8));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_8(void)
{
	static ex * e = new ex(_num_8());
	return *e;
}

// numeric -7
const numeric & _num_7(void)
{
	const static ex e = ex(numeric(-7));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_7(void)
{
	static ex * e = new ex(_num_7());
	return *e;
}

// numeric -6
const numeric & _num_6(void)
{
	const static ex e = ex(numeric(-6));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_6(void)
{
	static ex * e = new ex(_num_6());
	return *e;
}

// numeric -5
const numeric & _num_5(void)
{
	const static ex e = ex(numeric(-5));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_5(void)
{
	static ex * e = new ex(_num_5());
	return *e;
}

// numeric -4
const numeric & _num_4(void)
{
	const static ex e = ex(numeric(-4));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_4(void)
{
	static ex * e = new ex(_num_4());
	return *e;
}

// numeric -3
const numeric & _num_3(void)
{
	const static ex e = ex(numeric(-3));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_3(void)
{
	static ex * e = new ex(_num_3());
	return *e;
}

// numeric -2
const numeric & _num_2(void)
{
	const static ex e = ex(numeric(-2));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_2(void)
{
	static ex * e = new ex(_num_2());
	return *e;
}

// numeric -1
const numeric & _num_1(void)
{
	const static ex e = ex(numeric(-1));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_1(void)
{
	static ex * e = new ex(_num_1());
	return *e;
}

// numeric -1/2
const numeric & _num_1_2(void)
{
	const static ex e = ex(numeric(-1,2));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_1_2(void)
{
	static ex * e = new ex(_num_1_2());
	return *e;
}    

// numeric -1/3
const numeric & _num_1_3(void)
{
	const static ex e = ex(numeric(-1,3));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_1_3(void)
{
	static ex * e = new ex(_num_1_3());
	return *e;
}    

// numeric -1/4
const numeric & _num_1_4(void)
{
	const static ex e = ex(numeric(-1,4));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex_1_4(void)
{
	static ex * e = new ex(_num_1_4());
	return *e;
}    

// numeric  0
const numeric & _num0(void)
{
	const static ex e = ex(numeric(0));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex0(void)
{
	static ex * e = new ex(_num0());
	return *e;
}

// numeric  1/4
const numeric & _num1_4(void)
{
	const static ex e = ex(numeric(1,4));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex1_4(void)
{
	static ex * e = new ex(_num1_4());
	return *e;
}    

// numeric  1/3
const numeric & _num1_3(void)
{
	const static ex e = ex(numeric(1,3));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex1_3(void)
{
	static ex * e = new ex(_num1_3());
	return *e;
}    

// numeric  1/2
const numeric & _num1_2(void)
{
	const static ex e = ex(numeric(1,2));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex1_2(void)
{
	static ex * e = new ex(_num1_2());
	return *e;
}    

// numeric  1
const numeric & _num1(void)
{
	const static ex e = ex(numeric(1));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex1(void)
{
	static ex * e = new ex(_num1());
	return *e;
}

// numeric  2
const numeric & _num2(void)
{
	const static ex e = ex(numeric(2));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex2(void)
{
	static ex * e = new ex(_num2());
	return *e;
}

// numeric  3
const numeric & _num3(void)
{
	const static ex e = ex(numeric(3));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex3(void)
{
	static ex * e = new ex(_num3());
	return *e;
}

// numeric  4
const numeric & _num4(void)
{
	const static ex e = ex(numeric(4));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex4(void)
{
	static ex * e = new ex(_num4());
	return *e;
}

// numeric  5
const numeric & _num5(void)
{
	const static ex e = ex(numeric(5));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex5(void)
{
	static ex * e = new ex(_num5());
	return *e;
}

// numeric  6
const numeric & _num6(void)
{
	const static ex e = ex(numeric(6));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex6(void)
{
	static ex * e = new ex(_num6());
	return *e;
}

// numeric  7
const numeric & _num7(void)
{
	const static ex e = ex(numeric(7));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex7(void)
{
	static ex * e = new ex(_num7());
	return *e;
}

// numeric  8
const numeric & _num8(void)
{
	const static ex e = ex(numeric(8));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex8(void)
{
	static ex * e = new ex(_num8());
	return *e;
}

// numeric  9
const numeric & _num9(void)
{
	const static ex e = ex(numeric(9));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex9(void)
{
	static ex * e = new ex(_num9());
	return *e;
}

// numeric  10
const numeric & _num10(void)
{
	const static ex e = ex(numeric(10));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex10(void)
{
	static ex * e = new ex(_num10());
	return *e;
}

// numeric  11
const numeric & _num11(void)
{
	const static ex e = ex(numeric(11));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex11(void)
{
	static ex * e = new ex(_num11());
	return *e;
}

// numeric  12
const numeric & _num12(void)
{
	const static ex e = ex(numeric(12));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex12(void)
{
	static ex * e = new ex(_num12());
	return *e;
}

// numeric  15
const numeric & _num15(void)
{
	const static ex e = ex(numeric(15));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex15(void)
{
	static ex * e = new ex(_num15());
	return *e;
}

// numeric  18
const numeric & _num18(void)
{
	const static ex e = ex(numeric(18));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex18(void)
{
	static ex * e = new ex(_num18());
	return *e;
}

// numeric  20
const numeric & _num20(void)
{
	const static ex e = ex(numeric(20));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex20(void)
{
	static ex * e = new ex(_num20());
	return *e;
}

// numeric  24
const numeric & _num24(void)
{
	const static ex e = ex(numeric(24));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex24(void)
{
	static ex * e = new ex(_num24());
	return *e;
}

// numeric  25
const numeric & _num25(void)
{
	const static ex e = ex(numeric(25));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex25(void)
{
	static ex * e = new ex(_num25());
	return *e;
}

// numeric  30
const numeric & _num30(void)
{
	const static ex e = ex(numeric(30));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex30(void)
{
	static ex * e = new ex(_num30());
	return *e;
}

// numeric  48
const numeric & _num48(void)
{
	const static ex e = ex(numeric(48));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex48(void)
{
	static ex * e = new ex(_num48());
	return *e;
}

// numeric  60
const numeric & _num60(void)
{
	const static ex e = ex(numeric(60));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex60(void)
{
	static ex * e = new ex(_num60());
	return *e;
}

// numeric  120
const numeric & _num120(void)
{
	const static ex e = ex(numeric(120));
	const static numeric * n = static_cast<const numeric *>(e.bp);
	return *n;
}

const ex & _ex120(void)
{
	static ex * e = new ex(_num120());
	return *e;
}

// comment skeleton for header files


// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
	// none

	// other constructors
	// none

	// functions overriding virtual functions from bases classes
	// none
	
	// new virtual functions which can be overridden by derived classes
	// none

	// non-virtual functions in this class
	// none

// member variables
// none
	


// comment skeleton for implementation files


//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public
// protected

//////////
// other constructors
//////////

// public
// none

//////////
// functions overriding virtual functions from bases classes
//////////

// public
// protected
// none

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public
// protected
// none

//////////
// non-virtual functions in this class
//////////

// public
// protected
// none

//////////
// static member variables
//////////

// protected
// private
// none


#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
