/** @file clifford.cpp
 *
 *  Implementation of GiNaC's clifford objects.
 *  No real implementation yet, to be done.     */

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

#include <string>

#include "clifford.h"
#include "ex.h"
#include "ncmul.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

clifford::clifford()
{
	debugmsg("clifford default constructor",LOGLEVEL_CONSTRUCT);
	serial=next_serial++;
	name=autoname_prefix()+ToString(serial);
	tinfo_key=TINFO_clifford;
}

clifford::~clifford()
{
	debugmsg("clifford destructor",LOGLEVEL_DESTRUCT);
	destroy(false);
}

clifford::clifford(const clifford & other)
{
	debugmsg("clifford copy constructor",LOGLEVEL_CONSTRUCT);
	copy (other);
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
	indexed::copy(other);
	name=other.name;
	serial=other.serial;
}

void clifford::destroy(bool call_parent)
{
	if (call_parent) {
		indexed::destroy(call_parent);
	}
}

//////////
// other constructors
//////////

// public

clifford::clifford(const std::string & initname)
{
	debugmsg("clifford constructor from string",LOGLEVEL_CONSTRUCT);
	name=initname;
	serial=next_serial++;
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
	os << "clifford(" << "name=" << name << ",serial=" << serial
	   << ",indices=";
	printrawindices(os);
	os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void clifford::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("clifford printtree",LOGLEVEL_PRINT);
	os << std::string(indent,' ') << name << " (clifford): "
	   << "serial=" << serial << ","
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

void clifford::printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence) const
{
	debugmsg("clifford print csrc",LOGLEVEL_PRINT);
	print(os,upper_precedence);
}

bool clifford::info(unsigned inf) const
{
	return indexed::info(inf);
}

// protected

int clifford::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(other.tinfo() == TINFO_clifford);
	const clifford *o = static_cast<const clifford *>(&other);
	if (serial==o->serial) {
		return indexed::compare_same_type(other);
	}
	return serial < o->serial ? -1 : 1;
}

ex clifford::simplify_ncmul(const exvector & v) const
{
	return simplified_ncmul(v);
}

unsigned clifford::calchash(void) const
{
	hashvalue=golden_ratio_hash(golden_ratio_hash(0x55555556U ^
												  golden_ratio_hash(tinfo_key) ^
												  serial));
	setflag(status_flags::hash_calculated);
	return hashvalue;
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

void clifford::setname(const std::string & n)
{
	name = n;
}

// private

std::string & clifford::autoname_prefix(void)
{
	static std::string * s = new std::string("clifford");
	return *s;
}

//////////
// static member variables
//////////

// private

unsigned clifford::next_serial=0;

//////////
// global constants
//////////

const clifford some_clifford;
const type_info & typeid_clifford=typeid(some_clifford);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
