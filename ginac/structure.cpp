/** @file structure.cpp
 *
 *  Implementation of 'abstract' class structure. */

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

#include "structure.h"
#include "archive.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(structure, basic)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

structure::structure()
{
	debugmsg("structure default constructor",LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_structure;
}

// protected

void structure::copy(const structure & other)
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
// archiving
//////////

/** Construct object from archive_node. */
structure::structure(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("structure constructor from archive_node", LOGLEVEL_CONSTRUCT);
}

/** Unarchive the object. */
ex structure::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new structure(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void structure::archive(archive_node &n) const
{
	inherited::archive(n);
}

//////////
// structures overriding virtual structures from bases classes
//////////

// public

void structure::printraw(std::ostream & os) const
{
	debugmsg("structure printraw",LOGLEVEL_PRINT);

	os << "structure(hash=" << hashvalue << ",flags=" << flags << ")";
}

void structure::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("structure print",LOGLEVEL_PRINT);

	os << "structure()";
}

void structure::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("structure printtree",LOGLEVEL_PRINT);

	os << std::string(indent,' ') << "structure "
	   << "hash=" << hashvalue
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags << std::endl;
}

void structure::printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence) const
{
	debugmsg("structure print csrc",LOGLEVEL_PRINT);

	os << "structure()";
}

// protected

int structure::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, structure));
	return 0; // all structures are the same
}

bool structure::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, structure));
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

std::vector<registered_structure_info> & structure::registered_structures(void)
{
	static std::vector<registered_structure_info> * rs = new std::vector<registered_structure_info>;
	return *rs;
}

// public

unsigned structure::register_new(const char * nm)
{
	registered_structure_info rsi={nm};
	registered_structures().push_back(rsi);
	return registered_structures().size()-1;
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC
