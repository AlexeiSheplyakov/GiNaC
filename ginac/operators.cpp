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
#include "power.h"
#include "relational.h"
#include "print.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

// binary arithmetic operators ex with ex

ex operator+(const ex & lh, const ex & rh)
{
	debugmsg("operator+(ex,ex)",LOGLEVEL_OPERATOR);
	return lh.exadd(rh);
}

ex operator-(const ex & lh, const ex & rh)
{
	debugmsg("operator-(ex,ex)",LOGLEVEL_OPERATOR);
	return lh.exadd(rh.exmul(_ex_1()));
}

ex operator*(const ex & lh, const ex & rh)
{
	debugmsg("operator*(ex,ex)",LOGLEVEL_OPERATOR);
	return lh.exmul(rh);
}

ex operator/(const ex & lh, const ex & rh)
{
	debugmsg("operator/(ex,ex)",LOGLEVEL_OPERATOR);
	return lh.exmul(power(rh,_ex_1()));
}


// binary arithmetic operators numeric with numeric

numeric operator+(const numeric & lh, const numeric & rh)
{
	debugmsg("operator+(numeric,numeric)",LOGLEVEL_OPERATOR);
	return lh.add(rh);
}

numeric operator-(const numeric & lh, const numeric & rh)
{
	debugmsg("operator-(numeric,numeric)",LOGLEVEL_OPERATOR);
	return lh.sub(rh);
}

numeric operator*(const numeric & lh, const numeric & rh)
{
	debugmsg("operator*(numeric,numeric)",LOGLEVEL_OPERATOR);
	return lh.mul(rh);
}

numeric operator/(const numeric & lh, const numeric & rh)
{
	debugmsg("operator/(numeric,ex)",LOGLEVEL_OPERATOR);
	return lh.div(rh);
}


// binary arithmetic assignment operators with ex

const ex & operator+=(ex & lh, const ex & rh)
{
	debugmsg("operator+=(ex,ex)",LOGLEVEL_OPERATOR);
	return (lh=lh.exadd(rh));
}

const ex & operator-=(ex & lh, const ex & rh)
{
	debugmsg("operator-=(ex,ex)",LOGLEVEL_OPERATOR);
	return (lh=lh.exadd(rh.exmul(_ex_1())));
}

const ex & operator*=(ex & lh, const ex & rh)
{
	debugmsg("operator*=(ex,ex)",LOGLEVEL_OPERATOR);
	return (lh=lh.exmul(rh));
}

const ex & operator/=(ex & lh, const ex & rh)
{
	debugmsg("operator/=(ex,ex)",LOGLEVEL_OPERATOR);
	return (lh=lh.exmul(power(rh,_ex_1())));
}


// binary arithmetic assignment operators with numeric

const numeric & operator+=(numeric & lh, const numeric & rh)
{
	debugmsg("operator+=(numeric,numeric)",LOGLEVEL_OPERATOR);
	return (lh=lh.add(rh));
}

const numeric & operator-=(numeric & lh, const numeric & rh)
{
	debugmsg("operator-=(numeric,numeric)",LOGLEVEL_OPERATOR);
	return (lh=lh.sub(rh));
}

const numeric & operator*=(numeric & lh, const numeric & rh)
{
	debugmsg("operator*=(numeric,numeric)",LOGLEVEL_OPERATOR);
	return (lh=lh.mul(rh));
}

const numeric & operator/=(numeric & lh, const numeric & rh)
{
	debugmsg("operator/=(numeric,numeric)",LOGLEVEL_OPERATOR);
	return (lh=lh.div(rh));
}

// unary operators

ex operator+(const ex & lh)
{
	debugmsg("operator+(ex)",LOGLEVEL_OPERATOR);
	return lh;
}

ex operator-(const ex & lh)
{
	debugmsg("operator-(ex)",LOGLEVEL_OPERATOR);
	return lh.exmul(_ex_1());
}

numeric operator+(const numeric & lh)
{
	debugmsg("operator+(numeric)",LOGLEVEL_OPERATOR);
	return lh;
}

numeric operator-(const numeric & lh)
{
	debugmsg("operator-(numeric)",LOGLEVEL_OPERATOR);
	return _num_1().mul(lh);
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
numeric operator++(numeric & lh, int)
{
	debugmsg("operator++(numeric,int)",LOGLEVEL_OPERATOR);
	numeric tmp(lh);
	lh = lh.add(_num1());
	return tmp;
}

/** Numeric Postfix decrement.  Returns the number and leaves the original
 *  decremented by 1. */
numeric operator--(numeric & lh, int)
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
