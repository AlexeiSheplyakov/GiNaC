/** @file isospin.h
 *
 *  Interface to GiNaC's isospin objects. */

#ifndef _ISOSPIN_H_
#define _ISOSPIN_H_

#include <string>

class isospin;

#include "indexed.h"

/** Base class for isospin object */
class isospin : public indexed
{
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    isospin();
    ~isospin();
    isospin(isospin const & other);
    isospin const & operator=(isospin const & other);
protected:
    void copy(isospin const & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    explicit isospin(string const & initname);

    // functions overriding virtual functions from base classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void printtree(ostream & os, unsigned indent) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
protected:
    int compare_same_type(basic const & other) const;
    ex simplify_ncmul(exvector const & v) const;
    unsigned calchash(void) const;

    // new virtual functions which can be overridden by derived classes
    // none
    
    // non-virtual functions in this class
public:
    void setname(string const & n);
private:
    string & autoname_prefix(void);

// member variables

protected:
    string name;
    unsigned serial; // unique serial number for comparision
private:
    static unsigned next_serial;
};

// global constants

extern const isospin some_isospin;
extern type_info const & typeid_isospin;

// macros

#define ex_to_isospin(X) static_cast<isospin const &>(*(X).bp)

#endif // ndef _ISOSPIN_H_


