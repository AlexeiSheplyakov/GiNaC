/** @file constant.h
 *
 *  Interface to GiNaC's constant types and some special constants. */

/*
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __GINAC_CONSTANT_H__
#define __GINAC_CONSTANT_H__

#include <string>
#include <ginac/basic.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

/** This class holds constants, symbols with specific numerical value. Each
 *  object of this class must either provide their own function to evaluate it
 *  to class numeric or provide the constant as a numeric (if it's an exact
 *  number). */
class constant : public basic
{

// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    ~constant();
    constant(constant const & other);
    // constant const & operator=(constant const & other); /* it's pervert! */
protected:
    void copy(constant const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    constant(string const & initname, ex (*efun)()=0);
    constant(string const & initname, numeric const & initnumber);

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    ex evalf(int level=0) const;
    ex diff(symbol const & s) const;
protected:
    int compare_same_type(basic const & other) const;
    bool is_equal_same_type(basic const & other) const;
    
    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
    // none

// member variables

private:
    string name;
    ex (*ef)();
    numeric * number;
    bool fct_assigned;
    unsigned serial;  //!< unique serial number for comparision
    static unsigned next_serial;
};

// global constants

extern const constant some_constant;
extern type_info const & typeid_constant;

// extern const numeric I;
extern const constant Pi;
extern const constant Catalan;
extern const constant EulerGamma;

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_CONSTANT_H__
