/** @file fail.h
 *
 *  Interface to class signaling failure of operation. Considered obsolete all
 *  this stuff ought to be replaced by exceptions. */

#ifndef _FAIL_H_
#define _FAIL_H_

class fail;

#include "basic.h"
#include "flags.h"

class fail : public basic
{

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    fail();
    ~fail();
    fail(fail const & other);
    fail const & operator=(fail const & other);
protected:
    void copy(fail const & other);
    void destroy(bool call_parent);

    // other constructors
    // none

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
protected:
    int compare_same_type(basic const & other) const;
    unsigned return_type(void) const { return return_types::noncommutative_composite; };
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
    // none

// member variables
// none
};

// global constants

extern const fail some_fail;
extern type_info const & typeid_fail;

#endif // ndef _FAIL_H_


