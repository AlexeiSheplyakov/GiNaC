/** @file indexed.cpp
 *
 *  Implementation of GiNaC's indexed expressions. */

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

#include <iostream>
#include <stdexcept>

#include "indexed.h"
#include "idx.h"
#include "add.h"
#include "mul.h"
#include "ncmul.h"
#include "power.h"
#include "symmetry.h"
#include "lst.h"
#include "print.h"
#include "archive.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(indexed, exprseq)

//////////
// default ctor, dtor, copy ctor, assignment operator and helpers
//////////

indexed::indexed() : symtree(sy_none())
{
	tinfo_key = TINFO_indexed;
}

void indexed::copy(const indexed & other)
{
	inherited::copy(other);
	symtree = other.symtree;
}

DEFAULT_DESTROY(indexed)

//////////
// other constructors
//////////

indexed::indexed(const ex & b) : inherited(b), symtree(sy_none())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1) : inherited(b, i1), symtree(sy_none())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2) : inherited(b, i1, i2), symtree(sy_none())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3) : inherited(b, i1, i2, i3), symtree(sy_none())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3, const ex & i4) : inherited(b, i1, i2, i3, i4), symtree(sy_none())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const symmetry & symm, const ex & i1, const ex & i2) : inherited(b, i1, i2), symtree(symm)
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const symmetry & symm, const ex & i1, const ex & i2, const ex & i3) : inherited(b, i1, i2, i3), symtree(symm)
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const symmetry & symm, const ex & i1, const ex & i2, const ex & i3, const ex & i4) : inherited(b, i1, i2, i3, i4), symtree(symm)
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const exvector & v) : inherited(b), symtree(sy_none())
{
	seq.insert(seq.end(), v.begin(), v.end());
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const symmetry & symm, const exvector & v) : inherited(b), symtree(symm)
{
	seq.insert(seq.end(), v.begin(), v.end());
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const symmetry & symm, const exprseq & es) : inherited(es), symtree(symm)
{
	tinfo_key = TINFO_indexed;
}

indexed::indexed(const symmetry & symm, const exvector & v, bool discardable) : inherited(v, discardable), symtree(symm)
{
	tinfo_key = TINFO_indexed;
}

indexed::indexed(const symmetry & symm, exvector * vp) : inherited(vp), symtree(symm)
{
	tinfo_key = TINFO_indexed;
}

//////////
// archiving
//////////

indexed::indexed(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	if (!n.find_ex("symmetry", symtree, sym_lst)) {
		// GiNaC versions <= 0.9.0 had an unsigned "symmetry" property
		unsigned symm = 0;
		n.find_unsigned("symmetry", symm);
		switch (symm) {
			case 1:
				symtree = sy_symm();
				break;
			case 2:
				symtree = sy_anti();
				break;
			default:
				symtree = sy_none();
				break;
		}
		const_cast<symmetry &>(ex_to<symmetry>(symtree)).validate(seq.size() - 1);
	}
}

void indexed::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_ex("symmetry", symtree);
}

DEFAULT_UNARCHIVE(indexed)

//////////
// functions overriding virtual functions from base classes
//////////

void indexed::print(const print_context & c, unsigned level) const
{
	GINAC_ASSERT(seq.size() > 0);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << ", " << seq.size()-1 << " indices"
		    << ", symmetry=" << symtree << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		seq[0].print(c, level + delta_indent);
		printindices(c, level + delta_indent);

	} else {

		bool is_tex = is_of_type(c, print_latex);
		const ex & base = seq[0];
		bool need_parens = is_ex_exactly_of_type(base, add) || is_ex_exactly_of_type(base, mul)
		                || is_ex_exactly_of_type(base, ncmul) || is_ex_exactly_of_type(base, power)
		                || is_ex_of_type(base, indexed);
		if (is_tex)
			c.s << "{";
		if (need_parens)
			c.s << "(";
		base.print(c);
		if (need_parens)
			c.s << ")";
		if (is_tex)
			c.s << "}";
		printindices(c, level);
	}
}

