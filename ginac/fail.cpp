/** @file fail.cpp
 *
 *  Implementation of class signaling failure of operation. Considered
 *  obsolete all this stuff ought to be replaced by exceptions. */

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

#include "fail.h"
#include "archive.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(fail, basic)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

fail::fail() : inherited(TINFO_fail)
{
	debugmsg("fail default constructor",LOGLEVEL_CONSTRUCT);
}

// protected

void fail::copy(const fail & other)
{
	inherited::copy(other);
}

void fail::destroy(bool call_parent)
{
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
fail::fail(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("fail constructor from archive_node", LOGLEVEL_CONSTRUCT);
}

/** Unarchive the object. */
ex fail::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new fail(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void fail::archive(archive_node &n) const
{
	inherited::archive(n);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

void fail::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("fail print",LOGLEVEL_PRINT);
	os << "FAIL";
}

void fail::printraw(std::ostream & os) const
{
	debugmsg("fail printraw",LOGLEVEL_PRINT);
	os << "FAIL";
}

// protected

int fail::compare_same_type(const basic & other) const
{
	// two fails are always identical
	return 0;
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
