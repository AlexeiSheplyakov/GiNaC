/** @file idx.h
 *
 *  Interface to GiNaC's indices. */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class idx : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(idx, basic)

// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	idx();
	~idx();
	idx (const idx & other);
	const idx & operator=(const idx & other);
protected:
	void copy(const idx & other);
	void destroy(bool call_parent);

	// other constructors
public:
	explicit idx(bool cov);
	explicit idx(const std::string & n, bool cov=false);
	explicit idx(const char * n, bool cov=false);
	explicit idx(unsigned v, bool cov=false); 

	// functions overriding virtual functions from bases classes
public:
	basic * duplicate() const;
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
protected:
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned calchash(void) const;
	ex subs(const lst & ls, const lst & lr) const;

	// new virtual functions which can be overridden by derived classes
public:
	virtual bool is_co_contra_pair(const basic & other) const;
	virtual ex toggle_covariant(void) const;

	// non-virtual functions in this class
public:
	bool is_symbolic(void) const;
	unsigned get_value(void) const;
	bool is_covariant(void) const;
	void setname(const std::string & n) {name=n;}
	std::string getname(void) const {return name;}

	// member variables
protected:
	unsigned serial;
	bool symbolic;
	std::string name;
	unsigned value;
	static unsigned next_serial;
	bool covariant; // x_mu, default is contravariant: x^mu
};

// global constants

extern const idx some_idx;
extern const type_info & typeid_idx;

// utility functions
inline const idx &ex_to_idx(const ex &e)
{
	return static_cast<const idx &>(*e.bp);
}

// global functions

int canonicalize_indices(exvector & iv, bool antisymmetric=false);
exvector idx_intersect(const exvector & iv1, const exvector & iv2);
ex permute_free_index_to_front(const exvector & iv3, const exvector & iv2,
							   bool antisymmetric, int * sig);
unsigned subs_index_in_exvector(exvector & v, const ex & is, const ex & ir);
ex subs_indices(const ex & e, const exvector & idxv_contra,
				const exvector & idxv_co);
unsigned count_index(const ex & e, const ex & i);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_IDX_H__
