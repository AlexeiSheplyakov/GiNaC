/** @file color.cpp
 *
 *  Implementation of GiNaC's color (SU(3) Lie algebra) objects. */

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

#include <algorithm>
#include <stdexcept>

#include "color.h"
#include "ex.h"
#include "idx.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h" // for sqrt()
#include "print.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(color, indexed)
GINAC_IMPLEMENT_REGISTERED_CLASS(su3one, tensor)
GINAC_IMPLEMENT_REGISTERED_CLASS(su3t, tensor)
GINAC_IMPLEMENT_REGISTERED_CLASS(su3f, tensor)
GINAC_IMPLEMENT_REGISTERED_CLASS(su3d, tensor)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

color::color() : representation_label(0)
{
	debugmsg("color default constructor", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_color;
}

void color::copy(const color & other)
{
	inherited::copy(other);
	representation_label = other.representation_label;
}

DEFAULT_DESTROY(color)
DEFAULT_CTORS(su3one)
DEFAULT_CTORS(su3t)
DEFAULT_CTORS(su3f)
DEFAULT_CTORS(su3d)

//////////
// other constructors
//////////

/** Construct object without any color index. This constructor is for
 *  internal use only. Use the color_ONE() function instead.
 *  @see color_ONE */
color::color(const ex & b, unsigned rl = 0) : inherited(b), representation_label(rl)
{
	debugmsg("color constructor from ex,unsigned", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_color;
}

/** Construct object with one color index. This constructor is for internal
 *  use only. Use the color_T() function instead.
 *  @see color_T */
color::color(const ex & b, const ex & i1, unsigned rl = 0) : inherited(b, i1), representation_label(rl)
{
	debugmsg("color constructor from ex,ex,unsigned", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_color;
}

color::color(unsigned rl, const exvector & v, bool discardable) : inherited(indexed::unknown, v, discardable), representation_label(rl)
{
	debugmsg("color constructor from unsigned,exvector", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_color;
}

color::color(unsigned rl, exvector * vp) : inherited(indexed::unknown, vp), representation_label(rl)
{
	debugmsg("color constructor from unsigned,exvector *", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_color;
}

//////////
// archiving
//////////

color::color(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	debugmsg("color constructor from archive_node", LOGLEVEL_CONSTRUCT);
	n.find_unsigned("representation", representation_label);
}

void color::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_unsigned("representation", representation_label);
}

DEFAULT_UNARCHIVE(color)
DEFAULT_ARCHIVING(su3one)
DEFAULT_ARCHIVING(su3t)
DEFAULT_ARCHIVING(su3f)
DEFAULT_ARCHIVING(su3d)

//////////
// functions overriding virtual functions from bases classes
//////////

int color::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(other.tinfo() == TINFO_color);
	const color &o = static_cast<const color &>(other);

	if (representation_label != o.representation_label) {
		// different representation label
		return representation_label < o.representation_label ? -1 : 1;
	}

	return inherited::compare_same_type(other);
}

DEFAULT_COMPARE(su3one)
DEFAULT_COMPARE(su3t)
DEFAULT_COMPARE(su3f)
DEFAULT_COMPARE(su3d)

DEFAULT_PRINT(su3one, "ONE")
DEFAULT_PRINT(su3t, "T")
DEFAULT_PRINT(su3f, "f")
DEFAULT_PRINT(su3d, "d")

/** Perform automatic simplification on noncommutative product of color
 *  objects. This removes superfluous ONEs. */
ex color::simplify_ncmul(const exvector & v) const
{
	//!! TODO: sort by representation label
	exvector s;
	s.reserve(v.size());

	exvector::const_iterator it = v.begin(), itend = v.end();
	while (it != itend) {
		if (!is_ex_of_type(it->op(0), su3one))
			s.push_back(*it);
		it++;
	}

	if (s.size() == 0)
		return color(su3one());
	else if (s.size() == v.size())
		return simplified_ncmul(v);
	else
		return simplified_ncmul(s);
}

ex color::thisexprseq(const exvector & v) const
{
	return color(representation_label, v);
}

ex color::thisexprseq(exvector * vp) const
{
	return color(representation_label, vp);
}

/** Given a vector iv3 of three indices and a vector iv2 of two indices that
 *  is a subset of iv3, return the (free) index that is in iv3 but not in
 *  iv2 and the sign introduced by permuting that index to the front.
 *
 *  @param iv3 Vector of 3 indices
 *  @param iv2 Vector of 2 indices, must be a subset of iv3
 *  @param sig Returs sign introduced by index permutation
 *  @return the free index (the one that is in iv3 but not in iv2) */
static ex permute_free_index_to_front(const exvector & iv3, const exvector & iv2, int & sig)
{
	GINAC_ASSERT(iv3.size() == 3);
	GINAC_ASSERT(iv2.size() == 2);

	sig = 1;

#define TEST_PERMUTATION(A,B,C,P) \
	if (iv3[B].is_equal(iv2[0]) && iv3[C].is_equal(iv2[1])) { \
		sig = P; \
		return iv3[A]; \
	}
	
	TEST_PERMUTATION(0,1,2,  1);
	TEST_PERMUTATION(0,2,1, -1);
	TEST_PERMUTATION(1,0,2, -1);
	TEST_PERMUTATION(1,2,0,  1);
	TEST_PERMUTATION(2,0,1,  1);
	TEST_PERMUTATION(2,1,0, -1);

	throw(std::logic_error("permute_free_index_to_front(): no valid permutation found"));
}

/** Automatic symbolic evaluation of indexed symmetric structure constant. */
ex su3d::eval_indexed(const basic & i) const
{
	GINAC_ASSERT(is_of_type(i, indexed));
	GINAC_ASSERT(i.nops() == 4);
	GINAC_ASSERT(is_ex_of_type(i.op(0), su3d));

	// Convolutions are zero
	if (static_cast<const indexed &>(i).get_dummy_indices().size() != 0)
		return _ex0();

	// Numeric evaluation
	if (static_cast<const indexed &>(i).all_index_values_are(info_flags::nonnegint)) {

		// Sort indices
		int v[3];
		for (unsigned j=0; j<3; j++)
			v[j] = ex_to_numeric(ex_to_idx(i.op(j + 1)).get_value()).to_int();
		if (v[0] > v[1]) std::swap(v[0], v[1]);
		if (v[0] > v[2]) std::swap(v[0], v[2]);
		if (v[1] > v[2]) std::swap(v[1], v[2]);

#define CMPINDICES(A,B,C) ((v[0] == (A)) && (v[1] == (B)) && (v[2] == (C)))

		// Check for non-zero elements
		if (CMPINDICES(1,4,6) || CMPINDICES(1,5,7) || CMPINDICES(2,5,6)
		 || CMPINDICES(3,4,4) || CMPINDICES(3,5,5))
			return _ex1_2();
		else if (CMPINDICES(2,4,7) || CMPINDICES(3,6,6) || CMPINDICES(3,7,7))
			return _ex_1_2();
		else if (CMPINDICES(1,1,8) || CMPINDICES(2,2,8) || CMPINDICES(3,3,8))
			return sqrt(_ex3())/3;
		else if (CMPINDICES(8,8,8))
			return -sqrt(_ex3())/3;
		else if (CMPINDICES(4,4,8) || CMPINDICES(5,5,8)
		      || CMPINDICES(6,6,8) || CMPINDICES(7,7,8))
			return -sqrt(_ex3())/6;
		else
			return _ex0();
	}

	// No further simplifications
	return i.hold();
}

/** Automatic symbolic evaluation of indexed antisymmetric structure constant. */
ex su3f::eval_indexed(const basic & i) const
{
	GINAC_ASSERT(is_of_type(i, indexed));
	GINAC_ASSERT(i.nops() == 4);
	GINAC_ASSERT(is_ex_of_type(i.op(0), su3f));

	// Numeric evaluation
	if (static_cast<const indexed &>(i).all_index_values_are(info_flags::nonnegint)) {

		// Sort indices, remember permutation sign
		int v[3];
		for (unsigned j=0; j<3; j++)
			v[j] = ex_to_numeric(ex_to_idx(i.op(j + 1)).get_value()).to_int();
		int sign = 1;
		if (v[0] > v[1]) { std::swap(v[0], v[1]); sign = -sign; }
		if (v[0] > v[2]) { std::swap(v[0], v[2]); sign = -sign; }
		if (v[1] > v[2]) { std::swap(v[1], v[2]); sign = -sign; }

		// Check for non-zero elements
		if (CMPINDICES(1,2,3))
			return sign;
		else if (CMPINDICES(1,4,7) || CMPINDICES(2,4,6)
		      || CMPINDICES(2,5,7) || CMPINDICES(3,4,5))
			return _ex1_2() * sign;
		else if (CMPINDICES(1,5,6) || CMPINDICES(3,6,7))
			return _ex_1_2() * sign;
		else if (CMPINDICES(4,5,8) || CMPINDICES(6,7,8))
			return sqrt(_ex3())/2 * sign;
		else
			return _ex0();
	}

	// No further simplifications
	return i.hold();
}


/** Contraction of an indexed symmetric structure constant with something else. */
bool su3d::contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const
{
	GINAC_ASSERT(is_ex_of_type(*self, indexed));
	GINAC_ASSERT(is_ex_of_type(*other, indexed));
	GINAC_ASSERT(self->nops() == 4);
	GINAC_ASSERT(is_ex_of_type(self->op(0), su3d));

	if (is_ex_exactly_of_type(other->op(0), su3d)) {

		// Find the dummy indices of the contraction
		exvector dummy_indices;
		dummy_indices = ex_to_indexed(*self).get_dummy_indices(ex_to_indexed(*other));

		// d.abc*d.abc=40/3
		if (dummy_indices.size() == 3) {
			*self = numeric(40, 3);
			*other = _ex1();
			return true;

		// d.akl*d.bkl=5/3*delta.ab
		} else if (dummy_indices.size() == 2) {
			exvector a = index_set_difference(ex_to_indexed(*self).get_indices(), dummy_indices);
			exvector b = index_set_difference(ex_to_indexed(*other).get_indices(), dummy_indices);
			GINAC_ASSERT(a.size() > 0);
			GINAC_ASSERT(b.size() > 0);
			*self = numeric(5, 3) * delta_tensor(a[0], b[0]);
			*other = _ex1();
			return true;
		}
	}

	return false;
}

/** Contraction of an indexed antisymmetric structure constant with something else. */
bool su3f::contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const
{
	GINAC_ASSERT(is_ex_of_type(*self, indexed));
	GINAC_ASSERT(is_ex_of_type(*other, indexed));
	GINAC_ASSERT(self->nops() == 4);
	GINAC_ASSERT(is_ex_of_type(self->op(0), su3f));

	if (is_ex_exactly_of_type(other->op(0), su3f)) { // f*d is handled by su3d class

		// Find the dummy indices of the contraction
		exvector dummy_indices;
		dummy_indices = ex_to_indexed(*self).get_dummy_indices(ex_to_indexed(*other));

		// f.abc*f.abc=24
		if (dummy_indices.size() == 3) {
			*self = 24;
			*other = _ex1();
			return true;

		// f.akl*f.bkl=3*delta.ab
		} else if (dummy_indices.size() == 2) {
			int sign1, sign2;
			ex a = permute_free_index_to_front(ex_to_indexed(*self).get_indices(), dummy_indices, sign1);
			ex b = permute_free_index_to_front(ex_to_indexed(*other).get_indices(), dummy_indices, sign2);
			*self = sign1 * sign2 * 3 * delta_tensor(a, b);
			*other = _ex1();
			return true;
		}
	}

	return false;
}

//////////
// global functions
//////////

ex color_ONE(unsigned rl)
{
	return color(su3one(), rl);
}

ex color_T(const ex & a, unsigned rl)
{
	if (!is_ex_of_type(a, idx))
		throw(std::invalid_argument("indices of color_T must be of type idx"));
	if (!ex_to_idx(a).get_dim().is_equal(8))
		throw(std::invalid_argument("index dimension for color_T must be 8"));

	return color(su3t(), a, rl);
}

ex color_f(const ex & a, const ex & b, const ex & c)
{
	if (!is_ex_of_type(a, idx) || !is_ex_of_type(b, idx) || !is_ex_of_type(c, idx))
		throw(std::invalid_argument("indices of color_f must be of type idx"));
	if (!ex_to_idx(a).get_dim().is_equal(8) || !ex_to_idx(b).get_dim().is_equal(8) || !ex_to_idx(c).get_dim().is_equal(8))
		throw(std::invalid_argument("index dimension for color_f must be 8"));

	return indexed(su3f(), indexed::antisymmetric, a, b, c);
}

ex color_d(const ex & a, const ex & b, const ex & c)
{
	if (!is_ex_of_type(a, idx) || !is_ex_of_type(b, idx) || !is_ex_of_type(c, idx))
		throw(std::invalid_argument("indices of color_d must be of type idx"));
	if (!ex_to_idx(a).get_dim().is_equal(8) || !ex_to_idx(b).get_dim().is_equal(8) || !ex_to_idx(c).get_dim().is_equal(8))
		throw(std::invalid_argument("index dimension for color_d must be 8"));

	return indexed(su3d(), indexed::symmetric, a, b, c);
}

ex color_h(const ex & a, const ex & b, const ex & c)
{
	return color_d(a, b, c) + I * color_f(a, b, c);
}

} // namespace GiNaC
