/** @file idx.cpp
 *
 *  Implementation of GiNaC's indices. */

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
#include <algorithm>

#include "idx.h"
#include "symbol.h"
#include "lst.h"
#include "print.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(idx, basic)
GINAC_IMPLEMENT_REGISTERED_CLASS(varidx, idx)
GINAC_IMPLEMENT_REGISTERED_CLASS(spinidx, varidx)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

idx::idx() : inherited(TINFO_idx)
{
	debugmsg("idx default constructor", LOGLEVEL_CONSTRUCT);
}

varidx::varidx() : covariant(false)
{
	debugmsg("varidx default constructor", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_varidx;
}

spinidx::spinidx() : dotted(false)
{
	debugmsg("spinidx default constructor", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_spinidx;
}

void idx::copy(const idx & other)
{
	inherited::copy(other);
	value = other.value;
	dim = other.dim;
}

void varidx::copy(const varidx & other)
{
	inherited::copy(other);
	covariant = other.covariant;
}

void spinidx::copy(const spinidx & other)
{
	inherited::copy(other);
	dotted = other.dotted;
}

DEFAULT_DESTROY(idx)
DEFAULT_DESTROY(varidx)
DEFAULT_DESTROY(spinidx)

//////////
// other constructors
//////////

idx::idx(const ex & v, const ex & d) : inherited(TINFO_idx), value(v), dim(d)
{
	debugmsg("idx constructor from ex,ex", LOGLEVEL_CONSTRUCT);
	if (is_dim_numeric())
		if (!dim.info(info_flags::posint))
			throw(std::invalid_argument("dimension of space must be a positive integer"));
}

varidx::varidx(const ex & v, const ex & d, bool cov) : inherited(v, d), covariant(cov)
{
	debugmsg("varidx constructor from ex,ex,bool", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_varidx;
}

spinidx::spinidx(const ex & v, const ex & d, bool cov, bool dot) : inherited(v, d, cov), dotted(dot)
{
	debugmsg("spinidx constructor from ex,ex,bool,bool", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_spinidx;
}

//////////
// archiving
//////////

idx::idx(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("idx constructor from archive_node", LOGLEVEL_CONSTRUCT);
	n.find_ex("value", value, sym_lst);
	n.find_ex("dim", dim, sym_lst);
}

varidx::varidx(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("varidx constructor from archive_node", LOGLEVEL_CONSTRUCT);
	n.find_bool("covariant", covariant);
}

spinidx::spinidx(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("spinidx constructor from archive_node", LOGLEVEL_CONSTRUCT);
	n.find_bool("dotted", dotted);
}

void idx::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_ex("value", value);
	n.add_ex("dim", dim);
}

void varidx::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_bool("covariant", covariant);
}

void spinidx::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_bool("dotted", dotted);
}

DEFAULT_UNARCHIVE(idx)
DEFAULT_UNARCHIVE(varidx)
DEFAULT_UNARCHIVE(spinidx)

//////////
// functions overriding virtual functions from bases classes
//////////

void idx::print(const print_context & c, unsigned level) const
{
	debugmsg("idx print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		value.print(c, level + delta_indent);
		dim.print(c, level + delta_indent);

	} else {

		if (!is_of_type(c, print_latex))
			c.s << ".";
		bool need_parens = !(is_ex_exactly_of_type(value, numeric) || is_ex_of_type(value, symbol));
		if (need_parens)
			c.s << "(";
		value.print(c);
		if (need_parens)
			c.s << ")";
	}
}

void varidx::print(const print_context & c, unsigned level) const
{
	debugmsg("varidx print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << (covariant ? ", covariant" : ", contravariant")
		    << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		value.print(c, level + delta_indent);
		dim.print(c, level + delta_indent);

	} else {

		if (!is_of_type(c, print_latex)) {
			if (covariant)
				c.s << ".";
			else
				c.s << "~";
		}
		bool need_parens = !(is_ex_exactly_of_type(value, numeric) || is_ex_of_type(value, symbol));
		if (need_parens)
			c.s << "(";
		value.print(c);
		if (need_parens)
			c.s << ")";
	}
}

void spinidx::print(const print_context & c, unsigned level) const
{
	debugmsg("spinidx print", LOGLEVEL_PRINT);

	if (is_of_type(c, print_tree)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << (covariant ? ", covariant" : ", contravariant")
		    << (dotted ? ", dotted" : ", undotted")
		    << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		value.print(c, level + delta_indent);
		dim.print(c, level + delta_indent);

	} else {

		bool is_tex = is_of_type(c, print_latex);
		if (!is_tex) {
			if (covariant)
				c.s << ".";
			else
				c.s << "~";
		}
		if (dotted) {
			if (is_tex)
				c.s << "\\dot{";
			else
				c.s << "*";
		}
		bool need_parens = !(is_ex_exactly_of_type(value, numeric) || is_ex_of_type(value, symbol));
		if (need_parens)
			c.s << "(";
		value.print(c);
		if (need_parens)
			c.s << ")";
		if (is_tex && dotted)
			c.s << "}";
	}
}

bool idx::info(unsigned inf) const
{
	if (inf == info_flags::idx)
		return true;
	return inherited::info(inf);
}

unsigned idx::nops() const
{
	// don't count the dimension as that is not really a sub-expression
	return 1;
}

ex & idx::let_op(int i)
{
	GINAC_ASSERT(i == 0);
	return value;
}

/** Returns order relation between two indices of the same type. The order
 *  must be such that dummy indices lie next to each other. */
int idx::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, idx));
	const idx &o = static_cast<const idx &>(other);

	int cmpval = value.compare(o.value);
	if (cmpval)
		return cmpval;
	return dim.compare(o.dim);
}

