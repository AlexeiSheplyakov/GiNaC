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
#include "print.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

#include <stdexcept>

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(clifford, indexed)
GINAC_IMPLEMENT_REGISTERED_CLASS(diracone, tensor)
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
DEFAULT_CTORS(diracone)
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

/** Construct object without any indices. This constructor is for internal
 *  use only. Use the dirac_one() function instead.
 *  @see dirac_one */
clifford::clifford(const ex & b) : inherited(b)
{
	debugmsg("clifford constructor from ex", LOGLEVEL_CONSTRUCT);
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
DEFAULT_ARCHIVING(diracone)
DEFAULT_ARCHIVING(diracgamma)

//////////
// functions overriding virtual functions from bases classes
//////////

int clifford::compare_same_type(const basic & other) const
{
	return inherited::compare_same_type(other);
}

DEFAULT_COMPARE(diracone)
DEFAULT_COMPARE(diracgamma)
DEFAULT_PRINT(diracone, "ONE")
DEFAULT_PRINT(diracgamma, "gamma")

/** Contraction of a gamma matrix with something else. */
bool diracgamma::contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const
{
	GINAC_ASSERT(is_ex_of_type(*self, indexed));
	GINAC_ASSERT(is_ex_of_type(*other, indexed));
	GINAC_ASSERT(is_ex_of_type(self->op(0), diracgamma));

	if (is_ex_of_type(other->op(0), diracgamma)) {

		ex dim = ex_to_idx(self->op(1)).get_dim();

		// gamma~mu*gamma.mu = dim*ONE
		if (other - self == 1) {
			*self = dim;
			*other = dirac_one();
			return true;

		// gamma~mu*gamma~alpha*gamma.mu = (2-dim)*gamma~alpha
		} else if (other - self == 2
		        && is_ex_of_type(self[1], clifford)) {
			*self = 2 - dim;
			*other = _ex1();
			return true;

		// gamma~mu*gamma~alpha*gamma~beta*gamma.mu = 4*g~alpha~beta+(dim-4)*gamam~alpha*gamma~beta
		} else if (other - self == 3
		        && is_ex_of_type(self[1], clifford)
		        && is_ex_of_type(self[2], clifford)) {
			*self = 4 * metric_tensor(self[1].op(1), self[2].op(1)) * dirac_one() + (dim - 4) * self[1] * self[2];
			self[1] = _ex1();
			self[2] = _ex1();
			*other = _ex1();
			return true;

		// gamma~mu*gamma~alpha*gamma~beta*gamma~delta*gamma.mu = -2*gamma~delta*gamma~beta*gamma~alpha+(4-dim)*gamma~alpha*gamma~beta*gamma~delta
		} else if (other - self == 4
		        && is_ex_of_type(self[1], clifford)
		        && is_ex_of_type(self[2], clifford)
		        && is_ex_of_type(self[3], clifford)) {
			*self = -2 * self[3] * self[2] * self[1] + (4 - dim) * self[1] * self[2] * self[3];
			self[1] = _ex1();
			self[2] = _ex1();
			self[3] = _ex1();
			*other = _ex1();
			return true;
		}
	}

	return false;
}

/** Perform automatic simplification on noncommutative product of clifford
 *  objects. This removes superfluous ONEs. */
ex clifford::simplify_ncmul(const exvector & v) const
{
	exvector s;
	s.reserve(v.size());

	exvector::const_iterator it = v.begin(), itend = v.end();
	while (it != itend) {
		if (!is_ex_of_type(it->op(0), diracone))
			s.push_back(*it);
		it++;
	}

	if (s.size() == 0)
		return clifford(diracone());
	else if (s.size() == v.size())
		return simplified_ncmul(v);
	else
		return simplified_ncmul(s);
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

ex dirac_one(void)
{
	return clifford(diracone());
}

ex dirac_gamma(const ex & mu)
{
	if (!is_ex_of_type(mu, varidx))
		throw(std::invalid_argument("index of Dirac gamma must be of type varidx"));

	return clifford(diracgamma(), mu);
}

} // namespace GiNaC
