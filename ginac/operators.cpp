/** @file operators.cpp
 *
 *  Implementation of GiNaC's overloaded operators. */

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

#include "operators.h"
#include "numeric.h"
#include "add.h"
#include "mul.h"
#include "power.h"
#include "ncmul.h"
#include "relational.h"
#include "print.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

/** Used internally by operator+() to add two ex objects together. */
static inline const ex exadd(const ex & lh, const ex & rh)
{
	return (new add(lh,rh))->setflag(status_flags::dynallocated);
}

/** Used internally by operator*() to multiply two ex objects together. */
static inline const ex exmul(const ex & lh, const ex & rh)
{
	// Check if we are constructing a mul object or a ncmul object.  Due to
	// ncmul::eval()'s rule to pull out commutative elements we need to check
	// only one of the elements.
	if (rh.return_type()==return_types::commutative ||
	    lh.return_type()==return_types::commutative)
		return (new mul(lh,rh))->setflag(status_flags::dynallocated);
	else
		return (new ncmul(lh,rh))->setflag(status_flags::dynallocated);
}

/** Used internally by operator-() and friends to change the sign of an argument. */
static inline const ex exminus(const ex & lh)
{
	return (new mul(lh,_ex_1()))->setflag(status_flags::dynallocated);
}

// binary arithmetic operators ex with ex

const ex operator+(const ex & lh, const ex & rh)
{
	debugmsg("operator+(ex,ex)",LOGLEVEL_OPERATOR);
	return exadd(lh, rh);
}

const ex operator-(const ex & lh, const ex & rh)
{
	debugmsg("operator-(ex,ex)",LOGLEVEL_OPERATOR);
	return exadd(lh, exminus(rh));
}

const ex operator*(const ex & lh, const ex & rh)
{
	debugmsg("operator*(ex,ex)",LOGLEVEL_OPERATOR);
	return exmul(lh, rh);
}

const ex operator/(const ex & lh, const ex & rh)
{
	debugmsg("operator/(ex,ex)",LOGLEVEL_OPERATOR);
	return exmul(lh, power(rh,_ex_1()));
}


// binary arithmetic operators numeric with numeric

const numeric operator+(const numeric & lh, const numeric & rh)
{
	debugmsg("operator+(numeric,numeric)",LOGLEVEL_OPERATOR);
	return lh.add(rh);
}

const numeric operator-(const numeric & lh, const numeric & rh)
{
	debugmsg("operator-(numeric,numeric)",LOGLEVEL_OPERATOR);
	return lh.sub(rh);
}

const numeric operator*(const numeric & lh, const numeric & rh)
{
	debugmsg("operator*(numeric,numeric)",LOGLEVEL_OPERATOR);
	return lh.mul(rh);
}

const numeric operator/(const numeric & lh, const numeric & rh)
{
	debugmsg("operator/(numeric,ex)",LOGLEVEL_OPERATOR);
	return lh.div(rh);
}


// binary arithmetic assignment operators with ex

ex & operator+=(ex & lh, const ex & rh)
{
	debugmsg("operator+=(ex,ex)",LOGLEVEL_OPERATOR);
	return lh = exadd(lh, rh);
}

ex & operator-=(ex & lh, const ex & rh)
{
	debugmsg("operator-=(ex,ex)",LOGLEVEL_OPERATOR);
	return lh = exadd(lh, exminus(rh));
}

ex & operator*=(ex & lh, const ex & rh)
{
	debugmsg("operator*=(ex,ex)",LOGLEVEL_OPERATOR);
	return lh = exmul(lh, rh);
}

ex & operator/=(ex & lh, const ex & rh)
{
	debugmsg("operator/=(ex,ex)",LOGLEVEL_OPERATOR);
	return lh = exmul(lh, power(rh,_ex_1()));
}


// binary arithmetic assignment operators with numeric

numeric & operator+=(numeric & lh, const numeric & rh)
{
	debugmsg("operator+=(numeric,numeric)",LOGLEVEL_OPERATOR);
	lh = lh.add(rh);
	return lh;
}

numeric & operator-=(numeric & lh, const numeric & rh)
{
	debugmsg("operator-=(numeric,numeric)",LOGLEVEL_OPERATOR);
	lh = lh.sub(rh);
	return lh;
}

numeric & operator*=(numeric & lh, const numeric & rh)
{
	debugmsg("operator*=(numeric,numeric)",LOGLEVEL_OPERATOR);
	lh = lh.mul(rh);
	return lh;
}

numeric & operator/=(numeric & lh, const numeric & rh)
{
	debugmsg("operator/=(numeric,numeric)",LOGLEVEL_OPERATOR);
	lh = lh.div(rh);
	return lh;
}


