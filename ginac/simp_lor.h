/** @file simp_lor.h
 *
 *  Interface to GiNaC's simp_lor objects. */

#ifndef _SIMP_LOR_H_
#define _SIMP_LOR_H_

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>

class simp_lor;

#include "indexed.h"
#include "lorentzidx.h"

typedef pair<string,string> strstrpair;
typedef pair<strstrpair,lorentzidx> spmapkey;

class spmapkey_is_less
{
public:
    bool operator()(spmapkey const & lh, spmapkey const & rh) const
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
    void reg(simp_lor const & v1, simp_lor const & v2, ex const & sp);
    bool is_defined(simp_lor const & v1, simp_lor const & v2) const;
    ex evaluate(simp_lor const & v1, simp_lor const & v2) const;
    void debugprint(void) const;
protected:
    static spmapkey make_key(simp_lor const & v1, simp_lor const & v2);
protected:
    spmap spm;
};

/** Base class for simp_lor object */
class simp_lor : public indexed
{
// friends

    friend class scalar_products;
    friend simp_lor lor_g(ex const & mu, ex const & nu);
    friend simp_lor lor_vec(string const & n, ex const & mu);
    friend ex simplify_simp_lor_mul(ex const & m, scalar_products const & sp);
    friend ex simplify_simp_lor(ex const & e, scalar_products const & sp);
    
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
    simp_lor(simp_lor const & other);
    simp_lor const & operator=(simp_lor const & other);
protected:
    void copy(simp_lor const & other); 
    void destroy(bool call_parent);

    // other constructors
protected:
    simp_lor(simp_lor_types const t);
    simp_lor(simp_lor_types const t, ex const & i1, ex const & i2);
    simp_lor(simp_lor_types const t, string const & n, ex const & i1);
    simp_lor(simp_lor_types const t, string const & n, exvector const & iv);
    simp_lor(simp_lor_types const t, string const & n, exvector * ivp);
    
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
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;
    ex thisexprseq(exvector const & v) const;
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
extern type_info const & typeid_simp_lor;

// macros

#define ex_to_simp_lor(X) static_cast<simp_lor const &>(*(X).bp)
#define ex_to_nonconst_simp_lor(X) static_cast<simp_lor &>(*(X).bp)

simp_lor lor_g(ex const & mu, ex const & nu);
simp_lor lor_vec(string const & n, ex const & mu);
ex simplify_simp_lor_mul(ex const & m, scalar_products const & sp);
ex simplify_simp_lor(ex const & e, scalar_products const & sp);
ex Dim(void);

#endif // ndef _SIMP_LOR_H_


