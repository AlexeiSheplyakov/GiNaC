/** @file coloridx.h
 *
 *  Interface to GiNaC's color indices. */

#ifndef _COLORIDX_H_
#define _COLORIDX_H_

#include <string>
#include <vector>

#include "idx.h"

class coloridx : public idx
{
    friend class color;

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    coloridx();
    ~coloridx();
    coloridx (coloridx const & other);
    coloridx const & operator=(coloridx const & other);
protected:
    void copy(coloridx const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    explicit coloridx(bool cov);
    explicit coloridx(string const & n, bool cov=false);
    explicit coloridx(char const * n, bool cov=false);
    explicit coloridx(unsigned const v, bool cov=false); 

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
    // none

    // member variables
    // none
};

// global constants

extern const coloridx some_coloridx;
extern type_info const & typeid_coloridx;

// macros

#define ex_to_coloridx(X) (static_cast<coloridx const &>(*(X).bp))

#endif // ndef _COLORIDX_H_
