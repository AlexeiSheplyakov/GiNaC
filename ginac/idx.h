/** @file idx.h
 *
 *  Interface to GiNaC's indices. */

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

#ifndef __GINAC_IDX_H__
#define __GINAC_IDX_H__

#include "ex.h"

namespace GiNaC {


/** This class holds one index of an indexed object. Indices can
 *  theoretically consist of any symbolic expression but they are usually
 *  only just a symbol (e.g. "mu", "i") or numeric (integer). Indices belong
 *  to a space with a certain numeric or symbolic dimension. */
class idx : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(idx, basic)

	// other constructors
public:
	/** Construct index with given value and dimension.
	 *
	 *  @param v Value of index (numeric or symbolic)
	 *  @param dim Dimension of index space (numeric or symbolic)
	 *  @return newly constructed index */
	explicit idx(const ex & v, const ex & dim);

	// functions overriding virtual functions from bases classes
public:
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
protected:
	ex subs(const lst & ls, const lst & lr) const;

	// new virtual functions in this class
public:
	/** Check whether the index forms a dummy index pair with another index
	 *  of the same type. */
	virtual bool is_dummy_pair_same_type(const basic & other) const;

	// non-virtual functions in this class
public:
	/** Get value of index. */
	ex get_value(void) const {return value;}

	/** Check whether the index is numeric. */
	bool is_numeric(void) const {return is_ex_exactly_of_type(value, numeric);}

	/** Check whether the index is symbolic. */
	bool is_symbolic(void) const {return !is_ex_exactly_of_type(value, numeric);}

	/** Get dimension of index space. */
	ex get_dim(void) const {return dim;}

	/** Check whether the dimension is numeric. */
	bool is_dim_numeric(void) const {return is_ex_exactly_of_type(dim, numeric);}

	/** Check whether the dimension is symbolic. */
	bool is_dim_symbolic(void) const {return !is_ex_exactly_of_type(dim, numeric);}

protected:
	ex value; /**< Expression that constitutes the index (numeric or symbolic name) */
	ex dim;   /**< Dimension of space (can be symbolic or numeric) */
};


/** This class holds an index with a variance (co- or contravariant). There
 *  is an associated metric tensor that can be used to raise/lower indices. */
class varidx : public idx
{
	GINAC_DECLARE_REGISTERED_CLASS(varidx, idx)

	// other constructors
public:
	/** Construct index with given value, dimension and variance.
	 *
	 *  @param v Value of index (numeric or symbolic)
	 *  @param dim Dimension of index space (numeric or symbolic)
	 *  @param covariant Make covariant index (default is contravariant)
	 *  @return newly constructed index */
	varidx(const ex & v, const ex & dim, bool covariant = false);

	// functions overriding virtual functions from bases classes
public:
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool is_dummy_pair_same_type(const basic & other) const;

	// non-virtual functions in this class
public:
	/** Check whether the index is covariant. */
	bool is_covariant(void) const {return covariant;}

	/** Check whether the index is contravariant (not covariant). */
	bool is_contravariant(void) const {return !covariant;}

	/** Make a new index with the same value but the opposite variance. */
	ex toggle_variance(void) const;

	// member variables
protected:
	bool covariant; /**< x.mu, default is contravariant: x~mu */
};


// utility functions
inline const idx &ex_to_idx(const ex & e)
{
	return static_cast<const idx &>(*e.bp);
}

inline const varidx &ex_to_varidx(const ex & e)
{
	return static_cast<const varidx &>(*e.bp);
}

/** Check whether two indices form a dummy pair. */
bool is_dummy_pair(const idx & i1, const idx & i2);

/** Check whether two expressions form a dummy index pair. */
bool is_dummy_pair(const ex & e1, const ex & e2);


} // namespace GiNaC

#endif // ndef __GINAC_IDX_H__