bool indexed::info(unsigned inf) const
{
	if (inf == info_flags::indexed) return true;
	if (inf == info_flags::has_indices) return seq.size() > 1;
	return inherited::info(inf);
}

struct idx_is_not : public std::binary_function<ex, unsigned, bool> {
	bool operator() (const ex & e, unsigned inf) const {
		return !(ex_to<idx>(e).get_value().info(inf));
	}
};

bool indexed::all_index_values_are(unsigned inf) const
{
	// No indices? Then no property can be fulfilled
	if (seq.size() < 2)
		return false;

	// Check all indices
	return find_if(seq.begin() + 1, seq.end(), bind2nd(idx_is_not(), inf)) == seq.end();
}

int indexed::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<indexed>(other));
	return inherited::compare_same_type(other);
}

ex indexed::eval(int level) const
{
	// First evaluate children, then we will end up here again
	if (level > 1)
		return indexed(ex_to<symmetry>(symtree), evalchildren(level));

	const ex &base = seq[0];

	// If the base object is 0, the whole object is 0
	if (base.is_zero())
		return _ex0;

	// If the base object is a product, pull out the numeric factor
	if (is_ex_exactly_of_type(base, mul) && is_ex_exactly_of_type(base.op(base.nops() - 1), numeric)) {
		exvector v(seq);
		ex f = ex_to<numeric>(base.op(base.nops() - 1));
		v[0] = seq[0] / f;
		return f * thisexprseq(v);
	}

	// Canonicalize indices according to the symmetry properties
	if (seq.size() > 2) {
		exvector v = seq;
		GINAC_ASSERT(is_exactly_a<symmetry>(symtree));
		int sig = canonicalize(v.begin() + 1, ex_to<symmetry>(symtree));
		if (sig != INT_MAX) {
			// Something has changed while sorting indices, more evaluations later
			if (sig == 0)
				return _ex0;
			return ex(sig) * thisexprseq(v);
		}
	}

	// Let the class of the base object perform additional evaluations
	return ex_to<basic>(base).eval_indexed(*this);
}

ex indexed::thisexprseq(const exvector & v) const
{
	return indexed(ex_to<symmetry>(symtree), v);
}

ex indexed::thisexprseq(exvector * vp) const
{
	return indexed(ex_to<symmetry>(symtree), vp);
}

