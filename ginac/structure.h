/** @file structure.h
 *
 *  Interface to 'abstract' class structure. */

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

#ifndef __GINAC_STRUCTURE_H__
#define __GINAC_STRUCTURE_H__

#include "basic.h"

namespace GiNaC {

struct registered_structure_info {
	const char * name;
};

/** The class structure is used to implement user defined classes
	with named members which behave similar to ordinary C structs.
	structure is an 'abstract' base class (it is possible but not
	meaningful to make instances), the user defined structures
	will be create by the perl script structure.pl */

class structure : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(structure, basic)
	
	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level=0) const;
protected:
	bool is_equal_same_type(const basic & other) const;
	
	// non-virtual functions in this class
protected:
	static std::vector<registered_structure_info> & registered_structures(void);
public:
	static unsigned register_new(const char * nm);
};

} // namespace GiNaC

#endif // ndef __GINAC_STRUCTURE_H__
