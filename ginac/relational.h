/** @file relational.h
 *
 *  Interface to relations between expressions. */

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

#ifndef __GINAC_RELATIONAL_H__
#define __GINAC_RELATIONAL_H__

#include <ginac/basic.h>
#include <ginac/ex.h>

namespace GiNaC {

/** This class holds a relation consisting of two expressions and a logical
 *  relation between them. */
class relational : public basic
{

// types
public:
    enum operators { equal,
           not_equal,
           less,
           less_or_equal,
           greater,
           greater_or_equal
    };
    
// member functions

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    relational();
    ~relational();
    relational(relational const & other);
    relational const & operator=(relational const & other);
protected:
    void copy(relational const & other);
    void destroy(bool call_parent);

    // other constructors
public:
    relational(ex const & lhs, ex const & rhs, operators oper=equal);
    
    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void printraw(ostream & os) const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printcsrc(ostream & os, unsigned type, unsigned upper_precedence=0) const;
    bool info(unsigned inf) const;
    int nops() const;
    ex & let_op(int const i);
    ex eval(int level=0) const;
    ex evalf(int level=0) const;
    ex simplify_ncmul(exvector const & v) const;
protected:
    int compare_same_type(basic const & other) const;
    unsigned return_type(void) const;
    unsigned return_type_tinfo(void) const;

    // new virtual functions which can be overridden by derived classes
    // none

    // non-virtual functions in this class
public:
    operator bool(void) const;
    
// member variables
    
protected:
    ex lh;
    ex rh;
    operators o;
    static unsigned precedence;
};

// global constants

extern const relational some_relational;
extern type_info const & typeid_relational;

// utility functions
inline const relational &ex_to_relational(const ex &e)
{
	return static_cast<const relational &>(*e.bp);
}

} // namespace GiNaC

#endif // ndef __GINAC_RELATIONAL_H__
