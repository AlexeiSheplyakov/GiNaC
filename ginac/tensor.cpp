/** @file tensor.cpp
 *
 *  Implementation of GiNaC's special tensors. */

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

#include "tensor.h"
#include "idx.h"
#include "indexed.h"
#include "relational.h"
#include "numeric.h"
#include "archive.h"
#include "utils.h"
#include "debugmsg.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(tensor, basic)
GINAC_IMPLEMENT_REGISTERED_CLASS(tensdelta, tensor)
GINAC_IMPLEMENT_REGISTERED_CLASS(tensmetric, tensor)
GINAC_IMPLEMENT_REGISTERED_CLASS(minkmetric, tensmetric)
GINAC_IMPLEMENT_REGISTERED_CLASS(tensepsilon, tensor)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

#define DEFAULT_CTORS(classname) \
classname::classname() : inherited(TINFO_##classname) \
{ \
	debugmsg(#classname " default constructor", LOGLEVEL_CONSTRUCT); \
} \
void classname::copy(const classname & other) \
{ \
	inherited::copy(other); \
} \
void classname::destroy(bool call_parent) \
{ \
	if (call_parent) \
		inherited::destroy(call_parent); \
}

tensor::tensor(unsigned ti) : inherited(ti)
{
	debugmsg("tensor constructor from unsigned", LOGLEVEL_CONSTRUCT); \
}

DEFAULT_CTORS(tensor)
DEFAULT_CTORS(tensdelta)
DEFAULT_CTORS(tensmetric)
DEFAULT_CTORS(tensepsilon)

minkmetric::minkmetric() : pos_sig(false)
{
	debugmsg("minkmetric default constructor", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_minkmetric;
}

minkmetric::minkmetric(bool ps) : pos_sig(ps)
{
	debugmsg("minkmetric constructor from bool", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_minkmetric;
}

void minkmetric::copy(const minkmetric & other)
{
	inherited::copy(other);
	pos_sig = other.pos_sig;
}

void minkmetric::destroy(bool call_parent)
{
	if (call_parent)
		inherited::destroy(call_parent);
}

//////////
// archiving
//////////

#define DEFAULT_ARCHIVING(classname) \
classname::classname(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst) \
{ \
	debugmsg(#classname " constructor from archive_node", LOGLEVEL_CONSTRUCT); \
} \
ex classname::unarchive(const archive_node &n, const lst &sym_lst) \
{ \
	return (new classname(n, sym_lst))->setflag(status_flags::dynallocated); \
} \
void classname::archive(archive_node &n) const \
{ \
	inherited::archive(n); \
}

DEFAULT_ARCHIVING(tensor)
DEFAULT_ARCHIVING(tensdelta)
DEFAULT_ARCHIVING(tensmetric)
DEFAULT_ARCHIVING(tensepsilon)

minkmetric::minkmetric(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("minkmetric constructor from archive_node", LOGLEVEL_CONSTRUCT);
	n.find_bool("pos_sig", pos_sig);
}

ex minkmetric::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new minkmetric(n, sym_lst))->setflag(status_flags::dynallocated);
}

void minkmetric::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_bool("pos_sig", pos_sig);
}

//////////
// functions overriding virtual functions from bases classes
//////////

#define DEFAULT_COMPARE(classname) \
int classname::compare_same_type(const basic & other) const \
{ \
	/* by default, two tensors of the same class are always identical */ \
	return 0; \
}

DEFAULT_COMPARE(tensor)
DEFAULT_COMPARE(tensdelta)
DEFAULT_COMPARE(tensmetric)
DEFAULT_COMPARE(tensepsilon)

int minkmetric::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other, minkmetric));
	const minkmetric &o = static_cast<const minkmetric &>(other);

	if (pos_sig != o.pos_sig)
		return pos_sig ? -1 : 1;
	else
		return inherited::compare_same_type(other);
}

void tensdelta::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("tensdelta print",LOGLEVEL_PRINT);
	os << "delta";
}

void tensmetric::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("tensmetric print",LOGLEVEL_PRINT);
	os << "g";
}

void minkmetric::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("minkmetric print",LOGLEVEL_PRINT);
	os << "eta";
}

void tensepsilon::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("tensepsilon print",LOGLEVEL_PRINT);
	os << "eps";
}

/** Automatic symbolic evaluation of an indexed delta tensor. */
ex tensdelta::eval_indexed(const basic & i) const
{
	GINAC_ASSERT(is_of_type(i, indexed));
	GINAC_ASSERT(i.nops() == 3);
	GINAC_ASSERT(is_ex_of_type(i.op(0), tensdelta));

	const idx & i1 = ex_to_idx(i.op(1));
	const idx & i2 = ex_to_idx(i.op(2));

	// Trace of delta tensor is the dimension of the space
	if (is_dummy_pair(i1, i2))
		return i1.get_dim();

	// No further simplifications
	return i.hold();
}

/** Automatic symbolic evaluation of an indexed metric tensor. */
ex tensmetric::eval_indexed(const basic & i) const
{
	GINAC_ASSERT(is_of_type(i, indexed));
	GINAC_ASSERT(i.nops() == 3);
	GINAC_ASSERT(is_ex_of_type(i.op(0), tensmetric));
	GINAC_ASSERT(is_ex_of_type(i.op(1), varidx));
	GINAC_ASSERT(is_ex_of_type(i.op(2), varidx));

	const varidx & i1 = ex_to_varidx(i.op(1));
	const varidx & i2 = ex_to_varidx(i.op(2));

	// A metric tensor with one covariant and one contravariant index gets
	// replaced by a delta tensor
	if (i1.is_covariant() != i2.is_covariant())
		return delta_tensor(i1, i2);

	// No further simplifications
	return i.hold();
}

