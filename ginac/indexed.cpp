/** @file indexed.cpp
 *
 *  Implementation of GiNaC's indexed expressions. */

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

#include <stdexcept>

#include "indexed.h"
#include "idx.h"
#include "add.h"
#include "mul.h"
#include "ncmul.h"
#include "power.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(indexed, exprseq)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

indexed::indexed() : symmetry(unknown)
{
	debugmsg("indexed default constructor", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
}

void indexed::copy(const indexed & other)
{
	inherited::copy(other);
	symmetry = other.symmetry;
}

DEFAULT_DESTROY(indexed)

//////////
// other constructors
//////////

indexed::indexed(const ex & b) : inherited(b), symmetry(unknown)
{
	debugmsg("indexed constructor from ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, const ex & i1) : inherited(b, i1), symmetry(unknown)
{
	debugmsg("indexed constructor from ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2) : inherited(b, i1, i2), symmetry(unknown)
{
	debugmsg("indexed constructor from ex,ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3) : inherited(b, i1, i2, i3), symmetry(unknown)
{
	debugmsg("indexed constructor from ex,ex,ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3, const ex & i4) : inherited(b, i1, i2, i3, i4), symmetry(unknown)
{
	debugmsg("indexed constructor from ex,ex,ex,ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, symmetry_type symm, const ex & i1, const ex & i2) : inherited(b, i1, i2), symmetry(symm)
{
	debugmsg("indexed constructor from ex,symmetry,ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, symmetry_type symm, const ex & i1, const ex & i2, const ex & i3) : inherited(b, i1, i2, i3), symmetry(symm)
{
	debugmsg("indexed constructor from ex,symmetry,ex,ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, symmetry_type symm, const ex & i1, const ex & i2, const ex & i3, const ex & i4) : inherited(b, i1, i2, i3, i4), symmetry(symm)
{
	debugmsg("indexed constructor from ex,symmetry,ex,ex,ex,ex", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, const exvector & v) : inherited(b), symmetry(unknown)
{
	debugmsg("indexed constructor from ex,exvector", LOGLEVEL_CONSTRUCT);
	seq.insert(seq.end(), v.begin(), v.end());
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(const ex & b, symmetry_type symm, const exvector & v) : inherited(b), symmetry(symm)
{
	debugmsg("indexed constructor from ex,symmetry,exvector", LOGLEVEL_CONSTRUCT);
	seq.insert(seq.end(), v.begin(), v.end());
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(symmetry_type symm, const exprseq & es) : inherited(es), symmetry(symm)
{
	debugmsg("indexed constructor from symmetry,exprseq", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(symmetry_type symm, const exvector & v, bool discardable) : inherited(v, discardable), symmetry(symm)
{
	debugmsg("indexed constructor from symmetry,exvector", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

indexed::indexed(symmetry_type symm, exvector * vp) : inherited(vp), symmetry(symm)
{
	debugmsg("indexed constructor from symmetry,exvector *", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_indexed;
	assert_all_indices_of_type_idx();
}

//////////
// archiving
//////////

indexed::indexed(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("indexed constructor from archive_node", LOGLEVEL_CONSTRUCT);
	unsigned int symm;
	if (!(n.find_unsigned("symmetry", symm)))
		throw (std::runtime_error("unknown indexed symmetry type in archive"));
}

void indexed::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_unsigned("symmetry", symmetry);
}

DEFAULT_UNARCHIVE(indexed)

//////////
// functions overriding virtual functions from bases classes
//////////

void indexed::printraw(std::ostream & os) const
{
	debugmsg("indexed printraw", LOGLEVEL_PRINT);
	GINAC_ASSERT(seq.size() > 0);

	os << class_name() << "(";
	seq[0].printraw(os);
	os << ",indices=";
	printrawindices(os);
	os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void indexed::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("indexed printtree", LOGLEVEL_PRINT);
	GINAC_ASSERT(seq.size() > 0);

	os << std::string(indent, ' ') << class_name() << ", " << seq.size()-1 << " indices";
	os << ",hash=" << hashvalue << ",flags=" << flags << std::endl;
	printtreeindices(os, indent);
}

void indexed::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("indexed print", LOGLEVEL_PRINT);
	GINAC_ASSERT(seq.size() > 0);

	const ex & base = seq[0];
	bool need_parens = is_ex_exactly_of_type(base, add) || is_ex_exactly_of_type(base, mul)
	                || is_ex_exactly_of_type(base, ncmul) || is_ex_exactly_of_type(base, power);
	if (need_parens)
		os << "(";
	os << base;
	if (need_parens)
		os << ")";
	printindices(os);
}

bool indexed::info(unsigned inf) const
{
	if (inf == info_flags::indexed) return true;
	if (inf == info_flags::has_indices) return seq.size() > 1;
	return inherited::info(inf);
}

bool indexed::all_index_values_are(unsigned inf) const
{
	// No indices? Then no property can be fulfilled
	if (seq.size() < 2)
		return false;

	// Check all indices
	exvector::const_iterator it = seq.begin() + 1, itend = seq.end();
	while (it != itend) {
		GINAC_ASSERT(is_ex_of_type(*it, idx));
		if (!ex_to_idx(*it).get_value().info(inf))
			return false;
		it++;
	}
	return true;
}

int indexed::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, indexed));
	return inherited::compare_same_type(other);
}

// The main difference between sort_index_vector() and canonicalize_indices()
// is that the latter takes the symmetry of the object into account. Once we
// implement mixed symmetries, canonicalize_indices() will only be able to
// reorder index pairs with known symmetry properties, while sort_index_vector()
// always sorts the whole vector.

/** Bring a vector of indices into a canonic order (don't care about the
 *  symmetry of the objects carrying the indices). Dummy indices will lie
 *  next to each other after the sorting.
 *
 *  @param v Index vector to be sorted */
static void sort_index_vector(exvector &v)
{
	// Nothing to sort if less than 2 elements
	if (v.size() < 2)
		return;

	// Simple bubble sort algorithm should be sufficient for the small
	// number of indices expected
	exvector::iterator it1 = v.begin(), itend = v.end(), next_to_last_idx = itend - 1;
	while (it1 != next_to_last_idx) {
		exvector::iterator it2 = it1 + 1;
		while (it2 != itend) {
			if (it1->compare(*it2) > 0)
				it1->swap(*it2);
			it2++;
		}
		it1++;
	}
}

/** Bring a vector of indices into a canonic order. This operation only makes
 *  sense if the object carrying these indices is either symmetric or totally
 *  antisymmetric with respect to the indices.
 *
 *  @param itbegin Start of index vector
 *  @param itend End of index vector
 *  @param antisymm Whether the object is antisymmetric
 *  @return the sign introduced by the reordering of the indices if the object
 *          is antisymmetric (or 0 if two equal indices are encountered). For
 *          symmetric objects, this is always +1. If the index vector was
 *          already in a canonic order this function returns INT_MAX. */
static int canonicalize_indices(exvector::iterator itbegin, exvector::iterator itend, bool antisymm)
{
	bool something_changed = false;
	int sig = 1;

	// Simple bubble sort algorithm should be sufficient for the small
	// number of indices expected
	exvector::iterator it1 = itbegin, next_to_last_idx = itend - 1;
	while (it1 != next_to_last_idx) {
		exvector::iterator it2 = it1 + 1;
		while (it2 != itend) {
			int cmpval = it1->compare(*it2);
			if (cmpval == 1) {
				it1->swap(*it2);
				something_changed = true;
				if (antisymm)
					sig = -sig;
			} else if (cmpval == 0 && antisymm) {
				something_changed = true;
				sig = 0;
			}
			it2++;
		}
		it1++;
	}

	return something_changed ? sig : INT_MAX;
}

ex indexed::eval(int level) const
{
	// First evaluate children, then we will end up here again
	if (level > 1)
		return indexed(symmetry, evalchildren(level));

	const ex &base = seq[0];

	// If the base object is 0, the whole object is 0
	if (base.is_zero())
		return _ex0();

	// If the base object is a product, pull out the numeric factor
	if (is_ex_exactly_of_type(base, mul) && is_ex_exactly_of_type(base.op(base.nops() - 1), numeric)) {
		exvector v = seq;
		ex f = ex_to_numeric(base.op(base.nops() - 1));
		v[0] = seq[0] / f;
		return f * thisexprseq(v);
	}

	// Canonicalize indices according to the symmetry properties
	if (seq.size() > 2 && (symmetry != unknown && symmetry != mixed)) {
		exvector v = seq;
		int sig = canonicalize_indices(v.begin() + 1, v.end(), symmetry == antisymmetric);
		if (sig != INT_MAX) {
			// Something has changed while sorting indices, more evaluations later
			if (sig == 0)
				return _ex0();
			return ex(sig) * thisexprseq(v);
		}
	}

	// Let the class of the base object perform additional evaluations
	return base.bp->eval_indexed(*this);
}

ex indexed::thisexprseq(const exvector & v) const
{
	return indexed(symmetry, v);
}

ex indexed::thisexprseq(exvector * vp) const
{
	return indexed(symmetry, vp);
}

ex indexed::expand(unsigned options) const
{
	GINAC_ASSERT(seq.size() > 0);

	if ((options & expand_options::expand_indexed) && is_ex_exactly_of_type(seq[0], add)) {

		// expand_indexed expands (a+b).i -> a.i + b.i
		const ex & base = seq[0];
		ex sum = _ex0();
		for (unsigned i=0; i<base.nops(); i++) {
			exvector s = seq;
			s[0] = base.op(i);
			sum += thisexprseq(s).expand();
		}
		return sum;

	} else
		return inherited::expand(options);
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

void indexed::printrawindices(std::ostream & os) const
{
	if (seq.size() > 1) {
		exvector::const_iterator it=seq.begin() + 1, itend = seq.end();
		while (it != itend) {
			it->printraw(os);
			it++;
			if (it != itend)
				os << ",";
		}
	}
}

void indexed::printtreeindices(std::ostream & os, unsigned indent) const
{
	if (seq.size() > 1) {
		exvector::const_iterator it=seq.begin() + 1, itend = seq.end();
		while (it != itend) {
			os << std::string(indent + delta_indent, ' ');
			it->printraw(os);
			os << std::endl;
			it++;
		}
	}
}

void indexed::printindices(std::ostream & os) const
{
	if (seq.size() > 1) {
		exvector::const_iterator it=seq.begin() + 1, itend = seq.end();
		while (it != itend) {
			it->print(os);
			it++;
		}
	}
}

/** Check whether all indices are of class idx. This function is used
 *  internally to make sure that all constructed indexed objects really
 *  carry indices and not some other classes. */
void indexed::assert_all_indices_of_type_idx(void) const
{
	GINAC_ASSERT(seq.size() > 0);
	exvector::const_iterator it = seq.begin() + 1, itend = seq.end();
	while (it != itend) {
		if (!is_ex_of_type(*it, idx))
			throw(std::invalid_argument("indices of indexed object must be of type idx"));
		it++;
	}
}

//////////
// global functions
//////////

/** Given a vector of indices, split them into two vectors, one containing
 *  the free indices, the other containing the dummy indices. */
static void find_free_and_dummy(exvector::const_iterator it, exvector::const_iterator itend, exvector & out_free, exvector & out_dummy)
{
	out_free.clear();
	out_dummy.clear();

	// No indices? Then do nothing
	if (it == itend)
		return;

	// Only one index? Then it is a free one if it's not numeric
	if (itend - it == 1) {
		if (ex_to_idx(*it).is_symbolic())
			out_free.push_back(*it);
		return;
	}

	// Sort index vector. This will cause dummy indices come to lie next
	// to each other (because the sort order is defined to guarantee this).
	exvector v(it, itend);
	sort_index_vector(v);

	// Find dummy pairs and free indices
	it = v.begin(); itend = v.end();
	exvector::const_iterator last = it++;
	while (it != itend) {
		if (is_dummy_pair(*it, *last)) {
			out_dummy.push_back(*last);
			it++;
			if (it == itend)
				return;
		} else {
			if (!it->is_equal(*last) && ex_to_idx(*last).is_symbolic())
				out_free.push_back(*last);
		}
		last = it++;
	}
	if (ex_to_idx(*last).is_symbolic())
		out_free.push_back(*last);
}

/** Check whether two sorted index vectors are consistent (i.e. equal). */
static bool indices_consistent(const exvector & v1, const exvector & v2)
{
	// Number of indices must be the same
	if (v1.size() != v2.size())
		return false;

	// And also the indices themselves
	exvector::const_iterator ait = v1.begin(), aitend = v1.end(),
	                         bit = v2.begin(), bitend = v2.end();
	while (ait != aitend) {
		if (!ait->is_equal(*bit))
			return false;
		ait++; bit++;
	}
	return true;
}

exvector indexed::get_dummy_indices(void) const
{
	exvector free_indices, dummy_indices;
	find_free_and_dummy(seq.begin() + 1, seq.end(), free_indices, dummy_indices);
	return dummy_indices;
}

exvector indexed::get_free_indices(void) const
{
	exvector free_indices, dummy_indices;
	find_free_and_dummy(seq.begin() + 1, seq.end(), free_indices, dummy_indices);
	return free_indices;
}

exvector add::get_free_indices(void) const
{
	exvector free_indices;
	for (unsigned i=0; i<nops(); i++) {
		if (i == 0)
			free_indices = op(i).get_free_indices();
		else {
			exvector free_indices_of_term = op(i).get_free_indices();
			if (!indices_consistent(free_indices, free_indices_of_term))
				throw (std::runtime_error("add::get_free_indices: inconsistent indices in sum"));
		}
	}
	return free_indices;
}

exvector mul::get_free_indices(void) const
{
	// Concatenate free indices of all factors
	exvector un;
	for (unsigned i=0; i<nops(); i++) {
		exvector free_indices_of_factor = op(i).get_free_indices();
		un.insert(un.end(), free_indices_of_factor.begin(), free_indices_of_factor.end());
	}

	// And remove the dummy indices
	exvector free_indices, dummy_indices;
	find_free_and_dummy(un.begin(), un.end(), free_indices, dummy_indices);
	return free_indices;
}

exvector ncmul::get_free_indices(void) const
{
	// Concatenate free indices of all factors
	exvector un;
	for (unsigned i=0; i<nops(); i++) {
		exvector free_indices_of_factor = op(i).get_free_indices();
		un.insert(un.end(), free_indices_of_factor.begin(), free_indices_of_factor.end());
	}

	// And remove the dummy indices
	exvector free_indices, dummy_indices;
	find_free_and_dummy(un.begin(), un.end(), free_indices, dummy_indices);
	return free_indices;
}

exvector power::get_free_indices(void) const
{
	// Return free indices of basis
	return basis.get_free_indices();
}

/** Simplify product of indexed expressions (commutative, noncommutative and
 *  simple squares), return list of free indices. */
ex simplify_indexed_product(const ex & e, exvector & free_indices, const scalar_products & sp)
{
	// Remember whether the product was commutative or noncommutative
	// (because we chop it into factors and need to reassemble later)
	bool non_commutative = is_ex_exactly_of_type(e, ncmul);

	// Collect factors in an exvector, store squares twice
	exvector v;
	v.reserve(e.nops() * 2);

	if (is_ex_exactly_of_type(e, power)) {
		// We only get called for simple squares, split a^2 -> a*a
		GINAC_ASSERT(e.op(1).is_equal(_ex2()));
		v.push_back(e.op(0));
		v.push_back(e.op(0));
	} else {
		for (int i=0; i<e.nops(); i++) {
			ex f = e.op(i);
			if (is_ex_exactly_of_type(f, power) && f.op(1).is_equal(_ex2())) {
				v.push_back(f.op(0));
	            v.push_back(f.op(0));
			} else if (is_ex_exactly_of_type(f, ncmul)) {
				// Noncommutative factor found, split it as well
				non_commutative = true; // everything becomes noncommutative, ncmul will sort out the commutative factors later
				for (int j=0; j<f.nops(); i++)
					v.push_back(f.op(j));
			} else
				v.push_back(f);
		}
	}

	// Perform contractions
	bool something_changed = false;
	GINAC_ASSERT(v.size() > 1);
	exvector::iterator it1, itend = v.end(), next_to_last = itend - 1;
	for (it1 = v.begin(); it1 != next_to_last; it1++) {

try_again:
		if (!is_ex_of_type(*it1, indexed))
			continue;

		// Indexed factor found, look for contraction candidates
		exvector::iterator it2;
		for (it2 = it1 + 1; it2 != itend; it2++) {

			if (!is_ex_of_type(*it2, indexed))
				continue;

			// Check whether the two factors share dummy indices
			exvector un(ex_to_indexed(*it1).seq.begin() + 1, ex_to_indexed(*it1).seq.end());
			un.insert(un.end(), ex_to_indexed(*it2).seq.begin() + 1, ex_to_indexed(*it2).seq.end());
			exvector free, dummy;
			find_free_and_dummy(un.begin(), un.end(), free, dummy);
			if (dummy.size() == 0)
				continue;

			// At least one dummy index, is it a defined scalar product?
			if (free.size() == 0) {
				if (sp.is_defined(*it1, *it2)) {
					*it1 = sp.evaluate(*it1, *it2);
					*it2 = _ex1();
					something_changed = true;
					goto try_again;
				}
			}

			// Contraction of symmetric with antisymmetric object is zero
			if ((ex_to_indexed(*it1).symmetry == indexed::symmetric &&
			     ex_to_indexed(*it2).symmetry == indexed::antisymmetric
			  || ex_to_indexed(*it1).symmetry == indexed::antisymmetric &&
			     ex_to_indexed(*it2).symmetry == indexed::symmetric)
			 && dummy.size() > 1) {
				free_indices.clear();
				return _ex0();
			}

			// Try to contract the first one with the second one
			bool contracted = it1->op(0).bp->contract_with(it1, it2, v);
			if (!contracted) {

				// That didn't work; maybe the second object knows how to
				// contract itself with the first one
				contracted = it2->op(0).bp->contract_with(it2, it1, v);
			}
			if (contracted) {
				something_changed = true;

				// Both objects may have new indices now or they might
				// even not be indexed objects any more, so we have to
				// start over
				goto try_again;
			}
		}
	}

	// Find free indices (concatenate them all and call find_free_and_dummy())
	exvector un, dummy_indices;
	it1 = v.begin(); itend = v.end();
	while (it1 != itend) {
		if (is_ex_of_type(*it1, indexed)) {
			const indexed & o = ex_to_indexed(*it1);
			un.insert(un.end(), o.seq.begin() + 1, o.seq.end());
		}
		it1++;
	}
	find_free_and_dummy(un.begin(), un.end(), free_indices, dummy_indices);

	ex r;
	if (something_changed) {
		if (non_commutative)
			r = ncmul(v);
		else
			r = mul(v);
	} else
		r = e;

	// Product of indexed object with a scalar?
	if (is_ex_exactly_of_type(r, mul) && r.nops() == 2
	 && is_ex_exactly_of_type(r.op(1), numeric) && is_ex_of_type(r.op(0), indexed))
		return r.op(0).op(0).bp->scalar_mul_indexed(r.op(0), ex_to_numeric(r.op(1)));
	else
		return r;
}

/** Simplify indexed expression, return list of free indices. */
ex simplify_indexed(const ex & e, exvector & free_indices, const scalar_products & sp)
{
	// Expand the expression
	ex e_expanded = e.expand();

	// Simplification of single indexed object: just find the free indices
	if (is_ex_of_type(e_expanded, indexed)) {
		const indexed &i = ex_to_indexed(e_expanded);
		exvector dummy_indices;
		find_free_and_dummy(i.seq.begin() + 1, i.seq.end(), free_indices, dummy_indices);
		return e_expanded;
	}

	// Simplification of sum = sum of simplifications, check consistency of
	// free indices in each term
	if (is_ex_exactly_of_type(e_expanded, add)) {
		bool first = true;
		ex sum = _ex0();
		free_indices.clear();

		for (unsigned i=0; i<e_expanded.nops(); i++) {
			exvector free_indices_of_term;
			ex term = simplify_indexed(e_expanded.op(i), free_indices_of_term, sp);
			if (!term.is_zero()) {
				if (first) {
					free_indices = free_indices_of_term;
					sum = term;
					first = false;
				} else {
					if (!indices_consistent(free_indices, free_indices_of_term))
						throw (std::runtime_error("simplify_indexed: inconsistent indices in sum"));
					if (is_ex_of_type(sum, indexed) && is_ex_of_type(term, indexed))
						sum = sum.op(0).bp->add_indexed(sum, term);
					else
						sum += term;
				}
			}
		}

		return sum;
	}

	// Simplification of products
	if (is_ex_exactly_of_type(e_expanded, mul)
	 || is_ex_exactly_of_type(e_expanded, ncmul)
	 || (is_ex_exactly_of_type(e_expanded, power) && is_ex_of_type(e_expanded.op(0), indexed) && e_expanded.op(1).is_equal(_ex2())))
		return simplify_indexed_product(e_expanded, free_indices, sp);

	// Cannot do anything
	free_indices.clear();
	return e_expanded;
}

ex simplify_indexed(const ex & e)
{
	exvector free_indices;
	scalar_products sp;
	return simplify_indexed(e, free_indices, sp);
}

ex simplify_indexed(const ex & e, const scalar_products & sp)
{
	exvector free_indices;
	return simplify_indexed(e, free_indices, sp);
}

//////////
// helper classes
//////////

void scalar_products::add(const ex & v1, const ex & v2, const ex & sp)
{
	spm[make_key(v1, v2)] = sp;
}

void scalar_products::clear(void)
{
	spm.clear();
}

/** Check whether scalar product pair is defined. */
bool scalar_products::is_defined(const ex & v1, const ex & v2) const
{
	return spm.find(make_key(v1, v2)) != spm.end();
}

/** Return value of defined scalar product pair. */
ex scalar_products::evaluate(const ex & v1, const ex & v2) const
{
	return spm.find(make_key(v1, v2))->second;
}

void scalar_products::debugprint(void) const
{
	std::cerr << "map size=" << spm.size() << std::endl;
	for (spmap::const_iterator cit=spm.begin(); cit!=spm.end(); ++cit) {
		const spmapkey & k = cit->first;
		std::cerr << "item key=(" << k.first << "," << k.second;
		std::cerr << "), value=" << cit->second << std::endl;
	}
}

/** Make key from object pair. */
spmapkey scalar_products::make_key(const ex & v1, const ex & v2)
{
	// If indexed, extract base objects
	ex s1 = is_ex_of_type(v1, indexed) ? v1.op(0) : v1;
	ex s2 = is_ex_of_type(v2, indexed) ? v2.op(0) : v2;

	// Enforce canonical order in pair
	if (s1.compare(s2) > 0)
		return spmapkey(s2, s1);
	else
		return spmapkey(s1, s2);
}

} // namespace GiNaC
