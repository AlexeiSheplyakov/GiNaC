/** @file lorentzidx.h
 *
 *  Interface to GiNaC's lorentz indices. */

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

#ifndef __GINAC_LORENTZIDX_H__
#define __GINAC_LORENTZIDX_H__

#include <string>
#include <vector>
#include "idx.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class lorentzidx : public idx
{
    GINAC_DECLARE_REGISTERED_CLASS(lorentzidx, idx)

    friend class simp_lor;
    friend class scalar_products;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    lorentzidx();
    ~lorentzidx();
    lorentzidx (const lorentzidx & other);
    const lorentzidx & operator=(const lorentzidx & other);
protected:
    void copy(const lorentzidx & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit lorentzidx(bool cov, bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(const string & n, bool cov=false,
                        bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(const char * n, bool cov=false,
                        bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(unsigned v, bool cov=false);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;

    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
public:
    bool is_orthogonal_only(void) const { return orthogonal_only; }
    unsigned get_dim_parallel_space(void) const { return dim_parallel_space; }
    lorentzidx create_anonymous_representative(void) const; 

    // member variables
protected:
    bool orthogonal_only;
    unsigned dim_parallel_space;
};

// global constants

extern const lorentzidx some_lorentzidx;
extern const type_info & typeid_lorentzidx;

// utility functions
inline const lorentzidx &ex_to_lorentzidx(const ex &e)
{
	return static_cast<const lorentzidx &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_LORENTZIDX_H__
