/** @file structure.cpp
 *
 *  Implementation of 'abstract' class structure. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#include <iostream>
#include <string>

#include "structure.h"
#include "archive.h"
#include "utils.h"
#include "print.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(structure, basic)

//////////
// default ctor, dtor, copy ctor, assignment operator and helpers
//////////

DEFAULT_CTORS(structure)

//////////
// archiving
//////////

DEFAULT_ARCHIVING(structure)

//////////
// functions overriding virtual functions from base classes
//////////

void structure::print(const print_context & c, unsigned level) const
{
	if (is_a<print_tree>(c)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << std::endl;

	} else
		c.s << class_name() << "()";
}

// protected

DEFAULT_COMPARE(structure)

bool structure::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<structure>(other));
	return true; // all structures are the same
}

//////////
// non-virtual functions in this class
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

} // namespace GiNaC
