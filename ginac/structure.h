/** @file structure.h
 *
 *  Interface to 'abstract' class structure. */

#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_

class structure;

#include "basic.h"

struct registered_structure_info {
    char const * name;
};

/** The class structure is used to implement user defined classes
    with named members which behave similar to ordinary C structs.
    structure is an 'abstract' base class (it is possible but not
    meaningful to make instances), the user defined structures
    will be create by the perl script structure.pl */

class structure : public basic
{
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    structure();
    ~structure();
    structure(structure const & other);
    structure const & operator=(structure const & other);
protected:
    void copy(structure const & other);
    void destroy(bool call_parent);

    // other constructors
    // none

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const; 
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printtree(ostream & os, unsigned indent) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    
    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
protected:
    static vector<registered_structure_info> & registered_structures(void);
public:
    static unsigned register_new(char const * nm);

// member variables
// none

};

// global constants

extern const structure some_structure;
extern type_info const & typeid_structure;

#endif // ndef _STRUCTURE_H_

