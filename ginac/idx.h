/** @file idx.h
 *
 *  Interface to GiNaC's indices. */

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

#ifndef __GINAC_IDX_H__
#define __GINAC_IDX_H__

#include <string>
//#include <vector>
#include "basic.h"
#include "ex.h"

namespace GiNaC {


/** This class holds one index of an indexed object. Indices can be symbolic
 *  (e.g. "mu", "i") or numeric (unsigned integer), and they can be contravariant
 *  (the default) or covariant. */
class idx : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(idx, basic)

	// other constructors
public:
	explicit idx(bool cov);
	explicit idx(const std::string & n, bool cov=false);
	explicit idx(const char * n, bool cov=false);
	explicit idx(unsigned v, bool cov=false); 

	// functions overriding virtual functions from bases classes
public:
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
protected:
	bool is_equal_same_type(const basic & other) const;
	unsigned calchash(void) const;
	ex subs(const lst & ls, const lst & lr) const;

	// new virtual functions which can be overridden by derived classes
public:
	virtual bool is_co_contra_pair(const basic & other) const;
	virtual ex toggle_covariant(void) const;

	// non-virtual functions in this class
public:
	/** Check whether index is symbolic (not numeric). */
	bool is_symbolic(void) const {return symbolic;}

	/** Get numeric value of index. Undefined for symbolic indices. */
	unsigned get_value(void) const {return value;}

	/** Check whether index is covariant (not contravariant). */
	bool is_covariant(void) const {return covariant;}

	void setname(const std::string & n) {name=n;}
	std::string getname(void) const {return name;}

private:
	std::string & autoname_prefix(void);

	// member variables
protected:
	unsigned serial;  /**< Unique serial number for comparing symbolic indices */
	bool symbolic;    /**< Is index symbolic? */
	std::string name; /**< Symbolic name (if symbolic == true) */
	unsigned value;   /**< Numeric value (if symbolic == false) */
	static unsigned next_serial;
	bool covariant;   /**< x_mu, default is contravariant: x~mu */
};

// utility functions
inline const idx &ex_to_idx(const ex &e)
{
	return static_cast<const idx &>(*e.bp);
}

// global functions

int canonicalize_indices(exvector & iv, bool antisymmetric=false);
exvector idx_intersect(const exvector & iv1, const exvector & iv2);
ex permute_free_index_to_front(const exvector & iv3, const exvector & iv2, int * sig);
unsigned subs_index_in_exvector(exvector & v, const ex & is, const ex & ir);
ex subs_indices(const ex & e, const exvector & idxv_contra, const exvector & idxv_co);
unsigned count_index(const ex & e, const ex & i);

} // namespace GiNaC

#endif // ndef __GINAC_IDX_H__
