/** @file ncmul.h
 *
 *  Interface to GiNaC's non-commutative products of expressions. */

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

#ifndef __GINAC_NCMUL_H__
#define __GINAC_NCMUL_H__

#include "exprseq.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** Non-commutative product of expressions. */
class ncmul : public exprseq
{
    GINAC_DECLARE_REGISTERED_CLASS(ncmul, exprseq)

    friend class power;
    friend ex nonsimplified_ncmul(const exvector & v);
    friend ex simplified_ncmul(const exvector & v);

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    ncmul();
    ~ncmul();
    ncmul(const ncmul & other);
    const ncmul & operator=(const ncmul & other);
protected:
    void copy(const ncmul & other);
    void destroy(bool call_parent);

    // other constructors
public:
    ncmul(const ex & lh, const ex & rh);
    ncmul(const ex & f1, const ex & f2, const ex & f3);
    ncmul(const ex & f1, const ex & f2, const ex & f3,
          const ex & f4);
    ncmul(const ex & f1, const ex & f2, const ex & f3,
          const ex & f4, const ex & f5);
    ncmul(const ex & f1, const ex & f2, const ex & f3,
          const ex & f4, const ex & f5, const ex & f6);
    ncmul(const exvector & v, bool discardable=false);
    ncmul(exvector * vp); // vp will be deleted

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void print(ostream & os, unsigned upper_precedence) const;
    void printraw(ostream & os) const;
    void printcsrc(ostream & os, unsigned upper_precedence) const;
    bool info(unsigned inf) const;
    int degree(const symbol & s) const;
    int ldegree(const symbol & s) const;
    ex expand(unsigned options=0) const;
    ex coeff(const symbol & s, int n=1) const;
    ex eval(int level=0) const;
    ex diff(const symbol & s) const;
    ex subs(const lst & ls, const lst & lr) const;
    exvector get_indices(void) const;
    ex thisexprseq(const exvector & v) const;
    ex thisexprseq(exvector * vp) const;
protected:
    int compare_same_type(const basic & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
protected:
    unsigned count_factors(const ex & e) const;
    void append_factors(exvector & v, const ex & e) const;
    exvector expandchildren(unsigned options) const;
public:
    const exvector & get_factors(void) const;

// member variables

protected:
    static unsigned precedence;
};

// global constants

extern const ncmul some_ncmul;
extern const type_info & typeid_ncmul;

// friend funtions 

ex nonsimplified_ncmul(const exvector & v);
ex simplified_ncmul(const exvector & v);

// utility functions
inline const ncmul &ex_to_ncmul(const ex &e)
{
	return static_cast <const ncmul &>(*e.bp);
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_NCMUL_H__
