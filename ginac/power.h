/** @file power.h
 *
 *  Interface to GiNaC's symbolic exponentiation (basis^exponent). */

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

#ifndef __GINAC_POWER_H__
#define __GINAC_POWER_H__

#include "basic.h"
#include "ex.h"

namespace GiNaC {

class numeric;
class add;

/** This class holds a two-component object, a basis and and exponent
 *  representing exponentiation. */
class power : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(power, basic)
	
	friend class mul;
	
// member functions
	
	// other ctors
public:
	power(const ex & lh, const ex & rh) : inherited(TINFO_power), basis(lh), exponent(rh) {}
	template<typename T> power(const ex & lh, const T & rh) : inherited(TINFO_power), basis(lh), exponent(rh) {}
	
	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned precedence(void) const {return 60;}
	bool info(unsigned inf) const;
	unsigned nops() const;
	ex & let_op(int i);
	ex map(map_function & f) const;
	int degree(const ex & s) const;
	int ldegree(const ex & s) const;
	ex coeff(const ex & s, int n = 1) const;
	ex eval(int level=0) const;
	ex evalf(int level=0) const;
	ex evalm(void) const;
	ex series(const relational & s, int order, unsigned options = 0) const;
	ex subs(const lst & ls, const lst & lr, bool no_pattern = false) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	ex to_rational(lst &repl_lst) const;
	exvector get_free_indices(void) const;
	ex simplify_ncmul(const exvector & v) const;
protected:
	ex derivative(const symbol & s) const;
	unsigned return_type(void) const;
	unsigned return_type_tinfo(void) const;
	ex expand(unsigned options = 0) const;
	
	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
protected:
	ex expand_add(const add & a, int n) const;
	ex expand_add_2(const add & a) const;
	ex expand_mul(const mul & m, const numeric & n) const;
	
// member variables
	
protected:
	ex basis;
	ex exponent;
};

// utility functions

/** Efficient specialization of is_exactly_a<power>(obj) for power objects. */
template<> inline bool is_exactly_a<power>(const basic & obj)
{
	return obj.tinfo()==TINFO_power;
}

// wrapper functions

/** Symbolic exponentiation.  Returns a power-object as a new expression.
 *
 *  @param b the basis expression
 *  @param e the exponent expression */
inline ex pow(const ex & b, const ex & e)
{
	return power(b, e);
}
template<typename T1, typename T2>
inline ex pow(const T1 & b, const T2 & e)
{
	return power(ex(b), ex(e));
}

/** Square root expression.  Returns a power-object with exponent 1/2. */
ex sqrt(const ex & a);

} // namespace GiNaC

#endif // ndef __GINAC_POWER_H__
