/** @file lorentzidx.h
 *
 *  Interface to GiNaC's lorentz indices. */

#ifndef _LORENTZIDX_H_
#define _LORENTZIDX_H_

#include <string>
#include <vector>

#include "idx.h"

class lorentzidx : public idx
{
    friend class simp_lor;
    friend class scalar_products;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    lorentzidx();
    ~lorentzidx();
    lorentzidx (lorentzidx const & other);
    lorentzidx const & operator=(lorentzidx const & other);
protected:
    void copy(lorentzidx const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit lorentzidx(bool cov, bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(string const & n, bool cov=false,
                        bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(char const * n, bool cov=false,
                        bool oonly=false, unsigned dimp=0);
    explicit lorentzidx(unsigned const v, bool cov=false);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;

    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
public:
    bool is_orthogonal_only(void) const { return orthogonal_only; }
    unsigned get_dim_parallel_space(void) const { return dim_parallel_space; }
    lorentzidx create_anonymous_representative(void) const; 

    // member variables
protected:
    bool orthogonal_only;
    unsigned dim_parallel_space;
};

// global constants

extern const lorentzidx some_lorentzidx;
extern type_info const & typeid_lorentzidx;

// macros

#define ex_to_lorentzidx(X) (static_cast<lorentzidx const &>(*(X).bp))

#endif // ndef _LORENTZIDX_H_
