/** @file add.h
 *
 *  Interface to GiNaC's sums of expressions. */

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

#ifndef __GINAC_ADD_H__
#define __GINAC_ADD_H__

#include "expairseq.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

/** Sum of expressions. */
class add : public expairseq
{
    GINAC_DECLARE_REGISTERED_CLASS(add, expairseq)

    friend class mul;
    friend class ncmul;
    friend class power;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    add();
    ~add();
    add(const add & other);
    const add & operator=(const add & other);
protected:
    void copy(const add & other);
    void destroy(bool call_parent);

    // other constructors
public:
    add(const ex & lh, const ex & rh);
    add(const exvector & v);
    add(const epvector & v);
    //add(const epvector & v, bool do_not_canonicalize=0);
    add(const epvector & v, const ex & oc);
    add(epvector * vp, const ex & oc);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void print(std::ostream & os, unsigned upper_precedence=0) const;
    void printraw(std::ostream & os) const;
    void printcsrc(std::ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    int degree(const symbol & s) const;
    int ldegree(const symbol & s) const;
    ex coeff(const symbol & s, int n=1) const;
    ex eval(int level=0) const;
    ex series(const relational & r, int order, bool branchcut = true) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    numeric integer_content(void) const;
    ex smod(const numeric &xi) const;
    numeric max_coefficient(void) const;
    exvector get_indices(void) const;
    ex simplify_ncmul(const exvector & v) const;
protected:
    ex derivative(const symbol & s) const;
    int compare_same_type(const basic & other) const;
    bool is_equal_same_type(const basic & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexpairseq(const epvector & v, const ex & oc) const;
    ex thisexpairseq(epvector * vp, const ex & oc) const;
    expair split_ex_to_pair(const ex & e) const;
    expair combine_ex_with_coeff_to_pair(const ex & e,
                                         const ex & c) const;
    expair combine_pair_with_coeff_to_pair(const expair & p,
                                           const ex & c) const;
    ex recombine_pair_to_ex(const expair & p) const;
    ex expand(unsigned options=0) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
    // none

// member variables

protected:
    static unsigned precedence;
};

// global constants

extern const add some_add;
extern const type_info & typeid_add;

// utility functions
inline const add &ex_to_add(const ex &e)
{
	return static_cast<const add &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_ADD_H__
