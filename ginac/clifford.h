/** @file clifford.h
 *
 *  Interface to GiNaC's clifford algebra (Dirac gamma) objects. */

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

#ifndef __GINAC_CLIFFORD_H__
#define __GINAC_CLIFFORD_H__

#include "indexed.h"
#include "tensor.h"

namespace GiNaC {


/** This class holds an object representing an element of the Clifford
 *  algebra (the Dirac gamma matrices). These objects only carry Lorentz
 *  indices. Spinor indices are hidden. */
class clifford : public indexed
{
	GINAC_DECLARE_REGISTERED_CLASS(clifford, indexed)

	// other constructors
public:
	clifford(const ex & b);
	clifford(const ex & b, const ex & mu);

	// internal constructors
	clifford(const exvector & v, bool discardable = false);
	clifford(exvector * vp); // vp will be deleted

	// functions overriding virtual functions from base classes
protected:
	ex simplify_ncmul(const exvector & v) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;
	unsigned return_type(void) const { return return_types::noncommutative; }
	unsigned return_type_tinfo(void) const { return TINFO_clifford; }
};


/** This class represents the Clifford algebra unity element. */
class diracone : public tensor
{
	GINAC_DECLARE_REGISTERED_CLASS(diracone, tensor)

	// functions overriding virtual functions from bases classes
public:
	void print(std::ostream & os, unsigned upper_precedence=0) const;
};


/** This class represents the Dirac gamma Lorentz vector. */
class diracgamma : public tensor
{
	GINAC_DECLARE_REGISTERED_CLASS(diracgamma, tensor)

	// functions overriding virtual functions from bases classes
public:
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool contract_with(exvector::iterator self, exvector::iterator other, exvector & v) const;
};


// global functions
inline const clifford &ex_to_clifford(const ex &e)
{
	return static_cast<const clifford &>(*e.bp);
}


/** Create a Clifford unity object.
 *
 *  @return newly constructed object */
ex dirac_one(void);

/** Create a Dirac gamma object.
 *
 *  @param mu Index (must be of class varidx or a derived class)
 *  @return newly constructed gamma object */
ex dirac_gamma(const ex & mu);


} // namespace GiNaC

#endif // ndef __GINAC_CLIFFORD_H__
