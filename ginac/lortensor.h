/** @file lortensor.h
 *
 *  Interface to GiNaC´s lortensor objects. */

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

#ifndef __GINAC_LORTENSOR_H__
#define __GINAC_LORTENSOR_H__

#include <string>
#include <vector>
#include "config.h"
#include <iostream>
#include <ginac/indexed.h>
#include <ginac/lorentzidx.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** Base class for lortensor object */
class lortensor : public indexed
{
    // friends
    friend lortensor lortensor_g(ex const & mu, ex const & nu);
    // friend lortensor lortensor_delta(ex const & mu, ex const & nu);
    friend lortensor lortensor_epsilon(ex const & mu, ex const & nu,
                                       ex const & rho, ex const & sigma);
    friend lortensor lortensor_rankn(string const & n, exvector const & iv);
    friend lortensor lortensor_rank1(string const & n, ex const & mu);
    friend lortensor lortensor_rank2(string const & n, ex const & mu, ex const & nu);
    friend ex simplify_lortensor_mul(ex const & m);
    friend ex simplify_lortensor(ex const & e);
    
    // types

public:
    typedef enum { invalid,
                   lortensor_g,
                   lortensor_rankn,
                   lortensor_rank1,
                   lortensor_rank2,
                   // lortensor_delta,
                   lortensor_epsilon
    } lortensor_types;

    // member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    lortensor();
    ~lortensor();
    lortensor(lortensor const & other);
    lortensor const & operator=(lortensor const & other);
protected:
    void copy(lortensor const & other);
    void destroy(bool call_parent);

    // other constructors
protected:
    lortensor(lortensor_types const lt, string const & n);
    lortensor(lortensor_types const lt, string const & n, ex const & mu);
    lortensor(lortensor_types const lt, string const & n, ex const & mu, ex const & nu);
    lortensor(lortensor_types const lt, string const & n, ex const & mu, ex const & nu,
              ex const & rho);
    lortensor(lortensor_types const lt, string const & n, ex const & mu, ex const & nu, ex const & rho, ex const & sigma);
    lortensor(lortensor_types const lt, string const & n, exvector const & iv);
    lortensor(lortensor_types const lt, string const & n, unsigned s, exvector const & iv);
    lortensor(lortensor_types const lt, string const & n, unsigned s, exvector * ivp);
    
    //functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    ex eval(int level=0) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexprseq(exvector const & v) const;
    ex thisexprseq(exvector * vp) const;

    // new virtual functions which can be overridden by derived classes
    // none

    //non virtual functions in this class
public:
    void setname(string const & n);
    string getname(void) const {return name;}
protected:
    bool all_of_type_lorentzidx(void) const;
private:
    string & autoname_prefix(void);

    //member variables

protected:
    lortensor_types type;
    string name;
    unsigned serial;
private:
    static unsigned next_serial;
};

// global constants

    extern const lortensor some_lortensor;
    extern type_info const & typeid_lortensor;

// utility functions
    
inline const lortensor & ex_to_lortensor(const ex &e)
{
	return static_cast<const lortensor &>(*e.bp);
}

inline lortensor &ex_to_nonconst_lortensor(const ex &e)
{
	return static_cast<lortensor &>(*e.bp);
}

lortensor lortensor_g(ex const & mu, ex const & nu);
ex simplify_lortensor_mul(ex const & m);
ex simplify_lortensor(ex const & e);
ex Dim(void);    

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_LORTENSOR_H__
