/** @file clifford.cpp
 *
 *  Implementation of GiNaC's clifford algebra (Dirac gamma) objects. */

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

#include "clifford.h"
#include "ex.h"
#include "idx.h"
#include "ncmul.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

#include <stdexcept>

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(clifford, indexed)
GINAC_IMPLEMENT_REGISTERED_CLASS(diracgamma, tensor)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

clifford::clifford()
{
	debugmsg("clifford default constructor", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_clifford;
}

DEFAULT_COPY(clifford)
DEFAULT_DESTROY(clifford)
DEFAULT_CTORS(diracgamma)

//////////
// other constructors
//////////

/** Construct object with one Lorentz index. This constructor is for internal
 *  use only. Use the dirac_gamma() function instead.
 *  @see dirac_gamma */
clifford::clifford(const ex & b, const ex & mu) : inherited(b, mu)
{
	debugmsg("clifford constructor from ex,ex", LOGLEVEL_CONSTRUCT);
	GINAC_ASSERT(is_ex_of_type(mu, varidx));
	tinfo_key = TINFO_clifford;
}

clifford::clifford(const exvector & v, bool discardable) : inherited(indexed::unknown, v, discardable)
{
	debugmsg("clifford constructor from exvector", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_clifford;
}

clifford::clifford(exvector * vp) : inherited(indexed::unknown, vp)
{
	debugmsg("clifford constructor from exvector *", LOGLEVEL_CONSTRUCT);
	tinfo_key = TINFO_clifford;
}

//////////
// archiving
//////////

DEFAULT_ARCHIVING(clifford)
DEFAULT_ARCHIVING(diracgamma)

//////////
// functions overriding virtual functions from bases classes
//////////

int clifford::compare_same_type(const basic & other) const
{
	return inherited::compare_same_type(other);
}

DEFAULT_COMPARE(diracgamma)
DEFAULT_PRINT(diracgamma, "gamma")

/** Perform automatic simplification on noncommutative product of clifford
 *  objects. */
ex clifford::simplify_ncmul(const exvector & v) const
{
	//!! to be implemented
	return nonsimplified_ncmul(v);
}

ex clifford::thisexprseq(const exvector & v) const
{
	return clifford(v);
}

ex clifford::thisexprseq(exvector * vp) const
{
	return clifford(vp);
}

//////////
// global functions
//////////

ex dirac_gamma(const ex & mu)
{
	if (!is_ex_of_type(mu, varidx))
		throw(std::invalid_argument("index of Dirac gamma must be of type varidx"));
	return clifford(diracgamma(), mu);
}

} // namespace GiNaC