ex indexed::expand(unsigned options) const
{
	GINAC_ASSERT(seq.size() > 0);

	if ((options & expand_options::expand_indexed) && is_ex_exactly_of_type(seq[0], add)) {

		// expand_indexed expands (a+b).i -> a.i + b.i
		const ex & base = seq[0];
		ex sum = _ex0;
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

void indexed::printindices(const print_context & c, unsigned level) const
{
	if (seq.size() > 1) {

		exvector::const_iterator it=seq.begin() + 1, itend = seq.end();

		if (is_of_type(c, print_latex)) {

			// TeX output: group by variance
			bool first = true;
			bool covariant = true;

			while (it != itend) {
				bool cur_covariant = (is_ex_of_type(*it, varidx) ? ex_to<varidx>(*it).is_covariant() : true);
				if (first || cur_covariant != covariant) { // Variance changed
					// The empty {} prevents indices from ending up on top of each other
					if (!first)
						c.s << "}{}";
					covariant = cur_covariant;
					if (covariant)
						c.s << "_{";
					else
						c.s << "^{";
				}
				it->print(c, level);
				c.s << " ";
				first = false;
				it++;
			}
			c.s << "}";

		} else {

			// Ordinary output
			while (it != itend) {
				it->print(c, level);
				it++;
			}
		}
	}
}

/** Check whether all indices are of class idx and validate the symmetry
 *  tree. This function is used internally to make sure that all constructed
 *  indexed objects really carry indices and not some other classes. */
void indexed::validate(void) const
{
	GINAC_ASSERT(seq.size() > 0);
	exvector::const_iterator it = seq.begin() + 1, itend = seq.end();
	while (it != itend) {
		if (!is_ex_of_type(*it, idx))
			throw(std::invalid_argument("indices of indexed object must be of type idx"));
		it++;
	}

	if (!symtree.is_zero()) {
		if (!is_ex_exactly_of_type(symtree, symmetry))
			throw(std::invalid_argument("symmetry of indexed object must be of type symmetry"));
		const_cast<symmetry &>(ex_to<symmetry>(symtree)).validate(seq.size() - 1);
	}
}

/** Implementation of ex::diff() for an indexed object always returns 0.
 *
 *  @see ex::diff */
ex indexed::derivative(const symbol & s) const
{
	return _ex0;
}

//////////
// global functions
//////////

/** Check whether two sorted index vectors are consistent (i.e. equal). */
static bool indices_consistent(const exvector & v1, const exvector & v2)
{
	// Number of indices must be the same
	if (v1.size() != v2.size())
		return false;

	return equal(v1.begin(), v1.end(), v2.begin(), ex_is_equal());
}

exvector indexed::get_indices(void) const
{
	GINAC_ASSERT(seq.size() >= 1);
	return exvector(seq.begin() + 1, seq.end());
}

exvector indexed::get_dummy_indices(void) const
{
	exvector free_indices, dummy_indices;
	find_free_and_dummy(seq.begin() + 1, seq.end(), free_indices, dummy_indices);
	return dummy_indices;
}

exvector indexed::get_dummy_indices(const indexed & other) const
{
	exvector indices = get_free_indices();
	exvector other_indices = other.get_free_indices();
	indices.insert(indices.end(), other_indices.begin(), other_indices.end());
	exvector dummy_indices;
	find_dummy_indices(indices, dummy_indices);
	return dummy_indices;
}

bool indexed::has_dummy_index_for(const ex & i) const
{
	exvector::const_iterator it = seq.begin() + 1, itend = seq.end();
	while (it != itend) {
		if (is_dummy_pair(*it, i))
			return true;
		it++;
	}
	return false;
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
	find_free_and_dummy(un, free_indices, dummy_indices);
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
	find_free_and_dummy(un, free_indices, dummy_indices);
	return free_indices;
}

exvector power::get_free_indices(void) const
{
	// Return free indices of basis
	return basis.get_free_indices();
}

/** Rename dummy indices in an expression.
 *
 *  @param e Expression to be worked on
 *  @param local_dummy_indices The set of dummy indices that appear in the
 *    expression "e"
 *  @param global_dummy_indices The set of dummy indices that have appeared
 *    before and which we would like to use in "e", too. This gets updated
 *    by the function */
static ex rename_dummy_indices(const ex & e, exvector & global_dummy_indices, exvector & local_dummy_indices)
{
	unsigned global_size = global_dummy_indices.size(),
	         local_size = local_dummy_indices.size();

	// Any local dummy indices at all?
	if (local_size == 0)
		return e;

	if (global_size < local_size) {

		// More local indices than we encountered before, add the new ones
		// to the global set
		int old_global_size = global_size;
		int remaining = local_size - global_size;
		exvector::const_iterator it = local_dummy_indices.begin(), itend = local_dummy_indices.end();
		while (it != itend && remaining > 0) {
			if (find_if(global_dummy_indices.begin(), global_dummy_indices.end(), bind2nd(ex_is_equal(), *it)) == global_dummy_indices.end()) {
				global_dummy_indices.push_back(*it);
				global_size++;
				remaining--;
			}
			it++;
		}

		// If this is the first set of local indices, do nothing
		if (old_global_size == 0)
			return e;
	}
	GINAC_ASSERT(local_size <= global_size);

	// Construct lists of index symbols
	exlist local_syms, global_syms;
	for (unsigned i=0; i<local_size; i++)
		local_syms.push_back(local_dummy_indices[i].op(0));
	shaker_sort(local_syms.begin(), local_syms.end(), ex_is_less(), ex_swap());
	for (unsigned i=0; i<global_size; i++)
		global_syms.push_back(global_dummy_indices[i].op(0));
	shaker_sort(global_syms.begin(), global_syms.end(), ex_is_less(), ex_swap());

	// Remove common indices
	exlist local_uniq, global_uniq;
	set_difference(local_syms.begin(), local_syms.end(), global_syms.begin(), global_syms.end(), std::back_insert_iterator<exlist>(local_uniq), ex_is_less());
	set_difference(global_syms.begin(), global_syms.end(), local_syms.begin(), local_syms.end(), std::back_insert_iterator<exlist>(global_uniq), ex_is_less());

	// Replace remaining non-common local index symbols by global ones
	if (local_uniq.empty())
		return e;
	else {
		while (global_uniq.size() > local_uniq.size())
			global_uniq.pop_back();
		return e.subs(lst(local_uniq), lst(global_uniq));
	}
}

/** Simplify product of indexed expressions (commutative, noncommutative and
 *  simple squares), return list of free indices. */
ex simplify_indexed_product(const ex & e, exvector & free_indices, exvector & dummy_indices, const scalar_products & sp)
{
	// Remember whether the product was commutative or noncommutative
	// (because we chop it into factors and need to reassemble later)
	bool non_commutative = is_ex_exactly_of_type(e, ncmul);

	// Collect factors in an exvector, store squares twice
	exvector v;
	v.reserve(e.nops() * 2);

	if (is_ex_exactly_of_type(e, power)) {
		// We only get called for simple squares, split a^2 -> a*a
		GINAC_ASSERT(e.op(1).is_equal(_ex2));
		v.push_back(e.op(0));
		v.push_back(e.op(0));
	} else {
		for (unsigned i=0; i<e.nops(); i++) {
			ex f = e.op(i);
			if (is_ex_exactly_of_type(f, power) && f.op(1).is_equal(_ex2)) {
				v.push_back(f.op(0));
	            v.push_back(f.op(0));
			} else if (is_ex_exactly_of_type(f, ncmul)) {
				// Noncommutative factor found, split it as well
				non_commutative = true; // everything becomes noncommutative, ncmul will sort out the commutative factors later
				for (unsigned j=0; j<f.nops(); j++)
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

		bool first_noncommutative = (it1->return_type() != return_types::commutative);

		// Indexed factor found, get free indices and look for contraction
		// candidates
		exvector free1, dummy1;
		find_free_and_dummy(ex_to<indexed>(*it1).seq.begin() + 1, ex_to<indexed>(*it1).seq.end(), free1, dummy1);

		exvector::iterator it2;
		for (it2 = it1 + 1; it2 != itend; it2++) {

			if (!is_ex_of_type(*it2, indexed))
				continue;

			bool second_noncommutative = (it2->return_type() != return_types::commutative);

			// Find free indices of second factor and merge them with free
			// indices of first factor
			exvector un;
			find_free_and_dummy(ex_to<indexed>(*it2).seq.begin() + 1, ex_to<indexed>(*it2).seq.end(), un, dummy1);
			un.insert(un.end(), free1.begin(), free1.end());

			// Check whether the two factors share dummy indices
			exvector free, dummy;
			find_free_and_dummy(un, free, dummy);
			unsigned num_dummies = dummy.size();
			if (num_dummies == 0)
				continue;

			// At least one dummy index, is it a defined scalar product?
			bool contracted = false;
			if (free.empty()) {
				if (sp.is_defined(*it1, *it2)) {
					*it1 = sp.evaluate(*it1, *it2);
					*it2 = _ex1;
					goto contraction_done;
				}
			}

			// Try to contract the first one with the second one
			contracted = ex_to<basic>(it1->op(0)).contract_with(it1, it2, v);
			if (!contracted) {

				// That didn't work; maybe the second object knows how to
				// contract itself with the first one
				contracted = ex_to<basic>(it2->op(0)).contract_with(it2, it1, v);
			}
			if (contracted) {
contraction_done:
				if (first_noncommutative || second_noncommutative
				 || is_ex_exactly_of_type(*it1, add) || is_ex_exactly_of_type(*it2, add)
				 || is_ex_exactly_of_type(*it1, mul) || is_ex_exactly_of_type(*it2, mul)
				 || is_ex_exactly_of_type(*it1, ncmul) || is_ex_exactly_of_type(*it2, ncmul)) {

					// One of the factors became a sum or product:
					// re-expand expression and run again
					// Non-commutative products are always re-expanded to give
					// simplify_ncmul() the chance to re-order and canonicalize
					// the product
					ex r = (non_commutative ? ex(ncmul(v, true)) : ex(mul(v)));
					return simplify_indexed(r, free_indices, dummy_indices, sp);
				}

				// Both objects may have new indices now or they might
				// even not be indexed objects any more, so we have to
				// start over
				something_changed = true;
				goto try_again;
			}
		}
	}

	// Find free indices (concatenate them all and call find_free_and_dummy())
	// and all dummy indices that appear
	exvector un, individual_dummy_indices;
	it1 = v.begin(); itend = v.end();
	while (it1 != itend) {
		exvector free_indices_of_factor;
		if (is_ex_of_type(*it1, indexed)) {
			exvector dummy_indices_of_factor;
			find_free_and_dummy(ex_to<indexed>(*it1).seq.begin() + 1, ex_to<indexed>(*it1).seq.end(), free_indices_of_factor, dummy_indices_of_factor);
			individual_dummy_indices.insert(individual_dummy_indices.end(), dummy_indices_of_factor.begin(), dummy_indices_of_factor.end());
		} else
			free_indices_of_factor = it1->get_free_indices();
		un.insert(un.end(), free_indices_of_factor.begin(), free_indices_of_factor.end());
		it1++;
	}
	exvector local_dummy_indices;
	find_free_and_dummy(un, free_indices, local_dummy_indices);
	local_dummy_indices.insert(local_dummy_indices.end(), individual_dummy_indices.begin(), individual_dummy_indices.end());

	ex r;
	if (something_changed)
		r = non_commutative ? ex(ncmul(v, true)) : ex(mul(v));
	else
		r = e;

	// The result should be symmetric with respect to exchange of dummy
	// indices, so if the symmetrization vanishes, the whole expression is
	// zero. This detects things like eps.i.j.k * p.j * p.k = 0.
	if (local_dummy_indices.size() >= 2) {
		lst dummy_syms;
		for (int i=0; i<local_dummy_indices.size(); i++)
			dummy_syms.append(local_dummy_indices[i].op(0));
		if (r.symmetrize(dummy_syms).is_zero()) {
			free_indices.clear();
			return _ex0;
		}
	}

	// Dummy index renaming
	r = rename_dummy_indices(r, dummy_indices, local_dummy_indices);

	// Product of indexed object with a scalar?
	if (is_ex_exactly_of_type(r, mul) && r.nops() == 2
	 && is_ex_exactly_of_type(r.op(1), numeric) && is_ex_of_type(r.op(0), indexed))
		return ex_to<basic>(r.op(0).op(0)).scalar_mul_indexed(r.op(0), ex_to<numeric>(r.op(1)));
	else
		return r;
}

/** Simplify indexed expression, return list of free indices. */
ex simplify_indexed(const ex & e, exvector & free_indices, exvector & dummy_indices, const scalar_products & sp)
{
	// Expand the expression
	ex e_expanded = e.expand();

	// Simplification of single indexed object: just find the free indices
	// and perform dummy index renaming
	if (is_ex_of_type(e_expanded, indexed)) {
		const indexed &i = ex_to<indexed>(e_expanded);
		exvector local_dummy_indices;
		find_free_and_dummy(i.seq.begin() + 1, i.seq.end(), free_indices, local_dummy_indices);
		return rename_dummy_indices(e_expanded, dummy_indices, local_dummy_indices);
	}

	// Simplification of sum = sum of simplifications, check consistency of
	// free indices in each term
	if (is_ex_exactly_of_type(e_expanded, add)) {
		bool first = true;
		ex sum = _ex0;
		free_indices.clear();

		for (unsigned i=0; i<e_expanded.nops(); i++) {
			exvector free_indices_of_term;
			ex term = simplify_indexed(e_expanded.op(i), free_indices_of_term, dummy_indices, sp);
			if (!term.is_zero()) {
				if (first) {
					free_indices = free_indices_of_term;
					sum = term;
					first = false;
				} else {
					if (!indices_consistent(free_indices, free_indices_of_term))
						throw (std::runtime_error("simplify_indexed: inconsistent indices in sum"));
					if (is_ex_of_type(sum, indexed) && is_ex_of_type(term, indexed))
						sum = ex_to<basic>(sum.op(0)).add_indexed(sum, term);
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
	 || (is_ex_exactly_of_type(e_expanded, power) && is_ex_of_type(e_expanded.op(0), indexed) && e_expanded.op(1).is_equal(_ex2)))
		return simplify_indexed_product(e_expanded, free_indices, dummy_indices, sp);

	// Cannot do anything
	free_indices.clear();
	return e_expanded;
}

/** Simplify/canonicalize expression containing indexed objects. This
 *  performs contraction of dummy indices where possible and checks whether
 *  the free indices in sums are consistent.
 *
 *  @return simplified expression */
ex ex::simplify_indexed(void) const
{
	exvector free_indices, dummy_indices;
	scalar_products sp;
	return GiNaC::simplify_indexed(*this, free_indices, dummy_indices, sp);
}

/** Simplify/canonicalize expression containing indexed objects. This
 *  performs contraction of dummy indices where possible, checks whether
 *  the free indices in sums are consistent, and automatically replaces
 *  scalar products by known values if desired.
 *
 *  @param sp Scalar products to be replaced automatically
 *  @return simplified expression */
ex ex::simplify_indexed(const scalar_products & sp) const
{
	exvector free_indices, dummy_indices;
	return GiNaC::simplify_indexed(*this, free_indices, dummy_indices, sp);
}

/** Symmetrize expression over its free indices. */
ex ex::symmetrize(void) const
{
	return GiNaC::symmetrize(*this, get_free_indices());
}

/** Antisymmetrize expression over its free indices. */
ex ex::antisymmetrize(void) const
{
	return GiNaC::antisymmetrize(*this, get_free_indices());
}

/** Symmetrize expression by cyclic permutation over its free indices. */
ex ex::symmetrize_cyclic(void) const
{
	return GiNaC::symmetrize_cyclic(*this, get_free_indices());
}

//////////
// helper classes
//////////

void scalar_products::add(const ex & v1, const ex & v2, const ex & sp)
{
	spm[make_key(v1, v2)] = sp;
}

void scalar_products::add_vectors(const lst & l)
{
	// Add all possible pairs of products
	unsigned num = l.nops();
	for (unsigned i=0; i<num; i++) {
		ex a = l.op(i);
		for (unsigned j=0; j<num; j++) {
			ex b = l.op(j);
			add(a, b, a*b);
		}
	}
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
	spmap::const_iterator i = spm.begin(), end = spm.end();
	while (i != end) {
		const spmapkey & k = i->first;
		std::cerr << "item key=(" << k.first << "," << k.second;
		std::cerr << "), value=" << i->second << std::endl;
		++i;
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
