/** @file idx.h
 *
 *  Interface to GiNaC's indices. */

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

#ifndef __GINAC_IDX_H__
#define __GINAC_IDX_H__

#include "ex.h"
#include "numeric.h"

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

	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	bool info(unsigned inf) const;
	unsigned nops() const;
	ex & let_op(int i);
	ex evalf(int level = 0) const;
	ex subs(const lst & ls, const lst & lr, bool no_pattern = false) const;

protected:
	ex derivative(const symbol & s) const;
	bool match_same_type(const basic & other) const;

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
	bool is_numeric(void) const {return is_exactly_a<numeric>(value);}

	/** Check whether the index is symbolic. */
	bool is_symbolic(void) const {return !is_exactly_a<numeric>(value);}

	/** Get dimension of index space. */
	ex get_dim(void) const {return dim;}

	/** Check whether the dimension is numeric. */
	bool is_dim_numeric(void) const {return is_exactly_a<numeric>(dim);}

	/** Check whether the dimension is symbolic. */
	bool is_dim_symbolic(void) const {return !is_exactly_a<numeric>(dim);}

	/** Make a new index with the same value but a different dimension. */
	ex replace_dim(const ex & new_dim) const;

	/** Return the minimum of the dimensions of this and another index.
	 *  If this is undecidable, throw an exception. */
	ex minimal_dim(const idx & other) const;

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

	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	bool is_dummy_pair_same_type(const basic & other) const;

protected:
	bool match_same_type(const basic & other) const;

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


/** This class holds a spinor index that can be dotted or undotted and that
 *  also has a variance. This is used in the Weyl-van-der-Waerden formalism
 *  where the dot indicates complex conjugation. There is an associated
 *  (asymmetric) metric tensor that can be used to raise/lower spinor
 *  indices. */
class spinidx : public varidx
{
	GINAC_DECLARE_REGISTERED_CLASS(spinidx, varidx)

	// other constructors
public:
	/** Construct index with given value, dimension, variance and dot.
	 *
	 *  @param v Value of index (numeric or symbolic)
	 *  @param dim Dimension of index space (numeric or symbolic)
	 *  @param covariant Make covariant index (default is contravariant)
	 *  @param dotted Make covariant dotted (default is undotted)
	 *  @return newly constructed index */
	spinidx(const ex & v, const ex & dim = 2, bool covariant = false, bool dotted = false);

	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	bool is_dummy_pair_same_type(const basic & other) const;

protected:
	bool match_same_type(const basic & other) const;

	// non-virtual functions in this class
public:
	/** Check whether the index is dotted. */
	bool is_dotted(void) const {return dotted;}

	/** Check whether the index is not dotted. */
	bool is_undotted(void) const {return !dotted;}

	/** Make a new index with the same value and variance but the opposite
	 *  dottedness. */
	ex toggle_dot(void) const;

	/** Make a new index with the same value but opposite variance and
	 *  dottedness. */
	ex toggle_variance_dot(void) const;

	// member variables
protected:
	bool dotted;
};


// utility functions

/** Specialization of is_exactly_a<idx>(obj) for idx objects. */
template<> inline bool is_exactly_a<idx>(const basic & obj)
{
        return obj.tinfo()==TINFO_idx;
}

/** Specialization of is_exactly_a<varidx>(obj) for varidx objects. */
template<> inline bool is_exactly_a<varidx>(const basic & obj)
{
        return obj.tinfo()==TINFO_varidx;
}

/** Specialization of is_exactly_a<spinidx>(obj) for spinidx objects. */
template<> inline bool is_exactly_a<spinidx>(const basic & obj)
{
        return obj.tinfo()==TINFO_spinidx;
}

/** Check whether two indices form a dummy pair. */
bool is_dummy_pair(const idx & i1, const idx & i2);

/** Check whether two expressions form a dummy index pair. */
bool is_dummy_pair(const ex & e1, const ex & e2);

/** Given a vector of indices, split them into two vectors, one containing
 *  the free indices, the other containing the dummy indices (numeric
 *  indices are neither free nor dummy ones).
 *
 *  @param it Pointer to start of index vector
 *  @param itend Pointer to end of index vector
 *  @param out_free Vector of free indices (returned, sorted)
 *  @param out_dummy Vector of dummy indices (returned, sorted) */
void find_free_and_dummy(exvector::const_iterator it, exvector::const_iterator itend, exvector & out_free, exvector & out_dummy);

/** Given a vector of indices, split them into two vectors, one containing
 *  the free indices, the other containing the dummy indices (numeric
 *  indices are neither free nor dummy ones).
 *
 *  @param v Index vector
 *  @param out_free Vector of free indices (returned, sorted)
 *  @param out_dummy Vector of dummy indices (returned, sorted) */
inline void find_free_and_dummy(const exvector & v, exvector & out_free, exvector & out_dummy)
{
	find_free_and_dummy(v.begin(), v.end(), out_free, out_dummy);
}

/** Given a vector of indices, find the dummy indices.
 *
 *  @param v Index vector
 *  @param out_dummy Vector of dummy indices (returned, sorted) */
inline void find_dummy_indices(const exvector & v, exvector & out_dummy)
{
	exvector free_indices;
	find_free_and_dummy(v.begin(), v.end(), free_indices, out_dummy);
}

/** Count the number of dummy index pairs in an index vector. */
inline unsigned count_dummy_indices(const exvector & v)
{
	exvector free_indices, dummy_indices;
	find_free_and_dummy(v.begin(), v.end(), free_indices, dummy_indices);
	return dummy_indices.size();
}

/** Count the number of dummy index pairs in an index vector. */
inline unsigned count_free_indices(const exvector & v)
{
	exvector free_indices, dummy_indices;
	find_free_and_dummy(v.begin(), v.end(), free_indices, dummy_indices);
	return free_indices.size();
}

} // namespace GiNaC

#endif // ndef __GINAC_IDX_H__
