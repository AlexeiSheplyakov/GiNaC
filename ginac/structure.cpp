/** @file structure.cpp
 *
 *  Implementation of 'abstract' class structure.
 *
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

#include <string>

#include "structure.h"

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

structure::structure()
{
    debugmsg("structure default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key = TINFO_structure;
}

structure::~structure()
{
    debugmsg("structure destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

structure::structure(structure const & other)
{
    debugmsg("structure copy constructor",LOGLEVEL_CONSTRUCT);
    copy(other);
}

structure const & structure::operator=(structure const & other)
{
    debugmsg("structure operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
        copy(other);
    }
    return *this;
}

// protected

void structure::copy(structure const & other)
{
    basic::copy(other);
}

void structure::destroy(bool call_parent)
{
    if (call_parent) basic::destroy(call_parent);
}

//////////
// other constructors
//////////

// none

//////////
// structures overriding virtual structures from bases classes
//////////

// public

basic * structure::duplicate() const
{
    debugmsg("structure duplicate",LOGLEVEL_DUPLICATE);
    return new structure(*this);
}

void structure::printraw(ostream & os) const
{
    debugmsg("structure printraw",LOGLEVEL_PRINT);

    os << "structure(hash=" << hashvalue << ",flags=" << flags << ")";
}

void structure::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("structure print",LOGLEVEL_PRINT);

    os << "structure()";
}

void structure::printtree(ostream & os, unsigned indent) const
{
    debugmsg("structure printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "structure "
       << "hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void structure::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("structure print csrc",LOGLEVEL_PRINT);

    os << "structure()";
}

// protected

int structure::compare_same_type(basic const & other) const
{
    ASSERT(is_of_type(other, structure));
    return 0; // all structures are the same
}

bool structure::is_equal_same_type(basic const & other) const
{
    ASSERT(is_of_type(other, structure));
    return true; // all structures are the same
}

//////////
// new virtual structures which can be overridden by derived classes
//////////

// none

//////////
// non-virtual structures in this class
//////////

// protected

vector<registered_structure_info> & structure::registered_structures(void)
{
    static vector<registered_structure_info> * rs=new vector<registered_structure_info>;
    return *rs;
}

// public

unsigned structure::register_new(char const * nm)
{
    registered_structure_info rsi={nm};
    registered_structures().push_back(rsi);
    return registered_structures().size()-1;
}

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const structure some_structure;
type_info const & typeid_structure=typeid(some_structure);

