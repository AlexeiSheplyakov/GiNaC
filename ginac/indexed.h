/** @file indexed.h
 *
 *  Interface to GiNaC's index carrying objects. */

#ifndef _INDEXED_H_
#define _INDEXED_H_

#include <string>

class indexed;

#include "exprseq.h"
#include "idx.h"

/** Base class for non-commutative indexed objects */
class indexed : public exprseq
{
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    indexed();
    ~indexed();
    indexed(indexed const & other);
    indexed const & operator=(indexed const & other);
protected:
    void copy(indexed const & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    indexed(ex const & i1);
    indexed(ex const & i1, ex const & i2);
    indexed(ex const & i1, ex const & i2, ex const & i3);
    indexed(exvector const & iv);
    indexed(exvector * iv);

    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const;
    bool info(unsigned inf) const;
    ex diff(symbol const & s) const;
    exvector get_indices(void) const;
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
    void printrawindices(ostream & os) const;
    void printtreeindices(ostream & os, unsigned indent) const;
    void printindices(ostream & os) const;
    bool all_of_type_idx(void) const;

// member variables
    // none
};

// global constants

extern const indexed some_indexed;
extern type_info const & typeid_indexed;

// macros

#define ex_to_indexed(X) static_cast<indexed const &>(*(X).bp)

#endif // ndef _INDEXED_H_