// unary operators

const ex operator+(const ex & lh)
{
	debugmsg("operator+(ex)",LOGLEVEL_OPERATOR);
	return lh;
}

const ex operator-(const ex & lh)
{
	debugmsg("operator-(ex)",LOGLEVEL_OPERATOR);
	return exminus(lh);
}

const numeric operator+(const numeric & lh)
{
	debugmsg("operator+(numeric)",LOGLEVEL_OPERATOR);
	return lh;
}

const numeric operator-(const numeric & lh)
{
	debugmsg("operator-(numeric)",LOGLEVEL_OPERATOR);
	return _num_1().mul(lh);
}


// increment / decrement operators

/** Expression prefix increment.  Adds 1 and returns incremented ex. */
ex & operator++(ex & rh)
{
	debugmsg("operator++(ex)",LOGLEVEL_OPERATOR);
	return rh = exadd(rh, _ex1());
}

/** Expression prefix decrement.  Subtracts 1 and returns decremented ex. */
ex & operator--(ex & rh)
{
	debugmsg("operator--(ex)",LOGLEVEL_OPERATOR);
	return rh = exadd(rh, _ex_1());
}

/** Expression postfix increment.  Returns the ex and leaves the original
 *  incremented by 1. */
const ex operator++(ex & lh, int)
{
	debugmsg("operator++(ex,int)",LOGLEVEL_OPERATOR);
	ex tmp(lh);
	lh = exadd(lh, _ex1());
	return tmp;
}

/** Expression Postfix decrement.  Returns the ex and leaves the original
 *  decremented by 1. */
const ex operator--(ex & lh, int)
{
	debugmsg("operator--(ex,int)",LOGLEVEL_OPERATOR);
	ex tmp(lh);
	lh = exadd(lh, _ex_1());
	return tmp;
}

/** Numeric prefix increment.  Adds 1 and returns incremented number. */
numeric& operator++(numeric & rh)
{
	debugmsg("operator++(numeric)",LOGLEVEL_OPERATOR);
	rh = rh.add(_num1());
	return rh;
}

/** Numeric prefix decrement.  Subtracts 1 and returns decremented number. */
numeric& operator--(numeric & rh)
{
	debugmsg("operator--(numeric)",LOGLEVEL_OPERATOR);
	rh = rh.add(_num_1());
	return rh;
}

/** Numeric postfix increment.  Returns the number and leaves the original
 *  incremented by 1. */
const numeric operator++(numeric & lh, int)
{
	debugmsg("operator++(numeric,int)",LOGLEVEL_OPERATOR);
	numeric tmp(lh);
	lh = lh.add(_num1());
	return tmp;
}

/** Numeric Postfix decrement.  Returns the number and leaves the original
 *  decremented by 1. */
const numeric operator--(numeric & lh, int)
{
	debugmsg("operator--(numeric,int)",LOGLEVEL_OPERATOR);
	numeric tmp(lh);
	lh = lh.add(_num_1());
	return tmp;
}

// binary relational operators ex with ex

relational operator==(const ex & lh, const ex & rh)
{
	debugmsg("operator==(ex,ex)",LOGLEVEL_OPERATOR);
	return relational(lh,rh,relational::equal);
}

relational operator!=(const ex & lh, const ex & rh)
{
	debugmsg("operator!=(ex,ex)",LOGLEVEL_OPERATOR);
	return relational(lh,rh,relational::not_equal);
}

relational operator<(const ex & lh, const ex & rh)
{
	debugmsg("operator<(ex,ex)",LOGLEVEL_OPERATOR);
	return relational(lh,rh,relational::less);
}

relational operator<=(const ex & lh, const ex & rh)
{
	debugmsg("operator<=(ex,ex)",LOGLEVEL_OPERATOR);
	return relational(lh,rh,relational::less_or_equal);
}

relational operator>(const ex & lh, const ex & rh)
{
	debugmsg("operator>(ex,ex)",LOGLEVEL_OPERATOR);
	return relational(lh,rh,relational::greater);
}

relational operator>=(const ex & lh, const ex & rh)
{
	debugmsg("operator>=(ex,ex)",LOGLEVEL_OPERATOR);
	return relational(lh,rh,relational::greater_or_equal);
}

// input/output stream operators

std::ostream & operator<<(std::ostream & os, const ex & e)
{
	e.print(print_context(os));
	return os;
}

std::istream & operator>>(std::istream & is, ex & e)
{
	throw (std::logic_error("expression input from streams not implemented"));
}

} // namespace GiNaC
