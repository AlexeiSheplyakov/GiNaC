/** @file clifford.h
 *
 *  Interface to GiNaC's clifford objects. */

#ifndef _CLIFFORD_H_
#define _CLIFFORD_H_

#include <string>

class clifford;

#include "indexed.h"

/** Base class for clifford object */
class clifford : public indexed
{
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    clifford();
    ~clifford();
    clifford(clifford const & other);
    clifford const & operator=(clifford const & other);
protected:
    void copy(clifford const & other); 
    void destroy(bool call_parent);

    // other constructors
public:
    explicit clifford(string const & initname);

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

extern const clifford some_clifford;
extern type_info const & typeid_clifford;

// macros

#define ex_to_clifford(X) static_cast<clifford const &>(*(X).bp)

#endif // ndef _CLIFFORD_H_


