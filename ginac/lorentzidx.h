/** @file lorentzidx.h
 *
 *  Interface to GiNaC's lorentz indices.
 *
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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

class lorentzidx : public idx
{
    friend class simp_lor;
    friend class scalar_products;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    lorentzidx();
    ~lorentzidx();
    lorentzidx (lorentzidx const & other);
    lorentzidx const & operator=(lorentzidx const & other);
protected:
    void copy(lorentzidx const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit lorentzidx(bool cov, bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(string const & n, bool cov=false,
                        bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(char const * n, bool cov=false,
                        bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(unsigned const v, bool cov=false);

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
extern type_info const & typeid_lorentzidx;

// macros

#define ex_to_lorentzidx(X) (static_cast<lorentzidx const &>(*(X).bp))

#endif // ndef __GINAC_LORENTZIDX_H__
