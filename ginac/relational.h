/** @file relational.h
 *
 *  Interface to relations between expressions. */

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

#ifndef __GINAC_RELATIONAL_H__
#define __GINAC_RELATIONAL_H__

#include "basic.h"
#include "ex.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/** This class holds a relation consisting of two expressions and a logical
 *  relation between them. */
class relational : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(relational, basic)

// types
public:
	enum operators { equal,
		   not_equal,
		   less,
		   less_or_equal,
		   greater,
		   greater_or_equal
	};
	
// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	relational();
	~relational();
	relational(const relational & other);
	const relational & operator=(const relational & other);
protected:
	void copy(const relational & other);
	void destroy(bool call_parent);

	// other constructors
public:
	relational(const ex & lhs, const ex & rhs, operators oper=equal);
	
	// functions overriding virtual functions from bases classes
public:
	basic * duplicate() const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	void printraw(std::ostream & os) const;
	void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
	unsigned nops() const;
	ex & let_op(int i);
	ex eval(int level=0) const;
	ex evalf(int level=0) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
	ex simplify_ncmul(const exvector & v) const;
protected:
	int compare_same_type(const basic & other) const;
	unsigned return_type(void) const;
	unsigned return_type_tinfo(void) const;

	// new virtual functions which can be overridden by derived classes
public:
	virtual ex lhs(void) const;
	virtual ex rhs(void) const;

	// non-virtual functions in this class
public:
	operator bool(void) const;
	
// member variables
	
protected:
	ex lh;
	ex rh;
	operators o;
	static unsigned precedence;
};

// global constants

extern const relational some_relational;
extern const type_info & typeid_relational;

// utility functions
inline const relational &ex_to_relational(const ex &e)
{
	return static_cast<const relational &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_RELATIONAL_H__
