/** @file basic.h
 *
 *  Interface to GiNaC's ABC. */

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

#ifndef __GINAC_BASIC_H__
#define __GINAC_BASIC_H__

#include <iostream>
#include <typeinfo>
#include <vector>

// CINT needs <algorithm> to work properly with <vector>
#include <algorithm>

#include "flags.h"
#include "tinfos.h"
#include "assertion.h"
#include "registrar.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class basic;
class ex;
class symbol;
class lst;
class numeric;
class relational;
class archive_node;

// Cint doesn't like vector<..,default_alloc> but malloc_alloc is
// unstandardized and not supported by newer GCCs.
#if defined(__GNUC__) && ((__GNUC__ == 2) && (__GNUC_MINOR__ < 97))
typedef std::vector<ex,malloc_alloc> exvector;
#else
typedef std::vector<ex> exvector;
#endif

#define INLINE_BASIC_CONSTRUCTORS

/** This class is the ABC (abstract base class) of GiNaC's class hierarchy.
 *  It is responsible for the reference counting. */
class basic
{
	GINAC_DECLARE_REGISTERED_CLASS(basic, void)

	friend class ex;

// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	basic()
#ifdef INLINE_BASIC_CONSTRUCTORS
	        : tinfo_key(TINFO_basic), flags(0), refcount(0)
	{
	}
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

	virtual ~basic()
#ifdef INLINE_BASIC_CONSTRUCTORS
	{
		destroy(0);
		GINAC_ASSERT((!(flags & status_flags::dynallocated))||(refcount==0));
	}
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

	basic(const basic & other)
#ifdef INLINE_BASIC_CONSTRUCTORS
	{
		copy(other);
	}
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS
	
	const basic & operator=(const basic & other);
	
protected:
	void copy(const basic & other)
	{
		flags = other.flags & ~status_flags::dynallocated;
		hashvalue = other.hashvalue;
		tinfo_key = other.tinfo_key;
	}
	void destroy(bool call_parent) {}

	// other constructors
	basic(unsigned ti)
#ifdef INLINE_BASIC_CONSTRUCTORS
	                   : tinfo_key(ti), flags(0), refcount(0)
	{
	}
#else
;
#endif // def INLINE_BASIC_CONSTRUCTORS

	// functions overriding virtual functions from bases classes
	// none
	
	// new virtual functions which can be overridden by derived classes
public: // only const functions please (may break reference counting)
	virtual basic * duplicate() const;
	virtual void print(std::ostream & os,unsigned upper_precedence = 0) const;
	virtual void printraw(std::ostream & os) const;
	virtual void printtree(std::ostream & os, unsigned indent) const;
	virtual void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence = 0) const;
	virtual void dbgprint(void) const;
	virtual void dbgprinttree(void) const;
	virtual bool info(unsigned inf) const;
	virtual unsigned nops() const;
	virtual ex op(int i) const;
	virtual ex & let_op(int i);
	virtual ex operator[](const ex & index) const;
	virtual ex operator[](int i) const;
	virtual bool has(const ex & other) const;
	virtual int degree(const symbol & s) const;
	virtual int ldegree(const symbol & s) const;
	virtual ex coeff(const symbol & s, int n = 1) const;
	virtual ex collect(const symbol & s) const;
	virtual ex eval(int level = 0) const;
	virtual ex evalf(int level = 0) const;
	virtual ex series(const relational & r, int order, unsigned options = 0) const;
	virtual ex subs(const lst & ls, const lst & lr) const;
	virtual ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	virtual ex to_rational(lst &repl_lst) const;
	virtual numeric integer_content(void) const;
	virtual ex smod(const numeric &xi) const;
	virtual numeric max_coefficient(void) const;
	virtual exvector get_indices(void) const;
	virtual ex simplify_ncmul(const exvector & v) const;
protected: // non-const functions should be called from class ex only
	virtual ex derivative(const symbol & s) const;
	virtual int compare_same_type(const basic & other) const;
	virtual bool is_equal_same_type(const basic & other) const;
	virtual unsigned return_type(void) const;
	virtual unsigned return_type_tinfo(void) const;
	virtual unsigned calchash(void) const;
	virtual ex expand(unsigned options=0) const;

	// non-virtual functions in this class
public:
	ex subs(const ex & e) const;
	ex diff(const symbol & s, unsigned nth=1) const;
	int compare(const basic & other) const;
	bool is_equal(const basic & other) const;
	const basic & hold(void) const;
	unsigned gethash(void) const {if (flags & status_flags::hash_calculated) return hashvalue; else return calchash();}
	unsigned tinfo(void) const {return tinfo_key;}
	const basic & setflag(unsigned f) const {flags |= f; return *this;}
	const basic & clearflag(unsigned f) const {flags &= ~f; return *this;}
protected:
	void ensure_if_modifiable(void) const;

// member variables
	
protected:
	unsigned tinfo_key;
	mutable unsigned flags;
	mutable unsigned hashvalue;
	static unsigned precedence;
	static unsigned delta_indent;
private:
	unsigned refcount;
};

// global constants

extern const basic some_basic;
extern const std::type_info & typeid_basic;

// global variables

extern int max_recursion_level;

// convenience macros

#ifndef NO_NAMESPACE_GINAC

#define is_of_type(OBJ,TYPE) \
	(dynamic_cast<TYPE *>(const_cast<GiNaC::basic *>(&OBJ))!=0)

#define is_exactly_of_type(OBJ,TYPE) \
	((OBJ).tinfo()==GiNaC::TINFO_##TYPE)

#define is_ex_of_type(OBJ,TYPE) \
	(dynamic_cast<TYPE *>(const_cast<GiNaC::basic *>((OBJ).bp))!=0)

#define is_ex_exactly_of_type(OBJ,TYPE) \
	((*(OBJ).bp).tinfo()==GiNaC::TINFO_##TYPE)

#else // ndef NO_NAMESPACE_GINAC

#define is_of_type(OBJ,TYPE) \
	(dynamic_cast<TYPE *>(const_cast<basic *>(&OBJ))!=0)

#define is_exactly_of_type(OBJ,TYPE) \
	((OBJ).tinfo()==TINFO_##TYPE)

#define is_ex_of_type(OBJ,TYPE) \
	(dynamic_cast<TYPE *>(const_cast<basic *>((OBJ).bp))!=0)

#define is_ex_exactly_of_type(OBJ,TYPE) \
	((*(OBJ).bp).tinfo()==TINFO_##TYPE)

#endif // ndef NO_NAMESPACE_GINAC

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_BASIC_H__
