/** @file structure.h
 *
 *  Interface to 'abstract' class structure. */

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

#ifndef __GINAC_STRUCTURE_H__
#define __GINAC_STRUCTURE_H__

#include "basic.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

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
// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	structure();
	~structure();
	structure(const structure & other);
	const structure & operator=(const structure & other);
protected:
	void copy(const structure & other);
	void destroy(bool call_parent);

	// other constructors
	// none

	// functions overriding virtual functions from bases classes
public:
	basic * duplicate() const;
	void printraw(std::ostream & os) const; 
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence=0) const;
protected:
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	
	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
protected:
	static std::vector<registered_structure_info> & registered_structures(void);
public:
	static unsigned register_new(const char * nm);

// member variables
// none

};

// global constants

extern const structure some_structure;
extern const type_info & typeid_structure;

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_STRUCTURE_H__
