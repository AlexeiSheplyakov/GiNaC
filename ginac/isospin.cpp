/** @file isospin.cpp
 *
 *  Implementation of GiNaC's isospin objects.
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

#include "isospin.h"
#include "ex.h"
#include "ncmul.h"
#include "utils.h"
#include "debugmsg.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

isospin::isospin()
{
    debugmsg("isospin default constructor",LOGLEVEL_CONSTRUCT);
    serial=next_serial++;
    name=autoname_prefix()+ToString(serial);
    tinfo_key=TINFO_isospin;
}

isospin::~isospin()
{
    debugmsg("isospin destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

isospin::isospin(const isospin & other)
{
    debugmsg("isospin copy constructor",LOGLEVEL_CONSTRUCT);
    copy (other);
}

const isospin & isospin::operator=(const isospin & other)
{
    debugmsg("isospin operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void isospin::copy(const isospin & other)
{
    indexed::copy(other);
    name=other.name;
    serial=other.serial;
}

void isospin::destroy(bool call_parent)
{
    if (call_parent) {
        indexed::destroy(call_parent);
    }
}

//////////
// other constructors
//////////

// public

isospin::isospin(const string & initname)
{
    debugmsg("isospin constructor from string",LOGLEVEL_CONSTRUCT);
    name=initname;
    serial=next_serial++;
    tinfo_key=TINFO_isospin;
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * isospin::duplicate() const
{
    debugmsg("isospin duplicate",LOGLEVEL_DUPLICATE);
    return new isospin(*this);
}

void isospin::printraw(ostream & os) const
{
    debugmsg("isospin printraw",LOGLEVEL_PRINT);
    os << "isospin(" << "name=" << name << ",serial=" << serial
       << ",indices=";
    printrawindices(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void isospin::printtree(ostream & os, unsigned indent) const
{
    debugmsg("isospin printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << name << " (isospin): "
       << "serial=" << serial << ","
       << seq.size() << "indices=";
    printtreeindices(os,indent);
    os << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void isospin::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("isospin print",LOGLEVEL_PRINT);
    os << name;
    printindices(os);
}

void isospin::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("isospin print csrc",LOGLEVEL_PRINT);
    print(os,upper_precedence);
}

bool isospin::info(unsigned inf) const
{
    return indexed::info(inf);
}

// protected

int isospin::compare_same_type(const basic & other) const
{
    GINAC_ASSERT(other.tinfo() == TINFO_isospin);
    const isospin *o = static_cast<const isospin *>(&other);
    if (serial==o->serial) {
        return indexed::compare_same_type(other);
    }
    return serial < o->serial ? -1 : 1;
}

ex isospin::simplify_ncmul(const exvector & v) const
{
    return simplified_ncmul(v);
}

unsigned isospin::calchash(void) const
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

void isospin::setname(const string & n)
{
    name=n;
}

// private

string & isospin::autoname_prefix(void)
{
    static string * s=new string("isospin");
    return *s;
}

//////////
// static member variables
//////////

// private

unsigned isospin::next_serial=0;

//////////
// global constants
//////////

const isospin some_isospin;
const type_info & typeid_isospin=typeid(some_isospin);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