/** Automatic symbolic evaluation of an indexed Lorentz metric tensor. */
ex minkmetric::eval_indexed(const basic & i) const
{
	GINAC_ASSERT(is_of_type(i, indexed));
	GINAC_ASSERT(i.nops() == 3);
	GINAC_ASSERT(is_ex_of_type(i.op(0), minkmetric));
	GINAC_ASSERT(is_ex_of_type(i.op(1), varidx));
	GINAC_ASSERT(is_ex_of_type(i.op(2), varidx));

	const varidx & i1 = ex_to_varidx(i.op(1));
	const varidx & i2 = ex_to_varidx(i.op(2));

	// Numeric evaluation
	if (static_cast<const indexed &>(i).all_index_values_are(info_flags::nonnegint)) {
		int n1 = ex_to_numeric(i1.get_value()).to_int(), n2 = ex_to_numeric(i2.get_value()).to_int();
		if (n1 != n2)
			return _ex0();
		else if (n1 == 0)
			return pos_sig ? _ex_1() : _ex1();
		else
			return pos_sig ? _ex1() : _ex_1();
	}

	// Perform the usual evaluations of a metric tensor
	return inherited::eval_indexed(i);
}

/** Contraction of an indexed delta tensor with something else. */
bool tensdelta::contract_with(ex & self, ex & other) const
{
	GINAC_ASSERT(is_ex_of_type(self, indexed));
	GINAC_ASSERT(is_ex_of_type(other, indexed));
	GINAC_ASSERT(self.nops() == 3);
	GINAC_ASSERT(is_ex_of_type(self.op(0), tensdelta));

	// Try to contract first index
	const idx *self_idx = &ex_to_idx(self.op(1));
	const idx *free_idx = &ex_to_idx(self.op(2));
	bool first_index_tried = false;

again:
	if (self_idx->is_symbolic()) {
		for (int i=1; i<other.nops(); i++) {
			const idx &other_idx = ex_to_idx(other.op(i));
			if (is_dummy_pair(*self_idx, other_idx)) {

				// Contraction found, remove delta tensor and substitute
				// index in second object
				self = _ex1();
				other = other.subs(other_idx == *free_idx);
				return true;
			}
		}
	}

	if (!first_index_tried) {

		// No contraction with first index found, try second index
		self_idx = &ex_to_idx(self.op(2));
		free_idx = &ex_to_idx(self.op(1));
		first_index_tried = true;
		goto again;
	}

	return false;
}

/** Contraction of an indexed metric tensor with something else. */
bool tensmetric::contract_with(ex & self, ex & other) const
{
	GINAC_ASSERT(is_ex_of_type(self, indexed));
	GINAC_ASSERT(is_ex_of_type(other, indexed));
	GINAC_ASSERT(self.nops() == 3);
	GINAC_ASSERT(is_ex_of_type(self.op(0), tensmetric));

	// If contracting with the delta tensor, let the delta do it
	// (don't raise/lower delta indices)
	if (is_ex_exactly_of_type(other.op(0), tensdelta))
		return false;

	// Try to contract first index
	const idx *self_idx = &ex_to_idx(self.op(1));
	const idx *free_idx = &ex_to_idx(self.op(2));
	bool first_index_tried = false;

again:
	if (self_idx->is_symbolic()) {
		for (int i=1; i<other.nops(); i++) {
			const idx &other_idx = ex_to_idx(other.op(i));
			if (is_dummy_pair(*self_idx, other_idx)) {

				// Contraction found, remove metric tensor and substitute
				// index in second object
				self = _ex1();
				other = other.subs(other_idx == *free_idx);
				return true;
			}
		}
	}

	if (!first_index_tried) {

		// No contraction with first index found, try second index
		self_idx = &ex_to_idx(self.op(2));
		free_idx = &ex_to_idx(self.op(1));
		first_index_tried = true;
		goto again;
	}

	return false;
}

//////////
// global functions
//////////

ex delta_tensor(const ex & i1, const ex & i2)
{
	if (!is_ex_of_type(i1, idx) || !is_ex_of_type(i2, idx))
		throw(std::invalid_argument("indices of delta tensor must be of type idx"));

	return indexed(tensdelta(), indexed::symmetric, i1, i2);
}

ex metric_tensor(const ex & i1, const ex & i2)
{
	if (!is_ex_of_type(i1, varidx) || !is_ex_of_type(i2, varidx))
		throw(std::invalid_argument("indices of metric tensor must be of type varidx"));

	return indexed(tensmetric(), i1, i2);
}

ex lorentz_g(const ex & i1, const ex & i2, bool pos_sig)
{
	if (!is_ex_of_type(i1, varidx) || !is_ex_of_type(i2, varidx))
		throw(std::invalid_argument("indices of metric tensor must be of type varidx"));

	return indexed(minkmetric(pos_sig), indexed::symmetric, i1, i2);
}

ex epsilon_tensor(const ex & i1, const ex & i2)
{
	if (!is_ex_of_type(i1, idx) || !is_ex_of_type(i2, idx))
		throw(std::invalid_argument("indices of epsilon tensor must be of type idx"));
	if (!ex_to_idx(i1).get_dim().is_equal(_ex2()) || !ex_to_idx(i2).get_dim().is_equal(_ex2()))
		throw(std::invalid_argument("index dimension of epsilon tensor must match number of indices"));

	return indexed(tensepsilon(), indexed::antisymmetric, i1, i2);
}

} // namespace GiNaC
