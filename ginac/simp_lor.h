/** @file simp_lor.h
 *
 *  Interface to GiNaC's simp_lor objects. */

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

#ifndef _SIMP__GINAC_LOR_H__
#define _SIMP__GINAC_LOR_H__

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include "indexed.h"
#include "lorentzidx.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

typedef std::pair<std::string,std::string> strstrpair;
typedef std::pair<strstrpair,lorentzidx> spmapkey;

class spmapkey_is_less
{
public:
	bool operator()(const spmapkey & lh, const spmapkey & rh) const
	{
		bool res = lh.first<rh.first
		        || (!(rh.first<lh.first) && lh.second.compare(rh.second)<0);
		return res;
	}
};

typedef std::map<spmapkey,ex,spmapkey_is_less> spmap;

class simp_lor;

/** helper class for scalar products */
class scalar_products
{
public:
	void reg(const simp_lor & v1, const simp_lor & v2, const ex & sp);
	bool is_defined(const simp_lor & v1, const simp_lor & v2) const;
	ex evaluate(const simp_lor & v1, const simp_lor & v2) const;
	void debugprint(void) const;
protected:
	static spmapkey make_key(const simp_lor & v1, const simp_lor & v2);
protected:
	spmap spm;
};

/** Base class for simp_lor object */
class simp_lor : public indexed
{
	GINAC_DECLARE_REGISTERED_CLASS(simp_lor, indexed)

// friends

	friend class scalar_products;
	friend simp_lor lor_g(const ex & mu, const ex & nu);
	friend simp_lor lor_vec(const std::string & n, const ex & mu);
	friend ex simplify_simp_lor_mul(const ex & m, const scalar_products & sp);
	friend ex simplify_simp_lor(const ex & e, const scalar_products & sp);
	
// types

public:
	typedef enum {
		invalid, // not properly constructed by one of the friend functions
		simp_lor_g,
		simp_lor_vec
	} simp_lor_types;
	
	// other constructors
protected:
	simp_lor(simp_lor_types const t);
	simp_lor(simp_lor_types const t, const ex & i1, const ex & i2);
	simp_lor(simp_lor_types const t, const std::string & n, const ex & i1);
	simp_lor(simp_lor_types const t, const std::string & n, const exvector & iv);
	simp_lor(simp_lor_types const t, const std::string & n, exvector * ivp);
	
	// functions overriding virtual functions from base classes
public:
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
	ex eval(int level=0) const;
protected:
	bool is_equal_same_type(const basic & other) const;
	unsigned return_type(void) const;
	unsigned return_type_tinfo(void) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;

	// new virtual functions which can be overridden by derived classes
	// none
	
	// non-virtual functions in this class
protected:
	bool all_of_type_lorentzidx(void) const;
	
// member variables

protected:
	simp_lor_types type;
	std::string name;
};

// utility functions
inline const simp_lor &ex_to_simp_lor(const ex &e)
{
	return static_cast<const simp_lor &>(*e.bp);
}

inline simp_lor &ex_to_nonconst_simp_lor(const ex &e)
{
	return static_cast<simp_lor &>(*e.bp);
}

simp_lor lor_g(const ex & mu, const ex & nu);
simp_lor lor_vec(const std::string & n, const ex & mu);
ex simplify_simp_lor_mul(const ex & m, const scalar_products & sp);
ex simplify_simp_lor(const ex & e, const scalar_products & sp=scalar_products());

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef _SIMP__GINAC_LOR_H__
