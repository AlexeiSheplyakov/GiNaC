/** @file clifford.cpp
 *
 *  Implementation of GiNaC's clifford objects.
 *  No real implementation yet, to be done.     */

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

#include "clifford.h"
#include "ex.h"
#include "ncmul.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(clifford, lortensor)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

clifford::clifford()
{
	debugmsg("clifford default constructor",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_clifford;
}

clifford::~clifford()
{
	debugmsg("clifford destructor",LOGLEVEL_DESTRUCT);
	destroy(false);
}

clifford::clifford(const clifford & other)
{
	debugmsg("clifford copy constructor",LOGLEVEL_CONSTRUCT);
	copy(other);
}

const clifford & clifford::operator=(const clifford & other)
{
	debugmsg("clifford operator=",LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(true);
		copy(other);
	}
	return *this;
}

// protected

void clifford::copy(const clifford & other)
{
	inherited::copy(other);
}

void clifford::destroy(bool call_parent)
{
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

clifford::clifford(const std::string & n, const ex & mu) : inherited(lortensor_symbolic, n, mu)
{
	debugmsg("clifford constructor from string,ex",LOGLEVEL_CONSTRUCT);
	tinfo_key=TINFO_clifford;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * clifford::duplicate() const
{
	debugmsg("clifford duplicate",LOGLEVEL_DUPLICATE);
	return new clifford(*this);
}

void clifford::printraw(std::ostream & os) const
{
	debugmsg("clifford printraw",LOGLEVEL_PRINT);
	os << "clifford(" << "indices=";
	printrawindices(os);
	os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void clifford::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("clifford printtree",LOGLEVEL_PRINT);
	os << std::string(indent,' ') << " (clifford): "
	   << seq.size() << "indices=";
	printtreeindices(os, indent);
	os << ", hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags << std::endl;
}

void clifford::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("clifford print",LOGLEVEL_PRINT);
	os << name;
	printindices(os);
}

bool clifford::info(unsigned inf) const
{
	return inherited::info(inf);
}

// protected

int clifford::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,clifford));
	// only compare indices
	return exprseq::compare_same_type(other);
}

bool clifford::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,clifford));
	// only compare indices
	return exprseq::is_equal_same_type(other);
}

ex clifford::simplify_ncmul(const exvector & v) const
{
	return simplified_ncmul(v);
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// none

//////////
// static member variables
//////////

// none

//////////
// friend functions
//////////

/** Construct an object representing a Dirac gamma matrix. The index must
 *  be of class lorentzidx.
 *
 *  @param mu Index
 *  @return newly constructed object */
clifford clifford_gamma(const ex & mu)
{
	return clifford("gamma", mu);
}

//////////
// global constants
//////////

const clifford some_clifford;
const std::type_info & typeid_clifford = typeid(some_clifford);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