int varidx::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, varidx));
	const varidx &o = static_cast<const varidx &>(other);

	int cmpval = inherited::compare_same_type(other);
	if (cmpval)
		return cmpval;

	// Check variance last so dummy indices will end up next to each other
	if (covariant != o.covariant)
		return covariant ? -1 : 1;
	return 0;
}

int spinidx::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, spinidx));
	const spinidx &o = static_cast<const spinidx &>(other);

	// Check dottedness first so dummy indices will end up next to each other
	if (dotted != o.dotted)
		return dotted ? -1 : 1;

	int cmpval = inherited::compare_same_type(other);
	if (cmpval)
		return cmpval;

	return 0;
}

bool idx::match(const ex & pattern, lst & repl_lst) const
{
	if (!is_ex_of_type(pattern, idx))
		return false;
	const idx &o = ex_to_idx(pattern);
	if (!dim.is_equal(o.dim))
		return false;
	return value.match(o.value, repl_lst);
}

bool varidx::match(const ex & pattern, lst & repl_lst) const
{
	if (!is_ex_of_type(pattern, varidx))
		return false;
	const varidx &o = ex_to_varidx(pattern);
	if (covariant != o.covariant)
		return false;
	return inherited::match(pattern, repl_lst);
}

bool spinidx::match(const ex & pattern, lst & repl_lst) const
{
	if (!is_ex_of_type(pattern, spinidx))
		return false;
	const spinidx &o = ex_to_spinidx(pattern);
	if (dotted != o.dotted)
		return false;
	return inherited::match(pattern, repl_lst);
}

ex idx::subs(const lst & ls, const lst & lr, bool no_pattern) const
{
	GINAC_ASSERT(ls.nops() == lr.nops());

	// First look for index substitutions
	for (unsigned i=0; i<ls.nops(); i++) {
		if (is_equal(*(ls.op(i)).bp)) {

			// Substitution index->index
			if (is_ex_of_type(lr.op(i), idx))
				return lr.op(i);

			// Otherwise substitute value
			idx *i_copy = static_cast<idx *>(duplicate());
			i_copy->value = lr.op(i);
			i_copy->clearflag(status_flags::hash_calculated);
			return i_copy->setflag(status_flags::dynallocated);
		}
	}

	// None, substitute objects in value (not in dimension)
	const ex &subsed_value = value.subs(ls, lr, no_pattern);
	if (are_ex_trivially_equal(value, subsed_value))
		return *this;

	idx *i_copy = static_cast<idx *>(duplicate());
	i_copy->value = subsed_value;
	i_copy->clearflag(status_flags::hash_calculated);
	return i_copy->setflag(status_flags::dynallocated);
}

//////////
// new virtual functions
//////////

bool idx::is_dummy_pair_same_type(const basic & other) const
{
	const idx &o = static_cast<const idx &>(other);

	// Only pure symbols form dummy pairs, "2n+1" doesn't
	if (!is_ex_of_type(value, symbol))
		return false;

	// Value must be equal, of course
	if (!value.is_equal(o.value))
		return false;

	// Also the dimension
	return dim.is_equal(o.dim);
}

bool varidx::is_dummy_pair_same_type(const basic & other) const
{
	const varidx &o = static_cast<const varidx &>(other);

	// Variance must be opposite
	if (covariant == o.covariant)
		return false;

	return inherited::is_dummy_pair_same_type(other);
}

bool spinidx::is_dummy_pair_same_type(const basic & other) const
{
	const spinidx &o = static_cast<const spinidx &>(other);

	// Dottedness must be the same
	if (dotted != o.dotted)
		return false;

	return inherited::is_dummy_pair_same_type(other);
}


//////////
// non-virtual functions
//////////

ex varidx::toggle_variance(void) const
{
	varidx *i_copy = static_cast<varidx *>(duplicate());
	i_copy->covariant = !i_copy->covariant;
	i_copy->clearflag(status_flags::hash_calculated);
	return i_copy->setflag(status_flags::dynallocated);
}

ex spinidx::toggle_dot(void) const
{
	spinidx *i_copy = static_cast<spinidx *>(duplicate());
	i_copy->dotted = !i_copy->dotted;
	i_copy->clearflag(status_flags::hash_calculated);
	return i_copy->setflag(status_flags::dynallocated);
}

ex spinidx::toggle_variance_dot(void) const
{
	spinidx *i_copy = static_cast<spinidx *>(duplicate());
	i_copy->covariant = !i_copy->covariant;
	i_copy->dotted = !i_copy->dotted;
	i_copy->clearflag(status_flags::hash_calculated);
	return i_copy->setflag(status_flags::dynallocated);
}

//////////
// global functions
//////////

bool is_dummy_pair(const idx & i1, const idx & i2)
{
	// The indices must be of exactly the same type
	if (i1.tinfo() != i2.tinfo())
		return false;

	// Same type, let the indices decide whether they are paired
	return i1.is_dummy_pair_same_type(i2);
}

bool is_dummy_pair(const ex & e1, const ex & e2)
{
	// The expressions must be indices
	if (!is_ex_of_type(e1, idx) || !is_ex_of_type(e2, idx))
		return false;

	return is_dummy_pair(ex_to_idx(e1), ex_to_idx(e2));
}

void find_free_and_dummy(exvector::const_iterator it, exvector::const_iterator itend, exvector & out_free, exvector & out_dummy)
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
	shaker_sort(v.begin(), v.end(), ex_is_less());

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

} // namespace GiNaC
