/** @file power.h
 *
 *  Interface to GiNaC's symbolic exponentiation (basis^exponent). */

/*
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

#ifndef __GINAC_POWER_H__
#define __GINAC_POWER_H__

#include <ginac/basic.h>
#include <ginac/ex.h>

namespace GiNaC {

class numeric;
class add;

/** This class holds a two-component object, a basis and and exponent
 *  representing exponentiation. */
class power : public basic
{
    friend class mul;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    power();
    ~power();
    power(power const & other);
    power const & operator=(power const & other);
protected:
    void copy(power const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    power(ex const & lh, ex const & rh);
    power(ex const & lh, numeric const & rh);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    int nops() const;
    ex & let_op(int const i);
    int degree(symbol const & s) const;
    int ldegree(symbol const & s) const;
    ex coeff(symbol const & s, int const n=1) const;
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex diff(symbol const & s) const;
    ex series(symbol const & s, ex const & point, int order) const;
    ex subs(lst const & ls, lst const & lr) const;
    ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
    ex simplify_ncmul(exvector const & v) const;
protected:
    int compare_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex expand(unsigned options=0) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    ex expand_add(add const & a, int const n) const;
    ex expand_add_2(add const & a) const;
    ex expand_mul(mul const & m, numeric const & n) const;
    //ex expand_commutative_3(ex const & basis, numeric const & exponent,
    //                         unsigned options) const;
    // ex expand_noncommutative(ex const & basis, numeric const & exponent, unsigned options) const;

// member variables

protected:
    ex basis;
    ex exponent;
    static unsigned precedence;
};

// global constants

extern const power some_power;
extern type_info const & typeid_power;

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
inline ex pow(ex const & b, ex const & e)
{ return power(b,e); }

/** Square root expression.  Returns a power-object with exponent 1/2 as a new
 *  expression.  */
inline ex sqrt(ex const & a)
{ return power(a,exHALF()); }

} // namespace GiNaC

#endif // ndef __GINAC_POWER_H__
