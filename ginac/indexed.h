/** @file indexed.h
 *
 *  Interface to GiNaC's indexed expressions. */

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

#ifndef __GINAC_INDEXED_H__
#define __GINAC_INDEXED_H__

#include <map>

#include "exprseq.h"

namespace GiNaC {


class scalar_products;
class symmetry;

/** This class holds an indexed expression. It consists of a 'base' expression
 *  (the expression being indexed) which can be accessed as op(0), and n (n >= 0)
 *  indices (all of class idx), accessible as op(1)..op(n). */
class indexed : public exprseq
{
	GINAC_DECLARE_REGISTERED_CLASS(indexed, exprseq)

	friend ex simplify_indexed(const ex & e, exvector & free_indices, exvector & dummy_indices, const scalar_products & sp);
	friend ex simplify_indexed_product(const ex & e, exvector & free_indices, exvector & dummy_indices, const scalar_products & sp);
	friend bool reposition_dummy_indices(ex & e, exvector & variant_dummy_indices, exvector & moved_indices);

	// other constructors
public:
	/** Construct indexed object with no index.
	 *
	 *  @param b Base expression
	 *  @return newly constructed indexed object */
	indexed(const ex & b);

	/** Construct indexed object with one index. The index must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param i1 The index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const ex & i1);

	/** Construct indexed object with two indices. The indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param i1 First index
	 *  @param i2 Second index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const ex & i1, const ex & i2);

	/** Construct indexed object with three indices. The indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param i1 First index
	 *  @param i2 Second index
	 *  @param i3 Third index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3);

	/** Construct indexed object with four indices. The indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param i1 First index
	 *  @param i2 Second index
	 *  @param i3 Third index
	 *  @param i4 Fourth index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3, const ex & i4);

	/** Construct indexed object with two indices and a specified symmetry. The
	 *  indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param symm Symmetry of indices
	 *  @param i1 First index
	 *  @param i2 Second index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const symmetry & symm, const ex & i1, const ex & i2);

	/** Construct indexed object with three indices and a specified symmetry.
	 *  The indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param symm Symmetry of indices
	 *  @param i1 First index
	 *  @param i2 Second index
	 *  @param i3 Third index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const symmetry & symm, const ex & i1, const ex & i2, const ex & i3);

	/** Construct indexed object with four indices and a specified symmetry. The
	 *  indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param symm Symmetry of indices
	 *  @param i1 First index
	 *  @param i2 Second index
	 *  @param i3 Third index
	 *  @param i4 Fourth index
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const symmetry & symm, const ex & i1, const ex & i2, const ex & i3, const ex & i4);

	/** Construct indexed object with a specified vector of indices. The indices
	 *  must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param iv Vector of indices
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const exvector & iv);

	/** Construct indexed object with a specified vector of indices and
	 *  symmetry. The indices must be of class idx.
	 *
	 *  @param b Base expression
	 *  @param symm Symmetry of indices
	 *  @param iv Vector of indices
	 *  @return newly constructed indexed object */
	indexed(const ex & b, const symmetry & symm, const exvector & iv);

	// internal constructors
	indexed(const symmetry & symm, const exprseq & es);
	indexed(const symmetry & symm, const exvector & v, bool discardable = false);
	indexed(const symmetry & symm, exvector * vp); // vp will be deleted

	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	bool info(unsigned inf) const;
	ex eval(int level = 0) const;
	exvector get_free_indices(void) const;

protected:
	ex derivative(const symbol & s) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;
	unsigned return_type(void) const { return return_types::commutative; }
	ex expand(unsigned options = 0) const;

	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
public:
	/** Check whether all index values have a certain property.
	 *  @see class info_flags */
	bool all_index_values_are(unsigned inf) const;

	/** Return a vector containing the object's indices. */
	exvector get_indices(void) const;

	/** Return a vector containing the dummy indices of the object, if any. */
	exvector get_dummy_indices(void) const;

	/** Return a vector containing the dummy indices in the contraction with
	 *  another indexed object. */
	exvector get_dummy_indices(const indexed & other) const;

	/** Check whether the object has an index that forms a dummy index pair
	 *  with a given index. */
	bool has_dummy_index_for(const ex & i) const;

	/** Return symmetry properties. */
	ex get_symmetry(void) const {return symtree;}

protected:
	void printindices(const print_context & c, unsigned level) const;
	void validate(void) const;

	// member variables
protected:
	ex symtree; /**< Index symmetry (tree of symmetry objects) */
};


typedef std::pair<ex, ex> spmapkey;

struct spmapkey_is_less {
	bool operator() (const spmapkey &p, const spmapkey &q) const 
	{
		int cmp = p.first.compare(q.first);
		return ((cmp<0) || (!(cmp>0) && p.second.compare(q.second)<0));
	}
};

typedef std::map<spmapkey, ex, spmapkey_is_less> spmap;

/** Helper class for storing information about known scalar products which
 *  are to be automatically replaced by simplify_indexed().
 *
 *  @see simplify_indexed */
class scalar_products {
public:
	/** Register scalar product pair and its value. */
	void add(const ex & v1, const ex & v2, const ex & sp);

	/** Register list of vectors. This adds all possible pairs of products
	 *  a.i * b.i with the value a*b (note that this is not a scalar vector
	 *  product but an ordinary product of scalars). */
	void add_vectors(const lst & l);

	/** Clear all registered scalar products. */
	void clear(void);

	bool is_defined(const ex & v1, const ex & v2) const;
	ex evaluate(const ex & v1, const ex & v2) const;
	void debugprint(void) const;

private:
	static spmapkey make_key(const ex & v1, const ex & v2);

	spmap spm; /*< Map from defined scalar product pairs to their values */
};


// utility functions

/** Specialization of is_exactly_a<indexed>(obj) for indexed objects. */
template<> inline bool is_exactly_a<indexed>(const basic & obj)
{
	return obj.tinfo()==TINFO_indexed;
}

} // namespace GiNaC

#endif // ndef __GINAC_INDEXED_H__
