/** @file power.h
 *
 *  Interface to GiNaC's symbolic exponentiation (basis^exponent). */

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

#ifndef __GINAC_POWER_H__
#define __GINAC_POWER_H__

#include "basic.h"
#include "ex.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

class numeric;
class add;

/** This class holds a two-component object, a basis and and exponent
 *  representing exponentiation. */
class power : public basic
{
    GINAC_DECLARE_REGISTERED_CLASS(power, basic)

    friend class mul;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    power();
    ~power();
    power(const power & other);
    const power & operator=(const power & other);
protected:
    void copy(const power & other);
    void destroy(bool call_parent);

    // other constructors
public:
    power(const ex & lh, const ex & rh);
    power(const ex & lh, const numeric & rh);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    unsigned nops() const;
    ex & let_op(int i);
    int degree(const symbol & s) const;
    int ldegree(const symbol & s) const;
    ex coeff(const symbol & s, int n=1) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex series(const relational & s, int order) const;
    ex subs(const lst & ls, const lst & lr) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    ex simplify_ncmul(const exvector & v) const;
protected:
    ex derivative(const symbol & s) const;
    int compare_same_type(const basic & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex expand(unsigned options=0) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    ex expand_add(const add & a, int n) const;
    ex expand_add_2(const add & a) const;
    ex expand_mul(const mul & m, const numeric & n) const;
    //ex expand_commutative_3(const ex & basis, const numeric & exponent,
    //                         unsigned options) const;
    // ex expand_noncommutative(const ex & basis, const numeric & exponent, unsigned options) const;

// member variables

protected:
    ex basis;
    ex exponent;
    static unsigned precedence;
};

// global constants

extern const power some_power;
extern const type_info & typeid_power;

// utility functions
inline const power &ex_to_power(const ex &e)
{
	return static_cast<const power &>(*e.bp);
}

// wrapper functions

/** Symbolic exponentiation.  Returns a power-object as a new expression.
 *
 *  @param b the basis expression
 *  @param e the exponent expression */
inline ex pow(const ex & b, const ex & e)
{ return power(b,e); }

/** Square root expression.  Returns a power-object with exponent 1/2 as a new
 *  expression.  */
ex sqrt(const ex & a);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_POWER_H__
