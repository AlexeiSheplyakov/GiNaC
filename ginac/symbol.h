/** @file symbol.h
 *
 *  Interface to GiNaC's symbolic objects. */

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

#ifndef __GINAC_SYMBOL_H__
#define __GINAC_SYMBOL_H__

#include <string>
#include "basic.h"
#include "ex.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/** Basic CAS symbol.  It has a name because it must know how to output itself.
 *  It may be assigned an expression, but this feature is only intended for
 *  programs like 'ginsh' that want to associate symbols with expressions.
 *  If you want to replace symbols by expressions in your code, you should
 *  use ex::subs() or use objects of class ex instead of class symbol in the
 *  first place. */
class symbol : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(symbol, basic)

// types

	/** Symbols as keys to expressions. */
	class assigned_ex_info {
	public:
		assigned_ex_info();     //!< Default ctor
		bool is_assigned;       //!< True if there is an expression assigned
		ex assigned_expression; //!< The actual expression
		unsigned refcount;      //!< Yet another refcounter. PLEASE EXPLAIN!
	};
	
// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	symbol();
	~symbol();
	symbol(const symbol & other);
protected:
	void copy(const symbol & other); 
	void destroy(bool call_parent);

	// other constructors
public:
	explicit symbol(const std::string & initname);

	// functions overriding virtual functions from base classes
public:
	basic * duplicate() const;
	void print(std::ostream & os, unsigned upper_precedence = 0) const;
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence = 0) const;
	bool info(unsigned inf) const;
	ex expand(unsigned options = 0) const;
	bool has(const ex & other) const;
	int degree(const symbol & s) const;
	int ldegree(const symbol & s) const;
	ex coeff(const symbol & s, int n = 1) const;
	ex eval(int level = 0) const;
	ex series(const relational & s, int order, unsigned options = 0) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	ex to_rational(lst &repl_lst) const;
	ex subs(const lst & ls, const lst & lr) const;
protected:
	ex derivative(const symbol & s) const;
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned return_type(void) const;
	unsigned return_type_tinfo(void) const;
	unsigned calchash(void) const;
	
	// non-virtual functions in this class
public:
	void assign(const ex & value);
	void unassign(void);
	void setname(const std::string & n) { name = n; }
	std::string getname(void) const { return name; }
private:
	std::string & autoname_prefix(void);

// member variables

protected:
	assigned_ex_info * asexinfop;
	unsigned serial;  //!< unique serial number for comparision
	std::string name;
private:
	static unsigned next_serial;
};

// global constants

extern const symbol some_symbol;
extern const std::type_info & typeid_symbol;

// utility functions
inline const symbol &ex_to_symbol(const ex &e)
{
	return static_cast<const symbol &>(*e.bp);
}

// wrapper functions around member functions
inline void unassign(symbol & symarg)
{ symarg.unassign(); }

inline int degree(const symbol & a, const symbol & s)
{ return a.degree(s); }

inline int ldegree(const symbol & a, const symbol & s)
{ return a.ldegree(s); }

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_SYMBOL_H__
