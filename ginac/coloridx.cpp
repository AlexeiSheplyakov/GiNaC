/** @file coloridx.cpp
 *
 *  Implementation of GiNaC's color indices. */

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

#include <stdexcept>

#include "coloridx.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(coloridx, idx)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

coloridx::coloridx()
{
	debugmsg("coloridx default constructor",LOGLEVEL_CONSTRUCT);
	// serial is incremented in idx::idx()
	name="color"+ToString(serial);
	tinfo_key=TINFO_coloridx;
}

coloridx::~coloridx() 
{
	debugmsg("coloridx destructor",LOGLEVEL_DESTRUCT);
	destroy(0);
}

coloridx::coloridx(const coloridx & other)
{
	debugmsg("coloridx copy constructor",LOGLEVEL_CONSTRUCT);
	copy(other);
}

const coloridx & coloridx::operator=(const coloridx & other)
{
	debugmsg("coloridx operator=",LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(1);
		copy(other);
	}
	return *this;
}

// protected

void coloridx::copy(const coloridx & other)
{
	inherited::copy(other);
}

void coloridx::destroy(bool call_parent)
{
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// other constructors
//////////

// public

coloridx::coloridx(bool cov) : idx(cov)
{
	debugmsg("coloridx constructor from bool",LOGLEVEL_CONSTRUCT);
	// serial is incremented in idx::idx(bool)
	name="color"+ToString(serial);
	tinfo_key=TINFO_coloridx;
}

coloridx::coloridx(const std::string & n, bool cov) : idx(n,cov)
{
	debugmsg("coloridx constructor from string,bool",LOGLEVEL_CONSTRUCT);
	tinfo_key=TINFO_coloridx;
}

coloridx::coloridx(const char * n, bool cov) : idx(n,cov)
{
	debugmsg("coloridx constructor from char*,bool",LOGLEVEL_CONSTRUCT);
	tinfo_key=TINFO_coloridx;
}

coloridx::coloridx(unsigned v, bool cov) : idx(v,cov)
{
	debugmsg("coloridx constructor from unsigned,bool",LOGLEVEL_CONSTRUCT);
	tinfo_key=TINFO_coloridx;
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
coloridx::coloridx(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("coloridx constructor from archive_node", LOGLEVEL_CONSTRUCT);
}

/** Unarchive the object. */
ex coloridx::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new coloridx(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void coloridx::archive(archive_node &n) const
{
	inherited::archive(n);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * coloridx::duplicate() const
{
	debugmsg("coloridx duplicate",LOGLEVEL_DUPLICATE);
	return new coloridx(*this);
}

void coloridx::printraw(std::ostream & os) const
{
	debugmsg("coloridx printraw",LOGLEVEL_PRINT);

	os << "coloridx(";

	if (symbolic) {
		os << "symbolic,name=" << name;
	} else {
		os << "non symbolic,value=" << value;
	}

	if (covariant) {
		os << ",covariant";
	} else {
		os << ",contravariant";
	}

	os << ",serial=" << serial;
	os << ",hash=" << hashvalue << ",flags=" << flags;
	os << ")";
}

void coloridx::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("coloridx printtree",LOGLEVEL_PRINT);

	os << std::string(indent,' ') << "coloridx: ";

	if (symbolic) {
		os << "symbolic,name=" << name;
	} else {
		os << "non symbolic,value=" << value;
	}

	if (covariant) {
		os << ",covariant";
	} else {
		os << ",contravariant";
	}

	os << ", serial=" << serial
	   << ", hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags << std::endl;
}

void coloridx::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("coloridx print",LOGLEVEL_PRINT);

	if (covariant) {
		os << "_";
	} else {
		os << "~";
	}
	if (symbolic) {
		os << name;
	} else {
		os << value;
	}
}

bool coloridx::info(unsigned inf) const
{
	if (inf==info_flags::coloridx) return true;
	return idx::info(inf);
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

// none

//////////
// global constants
//////////

const coloridx some_coloridx;
const type_info & typeid_coloridx=typeid(some_coloridx);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
