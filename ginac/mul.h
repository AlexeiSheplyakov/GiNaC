/** @file mul.h
 *
 *  Interface to GiNaC's products of expressions. */

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

#ifndef __GINAC_MUL_H__
#define __GINAC_MUL_H__

#include "expairseq.h"

namespace GiNaC {

/** Product of expressions. */
class mul : public expairseq
{
	GINAC_DECLARE_REGISTERED_CLASS(mul, expairseq)
	
	friend class add;
	friend class ncmul;
	friend class power;
	
	// other ctors
public:
	mul(const ex & lh, const ex & rh);
	mul(const exvector & v);
	mul(const epvector & v);
	mul(const epvector & v, const ex & oc);
	mul(epvector * vp, const ex & oc);
	mul(const ex & lh, const ex & mh, const ex & rh);
	
	// functions overriding virtual functions from bases classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned precedence(void) const {return 50;}
	bool info(unsigned inf) const;
	int degree(const ex & s) const;
	int ldegree(const ex & s) const;
	ex coeff(const ex & s, int n = 1) const;
	ex eval(int level=0) const;
	ex evalf(int level=0) const;
	ex series(const relational & s, int order, unsigned options = 0) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level = 0) const;
	numeric integer_content(void) const;
	ex smod(const numeric &xi) const;
	numeric max_coefficient(void) const;
	exvector get_free_indices(void) const;
	ex simplify_ncmul(const exvector & v) const;
protected:
	ex derivative(const symbol & s) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned return_type(void) const;
	unsigned return_type_tinfo(void) const;
	ex thisexpairseq(const epvector & v, const ex & oc) const;
	ex thisexpairseq(epvector * vp, const ex & oc) const;
	expair split_ex_to_pair(const ex & e) const;
	expair combine_ex_with_coeff_to_pair(const ex & e, const ex & c) const;
	expair combine_pair_with_coeff_to_pair(const expair & p, const ex & c) const;
	ex recombine_pair_to_ex(const expair & p) const;
	bool expair_needs_further_processing(epp it);
	ex default_overall_coeff(void) const;
	void combine_overall_coeff(const ex & c);
	void combine_overall_coeff(const ex & c1, const ex & c2);
	bool can_make_flat(const expair & p) const;
	ex expand(unsigned options=0) const;
	
	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
protected:
	epvector * expandchildren(unsigned options) const;
};

// utility functions
inline const mul &ex_to_mul(const ex &e)
{
	return static_cast<const mul &>(*e.bp);
}

} // namespace GiNaC

#endif // ndef __GINAC_MUL_H__
