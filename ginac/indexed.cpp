/** @file indexed.cpp
 *
 *  Implementation of GiNaC's indexed expressions. */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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
#include <sstream>
#include <stdexcept>

#include "indexed.h"
#include "idx.h"
#include "add.h"
#include "mul.h"
#include "ncmul.h"
#include "power.h"
#include "relational.h"
#include "symmetry.h"
#include "operators.h"
#include "lst.h"
#include "archive.h"
#include "utils.h"
#include "integral.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(indexed, exprseq,
  print_func<print_context>(&indexed::do_print).
  print_func<print_latex>(&indexed::do_print_latex).
  print_func<print_tree>(&indexed::do_print_tree))

//////////
// default constructor
//////////

indexed::indexed() : symtree(not_symmetric())
{
	tinfo_key = TINFO_indexed;
}

//////////
// other constructors
//////////

indexed::indexed(const ex & b) : inherited(b), symtree(not_symmetric())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1) : inherited(b, i1), symtree(not_symmetric())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2) : inherited(b, i1, i2), symtree(not_symmetric())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3) : inherited(b, i1, i2, i3), symtree(not_symmetric())
{
	tinfo_key = TINFO_indexed;
	validate();
}

indexed::indexed(const ex & b, const ex & i1, const ex & i2, const ex & i3, const ex & i4) : inherited(b, i1, i2, i3, i4), symtree(not_symmetric())
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

indexed::indexed(const ex & b, const exvector & v) : inherited(b), symtree(not_symmetric())
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

indexed::indexed(const symmetry & symm, std::auto_ptr<exvector> vp) : inherited(vp), symtree(symm)
{
	tinfo_key = TINFO_indexed;
}

//////////
// archiving
//////////

indexed::indexed(const archive_node &n, lst &sym_lst) : inherited(n, sym_lst)
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
				symtree = not_symmetric();
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

