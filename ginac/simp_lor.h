/** @file simp_lor.h
 *
 *  Interface to GiNaC's simp_lor objects. */

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

#ifndef _SIMP__GINAC_LOR_H__
#define _SIMP__GINAC_LOR_H__

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include "indexed.h"
#include "lorentzidx.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

typedef pair<string,string> strstrpair;
typedef pair<strstrpair,lorentzidx> spmapkey;

class spmapkey_is_less
{
public:
    bool operator()(const spmapkey & lh, const spmapkey & rh) const
    {
        /*
        cerr << "spmapkey_is_less" << endl;
        cerr << "lh=((" << lh.first.first
             << "," << lh.first.second << "),";
        lh.second.printraw(cerr);
        cerr << ")" << endl;

        cerr << "rh=((" << rh.first.first
             << "," << rh.first.second << "),";
        rh.second.printraw(cerr);
        cerr << ")" << endl;
        */
        bool res=lh.first<rh.first ||
            (!(rh.first<lh.first) && lh.second.compare(rh.second)<0 );
        // cout << "result=" << res << endl;
        return res;
    }
};

typedef map<spmapkey,ex,spmapkey_is_less> spmap;

class simp_lor;

/** helper class for scalar products */
class scalar_products
{
public:
    void reg(const simp_lor & v1, const simp_lor & v2, const ex & sp);
    bool is_defined(const simp_lor & v1, const simp_lor & v2) const;
    ex evaluate(const simp_lor & v1, const simp_lor & v2) const;
    void debugprint(void) const;
protected:
    static spmapkey make_key(const simp_lor & v1, const simp_lor & v2);
protected:
    spmap spm;
};

/** Base class for simp_lor object */
class simp_lor : public indexed
{
// friends

    friend class scalar_products;
    friend simp_lor lor_g(const ex & mu, const ex & nu);
    friend simp_lor lor_vec(const string & n, const ex & mu);
    friend ex simplify_simp_lor_mul(const ex & m, const scalar_products & sp);
    friend ex simplify_simp_lor(const ex & e, const scalar_products & sp);
    
// types

public:
    typedef enum { invalid, // not properly constructed by one of the friend functions
                   simp_lor_g,
                   simp_lor_vec
    } simp_lor_types;
    
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    simp_lor();
    ~simp_lor();
    simp_lor(const simp_lor & other);
    const simp_lor & operator=(const simp_lor & other);
protected:
    void copy(const simp_lor & other); 
    void destroy(bool call_parent);

    // other constructors
protected:
    simp_lor(simp_lor_types const t);
    simp_lor(simp_lor_types const t, const ex & i1, const ex & i2);
    simp_lor(simp_lor_types const t, const string & n, const ex & i1);
    simp_lor(simp_lor_types const t, const string & n, const exvector & iv);
    simp_lor(simp_lor_types const t, const string & n, exvector * ivp);
    
    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    ex eval(int level=0) const;
protected:
    int compare_same_type(const basic & other) const;
    bool is_equal_same_type(const basic & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexprseq(const exvector & v) const;
    ex thisexprseq(exvector * vp) const;

    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    bool all_of_type_lorentzidx(void) const;
    
// member variables

protected:
    simp_lor_types type;
    string name;
};

// global constants

extern const simp_lor some_simp_lor;
extern const type_info & typeid_simp_lor;

// utility functions
inline const simp_lor &ex_to_simp_lor(const ex &e)
{
	return static_cast<const simp_lor &>(*e.bp);
}

inline simp_lor &ex_to_nonconst_simp_lor(const ex &e)
{
	return static_cast<simp_lor &>(*e.bp);
}

simp_lor lor_g(const ex & mu, const ex & nu);
simp_lor lor_vec(const string & n, const ex & mu);
ex simplify_simp_lor_mul(const ex & m, const scalar_products & sp);
ex simplify_simp_lor(const ex & e, const scalar_products & sp);
ex Dim(void);

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef _SIMP__GINAC_LOR_H__
