/** @file constant.cpp
 *
 *  Implementation of GiNaC's constant types and some special constants. */

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

#include <string>
#include <stdexcept>

#include "constant.h"
#include "numeric.h"
#include "ex.h"
#include "print.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(constant, basic)

//////////
// default ctor, dtor, copy ctor assignment operator and helpers
//////////

// public

constant::constant() : basic(TINFO_constant), name(""), ef(0), number(0), serial(next_serial++)
{
	debugmsg("constant default ctor",LOGLEVEL_CONSTRUCT);
}

// protected

/** For use by copy ctor and assignment operator. */
void constant::copy(const constant & other)
{
	inherited::copy(other);
	name = other.name;
	serial = other.serial;
	ef = other.ef;
	if (other.number != 0)
		number = new numeric(*other.number);
	else
		number = 0;
}

void constant::destroy(bool call_parent)
{
	delete number;
	if (call_parent)
		inherited::destroy(call_parent);
}

//////////
// other ctors
//////////

// public

constant::constant(const std::string & initname, evalffunctype efun)
  : basic(TINFO_constant), name(initname), ef(efun), number(0), serial(next_serial++)
{
	debugmsg("constant ctor from string, function",LOGLEVEL_CONSTRUCT);
	setflag(status_flags::evaluated);
}

constant::constant(const std::string & initname, const numeric & initnumber)
  : basic(TINFO_constant), name(initname), ef(0), number(new numeric(initnumber)), serial(next_serial++)
{
	debugmsg("constant ctor from string, numeric",LOGLEVEL_CONSTRUCT);
	setflag(status_flags::evaluated);
}

//////////
// archiving
//////////

constant::constant(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("constant ctor from archive_node", LOGLEVEL_CONSTRUCT);
}

ex constant::unarchive(const archive_node &n, const lst &sym_lst)
{
	// Find constant by name (!! this is bad: 'twould be better if there
	// was a list of all global constants that we could search)
	std::string s;
	if (n.find_string("name", s)) {
		if (s == Pi.name)
			return Pi;
		else if (s == Catalan.name)
			return Catalan;
		else if (s == Euler.name)
			return Euler;
		else
			throw (std::runtime_error("unknown constant '" + s + "' in archive"));
	} else
		throw (std::runtime_error("unnamed constant in archive"));
}

void constant::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_string("name", name);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

void constant::print(const print_context & c, unsigned level) const
{
	debugmsg("constant print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << name << " (" << class_name() << ")"
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << std::endl;

	} else
		c.s << name;
}

int constant::degree(const ex & s) const
{
	return is_equal(*s.bp) ? 1 : 0;
}

int constant::ldegree(const ex & s) const
{
	return is_equal(*s.bp) ? 1 : 0;
}

ex constant::coeff(const ex & s, int n) const
{
	if (is_equal(*s.bp))
		return n==1 ? _ex1() : _ex0();
	else
		return n==0 ? *this : _ex0();
}

ex constant::evalf(int level) const
{
	if (ef!=0) {
		return ef();
	} else if (number != 0) {
		return number->evalf();
	}
	return *this;
}

// protected

/** Implementation of ex::diff() for a constant. It always returns 0.
 *
 *  @see ex::diff */
ex constant::derivative(const symbol & s) const
{
	return _ex0();
}

int constant::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_exactly_of_type(other, constant));
	// const constant & o=static_cast<constant &>(const_cast<basic &>(other));
	// return name.compare(o.name);
	const constant *o = static_cast<const constant *>(&other);
	if (serial==o->serial) return 0;
	return serial < o->serial ? -1 : 1;
}

bool constant::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_exactly_of_type(other, constant));
	const constant *o = static_cast<const constant *>(&other);
	return serial==o->serial;
}

unsigned constant::calchash(void) const
{
	hashvalue = golden_ratio_hash(tinfo() ^ serial);
	// mask out numeric hashes:
	hashvalue &= 0x7FFFFFFFU;
	
	setflag(status_flags::hash_calculated);
	
	return hashvalue;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// none

//////////
// static member variables
//////////

unsigned constant::next_serial = 0;

//////////
// global constants
//////////

/**  Pi. (3.14159...)  Diverts straight into CLN for evalf(). */
const constant Pi("Pi", PiEvalf);

/** Euler's constant. (0.57721...)  Sometimes called Euler-Mascheroni constant.
 *  Diverts straight into CLN for evalf(). */
const constant Euler("Euler", EulerEvalf);

/** Catalan's constant. (0.91597...)  Diverts straight into CLN for evalf(). */
const constant Catalan("Catalan", CatalanEvalf);

} // namespace GiNaC
