/** @file fail.cpp
 *
 *  Implementation of class signaling failure of operation. Considered
 *  obsolete all this stuff ought to be replaced by exceptions. */

/*
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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
#include "debugmsg.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

fail::fail() : basic(TINFO_fail)
{
    debugmsg("fail default constructor",LOGLEVEL_CONSTRUCT);
}

fail::~fail()
{
    debugmsg("fail destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

fail::fail(fail const & other)
{
    debugmsg("fail copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

fail const & fail::operator=(fail const & other)
{
    debugmsg("fail operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void fail::copy(fail const & other)
{
    basic::copy(other);
}

void fail::destroy(bool call_parent)
{
    if (call_parent) basic::destroy(call_parent);
}


//////////
// other constructors
//////////

// none

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * fail::duplicate() const
{
    debugmsg("fail duplicate",LOGLEVEL_DUPLICATE);
    return new fail(*this);
}

void fail::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("fail print",LOGLEVEL_PRINT);
    os << "FAIL";
}

void fail::printraw(ostream & os) const
{
    debugmsg("fail printraw",LOGLEVEL_PRINT);
    os << "FAIL";
}

// protected

int fail::compare_same_type(basic const & other) const
{
	// two fails are always identical
    return 0;
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

const fail some_fail;
type_info const & typeid_fail=typeid(some_fail);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE
