/** @file pseries.h
 *
 *  Interface to class for extended truncated power series. */

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

#ifndef __GINAC_SERIES_H__
#define __GINAC_SERIES_H__

#include "basic.h"
#include "expairseq.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/** This class holds a extended truncated power series (positive and negative
 *  integer powers). It consists of expression coefficients (only non-zero
 *  coefficients are stored), an expansion variable and an expansion point.
 *  Other classes must provide members to convert into this type. */
class pseries : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(pseries, basic)

	// default constructor, destructor, copy constructor, assignment operator and helpers
public:
	pseries();
	~pseries();
	pseries(const pseries &other);
	const pseries &operator=(const pseries &other);
protected:
	void copy(const pseries &other);
	void destroy(bool call_parent);

	// other constructors
public:
	pseries(const ex &rel_, const epvector &ops_);

	// functions overriding virtual functions from base classes
public:
	basic *duplicate() const;
	void print(std::ostream &os, unsigned upper_precedence = 0) const;
	void printraw(std::ostream &os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	unsigned nops(void) const;
	ex op(int i) const;
	ex &let_op(int i);
	int degree(const symbol &s) const;
	int ldegree(const symbol &s) const;
	ex coeff(const symbol &s, int n = 1) const;
	ex collect(const symbol &s) const;
	ex eval(int level=0) const;
	ex evalf(int level=0) const;
	ex series(const relational & r, int order, unsigned options = 0) const;
	ex subs(const lst & ls, const lst & lr) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	ex expand(unsigned options = 0) const;
protected:
	ex derivative(const symbol & s) const;

	// non-virtual functions in this class
public:
	ex convert_to_poly(bool no_order = false) const;
	bool is_compatible_to(const pseries &other) const {return var.compare(other.var) == 0 && point.compare(other.point) == 0;}
	bool is_zero(void) const {return seq.size() == 0;}
	bool is_terminating(void) const;
	ex add_series(const pseries &other) const;
	ex mul_const(const numeric &other) const;
	ex mul_series(const pseries &other) const;
	ex power_const(const numeric &p, int deg) const;
	pseries shift_exponents(int deg) const;

protected:
	/** Vector of {coefficient, power} pairs */
	epvector seq;

	/** Series variable (holds a symbol) */
	ex var;

	/** Expansion point */
	ex point;
	static unsigned precedence;
};

// global constants
extern const pseries some_pseries;
extern const std::type_info & typeid_pseries;

/** Return a reference to the pseries object embedded in an expression.
 *  The result is undefined if the expression does not contain a pseries
 *  object at its top level.
 *
 *  @param e expression
 *  @return reference to pseries object
 *  @see is_ex_of_type */
inline const pseries &ex_to_pseries(const ex &e)
{
	return static_cast<const pseries &>(*e.bp);
}

/** Convert the pseries object embedded in an expression to an ordinary
 *  polynomial in the expansion variable. The result is undefined if the
 *  expression does not contain a pseries object at its top level.
 *
 *  @param e expression
 *  @return polynomial expression
 *  @see is_ex_of_type
 *  @see pseries::convert_to_poly */
inline ex series_to_poly(const ex &e)
{
	return (static_cast<const pseries &>(*e.bp).convert_to_poly(true));
}

inline bool is_terminating(const pseries & s)
{
	return s.is_terminating();
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_SERIES_H__