void indexed::printindices(const print_context & c, unsigned level) const
{
	if (seq.size() > 1) {

		exvector::const_iterator it=seq.begin() + 1, itend = seq.end();

		if (is_a<print_latex>(c)) {

			// TeX output: group by variance
			bool first = true;
			bool covariant = true;

			while (it != itend) {
				bool cur_covariant = (is_a<varidx>(*it) ? ex_to<varidx>(*it).is_covariant() : true);
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

void indexed::print_indexed(const print_context & c, const char *openbrace, const char *closebrace, unsigned level) const
{
	if (precedence() <= level)
		c.s << openbrace << '(';
	c.s << openbrace;
	seq[0].print(c, precedence());
	c.s << closebrace;
	printindices(c, level);
	if (precedence() <= level)
		c.s << ')' << closebrace;
}

void indexed::do_print(const print_context & c, unsigned level) const
{
	print_indexed(c, "", "", level);
}

void indexed::do_print_latex(const print_latex & c, unsigned level) const
{
	print_indexed(c, "{", "}", level);
}

void indexed::do_print_tree(const print_tree & c, unsigned level) const
{
	c.s << std::string(level, ' ') << class_name() << " @" << this
	    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
	    << ", " << seq.size()-1 << " indices"
	    << ", symmetry=" << symtree << std::endl;
	seq[0].print(c, level + c.delta_indent);
	printindices(c, level + c.delta_indent);
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
	if (is_exactly_a<mul>(base) && is_exactly_a<numeric>(base.op(base.nops() - 1))) {
		exvector v(seq);
		ex f = ex_to<numeric>(base.op(base.nops() - 1));
		v[0] = seq[0] / f;
		return f * thiscontainer(v);
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
			return ex(sig) * thiscontainer(v);
		}
	}

	// Let the class of the base object perform additional evaluations
	return ex_to<basic>(base).eval_indexed(*this);
}

ex indexed::thiscontainer(const exvector & v) const
{
	return indexed(ex_to<symmetry>(symtree), v);
}

ex indexed::thiscontainer(std::auto_ptr<exvector> vp) const
{
	return indexed(ex_to<symmetry>(symtree), vp);
}

ex indexed::expand(unsigned options) const
{
	GINAC_ASSERT(seq.size() > 0);

	if (options & expand_options::expand_indexed) {
		ex newbase = seq[0].expand(options);
		if (is_exactly_a<add>(newbase)) {
			ex sum = _ex0;
			for (size_t i=0; i<newbase.nops(); i++) {
				exvector s = seq;
				s[0] = newbase.op(i);
				sum += thiscontainer(s).expand(options);
			}
			return sum;
		}
		if (!are_ex_trivially_equal(newbase, seq[0])) {
			exvector s = seq;
			s[0] = newbase;
			return ex_to<indexed>(thiscontainer(s)).inherited::expand(options);
		}
	}
	return inherited::expand(options);
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

/** Check whether all indices are of class idx and validate the symmetry
 *  tree. This function is used internally to make sure that all constructed
 *  indexed objects really carry indices and not some other classes. */
void indexed::validate() const
{
	GINAC_ASSERT(seq.size() > 0);
	exvector::const_iterator it = seq.begin() + 1, itend = seq.end();
	while (it != itend) {
		if (!is_a<idx>(*it))
			throw(std::invalid_argument("indices of indexed object must be of type idx"));
		it++;
	}

	if (!symtree.is_zero()) {
		if (!is_exactly_a<symmetry>(symtree))
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

struct idx_is_equal_ignore_dim : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const
	{
		if (lh.is_equal(rh))
			return true;
		else
			try {
				// Replacing the dimension might cause an error (e.g. with
				// index classes that only work in a fixed number of dimensions)
				return lh.is_equal(ex_to<idx>(rh).replace_dim(ex_to<idx>(lh).get_dim()));
			} catch (...) {
				return false;
			}
	}
};

/** Check whether two sorted index vectors are consistent (i.e. equal). */
static bool indices_consistent(const exvector & v1, const exvector & v2)
{
	// Number of indices must be the same
	if (v1.size() != v2.size())
		return false;

	return equal(v1.begin(), v1.end(), v2.begin(), idx_is_equal_ignore_dim());
}

exvector indexed::get_indices() const
{
	GINAC_ASSERT(seq.size() >= 1);
	return exvector(seq.begin() + 1, seq.end());
}

exvector indexed::get_dummy_indices() const
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

exvector indexed::get_free_indices() const
{
	exvector free_indices, dummy_indices;
	find_free_and_dummy(seq.begin() + 1, seq.end(), free_indices, dummy_indices);
	return free_indices;
}

exvector add::get_free_indices() const
{
	exvector free_indices;
	for (size_t i=0; i<nops(); i++) {
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

exvector mul::get_free_indices() const
{
	// Concatenate free indices of all factors
	exvector un;
	for (size_t i=0; i<nops(); i++) {
		exvector free_indices_of_factor = op(i).get_free_indices();
		un.insert(un.end(), free_indices_of_factor.begin(), free_indices_of_factor.end());
	}

	// And remove the dummy indices
	exvector free_indices, dummy_indices;
	find_free_and_dummy(un, free_indices, dummy_indices);
	return free_indices;
}

exvector ncmul::get_free_indices() const
{
	// Concatenate free indices of all factors
	exvector un;
	for (size_t i=0; i<nops(); i++) {
		exvector free_indices_of_factor = op(i).get_free_indices();
		un.insert(un.end(), free_indices_of_factor.begin(), free_indices_of_factor.end());
	}

	// And remove the dummy indices
	exvector free_indices, dummy_indices;
	find_free_and_dummy(un, free_indices, dummy_indices);
	return free_indices;
}

struct is_summation_idx : public std::unary_function<ex, bool> {
	bool operator()(const ex & e)
	{
		return is_dummy_pair(e, e);
	}
};

exvector power::get_free_indices() const
{
	// Get free indices of basis
	exvector basis_indices = basis.get_free_indices();

	if (exponent.info(info_flags::even)) {
		// If the exponent is an even number, then any "free" index that
		// forms a dummy pair with itself is actually a summation index
		exvector really_free;
		std::remove_copy_if(basis_indices.begin(), basis_indices.end(),
		                    std::back_inserter(really_free), is_summation_idx());
		return really_free;
	} else
		return basis_indices;
}

exvector integral::get_free_indices() const
{
	if (a.get_free_indices().size() || b.get_free_indices().size())
		throw (std::runtime_error("integral::get_free_indices: boundary values should not have free indices"));
	return f.get_free_indices();
}

/** Rename dummy indices in an expression.
 *
 *  @param e Expression to work on
 *  @param local_dummy_indices The set of dummy indices that appear in the
 *    expression "e"
 *  @param global_dummy_indices The set of dummy indices that have appeared
 *    before and which we would like to use in "e", too. This gets updated
 *    by the function */
static ex rename_dummy_indices(const ex & e, exvector & global_dummy_indices, exvector & local_dummy_indices)
{
	size_t global_size = global_dummy_indices.size(),
	       local_size = local_dummy_indices.size();

	// Any local dummy indices at all?
	if (local_size == 0)
		return e;

	if (global_size < local_size) {

		// More local indices than we encountered before, add the new ones
		// to the global set
		size_t old_global_size = global_size;
		int remaining = local_size - global_size;
		exvector::const_iterator it = local_dummy_indices.begin(), itend = local_dummy_indices.end();
		while (it != itend && remaining > 0) {
			if (find_if(global_dummy_indices.begin(), global_dummy_indices.end(), bind2nd(op0_is_equal(), *it)) == global_dummy_indices.end()) {
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

	// Construct vectors of index symbols
	exvector local_syms, global_syms;
	local_syms.reserve(local_size);
	global_syms.reserve(local_size);
	for (size_t i=0; i<local_size; i++)
		local_syms.push_back(local_dummy_indices[i].op(0));
	shaker_sort(local_syms.begin(), local_syms.end(), ex_is_less(), ex_swap());
	for (size_t i=0; i<local_size; i++) // don't use more global symbols than necessary
		global_syms.push_back(global_dummy_indices[i].op(0));
	shaker_sort(global_syms.begin(), global_syms.end(), ex_is_less(), ex_swap());

	// Remove common indices
	exvector local_uniq, global_uniq;
	set_difference(local_syms.begin(), local_syms.end(), global_syms.begin(), global_syms.end(), std::back_insert_iterator<exvector>(local_uniq), ex_is_less());
	set_difference(global_syms.begin(), global_syms.end(), local_syms.begin(), local_syms.end(), std::back_insert_iterator<exvector>(global_uniq), ex_is_less());

	// Replace remaining non-common local index symbols by global ones
	if (local_uniq.empty())
		return e;
	else {
		while (global_uniq.size() > local_uniq.size())
			global_uniq.pop_back();
		return e.subs(lst(local_uniq.begin(), local_uniq.end()), lst(global_uniq.begin(), global_uniq.end()), subs_options::no_pattern);
	}
}

/** Given a set of indices, extract those of class varidx. */
static void find_variant_indices(const exvector & v, exvector & variant_indices)
{
	exvector::const_iterator it1, itend;
	for (it1 = v.begin(), itend = v.end(); it1 != itend; ++it1) {
		if (is_exactly_a<varidx>(*it1))
			variant_indices.push_back(*it1);
	}
}

/** Raise/lower dummy indices in a single indexed objects to canonicalize their
 *  variance.
 *
 *  @param e Object to work on
 *  @param variant_dummy_indices The set of indices that might need repositioning (will be changed by this function)
 *  @param moved_indices The set of indices that have been repositioned (will be changed by this function)
 *  @return true if 'e' was changed */
bool reposition_dummy_indices(ex & e, exvector & variant_dummy_indices, exvector & moved_indices)
{
	bool something_changed = false;

	// If a dummy index is encountered for the first time in the
	// product, pull it up, otherwise, pull it down
	exvector::const_iterator it2, it2start, it2end;
	for (it2start = ex_to<indexed>(e).seq.begin(), it2end = ex_to<indexed>(e).seq.end(), it2 = it2start + 1; it2 != it2end; ++it2) {
		if (!is_exactly_a<varidx>(*it2))
			continue;

		exvector::iterator vit, vitend;
		for (vit = variant_dummy_indices.begin(), vitend = variant_dummy_indices.end(); vit != vitend; ++vit) {
			if (it2->op(0).is_equal(vit->op(0))) {
				if (ex_to<varidx>(*it2).is_covariant()) {
					e = e.subs(lst(
						*it2 == ex_to<varidx>(*it2).toggle_variance(),
						ex_to<varidx>(*it2).toggle_variance() == *it2
					), subs_options::no_pattern);
					something_changed = true;
					it2 = ex_to<indexed>(e).seq.begin() + (it2 - it2start);
					it2start = ex_to<indexed>(e).seq.begin();
					it2end = ex_to<indexed>(e).seq.end();
				}
				moved_indices.push_back(*vit);
				variant_dummy_indices.erase(vit);
				goto next_index;
			}
		}

		for (vit = moved_indices.begin(), vitend = moved_indices.end(); vit != vitend; ++vit) {
			if (it2->op(0).is_equal(vit->op(0))) {
				if (ex_to<varidx>(*it2).is_contravariant()) {
					e = e.subs(*it2 == ex_to<varidx>(*it2).toggle_variance(), subs_options::no_pattern);
					something_changed = true;
					it2 = ex_to<indexed>(e).seq.begin() + (it2 - it2start);
					it2start = ex_to<indexed>(e).seq.begin();
					it2end = ex_to<indexed>(e).seq.end();
				}
				goto next_index;
			}
		}

next_index: ;
	}

	return something_changed;
}

/* Ordering that only compares the base expressions of indexed objects. */
struct ex_base_is_less : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const
	{
		return (is_a<indexed>(lh) ? lh.op(0) : lh).compare(is_a<indexed>(rh) ? rh.op(0) : rh) < 0;
	}
};

/** Simplify product of indexed expressions (commutative, noncommutative and
 *  simple squares), return list of free indices. */
ex simplify_indexed_product(const ex & e, exvector & free_indices, exvector & dummy_indices, const scalar_products & sp)
{
	// Remember whether the product was commutative or noncommutative
	// (because we chop it into factors and need to reassemble later)
	bool non_commutative = is_exactly_a<ncmul>(e);

	// Collect factors in an exvector, store squares twice
	exvector v;
	v.reserve(e.nops() * 2);

	if (is_exactly_a<power>(e)) {
		// We only get called for simple squares, split a^2 -> a*a
		GINAC_ASSERT(e.op(1).is_equal(_ex2));
		v.push_back(e.op(0));
		v.push_back(e.op(0));
	} else {
		for (size_t i=0; i<e.nops(); i++) {
			ex f = e.op(i);
			if (is_exactly_a<power>(f) && f.op(1).is_equal(_ex2)) {
				v.push_back(f.op(0));
	            v.push_back(f.op(0));
			} else if (is_exactly_a<ncmul>(f)) {
				// Noncommutative factor found, split it as well
				non_commutative = true; // everything becomes noncommutative, ncmul will sort out the commutative factors later
				for (size_t j=0; j<f.nops(); j++)
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
		if (!is_a<indexed>(*it1))
			continue;

		bool first_noncommutative = (it1->return_type() != return_types::commutative);

		// Indexed factor found, get free indices and look for contraction
		// candidates
		exvector free1, dummy1;
		find_free_and_dummy(ex_to<indexed>(*it1).seq.begin() + 1, ex_to<indexed>(*it1).seq.end(), free1, dummy1);

		exvector::iterator it2;
		for (it2 = it1 + 1; it2 != itend; it2++) {

			if (!is_a<indexed>(*it2))
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
			size_t num_dummies = dummy.size();
			if (num_dummies == 0)
				continue;

			// At least one dummy index, is it a defined scalar product?
			bool contracted = false;
			if (free.empty()) {

				// Find minimal dimension of all indices of both factors
				exvector::const_iterator dit = ex_to<indexed>(*it1).seq.begin() + 1, ditend = ex_to<indexed>(*it1).seq.end();
				ex dim = ex_to<idx>(*dit).get_dim();
				++dit;
				for (; dit != ditend; ++dit) {
					dim = minimal_dim(dim, ex_to<idx>(*dit).get_dim());
				}
				dit = ex_to<indexed>(*it2).seq.begin() + 1;
				ditend = ex_to<indexed>(*it2).seq.end();
				for (; dit != ditend; ++dit) {
					dim = minimal_dim(dim, ex_to<idx>(*dit).get_dim());
				}

				// User-defined scalar product?
				if (sp.is_defined(*it1, *it2, dim)) {

					// Yes, substitute it
					*it1 = sp.evaluate(*it1, *it2, dim);
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
				 || is_exactly_a<add>(*it1) || is_exactly_a<add>(*it2)
				 || is_exactly_a<mul>(*it1) || is_exactly_a<mul>(*it2)
				 || is_exactly_a<ncmul>(*it1) || is_exactly_a<ncmul>(*it2)) {

					// One of the factors became a sum or product:
					// re-expand expression and run again
					// Non-commutative products are always re-expanded to give
					// eval_ncmul() the chance to re-order and canonicalize
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
	for (it1 = v.begin(), itend = v.end(); it1 != itend; ++it1) {
		exvector free_indices_of_factor;
		if (is_a<indexed>(*it1)) {
			exvector dummy_indices_of_factor;
			find_free_and_dummy(ex_to<indexed>(*it1).seq.begin() + 1, ex_to<indexed>(*it1).seq.end(), free_indices_of_factor, dummy_indices_of_factor);
			individual_dummy_indices.insert(individual_dummy_indices.end(), dummy_indices_of_factor.begin(), dummy_indices_of_factor.end());
		} else
			free_indices_of_factor = it1->get_free_indices();
		un.insert(un.end(), free_indices_of_factor.begin(), free_indices_of_factor.end());
	}
	exvector local_dummy_indices;
	find_free_and_dummy(un, free_indices, local_dummy_indices);
	local_dummy_indices.insert(local_dummy_indices.end(), individual_dummy_indices.begin(), individual_dummy_indices.end());

	// Filter out the dummy indices with variance
	exvector variant_dummy_indices;
	find_variant_indices(local_dummy_indices, variant_dummy_indices);

	// Any indices with variance present at all?
	if (!variant_dummy_indices.empty()) {

		// Yes, bring the product into a canonical order that only depends on
		// the base expressions of indexed objects
		if (!non_commutative)
			std::sort(v.begin(), v.end(), ex_base_is_less());

		exvector moved_indices;

		// Iterate over all indexed objects in the product
		for (it1 = v.begin(), itend = v.end(); it1 != itend; ++it1) {
			if (!is_a<indexed>(*it1))
				continue;

			if (reposition_dummy_indices(*it1, variant_dummy_indices, moved_indices))
				something_changed = true;
		}
	}

	ex r;
	if (something_changed)
		r = non_commutative ? ex(ncmul(v, true)) : ex(mul(v));
	else
		r = e;

	// The result should be symmetric with respect to exchange of dummy
	// indices, so if the symmetrization vanishes, the whole expression is
	// zero. This detects things like eps.i.j.k * p.j * p.k = 0.
	if (local_dummy_indices.size() >= 2) {
		exvector dummy_syms;
		dummy_syms.reserve(local_dummy_indices.size());
		for (exvector::const_iterator it = local_dummy_indices.begin(); it != local_dummy_indices.end(); ++it)
			dummy_syms.push_back(it->op(0));
		if (symmetrize(r, dummy_syms).is_zero()) {
			free_indices.clear();
			return _ex0;
		}
	}

	// Dummy index renaming
	r = rename_dummy_indices(r, dummy_indices, local_dummy_indices);

	// Product of indexed object with a scalar?
	if (is_exactly_a<mul>(r) && r.nops() == 2
	 && is_exactly_a<numeric>(r.op(1)) && is_a<indexed>(r.op(0)))
		return ex_to<basic>(r.op(0).op(0)).scalar_mul_indexed(r.op(0), ex_to<numeric>(r.op(1)));
	else
		return r;
}

/** This structure stores the original and symmetrized versions of terms
 *  obtained during the simplification of sums. */
class terminfo {
public:
	terminfo(const ex & orig_, const ex & symm_) : orig(orig_), symm(symm_) {}

	ex orig; /**< original term */
	ex symm; /**< symmtrized term */
};

class terminfo_is_less {
public:
	bool operator() (const terminfo & ti1, const terminfo & ti2) const
	{
		return (ti1.symm.compare(ti2.symm) < 0);
	}
};

/** This structure stores the individual symmetrized terms obtained during
 *  the simplification of sums. */
class symminfo {
public:
	symminfo() : num(0) {}

	symminfo(const ex & symmterm_, const ex & orig_, size_t num_) : orig(orig_), num(num_)
	{
		if (is_exactly_a<mul>(symmterm_) && is_exactly_a<numeric>(symmterm_.op(symmterm_.nops()-1))) {
			coeff = symmterm_.op(symmterm_.nops()-1);
			symmterm = symmterm_ / coeff;
		} else {
			coeff = 1;
			symmterm = symmterm_;
		}
	}

	ex symmterm;  /**< symmetrized term */
	ex coeff;     /**< coefficient of symmetrized term */
	ex orig;      /**< original term */
	size_t num; /**< how many symmetrized terms resulted from the original term */
};

class symminfo_is_less_by_symmterm {
public:
	bool operator() (const symminfo & si1, const symminfo & si2) const
	{
		return (si1.symmterm.compare(si2.symmterm) < 0);
	}
};

class symminfo_is_less_by_orig {
public:
	bool operator() (const symminfo & si1, const symminfo & si2) const
	{
		return (si1.orig.compare(si2.orig) < 0);
	}
};

/** Simplify indexed expression, return list of free indices. */
ex simplify_indexed(const ex & e, exvector & free_indices, exvector & dummy_indices, const scalar_products & sp)
{
	// Expand the expression
	ex e_expanded = e.expand();

	// Simplification of single indexed object: just find the free indices
	// and perform dummy index renaming/repositioning
	if (is_a<indexed>(e_expanded)) {

		// Find the dummy indices
		const indexed &i = ex_to<indexed>(e_expanded);
		exvector local_dummy_indices;
		find_free_and_dummy(i.seq.begin() + 1, i.seq.end(), free_indices, local_dummy_indices);

		// Filter out the dummy indices with variance
		exvector variant_dummy_indices;
		find_variant_indices(local_dummy_indices, variant_dummy_indices);

		// Any indices with variance present at all?
		if (!variant_dummy_indices.empty()) {

			// Yes, reposition them
			exvector moved_indices;
			reposition_dummy_indices(e_expanded, variant_dummy_indices, moved_indices);
		}

		// Rename the dummy indices
		return rename_dummy_indices(e_expanded, dummy_indices, local_dummy_indices);
	}

	// Simplification of sum = sum of simplifications, check consistency of
	// free indices in each term
	if (is_exactly_a<add>(e_expanded)) {
		bool first = true;
		ex sum;
		free_indices.clear();

		for (size_t i=0; i<e_expanded.nops(); i++) {
			exvector free_indices_of_term;
			ex term = simplify_indexed(e_expanded.op(i), free_indices_of_term, dummy_indices, sp);
			if (!term.is_zero()) {
				if (first) {
					free_indices = free_indices_of_term;
					sum = term;
					first = false;
				} else {
					if (!indices_consistent(free_indices, free_indices_of_term)) {
						std::ostringstream s;
						s << "simplify_indexed: inconsistent indices in sum: ";
						s << exprseq(free_indices) << " vs. " << exprseq(free_indices_of_term);
						throw (std::runtime_error(s.str()));
					}
					if (is_a<indexed>(sum) && is_a<indexed>(term))
						sum = ex_to<basic>(sum.op(0)).add_indexed(sum, term);
					else
						sum += term;
				}
			}
		}

		// If the sum turns out to be zero, we are finished
		if (sum.is_zero()) {
			free_indices.clear();
			return sum;
		}

		// More than one term and more than one dummy index?
		size_t num_terms_orig = (is_exactly_a<add>(sum) ? sum.nops() : 1);
		if (num_terms_orig < 2 || dummy_indices.size() < 2)
			return sum;

		// Yes, construct vector of all dummy index symbols
		exvector dummy_syms;
		dummy_syms.reserve(dummy_indices.size());
		for (exvector::const_iterator it = dummy_indices.begin(); it != dummy_indices.end(); ++it)
			dummy_syms.push_back(it->op(0));

		// Chop the sum into terms and symmetrize each one over the dummy
		// indices
		std::vector<terminfo> terms;
		for (size_t i=0; i<sum.nops(); i++) {
			const ex & term = sum.op(i);
			ex term_symm = symmetrize(term, dummy_syms);
			if (term_symm.is_zero())
				continue;
			terms.push_back(terminfo(term, term_symm));
		}

		// Sort by symmetrized terms
		std::sort(terms.begin(), terms.end(), terminfo_is_less());

		// Combine equal symmetrized terms
		std::vector<terminfo> terms_pass2;
		for (std::vector<terminfo>::const_iterator i=terms.begin(); i!=terms.end(); ) {
			size_t num = 1;
			std::vector<terminfo>::const_iterator j = i + 1;
			while (j != terms.end() && j->symm == i->symm) {
				num++;
				j++;
			}
			terms_pass2.push_back(terminfo(i->orig * num, i->symm * num));
			i = j;
		}

		// If there is only one term left, we are finished
		if (terms_pass2.size() == 1)
			return terms_pass2[0].orig;

		// Chop the symmetrized terms into subterms
		std::vector<symminfo> sy;
		for (std::vector<terminfo>::const_iterator i=terms_pass2.begin(); i!=terms_pass2.end(); ++i) {
			if (is_exactly_a<add>(i->symm)) {
				size_t num = i->symm.nops();
				for (size_t j=0; j<num; j++)
					sy.push_back(symminfo(i->symm.op(j), i->orig, num));
			} else
				sy.push_back(symminfo(i->symm, i->orig, 1));
		}

		// Sort by symmetrized subterms
		std::sort(sy.begin(), sy.end(), symminfo_is_less_by_symmterm());

		// Combine equal symmetrized subterms
		std::vector<symminfo> sy_pass2;
		exvector result;
		for (std::vector<symminfo>::const_iterator i=sy.begin(); i!=sy.end(); ) {

			// Combine equal terms
			std::vector<symminfo>::const_iterator j = i + 1;
			if (j != sy.end() && j->symmterm == i->symmterm) {

				// More than one term, collect the coefficients
				ex coeff = i->coeff;
				while (j != sy.end() && j->symmterm == i->symmterm) {
					coeff += j->coeff;
					j++;
				}

				// Add combined term to result
				if (!coeff.is_zero())
					result.push_back(coeff * i->symmterm);

			} else {

				// Single term, store for second pass
				sy_pass2.push_back(*i);
			}

			i = j;
		}

		// Were there any remaining terms that didn't get combined?
		if (sy_pass2.size() > 0) {

			// Yes, sort by their original terms
			std::sort(sy_pass2.begin(), sy_pass2.end(), symminfo_is_less_by_orig());

			for (std::vector<symminfo>::const_iterator i=sy_pass2.begin(); i!=sy_pass2.end(); ) {

				// How many symmetrized terms of this original term are left?
				size_t num = 1;
				std::vector<symminfo>::const_iterator j = i + 1;
				while (j != sy_pass2.end() && j->orig == i->orig) {
					num++;
					j++;
				}

				if (num == i->num) {

					// All terms left, then add the original term to the result
					result.push_back(i->orig);

				} else {

					// Some terms were combined with others, add up the remaining symmetrized terms
					std::vector<symminfo>::const_iterator k;
					for (k=i; k!=j; k++)
						result.push_back(k->coeff * k->symmterm);
				}

				i = j;
			}
		}

		// Add all resulting terms
		ex sum_symm = (new add(result))->setflag(status_flags::dynallocated);
		if (sum_symm.is_zero())
			free_indices.clear();
		return sum_symm;
	}

	// Simplification of products
	if (is_exactly_a<mul>(e_expanded)
	 || is_exactly_a<ncmul>(e_expanded)
	 || (is_exactly_a<power>(e_expanded) && is_a<indexed>(e_expanded.op(0)) && e_expanded.op(1).is_equal(_ex2)))
		return simplify_indexed_product(e_expanded, free_indices, dummy_indices, sp);

	// Cannot do anything
	free_indices.clear();
	return e_expanded;
}

/** Simplify/canonicalize expression containing indexed objects. This
 *  performs contraction of dummy indices where possible and checks whether
 *  the free indices in sums are consistent.
 *
 *  @param options Simplification options (currently unused)
 *  @return simplified expression */
ex ex::simplify_indexed(unsigned options) const
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
 *  @param options Simplification options (currently unused)
 *  @return simplified expression */
ex ex::simplify_indexed(const scalar_products & sp, unsigned options) const
{
	exvector free_indices, dummy_indices;
	return GiNaC::simplify_indexed(*this, free_indices, dummy_indices, sp);
}

/** Symmetrize expression over its free indices. */
ex ex::symmetrize() const
{
	return GiNaC::symmetrize(*this, get_free_indices());
}

/** Antisymmetrize expression over its free indices. */
ex ex::antisymmetrize() const
{
	return GiNaC::antisymmetrize(*this, get_free_indices());
}

/** Symmetrize expression by cyclic permutation over its free indices. */
ex ex::symmetrize_cyclic() const
{
	return GiNaC::symmetrize_cyclic(*this, get_free_indices());
}

//////////
// helper classes
//////////

spmapkey::spmapkey(const ex & v1_, const ex & v2_, const ex & dim_) : dim(dim_)
{
	// If indexed, extract base objects
	ex s1 = is_a<indexed>(v1_) ? v1_.op(0) : v1_;
	ex s2 = is_a<indexed>(v2_) ? v2_.op(0) : v2_;

	// Enforce canonical order in pair
	if (s1.compare(s2) > 0) {
		v1 = s2;
		v2 = s1;
	} else {
		v1 = s1;
		v2 = s2;
	}
}

bool spmapkey::operator==(const spmapkey &other) const
{
	if (!v1.is_equal(other.v1))
		return false;
	if (!v2.is_equal(other.v2))
		return false;
	if (is_a<wildcard>(dim) || is_a<wildcard>(other.dim))
		return true;
	else
		return dim.is_equal(other.dim);
}

bool spmapkey::operator<(const spmapkey &other) const
{
	int cmp = v1.compare(other.v1);
	if (cmp)
		return cmp < 0;
	cmp = v2.compare(other.v2);
	if (cmp)
		return cmp < 0;

	// Objects are equal, now check dimensions
	if (is_a<wildcard>(dim) || is_a<wildcard>(other.dim))
		return false;
	else
		return dim.compare(other.dim) < 0;
}

void spmapkey::debugprint() const
{
	std::cerr << "(" << v1 << "," << v2 << "," << dim << ")";
}

void scalar_products::add(const ex & v1, const ex & v2, const ex & sp)
{
	spm[spmapkey(v1, v2)] = sp;
}

void scalar_products::add(const ex & v1, const ex & v2, const ex & dim, const ex & sp)
{
	spm[spmapkey(v1, v2, dim)] = sp;
}

void scalar_products::add_vectors(const lst & l, const ex & dim)
{
	// Add all possible pairs of products
	for (lst::const_iterator it1 = l.begin(); it1 != l.end(); ++it1)
		for (lst::const_iterator it2 = l.begin(); it2 != l.end(); ++it2)
			add(*it1, *it2, *it1 * *it2);
}

void scalar_products::clear()
{
	spm.clear();
}

/** Check whether scalar product pair is defined. */
bool scalar_products::is_defined(const ex & v1, const ex & v2, const ex & dim) const
{
	return spm.find(spmapkey(v1, v2, dim)) != spm.end();
}

/** Return value of defined scalar product pair. */
ex scalar_products::evaluate(const ex & v1, const ex & v2, const ex & dim) const
{
	return spm.find(spmapkey(v1, v2, dim))->second;
}

void scalar_products::debugprint() const
{
	std::cerr << "map size=" << spm.size() << std::endl;
	spmap::const_iterator i = spm.begin(), end = spm.end();
	while (i != end) {
		const spmapkey & k = i->first;
		std::cerr << "item key=";
		k.debugprint();
		std::cerr << ", value=" << i->second << std::endl;
		++i;
	}
}

} // namespace GiNaC
