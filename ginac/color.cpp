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

#include "color.h"
#include "ex.h"
#include "ncmul.h"
#include "numeric.h"
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
	//!! to be implemented
	return nonsimplified_ncmul(v);
}

ex color::thisexprseq(const exvector & v) const
{
	return color(representation_label, v);
}

ex color::thisexprseq(exvector * vp) const
{
	return color(representation_label, vp);
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
	return color(su3t(), a, rl);
}

ex color_f(const ex & a, const ex & b, const ex & c)
{
	return indexed(su3f(), indexed::antisymmetric, a, b, c);
}

ex color_d(const ex & a, const ex & b, const ex & c)
{
	return indexed(su3d(), indexed::symmetric, a, b, c);
}

ex color_h(const ex & a, const ex & b, const ex & c)
{
	return color_d(a, b, c) + I * color_f(a, b, c);
}

} // namespace GiNaC
