/** @file ncmul.h
 *
 *  Interface to GiNaC's non-commutative products of expressions.
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

#ifndef __GINAC_NCMUL_H__
#define __GINAC_NCMUL_H__

/** Non-commutative product of expressions. */
class ncmul : public exprseq
{
    friend class power;
    friend ex nonsimplified_ncmul(exvector const & v);
    friend ex simplified_ncmul(exvector const & v);

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    ncmul();
    ~ncmul();
    ncmul(ncmul const & other);
    ncmul const & operator=(ncmul const & other);
protected:
    void copy(ncmul const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    ncmul(ex const & lh, ex const & rh);
    ncmul(ex const & f1, ex const & f2, ex const & f3);
    ncmul(ex const & f1, ex const & f2, ex const & f3,
          ex const & f4);
    ncmul(ex const & f1, ex const & f2, ex const & f3,
          ex const & f4, ex const & f5);
    ncmul(ex const & f1, ex const & f2, ex const & f3,
          ex const & f4, ex const & f5, ex const & f6);
    ncmul(exvector const & v, bool discardable=false);
    ncmul(exvector * vp); // vp will be deleted

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence) const;
    void printcsrc(ostream & os, unsigned upper_precedence) const;
    bool info(unsigned inf) const;
    int degree(symbol const & s) const;
    int ldegree(symbol const & s) const;
    ex expand(unsigned options=0) const;
    ex coeff(symbol const & s, int const n=1) const;
    ex eval(int level=0) const;
    ex diff(symbol const & s) const;
    ex subs(lst const & ls, lst const & lr) const;
    exvector get_indices(void) const;
    ex thisexprseq(exvector const & v) const;
    ex thisexprseq(exvector * vp) const;
protected:
    int compare_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
protected:
    unsigned count_factors(ex const & e) const;
    void append_factors(exvector & v, ex const & e) const;
    exvector expandchildren(unsigned options) const;
public:
    exvector const & get_factors(void) const;

// member variables

protected:
    static unsigned precedence;
};

// global constants

extern const ncmul some_ncmul;
extern type_info const & typeid_ncmul;

// friend funtions 

ex nonsimplified_ncmul(exvector const & v);
ex simplified_ncmul(exvector const & v);

#define ex_to_ncmul(X) static_cast<ncmul const &>(*(X).bp)

#endif // ndef __GINAC_NCMUL_H__

