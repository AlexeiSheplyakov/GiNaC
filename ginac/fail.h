/** @file fail.h
 *
 *  Interface to class signaling failure of operation. Considered obsolete all
 *  this stuff ought to be replaced by exceptions. */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_FAIL_H__
#define __GINAC_FAIL_H__

#include <ginac/basic.h>

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

class fail : public basic
{
    GINAC_DECLARE_REGISTERED_CLASS(fail, basic)

    // default constructor, destructor, copy constructor assignment operator and helpers
public:
    fail();
    ~fail();
    fail(const fail & other);
    const fail & operator=(const fail & other);
protected:
    void copy(const fail & other);
    void destroy(bool call_parent);

    // other constructors
    // none

    // functions overriding virtual functions from bases classes
public:
    basic * duplicate() const;
    void print(ostream & os, unsigned upper_precedence=0) const;
    void printraw(ostream & os) const;
protected:
    int compare_same_type(const basic & other) const;
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
extern const type_info & typeid_fail;

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

#endif // ndef __GINAC_FAIL_H__
