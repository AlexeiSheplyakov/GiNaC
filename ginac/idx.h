/** @file idx.h
 *
 *  Interface to GiNaC's indices. */

#ifndef _IDX_H_
#define _IDX_H_

#include <string>
#include <vector>

#include "basic.h"

class idx : public basic
{
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    idx();
    ~idx();
    idx (idx const & other);
    idx const & operator=(idx const & other);
protected:
    void copy(idx const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit idx(bool cov);
    explicit idx(string const & n, bool cov=false);
    explicit idx(char const * n, bool cov=false);
    explicit idx(unsigned const v, bool cov=false); 

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    unsigned calchash(void) const;
    ex subs(lst const & ls, lst const & lr) const;

    // new virtual functions which can be overridden by derived classes
public:
    virtual bool is_co_contra_pair(basic const & other) const;
    virtual ex toggle_covariant(void) const;

    // non-virtual functions in this class
public:
    bool is_symbolic(void) const;
    unsigned get_value(void) const;
    bool is_covariant(void) const;

    // member variables
protected:
    unsigned serial;
    bool symbolic;
    string name;
    unsigned value;
    static unsigned next_serial;
    bool covariant; // x_mu, default is contravariant: x^mu
};

// global constants

extern const idx some_idx;
extern type_info const & typeid_idx;

// macros

#define ex_to_idx(X) (static_cast<idx const &>(*(X).bp))

// other functions

typedef vector<ex> exvector;

int canonicalize_indices(exvector & iv, bool antisymmetric=false);
exvector idx_intersect(exvector const & iv1, exvector const & iv2);
ex permute_free_index_to_front(exvector const & iv3, exvector const & iv2,
                               bool antisymmetric, int * sig);
unsigned subs_index_in_exvector(exvector & v, ex const & is, ex const & ir);
ex subs_indices(ex const & e, exvector const & idxv_contra,
                exvector const & idxv_co);
unsigned count_index(ex const & e, ex const & i);

#endif // ndef _IDX_H_
