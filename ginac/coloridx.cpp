/** @file coloridx.cpp
 *
 *  Implementation of GiNaC's color indices. */

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

#include <stdexcept>

#include "coloridx.h"
#include "utils.h"

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

coloridx::coloridx(coloridx const & other)
{
    debugmsg("coloridx copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

coloridx const & coloridx::operator=(coloridx const & other)
{
    debugmsg("coloridx operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void coloridx::copy(coloridx const & other)
{
    idx::copy(other);
}

void coloridx::destroy(bool call_parent)
{
    if (call_parent) idx::destroy(call_parent);
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

coloridx::coloridx(string const & n, bool cov) : idx(n,cov)
{
    debugmsg("coloridx constructor from string,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_coloridx;
}

coloridx::coloridx(char const * n, bool cov) : idx(n,cov)
{
    debugmsg("coloridx constructor from char*,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_coloridx;
}

coloridx::coloridx(unsigned const v, bool cov) : idx(v,cov)
{
    debugmsg("coloridx constructor from unsigned,bool",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_coloridx;
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

void coloridx::printraw(ostream & os) const
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

void coloridx::printtree(ostream & os, unsigned indent) const
{
    debugmsg("coloridx printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "coloridx: ";

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
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void coloridx::print(ostream & os, unsigned upper_precedence) const
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
type_info const & typeid_coloridx=typeid(some_coloridx);



