/** @file clifford.h
 *
 *  Interface to GiNaC's clifford objects. */

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

#include <string>
#include "lortensor.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC


/** This class holds an object representing an element of the Clifford
 *  algebra (the Dirac gamma matrices). These objects only carry Lorentz
 *  indices. Spinor indices are always hidden in our implementation. */
class clifford : public lortensor
{
	GINAC_DECLARE_REGISTERED_CLASS(clifford, lortensor)

// friends

	friend clifford clifford_gamma(const ex & mu);

	// other constructors
public:
	clifford(const std::string & n, const ex & mu);
	clifford(const std::string & n, const exvector & iv);
	clifford(const std::string & n, exvector * ivp);

	// functions overriding virtual functions from base classes
public:
	basic * duplicate() const;
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
	// ex eval(int level=0) const;
protected:
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	ex simplify_ncmul(const exvector & v) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;

	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
	// none

	// member variables
	// none
};

// global functions
inline const clifford &ex_to_clifford(const ex &e)
{
	return static_cast<const clifford &>(*e.bp);
}

clifford clifford_gamma(const ex & mu);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_CLIFFORD_H__
