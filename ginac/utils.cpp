/** @file utils.cpp
 *
 *  Implementation of several small and furry utilities needed within GiNaC
 *  but not of any interest to the user of the library. */

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

#include "ex.h"
#include "numeric.h"
#include "utils.h"
#include "version.h"

namespace GiNaC {

/* Version information buried into the library */
const int version_major = GINACLIB_MAJOR_VERSION;
const int version_minor = GINACLIB_MINOR_VERSION;
const int version_micro = GINACLIB_MICRO_VERSION;


/** ctor for pole_error exception class. */
pole_error::pole_error(const std::string& what_arg, int degree)
	: domain_error(what_arg), deg(degree) { }

/** Return the degree of the pole_error exception class. */
int pole_error::degree() const
{
	return deg;
}

/** Integer binary logarithm */
unsigned log2(unsigned n)
{
	unsigned k;
	for (k = 0; n > 1; n >>= 1)
		++k;
	return k;
}


//////////
// flyweight chest of numbers is initialized here:
//////////

/** How many static objects were created?  Only the first one must create
 *  the static flyweights on the heap. */
int library_init::count = 0;

// static numeric -120
const numeric *_num_120_p;
const numeric &_num_120 = *_num_120_p;
const ex _ex_120 = _num_120;

// static numeric -60
const numeric *_num_60_p;
const numeric &_num_60 = *_num_60_p;
const ex _ex_60 = _num_60;

// static numeric -48
const numeric *_num_48_p;
const numeric &_num_48 = *_num_48_p;
const ex _ex_48 = _num_48;

// static numeric -30
const numeric *_num_30_p;
const numeric &_num_30 = *_num_30_p;
const ex _ex_30 = _num_30;

// static numeric -25
const numeric *_num_25_p;
const numeric &_num_25 = *_num_25_p;
const ex _ex_25 = _num_25;

// static numeric -24
const numeric *_num_24_p;
const numeric &_num_24 = *_num_24_p;
const ex _ex_24 = _num_24;

// static numeric -20
const numeric *_num_20_p;
const numeric &_num_20 = *_num_20_p;
const ex _ex_20 = _num_20;

// static numeric -18
const numeric *_num_18_p;
const numeric &_num_18 = *_num_18_p;
const ex _ex_18 = _num_18;

// static numeric -15
const numeric *_num_15_p;
const numeric &_num_15 = *_num_15_p;
const ex _ex_15 = _num_15;

// static numeric -12
const numeric *_num_12_p;
const numeric &_num_12 = *_num_12_p;
const ex _ex_12 = _num_12;

// static numeric -11
const numeric *_num_11_p;
const numeric &_num_11 = *_num_11_p;
const ex _ex_11 = _num_11;

// static numeric -10
const numeric *_num_10_p;
const numeric &_num_10 = *_num_10_p;
const ex _ex_10 = _num_10;

// static numeric -9
const numeric *_num_9_p;
const numeric &_num_9 = *_num_9_p;
const ex _ex_9 = _num_9;

// static numeric -8
const numeric *_num_8_p;
const numeric &_num_8 = *_num_8_p;
const ex _ex_8 = _num_8;

// static numeric -7
const numeric *_num_7_p;
const numeric &_num_7 = *_num_7_p;
const ex _ex_7 = _num_7;

// static numeric -6
const numeric *_num_6_p;
const numeric &_num_6 = *_num_6_p;
const ex _ex_6 = _num_6;

// static numeric -5
const numeric *_num_5_p;
const numeric &_num_5 = *_num_5_p;
const ex _ex_5 = _num_5;

// static numeric -4
const numeric *_num_4_p;
const numeric &_num_4 = *_num_4_p;
const ex _ex_4 = _num_4;

// static numeric -3
const numeric *_num_3_p;
const numeric &_num_3 = *_num_3_p;
const ex _ex_3 = _num_3;

// static numeric -2
const numeric *_num_2_p;
const numeric &_num_2 = *_num_2_p;
const ex _ex_2 = _num_2;

// static numeric -1
const numeric *_num_1_p;
const numeric &_num_1 = *_num_1_p;
const ex _ex_1 = _num_1;

// static numeric -1/2
const numeric *_num_1_2_p;
const numeric &_num_1_2= *_num_1_2_p;
const ex _ex_1_2= _num_1_2;

// static numeric -1/3
const numeric *_num_1_3_p;
const numeric &_num_1_3= *_num_1_3_p;
const ex _ex_1_3= _num_1_3;

// static numeric -1/4
const numeric *_num_1_4_p;
const numeric &_num_1_4= *_num_1_4_p;
const ex _ex_1_4= _num_1_4;

// static numeric 0
const numeric *_num0_p;
const basic *_num0_bp;
const numeric &_num0 = *_num0_p;
const ex _ex0 = _num0;

// static numeric 1/4
const numeric *_num1_4_p;
const numeric &_num1_4 = *_num1_4_p;
const ex _ex1_4 = _num1_4;

// static numeric 1/3
const numeric *_num1_3_p;
const numeric &_num1_3 = *_num1_3_p;
const ex _ex1_3 = _num1_3;

// static numeric 1/2
const numeric *_num1_2_p;
const numeric &_num1_2 = *_num1_2_p;
const ex _ex1_2 = _num1_2;

// static numeric 1
const numeric *_num1_p;
const numeric &_num1 = *_num1_p;
const ex _ex1 = _num1;

// static numeric 2
const numeric *_num2_p;
const numeric &_num2 = *_num2_p;
const ex _ex2 = _num2;

// static numeric 3
const numeric *_num3_p;
const numeric &_num3 = *_num3_p;
const ex _ex3 = _num3;

// static numeric 4
const numeric *_num4_p;
const numeric &_num4 = *_num4_p;
const ex _ex4 = _num4;

// static numeric 5
const numeric *_num5_p;
const numeric &_num5 = *_num5_p;
const ex _ex5 = _num5;

// static numeric 6
const numeric *_num6_p;
const numeric &_num6 = *_num6_p;
const ex _ex6 = _num6;

// static numeric 7
const numeric *_num7_p;
const numeric &_num7 = *_num7_p;
const ex _ex7 = _num7;

// static numeric 8
const numeric *_num8_p;
const numeric &_num8 = *_num8_p;
const ex _ex8 = _num8;

// static numeric 9
const numeric *_num9_p;
const numeric &_num9 = *_num9_p;
const ex _ex9 = _num9;

// static numeric 10
const numeric *_num10_p;
const numeric &_num10 = *_num10_p;
const ex _ex10 = _num10;

// static numeric 11
const numeric *_num11_p;
const numeric &_num11 = *_num11_p;
const ex _ex11 = _num11;

// static numeric 12
const numeric *_num12_p;
const numeric &_num12 = *_num12_p;
const ex _ex12 = _num12;

// static numeric 15
const numeric *_num15_p;
const numeric &_num15 = *_num15_p;
const ex _ex15 = _num15;

// static numeric 18
const numeric *_num18_p;
const numeric &_num18 = *_num18_p;
const ex _ex18 = _num18;

// static numeric 20
const numeric *_num20_p;
const numeric &_num20 = *_num20_p;
const ex _ex20 = _num20;

// static numeric 24
const numeric *_num24_p;
const numeric &_num24 = *_num24_p;
const ex _ex24 = _num24;

// static numeric 25
const numeric *_num25_p;
const numeric &_num25 = *_num25_p;
const ex _ex25 = _num25;

// static numeric 30
const numeric *_num30_p;
const numeric &_num30 = *_num30_p;
const ex _ex30 = _num30;

// static numeric 48
const numeric *_num48_p;
const numeric &_num48 = *_num48_p;
const ex _ex48 = _num48;

// static numeric 60
const numeric *_num60_p;
const numeric &_num60 = *_num60_p;
const ex _ex60 = _num60;

// static numeric 120
const numeric *_num120_p;
const numeric &_num120 = *_num120_p;
const ex _ex120 = _num120;

/** Ctor of static initialization helpers.  The fist call to this is going
 *  to initialize the library, the others do nothing. */
library_init::library_init()
{
	if (count++==0) {
		_num_120_p= static_cast<const numeric*>(&((new numeric(-120))->setflag(status_flags::dynallocated)));
		_num_60_p = static_cast<const numeric*>(&((new numeric(-60))->setflag(status_flags::dynallocated)));
		_num_48_p = static_cast<const numeric*>(&((new numeric(-48))->setflag(status_flags::dynallocated)));
		_num_30_p = static_cast<const numeric*>(&((new numeric(-30))->setflag(status_flags::dynallocated)));
		_num_25_p = static_cast<const numeric*>(&((new numeric(-25))->setflag(status_flags::dynallocated)));
		_num_24_p = static_cast<const numeric*>(&((new numeric(-24))->setflag(status_flags::dynallocated)));
		_num_20_p = static_cast<const numeric*>(&((new numeric(-20))->setflag(status_flags::dynallocated)));
		_num_18_p = static_cast<const numeric*>(&((new numeric(-18))->setflag(status_flags::dynallocated)));
		_num_15_p = static_cast<const numeric*>(&((new numeric(-15))->setflag(status_flags::dynallocated)));
		_num_12_p = static_cast<const numeric*>(&((new numeric(-12))->setflag(status_flags::dynallocated)));
		_num_11_p = static_cast<const numeric*>(&((new numeric(-11))->setflag(status_flags::dynallocated)));
		_num_10_p = static_cast<const numeric*>(&((new numeric(-10))->setflag(status_flags::dynallocated)));
		_num_9_p  = static_cast<const numeric*>(&((new numeric(-9))->setflag(status_flags::dynallocated)));
		_num_8_p  = static_cast<const numeric*>(&((new numeric(-8))->setflag(status_flags::dynallocated)));
		_num_7_p  = static_cast<const numeric*>(&((new numeric(-7))->setflag(status_flags::dynallocated)));
		_num_6_p  = static_cast<const numeric*>(&((new numeric(-6))->setflag(status_flags::dynallocated)));
		_num_5_p  = static_cast<const numeric*>(&((new numeric(-5))->setflag(status_flags::dynallocated)));
		_num_4_p  = static_cast<const numeric*>(&((new numeric(-4))->setflag(status_flags::dynallocated)));
		_num_3_p  = static_cast<const numeric*>(&((new numeric(-3))->setflag(status_flags::dynallocated)));
		_num_2_p  = static_cast<const numeric*>(&((new numeric(-2))->setflag(status_flags::dynallocated)));
		_num_1_p  = static_cast<const numeric*>(&((new numeric(-1))->setflag(status_flags::dynallocated)));
		_num_1_2_p= static_cast<const numeric*>(&((new numeric(-1,2))->setflag(status_flags::dynallocated)));
		_num_1_3_p= static_cast<const numeric*>(&((new numeric(-1,3))->setflag(status_flags::dynallocated)));
		_num_1_4_p= static_cast<const numeric*>(&((new numeric(-1,4))->setflag(status_flags::dynallocated)));
		_num0_p   = static_cast<const numeric*>(&((new numeric(0))->setflag(status_flags::dynallocated)));
		_num0_bp  = _num0_p;  // Cf. class ex default ctor.
		_num1_4_p = static_cast<const numeric*>(&((new numeric(1,4))->setflag(status_flags::dynallocated)));
		_num1_3_p = static_cast<const numeric*>(&((new numeric(1,3))->setflag(status_flags::dynallocated)));
		_num1_2_p = static_cast<const numeric*>(&((new numeric(1,2))->setflag(status_flags::dynallocated)));
		_num1_p   = static_cast<const numeric*>(&((new numeric(1))->setflag(status_flags::dynallocated)));
		_num2_p   = static_cast<const numeric*>(&((new numeric(2))->setflag(status_flags::dynallocated)));
		_num3_p   = static_cast<const numeric*>(&((new numeric(3))->setflag(status_flags::dynallocated)));
		_num4_p   = static_cast<const numeric*>(&((new numeric(4))->setflag(status_flags::dynallocated)));
		_num5_p   = static_cast<const numeric*>(&((new numeric(5))->setflag(status_flags::dynallocated)));
		_num6_p   = static_cast<const numeric*>(&((new numeric(6))->setflag(status_flags::dynallocated)));
		_num7_p   = static_cast<const numeric*>(&((new numeric(7))->setflag(status_flags::dynallocated)));
		_num8_p   = static_cast<const numeric*>(&((new numeric(8))->setflag(status_flags::dynallocated)));
		_num9_p   = static_cast<const numeric*>(&((new numeric(9))->setflag(status_flags::dynallocated)));
		_num10_p  = static_cast<const numeric*>(&((new numeric(10))->setflag(status_flags::dynallocated)));
		_num11_p  = static_cast<const numeric*>(&((new numeric(11))->setflag(status_flags::dynallocated)));
		_num12_p  = static_cast<const numeric*>(&((new numeric(12))->setflag(status_flags::dynallocated)));
		_num15_p  = static_cast<const numeric*>(&((new numeric(15))->setflag(status_flags::dynallocated)));
		_num18_p  = static_cast<const numeric*>(&((new numeric(18))->setflag(status_flags::dynallocated)));
		_num20_p  = static_cast<const numeric*>(&((new numeric(20))->setflag(status_flags::dynallocated)));
		_num24_p  = static_cast<const numeric*>(&((new numeric(24))->setflag(status_flags::dynallocated)));
		_num25_p  = static_cast<const numeric*>(&((new numeric(25))->setflag(status_flags::dynallocated)));
		_num30_p  = static_cast<const numeric*>(&((new numeric(30))->setflag(status_flags::dynallocated)));
		_num48_p  = static_cast<const numeric*>(&((new numeric(48))->setflag(status_flags::dynallocated)));
		_num60_p  = static_cast<const numeric*>(&((new numeric(60))->setflag(status_flags::dynallocated)));
		_num120_p = static_cast<const numeric*>(&((new numeric(120))->setflag(status_flags::dynallocated)));

		// Initialize print context class info (this is not strictly necessary
		// but we do it anyway to make print_context_class_info::dump_hierarchy()
		// output the whole hierarchy whether or not the classes are actually
		// used)
		print_context::get_class_info_static();
		print_dflt::get_class_info_static();
		print_latex::get_class_info_static();
		print_python::get_class_info_static();
		print_python_repr::get_class_info_static();
		print_tree::get_class_info_static();
		print_csrc::get_class_info_static();
		print_csrc_float::get_class_info_static();
		print_csrc_double::get_class_info_static();
		print_csrc_cl_N::get_class_info_static();
	}
}


/** Dtor of static initialization helpers.  The last call to this is going
 *  to shut down the library, the others do nothing. */
library_init::~library_init()
{
	if (--count==0) {
		// In theory, we would have to clean up here.  But since we were
		// only initializing memory in the ctor and that memory is reclaimed
		// anyways by the OS when the program exits, we skip this.
	}
}

// comment skeleton for header files


// member functions

	// default constructor, destructor, copy constructor and assignment operator
	// none

	// other constructors
	// none

	// functions overriding virtual functions from base classes
	// none
	
	// new virtual functions which can be overridden by derived classes
	// none

	// non-virtual functions in this class
	// none

// member variables
// none
	


// comment skeleton for implementation files


//////////
// default constructor, destructor, copy constructor and assignment operator
//////////

// public
// protected

//////////
// other constructors
//////////

// public
// none

//////////
// functions overriding virtual functions from base classes
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


} // namespace GiNaC
