/** @file mul.h
 *
 *  Interface to GiNaC's products of expressions.
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

#ifndef __GINAC_MUL_H__
#define __GINAC_MUL_H__

#include <ginac/expairseq.h>

/** Product of expressions. */
class mul : public expairseq
{
    friend class add;
    friend class ncmul;
    friend class power;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    mul();
    ~mul();
    mul(mul const & other);
    mul const & operator=(mul const & other);
protected:
    void copy(mul const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    mul(ex const & lh, ex const & rh);
    mul(exvector const & v);
    mul(epvector const & v);
    //mul(epvector const & v, bool do_not_canonicalize=0);
    mul(epvector const & v, ex const & oc);
    mul(epvector * vp, ex const & oc);
    mul(ex const & lh, ex const & mh, ex const & rh);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const;
    bool info(unsigned inf) const;
    int degree(symbol const & s) const;
    int ldegree(symbol const & s) const;
    ex coeff(symbol const & s, int const n=1) const;
    ex eval(int level=0) const;
    ex diff(symbol const & s) const;
    ex series(symbol const & s, ex const & point, int order) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    numeric integer_content(void) const;
    ex smod(numeric const &xi) const;
    numeric max_coefficient(void) const;
    exvector get_indices(void) const;
    ex simplify_ncmul(exvector const & v) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexpairseq(epvector const & v, ex const & oc) const;
    ex thisexpairseq(epvector * vp, ex const & oc) const;
    void printpair(ostream & os, expair const & p,
                   unsigned upper_precedence) const;
    expair split_ex_to_pair(ex const & e) const;
    expair combine_ex_with_coeff_to_pair(ex const & e,
                                         ex const & c) const;
    expair combine_pair_with_coeff_to_pair(expair const & p,
                                           ex const & c) const;
    ex recombine_pair_to_ex(expair const & p) const;
    bool expair_needs_further_processing(epp it);
    ex default_overall_coeff(void) const;
    void combine_overall_coeff(ex const & c);
    void combine_overall_coeff(ex const & c1, ex const & c2);
    bool can_make_flat(expair const & p) const;
    ex expand(unsigned options=0) const;
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
protected:
    epvector * expandchildren(unsigned options) const;

// member variables

protected:
    static unsigned precedence;
};

// global constants

extern const mul some_mul;
extern type_info const & typeid_mul;

#define ex_to_mul(X) static_cast<mul const &>(*(X).bp)

#endif // ndef __GINAC_MUL_H__
