/** @file symbol.h
 *
 *  Interface to GiNaC's symbolic objects. */

/*
 *  GiNaC Copyright (C) 1999-2002 Johannes Gutenberg University Mainz, Germany
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

namespace GiNaC {

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
	
	/** Symbols as keys to expressions - this is deprecated. */
	class assigned_ex_info {
	public:
		assigned_ex_info();     ///< Default ctor
		bool is_assigned;       ///< True if there is an expression assigned
		ex assigned_expression; ///< The actual expression
		unsigned refcount;      ///< Reference counter
	};
	
// member functions
	
	// other ctors
public:
	explicit symbol(const std::string & initname);
	explicit symbol(const std::string & initname, const std::string & texname);
	
	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	bool info(unsigned inf) const;
	ex eval(int level = 0) const;
	ex evalf(int level = 0) const { return *this; } // overwrites basic::evalf() for performance reasons
	ex series(const relational & s, int order, unsigned options = 0) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	ex to_rational(lst &repl_lst) const;
protected:
	ex derivative(const symbol & s) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned calchash(void) const;
	
	// non-virtual functions in this class
public:
	void assign(const ex & value);
	void unassign(void);
	void set_name(const std::string & n) { name = n; }
	std::string get_name(void) const { return name; }
private:
	std::string & autoname_prefix(void);
	std::string default_TeX_name(void) const;

// member variables

protected:
	assigned_ex_info * asexinfop;   ///< ptr to assigned expression, deprecated
	unsigned serial;       ///< unique serial number for comparison
	std::string name;      ///< printname of this symbol
	std::string TeX_name;  ///< LaTeX name of this symbol
private:
	static unsigned next_serial;
};


// utility functions

/** Specialization of is_exactly_a<symbol>(obj) for symbol objects. */
template<> inline bool is_exactly_a<symbol>(const basic & obj)
{
	return obj.tinfo()==TINFO_symbol;
}

// wrapper functions around member functions
inline void unassign(symbol & symarg)
{ symarg.unassign(); }

inline int degree(const symbol & a, const ex & s)
{ return a.degree(s); }

inline int ldegree(const symbol & a, const ex & s)
{ return a.ldegree(s); }

} // namespace GiNaC

#endif // ndef __GINAC_SYMBOL_H__
