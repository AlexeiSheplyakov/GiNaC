/** @file ncmul.h
 *
 *  Interface to GiNaC's non-commutative products of expressions. */

#ifndef _NCMUL_H_
#define _NCMUL_H_

#include "exprseq.h"

class ncmul;

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

#endif // ndef _NCMUL_H_

